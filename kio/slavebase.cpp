/*
 *
 *  This file is part of the KDE libraries
 *  Copyright (c) 2000 Waldo Bastian <bastian@kde.org>
 *  Copyright (c) 2000 David Faure <faure@kde.org>
 *  Copyright (c) 2000 Stephan Kulow <coolo@kde.org>
 *
 * $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include <assert.h>
#include <kdebug.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

#include <qfile.h>

#include <ksock.h>

#include "kio/slavebase.h"
#include "kio/slaveinterface.h"
#include "kio/connection.h"

using namespace KIO;

template class QList<QValueList<UDSAtom> >;

#define KIO_DATA QByteArray data; QDataStream stream( data, IO_WriteOnly ); stream

//////////////

SlaveBase::SlaveBase( const QCString &protocol,
                      const QCString &pool_socket,
                      const QCString &app_socket )
    : mProtocol(protocol), m_pConnection(0),
      mPoolSocket( QFile::decodeName(pool_socket)),
      mAppSocket( QFile::decodeName(app_socket))
{
    appconn = new Connection();
    pendingListEntries.setAutoDelete(true);
    listEntryCurrentSize = 0;
    struct timeval tp;
    gettimeofday(&tp, 0);
    listEntry_sec = tp.tv_sec;
    listEntry_usec = tp.tv_usec;
    mConnectedToApp = true;
    connectSlave(mAppSocket);
}

void SlaveBase::dispatchLoop()
{
    fd_set rfds;
    struct timeval tv;
    int retval;

    while (true) {
	FD_ZERO(&rfds);

	if (appconn->inited())
	  FD_SET(appconn->fd_from(), &rfds);

	/* Wait up to 30 seconds. */
	tv.tv_sec = 30;
	tv.tv_usec = 0;

	retval = select(appconn->fd_from()+ 1, &rfds, NULL, NULL, &tv);
	/* Don't rely on the value of tv now! */

	if (retval > 0)
        {
	    if (FD_ISSET(appconn->fd_from(), &rfds))
            { // dispatch application messages
		int cmd;
		QByteArray data;
                if ( appconn->read(&cmd, data) != -1 )
                {
		  kdDebug(7019) << "app said " << cmd << endl;
                  if (cmd == CMD_SLAVE_CONNECT)
                  {
                     QString app_socket;
                     QDataStream stream( data, IO_ReadOnly);
                     stream >> app_socket;
                     kdDebug(7019) << "slavewrapper: Connecting to new app (" << app_socket << ")." << endl;
                     appconn->send( MSG_SLAVE_ACK );
                     disconnectSlave();
                     mConnectedToApp = true;
                     connectSlave(app_socket);
                  }
                  else
                  {
                     dispatch(cmd, data);
                  }
                }
                else // some error occured, perhaps no more application
                {
// When the app exits, should the slave be put back in the pool ?
                  if (mConnectedToApp)
                  {
                     kdDebug(7019) << "slavewrapper: Communication with app lost. Returning to slave pool." << endl;
                     disconnectSlave();
                     mConnectedToApp = false;
                     connectSlave(mPoolSocket);
                  }
                  else
                  {
                     kdDebug(7019) << "slavewrapper: Communication with pool lost. Exiting." << endl;
                     exit(0);
                  }
                }
	    }
	}
        else if (retval == -1) // error
        {
          kdDebug(7019) << "slavewrapper: select returned error " << (errno==EBADF?"EBADF":errno==EINTR?"EINTR":errno==EINVAL?"EINVAL":errno==ENOMEM?"ENOMEM":"unknown") << " (" << errno << ")" << endl;
          exit(0);
        }
    }
}

void SlaveBase::connectSlave(const QString& path)
{
    appconn->init(new KSocket(path));
    setConnection(appconn);
}

void SlaveBase::disconnectSlave()
{
    appconn->close();
}


void SlaveBase::data( const QByteArray &data )
{
    m_pConnection->send( MSG_DATA, data );
}

void SlaveBase::dataReq( )
{
    m_pConnection->send( MSG_DATA_REQ );
}

void SlaveBase::error( int _errid, const QString &_text )
{
    KIO_DATA << _errid << _text;

    m_pConnection->send( MSG_ERROR, data );
}

void SlaveBase::connected()
{
    m_pConnection->send( MSG_CONNECTED );
}

void SlaveBase::finished()
{
    m_pConnection->send( MSG_FINISHED );
}

void SlaveBase::renamed( const QString &_new )
{
    KIO_DATA << _new;
    m_pConnection->send( MSG_RENAMED, data );
}

void SlaveBase::slaveStatus( const QString &host, bool connected )
{
    pid_t pid = getpid();
    Q_INT8 b = connected ? 1 : 0;
    KIO_DATA << pid << mProtocol << host << b;
    m_pConnection->send( MSG_SLAVE_STATUS, data );
}

void SlaveBase::canResume( bool _resume )
{
    KIO_DATA << (int)_resume;
    m_pConnection->send( MSG_RESUME, data );
}

void SlaveBase::totalSize( unsigned long _bytes )
{
    KIO_DATA << _bytes;
    m_pConnection->send( INF_TOTAL_SIZE, data );
}

void SlaveBase::processedSize( unsigned long _bytes )
{
    KIO_DATA << _bytes;
    m_pConnection->send( INF_PROCESSED_SIZE, data );
}

void SlaveBase::speed( unsigned long _bytes_per_second )
{
    KIO_DATA << _bytes_per_second;
    m_pConnection->send( INF_SPEED, data );
}

void SlaveBase::redirection( const KURL& _url )
{
    KIO_DATA << _url;
    m_pConnection->send( INF_REDIRECTION, data );
}

void SlaveBase::errorPage()
{
    m_pConnection->send( INF_ERROR_PAGE );
}

void SlaveBase::mimeType( const QString &_type)
{
    KIO_DATA << _type;
    m_pConnection->send( INF_MIME_TYPE, data );
}

// ?
void SlaveBase::gettingFile( const QString &file)
{
    KIO_DATA << file;
    m_pConnection->send( INF_GETTING_FILE, data );
}

void SlaveBase::warning( const QString &_msg)
{
    KIO_DATA << _msg;
    m_pConnection->send( INF_WARNING, data );
}

void SlaveBase::statEntry( const UDSEntry& entry )
{
    KIO_DATA << entry;
    m_pConnection->send( MSG_STAT_ENTRY, data );
}

void SlaveBase::listEntry( const UDSEntry& entry, bool _ready )
{
    static struct timeval tp;
    static const int maximum_updatetime = 800;
    static const int minimum_updatetime = (maximum_updatetime * 3) / 4;

    if (!_ready) {
	pendingListEntries.append(new UDSEntry(entry));
	
	if (pendingListEntries.count() > listEntryCurrentSize) {
	
            gettimeofday(&tp, 0);
	
            long diff = ((tp.tv_sec - listEntry_sec) * 1000000 +
                         tp.tv_usec - listEntry_usec) / 1000;

            if (diff > maximum_updatetime) {
                listEntryCurrentSize = listEntryCurrentSize * 3 / 4;
		_ready = true;
            } else if (diff < minimum_updatetime) {
                listEntryCurrentSize = listEntryCurrentSize * 5 / 4;
            } else {
		_ready = true;
	    }
        }
    }

    if (_ready) { // may happen when we started with !ready
	gettimeofday(&tp, 0);
	listEntry_sec = tp.tv_sec;
	listEntry_usec = tp.tv_usec;

	listEntries( pendingListEntries );
	pendingListEntries.clear();
    }
}

void SlaveBase::listEntries( const UDSEntryList& list )
{
    kdDebug(7007) << "listEntries " << list.count() << "d" << endl;

    KIO_DATA << list.count();
    UDSEntryListIterator it(list);
    for (; it.current(); ++it)
	stream << *it.current();
    m_pConnection->send( MSG_LIST_ENTRIES, data);
}

/*
void SlaveBase::isDirectory()
{
    m_pConnection->send( MSG_IS_DIRECTORY, QByteArray());
}

void SlaveBase::isFile()
{
    m_pConnection->send( MSG_IS_FILE, QByteArray());
}
*/

void SlaveBase::sigsegv_handler (int)
{
    // Debug and printf should be avoided because they might
    // call malloc.. and get in a nice recursive malloc loop
    write(2, "kioslave : ###############SEG FAULT#############\n", 49);
    exit(1);
}

void SlaveBase::setHost(QString const &host, int, QString const &, QString const &)
{
    kdDebug( 7007 ) << "setHost( host = " << host << ")" << endl;
}

void SlaveBase::openConnection(void)
{ error(  ERR_UNSUPPORTED_ACTION, "open" ); }
void SlaveBase::closeConnection(void)
{ error(  ERR_UNSUPPORTED_ACTION, "close" ); }
void SlaveBase::stat(QString const &)
{ error(  ERR_UNSUPPORTED_ACTION, "stat" ); }
void SlaveBase::put(QString const &, int, bool, bool)
{ error(  ERR_UNSUPPORTED_ACTION, "put" ); }
void SlaveBase::special(QArray<char> const &)
{ error(  ERR_UNSUPPORTED_ACTION, "special" ); }
void SlaveBase::listDir(QString const &)
{ error(  ERR_UNSUPPORTED_ACTION, "listDir" ); }
void SlaveBase::get(QString const &, QString const &, bool)
{ error(  ERR_UNSUPPORTED_ACTION, "get" ); }
void SlaveBase::mimetype(QString const &path)
{ get(path, QString::null, false); }
void SlaveBase::rename(QString const &, QString const &, bool)
{ error(  ERR_UNSUPPORTED_ACTION, "rename" ); }
void SlaveBase::copy(const QString &, const QString &, int, bool)
{ error(  ERR_UNSUPPORTED_ACTION, "copy" ); }
void SlaveBase::del(QString const &, bool)
{ error(  ERR_UNSUPPORTED_ACTION, "del" ); }
void SlaveBase::mkdir(QString const &, int)
{ error(  ERR_UNSUPPORTED_ACTION, "mkdir" ); }
void SlaveBase::chmod(QString const &, int)
{ error(  ERR_UNSUPPORTED_ACTION, "chmod" ); }

void SlaveBase::slave_status()
{ slaveStatus( QString::null, false ); }

bool SlaveBase::dispatch()
{
    assert( m_pConnection );

    int cmd;
    QByteArray data;
    if ( m_pConnection->read( &cmd, data ) == -1 )
	return false;

    dispatch( cmd, data );
    return true;
}

bool SlaveBase::openPassDlg( const QString& head, QString& user, QString& pass )
{
    kdDebug(7007) << "openPassDlg " << head << endl;
    KIO_DATA << head << user << pass;
    m_pConnection->send( INF_NEED_PASSWD, data );
    int cmd;
    if ( m_pConnection->read( &cmd, data ) == -1 ) {
	return false;
    }
    kdDebug(7007) << "reading " << cmd << endl;
    if (cmd != CMD_USERPASS) {
	if (cmd != CMD_NONE)
	    dispatch( cmd, data );
      return false;
    } else {
      QDataStream stream( data, IO_ReadOnly );
      stream >> user >> pass;
      kdDebug(7007) << "got " << cmd << " " << user << " " << pass << endl;
      return true;
    }
}

int SlaveBase::readData( QByteArray &buffer)
{
   int cmd;
   int result;
   result = m_pConnection->read( &cmd, buffer );

   if (result == -1)
      return -1;

   kdDebug(7007) << "readData: cmd = " << cmd << ", length = " << result << " " << endl;

   if (cmd != MSG_DATA)
      return -1;

   return result;
}


void SlaveBase::dispatch( int command, const QByteArray &data )
{
    QDataStream stream( data, IO_ReadOnly );

    QString str1, str2;
    int i;

    switch( command ) {
    case CMD_HOST: {
        QString passwd;
	stream >> str1 >> i >> str2 >> passwd;
	setHost( str1, i, str2, passwd );
    }
    break;
    case CMD_CONNECT:
	openConnection( );
	break;
    case CMD_DISCONNECT:
	closeConnection( );
	break;
    case CMD_SLAVE_STATUS:
        slave_status();
        break;
    case CMD_GET: {
        Q_INT8 iReload;

        stream >> str1 >> str2 >> iReload;

        bool reload = (iReload != 0);	

	get( str1, str2, reload );
    }
    break;
    case CMD_PUT: {
	int permissions;
	Q_INT8 iOverwrite, iResume;
	
	stream >> iOverwrite >> iResume >> permissions >> str1;
	
	bool overwrite = ( iOverwrite != 0 );
	bool resume = ( iResume != 0 );
	
	put( str1, permissions, overwrite, resume);
    }
    break;
    case CMD_STAT:
	stream >> str1;
	stat( str1 );
	break;
    case CMD_MIMETYPE:
	stream >> str1;
	mimetype( str1 );
	break;
    case CMD_LISTDIR:
	stream >> str1;
	listDir( str1 );
	break;
    case CMD_MKDIR:
	stream >> str1 >> i;
	mkdir( str1, i );
	break;
    case CMD_RENAME:{
	Q_INT8 iOverwrite;
        stream >> str1 >> str2 >> iOverwrite;
        bool overwrite = (iOverwrite != 0);
        rename( str1, str2, overwrite );
    }
    break;
    case CMD_COPY: {
        int permissions;
        Q_INT8 iOverwrite;
        stream >> str1 >> str2 >> permissions >> iOverwrite;
        bool overwrite = (iOverwrite != 0);
        copy( str1, str2, permissions, overwrite );
    }
    break;
    case CMD_DEL:
        Q_INT8 isFile;
        stream >> str1 >> isFile;
	del( str1, isFile != 0);
	break;
    case CMD_CHMOD:
        stream >> str1 >> i;
	chmod( str1, i);
	break;
    case CMD_SPECIAL:
	special( data );
	break;
    default:
	assert( 0 );
    }
}

