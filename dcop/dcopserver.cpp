/*****************************************************************

Copyright (c) 1999 Preston Brown <pbrown@kde.org>
Copyright (c) 1999 Matthias Ettrich <ettrich@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#include <qfile.h>
#include <qtextstream.h>
#include <qdatastream.h>
#include <qstack.h>

#include <dcopserver.h>
#include <dcopglobal.h>

static Bool HostBasedAuthProc ( char* /*hostname*/)
{
  return false; // no host based authentication
}

// SM DUMMY
#include <X11/SM/SMlib.h>

static Status NewClientProc ( SmsConn, SmPointer, unsigned long*, SmsCallbacks*, char** )
{
    return 0;
};

static void registerXSM()
{
    char 	errormsg[256];
    if (!SmsInitialize ((char *)"SAMPLE-SM", (char *)"1.0",
	NewClientProc, NULL,
	HostBasedAuthProc, 256, errormsg))
    {
	qFatal("register xsm failed");
    }
}



static DCOPServer* the_server = 0;
class DCOPListener : public QSocketNotifier
{
 public:
  DCOPListener( IceListenObj obj )
    : QSocketNotifier( IceGetListenConnectionNumber( obj ),
		       QSocketNotifier::Read, 0, 0)
    {
      listenObj = obj;
    }

  IceListenObj listenObj;
};

class DCOPConnection : public QSocketNotifier
{
 public:
  DCOPConnection( IceConn conn )
    : QSocketNotifier( IceConnectionNumber( conn ),
		       QSocketNotifier::Read, 0, 0 )
    {
      iceConn = conn;
      status = IceConnectPending;
    }

  QCString appId;
  IceConn iceConn;
  IceConnectStatus status;
  QStack< _IceConn> waitingForReply;
};

IceAuthDataEntry *authDataEntries = 0;
static char *addAuthFile = 0;
static char *remAuthFile = 0;

static IceListenObj *listenObjs = 0;
int numTransports = 0;

static IceIOErrorHandler prev_handler;


/* for printing hex digits */
static void fprintfhex (FILE *fp, unsigned int len, char *cp)
{
  static char hexchars[] = "0123456789abcdef";

  for (; len > 0; len--, cp++) {
    unsigned char s = *cp;
    putc(hexchars[s >> 4], fp);
    putc(hexchars[s & 0x0f], fp);
  }
}

/*
 * We use temporary files which contain commands to add/remove entries from
 * the .ICEauthority file.
 */
static void
write_iceauth (FILE *addfp, FILE *removefp, IceAuthDataEntry *entry)
{
  fprintf (addfp,
	   "add %s \"\" %s %s ",
	   entry->protocol_name,
	   entry->network_id,
	   entry->auth_name);
  fprintfhex (addfp, entry->auth_data_length, entry->auth_data);
  fprintf (addfp, "\n");

  fprintf (removefp,
	   "remove protoname=%s protodata=\"\" netid=%s authname=%s\n",
	   entry->protocol_name,
	   entry->network_id,
	   entry->auth_name);
}


#ifndef HAVE_MKSTEMP
static char *unique_filename (const char *path, const char *prefix)
#else
static char *unique_filename (const char *path, const char *prefix, int *pFd)
#endif
{
#ifndef HAVE_MKSTEMP
#ifndef X_NOT_POSIX
  return ((char *) tempnam (path, prefix));
#else
  char tempFile[PATH_MAX];
  char *tmp;

  sprintf (tempFile, "%s/%sXXXXXX", path, prefix);
  tmp = (char *) mktemp (tempFile);
  if (tmp)
    {
      char *ptr = (char *) malloc (strlen (tmp) + 1);
      strcpy (ptr, tmp);
      return (ptr);
    }
  else
    return (NULL);
#endif
#else
  char tempFile[PATH_MAX];
  char *ptr;

  sprintf (tempFile, "%s/%sXXXXXX", path, prefix);
  ptr = (char *)malloc(strlen(tempFile) + 1);
  if (ptr != NULL)
    {
      strcpy(ptr, tempFile);
      *pFd =  mkstemp(ptr);
    }
  return ptr;
#endif
}

#define MAGIC_COOKIE_LEN 16

Status
SetAuthentication (int count, IceListenObj *listenObjs,
		   IceAuthDataEntry **authDataEntries)
{
  FILE        *addfp = NULL;
  FILE        *removefp = NULL;
  const char  *path;
  int         original_umask;
  char        command[256];
  int         i;
#ifdef HAVE_MKSTEMP
  int         fd;
#endif

  original_umask = umask (0077);      /* disallow non-owner access */

  path = getenv ("DCOP_SAVE_DIR");
  if (!path)
    path = "/tmp";
#ifndef HAVE_MKSTEMP
  if ((addAuthFile = unique_filename (path, "dcop")) == NULL)
    goto bad;

  if (!(addfp = fopen (addAuthFile, "w")))
    goto bad;

  if ((remAuthFile = unique_filename (path, "dcop")) == NULL)
    goto bad;

  if (!(removefp = fopen (remAuthFile, "w")))
    goto bad;
#else
  if ((addAuthFile = unique_filename (path, "dcop", &fd)) == NULL)
    goto bad;

  if (!(addfp = fdopen(fd, "wb")))
    goto bad;

  if ((remAuthFile = unique_filename (path, "dcop", &fd)) == NULL)
    goto bad;

  if (!(removefp = fdopen(fd, "wb")))
    goto bad;
#endif

  if ((*authDataEntries = (IceAuthDataEntry *) malloc (
						       count * 2 * sizeof (IceAuthDataEntry))) == NULL)
    goto bad;

  for (i = 0; i < numTransports * 2; i += 2) {
    (*authDataEntries)[i].network_id =
      IceGetListenConnectionString (listenObjs[i/2]);
    (*authDataEntries)[i].protocol_name = (char *) "ICE";
    (*authDataEntries)[i].auth_name = (char *) "MIT-MAGIC-COOKIE-1";

    (*authDataEntries)[i].auth_data =
      IceGenerateMagicCookie (MAGIC_COOKIE_LEN);
    (*authDataEntries)[i].auth_data_length = MAGIC_COOKIE_LEN;

    (*authDataEntries)[i+1].network_id =
      IceGetListenConnectionString (listenObjs[i/2]);
    (*authDataEntries)[i+1].protocol_name = (char *) "DCOP";
    (*authDataEntries)[i+1].auth_name = (char *) "MIT-MAGIC-COOKIE-1";

    (*authDataEntries)[i+1].auth_data =
      IceGenerateMagicCookie (MAGIC_COOKIE_LEN);
    (*authDataEntries)[i+1].auth_data_length = MAGIC_COOKIE_LEN;

    write_iceauth (addfp, removefp, &(*authDataEntries)[i]);
    write_iceauth (addfp, removefp, &(*authDataEntries)[i+1]);

    IceSetPaAuthData (2, &(*authDataEntries)[i]);

    IceSetHostBasedAuthProc (listenObjs[i/2], HostBasedAuthProc);
  }

  fclose (addfp);
  fclose (removefp);

  umask (original_umask);

  sprintf (command, "iceauth source %s", addAuthFile);
  system (command);

  unlink (addAuthFile);

  return (1);

 bad:

  if (addfp)
    fclose (addfp);

  if (removefp)
    fclose (removefp);

  if (addAuthFile) {
    unlink(addAuthFile);
    free(addAuthFile);
  }
  if (remAuthFile) {
    unlink(remAuthFile);
    free(remAuthFile);
  }

  return (0);
}

/*
 * Free up authentication data.
 */
void
FreeAuthenticationData(int count, IceAuthDataEntry *authDataEntries)
{
  /* Each transport has entries for ICE and XSMP */

  char command[256];
  int i;

  for (i = 0; i < count * 2; i++) {
    free (authDataEntries[i].network_id);
    free (authDataEntries[i].auth_data);
  }

  free (authDataEntries);

  sprintf (command, "iceauth source %s", remAuthFile);
  system(command);

  unlink(remAuthFile);

  free(addAuthFile);
  free(remAuthFile);
}

void
MyIoErrorHandler ( IceConn iceConn )

{
    if (prev_handler)
	(*prev_handler) (iceConn);
}

void
InstallIOErrorHandler ()

{
    IceIOErrorHandler default_handler;

    prev_handler = IceSetIOErrorHandler (NULL);
    default_handler = IceSetIOErrorHandler (MyIoErrorHandler);
    if (prev_handler == default_handler)
	prev_handler = NULL;
}


void DCOPWatchProc ( IceConn iceConn, IcePointer client_data, Bool opening, IcePointer* watch_data)
{
  DCOPServer* ds = ( DCOPServer*) client_data;

  if (opening) {
    *watch_data = (IcePointer) ds->watchConnection( iceConn );
  }
  else  {
    ds->removeConnection( (void*) *watch_data );
  }
}

void DCOPProcessMessage( IceConn iceConn, IcePointer /*clientData*/,
			 int opcode, unsigned long length, Bool swap)
{
  the_server->processMessage( iceConn, opcode, length, swap );
}

void DCOPServer::processMessage( IceConn iceConn, int opcode,
				 unsigned long length, Bool /*swap*/)
{
  switch( opcode ) {
  case DCOPSend:
    {
      DCOPMsg *pMsg = 0;
      IceReadMessageHeader(iceConn, sizeof(DCOPMsg), DCOPMsg, pMsg);
      QByteArray ba( length );
      IceReadData(iceConn, length, ba.data() );
      QDataStream ds( ba, IO_ReadOnly );
      QCString appFrom, app;
      ds >> appFrom >> app;
      DCOPConnection* target = appIds.find( app );
      if ( target ) {
	IceGetHeader( target->iceConn, majorOpcode, DCOPSend,
		     sizeof(DCOPMsg), DCOPMsg, pMsg );
	int datalen = ba.size();
	pMsg->length += datalen;
	IceSendData(target->iceConn, datalen, (char *) ba.data());
      } else if ( app == "DCOPServer" ) {
	QCString obj, fun;
	QByteArray data;
	ds >> obj >> fun >> data;
	QCString replyType;
	QByteArray replyData;
	if ( !receive( app, obj, fun, data, replyType, replyData, iceConn ) ) {
	    qWarning("%s failure: object '%s' has no function '%s'", app.data(), obj.data(), fun.data() );
	}
      } else if ( app == "*") {
	// handle a broadcast.
	QDictIterator<DCOPConnection> aIt(appIds);
    for ( ; aIt.current(); ++aIt) {
	    IceGetHeader(aIt.current()->iceConn, majorOpcode, DCOPSend,
			 sizeof(DCOPMsg), DCOPMsg, pMsg);
	    int datalen = ba.size();
	    pMsg->length += datalen;
	    IceSendData(aIt.current()->iceConn, datalen, (char *) ba.data());
	}
      }
    }
    break;
  case DCOPCall:
    {
      DCOPMsg *pMsg = 0;
      IceReadMessageHeader(iceConn, sizeof(DCOPMsg), DCOPMsg, pMsg);
      QByteArray ba( length );
      IceReadData(iceConn, length, ba.data() );
      QDataStream ds( ba, IO_ReadOnly );
      QCString appFrom, app;
      ds >> appFrom >> app;
      DCOPConnection* target = appIds.find( app );
      int datalen = ba.size();
      if ( target ) {
	target->waitingForReply.push( iceConn );
	IceGetHeader( target->iceConn, majorOpcode, DCOPCall,
		     sizeof(DCOPMsg), DCOPMsg, pMsg );
	pMsg->length += datalen;
	IceSendData(target->iceConn, datalen, (char *) ba.data());
      } else {
	QCString replyType;
	QByteArray replyData;
	bool b = FALSE;
	if ( app == "DCOPServer" ) {
	  QCString obj, fun;
	  QByteArray data;
	  ds >> obj >> fun >> data;
	  b = receive( app, obj, fun, data, replyType, replyData, iceConn );
	  if ( !b )
	      qWarning("%s failure: object '%s' has no function '%s'", app.data(), obj.data(), fun.data() );
	}

	QByteArray reply;
	QDataStream replyStream( reply, IO_WriteOnly );
	replyStream << replyType << replyData.size();
	
	int datalen = reply.size() + replyData.size();
	IceGetHeader( iceConn, majorOpcode, b? DCOPReply : DCOPReplyFailed,
		      sizeof(DCOPMsg), DCOPMsg, pMsg );
	pMsg->length += datalen;
	IceSendData( iceConn, reply.size(), (char *) reply.data());
	IceSendData( iceConn, replyData.size(), (char *) replyData.data());
      }
    }
    break;
  case DCOPReply:
  case DCOPReplyFailed:
    {
      DCOPMsg *pMsg = 0;
      IceReadMessageHeader(iceConn, sizeof(DCOPMsg), DCOPMsg, pMsg);
      QByteArray ba( length );
      IceReadData(iceConn, length, ba.data() );
      DCOPConnection* conn = clients.find( iceConn );
      if ( conn ) {
	DCOPConnection* connreply = clients.find( conn->waitingForReply.pop() );
	if ( connreply ) {
	  IceGetHeader( connreply->iceConn, majorOpcode, opcode,
			sizeof(DCOPMsg), DCOPMsg, pMsg );
	  int datalen = ba.size();
	  pMsg->length += datalen;
	  IceSendData(connreply->iceConn, datalen, (char *) ba.data());
	}
      }
    }
    break;
  default:
    qDebug("DCOPServer::processMessage unknown message");
  }
}


IcePaVersionRec DCOPVersions[] = {
  { DCOPVersionMajor, DCOPVersionMinor,  DCOPProcessMessage }
};

typedef struct DCOPServerConnStruct *DCOPServerConn;

struct DCOPServerConnStruct
{
  /*
   * We use ICE to esablish a connection with the client.
   */

  IceConn		iceConn;


  /*
   * Major and minor versions of the XSMP.
   */

  int			proto_major_version;
  int			proto_minor_version;


  QCString clientId;
};


static Status DCOPServerProtocolSetupProc ( IceConn iceConn,
					    int majorVersion, int minorVersion,
					    char* vendor, char* release,
					    IcePointer *clientDataRet,
					    char **/*failureReasonRet*/)
{
    DCOPServerConn serverConn;
//     unsigned long 	mask;
//     Status		status;

    /*
     * vendor/release are undefined for ProtocolSetup in DCOP
     */

    if (vendor)
	free (vendor);
    if (release)
	free (release);


    /*
     * Allocate new DCOPServerConn.
     */

    serverConn = new DCOPServerConnStruct;

    serverConn->iceConn = iceConn;
    serverConn->proto_major_version = majorVersion;
    serverConn->proto_minor_version = minorVersion;
    //serverConn->clientId already initialized

    *clientDataRet = (IcePointer) serverConn;


    return 1;
}

static void
CloseListeners ()
{
  IceFreeListenObjs (numTransports, listenObjs);

  QCString fName;
  fName = ::getenv("DCOPSERVER");
  if (fName.isNull()) {
    fName = ::getenv("HOME");
    fName += "/.DCOPserver";
  }
  unlink(fName.data());

  FreeAuthenticationData(numTransports, authDataEntries);
  exit(0);
}

static void sighandler(int sig)
{
  if (sig == SIGHUP) {
    signal(SIGHUP, sighandler);
    return;
  }

  CloseListeners();
}

DCOPServer::DCOPServer()
  : QObject(0,0), appIds(200), clients(200)
{
    extern int _IceLastMajorOpcode; // from libICE
    if (_IceLastMajorOpcode < 1 )
	registerXSM();

  the_server = this;
  if (( majorOpcode = IceRegisterForProtocolReply ((char *) "DCOP",
						   (char *) DCOPVendorString,
						   (char *) DCOPReleaseString,
						   1, DCOPVersions,
						   1, (char **) DCOPAuthNames,
						   DCOPServerAuthProcs,
						   HostBasedAuthProc,
						   DCOPServerProtocolSetupProc,
						   NULL,	/* IceProtocolActivateProc - we don't care about
								   when the Protocol Reply is sent, because the
								   session manager can not immediately send a
								   message - it must wait for RegisterClient. */
						   NULL	/* IceIOErrorProc */
						   )) < 0)
    {
      qDebug("Could not register DCOP protocol with ICE");
    }
  char errormsg[256];
  qDebug("DCOPServer, dcop is major opcode %d", majorOpcode );

  if (!IceListenForConnections (&numTransports, &listenObjs,
				256, errormsg))
    {
      fprintf (stderr, "%s\n", errormsg);
      exit (1);
    } else {
      // publish available transports.
      QCString fName;
      fName = ::getenv("DCOPSERVER");
      if (fName.isNull()) {
	fName = ::getenv("HOME");
	fName += "/.DCOPserver";
      }
      FILE *f;
      f = ::fopen(fName.data(), "w+");
      fprintf(f, IceComposeNetworkIdList(numTransports, listenObjs));
      fprintf(f, "\n%i\n", getpid());
      fclose(f);
    }

  if (!SetAuthentication(numTransports, listenObjs, &authDataEntries)) {
    qDebug("dcopserver could not set authorization");
    exit(1);
  }

  IceAddConnectionWatch (DCOPWatchProc, (IcePointer) this);

  listener.setAutoDelete( TRUE );
  DCOPListener* con;
  for ( int i = 0; i < numTransports; i++) {
    con = new DCOPListener( listenObjs[i] );
    listener.append( con );
    connect( con, SIGNAL( activated(int) ), this, SLOT( newClient(int) ) );
  }
}

DCOPServer::~DCOPServer()
{
}


void DCOPServer::processData( int /*socket*/ )
{
  DCOPConnection* conn = (DCOPConnection*)sender();
  IceProcessMessagesStatus s =  IceProcessMessages( conn->iceConn, 0, 0 );


  if (s == IceProcessMessagesIOError) {
    IceCloseConnection( conn->iceConn );
    return;
  }
  conn->status = IceConnectionStatus(  conn->iceConn );
}

void DCOPServer::newClient( int /*socket*/ )
{
  IceAcceptStatus status;
  IceConn iceConn = IceAcceptConnection( ((DCOPListener*)sender())->listenObj, &status);
  IceSetShutdownNegotiation( iceConn, False );


  IceConnectStatus cstatus;
  while ((cstatus = IceConnectionStatus (iceConn))==IceConnectPending) {
    qApp->processOneEvent();
  }
  if (cstatus == IceConnectAccepted) {
    char* connstr = IceConnectionString (iceConn);
    free (connstr);
  } else {
    if (cstatus == IceConnectIOError)
      qDebug ("IO error opening ICE Connection!\n");
    else
      qDebug ("ICE Connection rejected!\n");
    IceCloseConnection (iceConn);
  }
}

void* DCOPServer::watchConnection( IceConn iceConn )
{
  qDebug("new connection (count=%d)", clients.count() );
  DCOPConnection* con = new DCOPConnection( iceConn );
  connect( con, SIGNAL( activated(int) ), this, SLOT( processData(int) ) );

  clients.insert(iceConn, con );

  return (void*) con;
}

void DCOPServer::removeConnection( void* data )
{
  DCOPConnection* conn = (DCOPConnection*)data;
  clients.remove(conn->iceConn );
  if ( !conn->appId.isNull() ) {
      qDebug("remove connection '%s' (count=%d)", conn->appId.data(), clients.count() );
      appIds.remove( conn->appId );
      QPtrDictIterator<DCOPConnection> it( clients );
      QByteArray data;
      QDataStream datas( data, IO_WriteOnly );
      datas << conn->appId;
      QByteArray ba;
      QDataStream ds( ba, IO_WriteOnly );
      ds << QCString("DCOPServer") << QCString("")
	 << QCString("") << QCString("applicationRemoved(QCString)") << data;
      int datalen = ba.size();
      DCOPMsg *pMsg = 0;
      while ( it.current() ) {
	  DCOPConnection* c = it.current();
	  ++it;
	  if ( c != conn ) {
	      IceGetHeader( c->iceConn, majorOpcode, DCOPSend,
			    sizeof(DCOPMsg), DCOPMsg, pMsg );
	      pMsg->length += datalen;
	      IceSendData(c->iceConn, datalen, (char *) ba.data());
	  }
      }
  }
  else
      qDebug("remove unregistered connection (count=%d)", clients.count() );
  delete conn;
}

bool DCOPServer::receive(const QCString &/*app*/, const QCString &/*obj*/,
			 const QCString &fun, const QByteArray& data,
			 QCString& replyType, QByteArray &replyData,
			 IceConn iceConn)
{
  if ( fun == "registerAs(QCString)" ) {
    QDataStream args( data, IO_ReadOnly );
    if (!args.atEnd()) {
      QCString app2;
      args >> app2;
      QDataStream reply( replyData, IO_WriteOnly );
      DCOPConnection* conn = clients.find( iceConn );
      if ( conn && !app2.isEmpty() ) {
	  if ( !conn->appId.isNull() &&
	       appIds.find( conn->appId ) == conn ) {
	      appIds.remove( conn->appId );
	      qDebug("remove '%s', will be reregistered", conn->appId.data() );
	  }
	
	  conn->appId = app2;
	  if ( appIds.find( app2 ) != 0 ) {
	      // we already have this application, unify
	      int n = 1;
	      QCString tmp;
	      do {
		  n++;
		  tmp.setNum( n );
		  tmp.prepend("-");
		  tmp.prepend( app2 );
	      } while ( appIds.find( tmp ) != 0 );
	      conn->appId = tmp;
	  }
	  qDebug("register '%s'", conn->appId.data() );
	  appIds.insert( conn->appId, conn );
	  QPtrDictIterator<DCOPConnection> it( clients );
	  QByteArray data;
	  QDataStream datas( data, IO_WriteOnly );
	  datas << conn->appId;
	  QByteArray ba;
	  QDataStream ds( ba, IO_WriteOnly );
	  ds <<QCString("DCOPServer") <<  QCString("") << QCString("")
	     << QCString("applicationRegistered(QCString)") << data;
	  int datalen = ba.size();
	  DCOPMsg *pMsg = 0;
	  while ( it.current() ) {
	      DCOPConnection* c = it.current();
	      ++it;
	      if ( c != conn ) {
		  IceGetHeader( c->iceConn, majorOpcode, DCOPSend,
				sizeof(DCOPMsg), DCOPMsg, pMsg );
		  pMsg->length += datalen;
		  IceWriteData( c->iceConn, datalen, (char *) ba.data());
		  IceFlush( c->iceConn );
	      }
	  }
      }
      replyType = "QCString";
      reply << conn->appId;
      return TRUE;
    }
  }
  else if ( fun == "registeredApplications()" ) {
    QDataStream reply( replyData, IO_WriteOnly );
    QCStringList applications;
    QDictIterator<DCOPConnection> it( appIds );
    while ( it.current() ) {
      applications << it.currentKey().ascii();
      ++it;
    }
    replyType = "QCStringList";
    reply << applications;
    return TRUE;
  } else if ( fun == "isApplicationRegistered(QCString)" ) {
    QDataStream args( data, IO_ReadOnly );
    if (!args.atEnd()) {
      QCString s;
      args >> s;
      QDataStream reply( replyData, IO_WriteOnly );
      int b = ( appIds.find( s ) != 0 );
      replyType = "bool";
      reply << b;
      return TRUE;
    }
  }

  return FALSE;
}

int main( int argc, char* argv[] )
{
  // check if we are already running
  QCString fName;
  fName = ::getenv("DCOPSERVER");
  if (fName.isNull()) {
    fName = ::getenv("HOME");
    fName += "/.DCOPserver";
  }
  if (::access(fName.data(), R_OK) == 0) {
    QFile f(fName);
    f.open(IO_ReadOnly);
    QTextStream t(&f);
    t.readLine(); // skip over connection list
    bool ok = false;
    pid_t pid = t.readLine().toUInt(&ok);
    f.close();
    if (ok && (kill(pid, SIGHUP) == 0)) {
      qWarning( "---------------------------------\n"
		"It looks like dcopserver is already running. If you are sure\n"
		"that it is not already running, remove %s\n"
		"and start dcopserver again.\n"
		"---------------------------------\n",
		fName.data() );

      // lock file present, die silently.
      exit(0);
    } else {
      // either we couldn't read the PID or kill returned an error.
      // remove lockfile and continue
      unlink(fName.data());
    }
  }

  bool nofork = false;
  for(int i = 1; i < argc; i++)
     if (strcmp(argv[i], "--nofork") == 0)
        nofork = true;

  if (!nofork)
  {
    if (fork() > 0)
      exit(0); // I am the parent

    setsid();

    if (fork() > 0)
      exit(0); // get rid of controlling terminal
  }

  signal(SIGHUP, sighandler);
  signal(SIGTERM, sighandler);
  signal(SIGPIPE, SIG_IGN);

  QApplication a( argc, argv );

  InstallIOErrorHandler();
  DCOPServer server;

  return a.exec();
}

#include "dcopserver.moc"
