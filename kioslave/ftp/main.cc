// $Id$

#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
#include <signal.h>

#include <kio_rename_dlg.h>
#include <kio_skip_dlg.h>
#include <kurl.h>
#include <kprotocolmanager.h>
#include <kinstance.h>
#include <kdebug.h>
#include <assert.h>
#include <iostream.h>

#include "main.h"

int check( KIOConnection *_con );

extern "C" {
	void sigalrm_handler(int);
}

int main( int, char ** )
{
  signal(SIGCHLD, KIOProtocol::sigchld_handler);
  signal(SIGSEGV, KIOProtocol::sigsegv_handler);

  //  KProtocolManager manager;

  KInstance instance( "kio_ftp" );

  kDebugInfo( 7102, "Starting");

  KIOConnection parent( 0, 1 );

  FtpProtocol ftp( &parent );
  ftp.dispatchLoop();

  kDebugInfo( 7102, "Done");
}

/*
 * We'll use an alarm that will set this flag when transfer has timed out
 */
char sigbreak = 0;

void sigalrm_handler(int)
{
  sigbreak = 1;
}

void setup_alarm(unsigned int timeout)
{
  sigbreak = 0;
  alarm(timeout);
  signal(SIGALRM, sigalrm_handler);
}


FtpProtocol::FtpProtocol( KIOConnection *_conn ) : KIOProtocol( _conn )
{
  m_cmd = CMD_NONE;
  m_bIgnoreJobErrors = false;
  m_bCanResume = true; // most of ftp servers support resuming ?
}


void FtpProtocol::slotMkdir( const char *_url, int _mode )
{
  KURL usrc( _url );
  if ( usrc.isMalformed() ) {
    error( ERR_MALFORMED_URL, strdup(_url) );
    m_cmd = CMD_NONE;
    return;
  }

  if ( strcmp( usrc.protocol(), "ftp" ) != 0L ) {
    error( ERR_INTERNAL, "kio_ftp got non ftp file in mkdir command" );
    m_cmd = CMD_NONE;
    return;
  }

  // Connect to the ftp server
  if ( !ftp.isConnected() )
    if ( !ftp.ftpConnect( usrc ) ) {
	error( ftp.error(), ftp.errorText() );
	ftp.ftpDisconnect( true );
	m_cmd = CMD_NONE;
	return;
      }

  FtpEntry* e = ftp.ftpStat( usrc, false );

  if ( !e  ) {
    if ( !ftp.ftpMkdir( usrc.path() ) ) {
	error( ERR_COULD_NOT_MKDIR, strdup(_url) );
	ftp.ftpDisconnect();
	m_cmd = CMD_NONE;
	return;
      } else {
	// set the desired attributes for dir
	if ( _mode != -1 )
	  if ( !ftp.ftpChmod( usrc.path(), _mode ) ) {
	      error( ERR_CANNOT_CHMOD, strdup(_url) );
	      ftp.ftpDisconnect();
	      m_cmd = CMD_NONE;
	      return;
	    }

	finished();
	ftp.ftpDisconnect();
	return;
      }
  }

  if ( S_ISDIR( e->type ) )  { // !!! ok ?
    error( ERR_DOES_ALREADY_EXIST, strdup(_url) );
    ftp.ftpDisconnect();
    m_cmd = CMD_NONE;
    return;
  }

  error( ERR_COULD_NOT_MKDIR, strdup(_url) );
  ftp.ftpDisconnect();
  m_cmd = CMD_NONE;
  return;
}


void FtpProtocol::slotCopy( QStringList& _source, const char *_dest )
{
  doCopy( _source, _dest, false );
}


void FtpProtocol::slotCopy( const char* _source, const char *_dest )
{
  QStringList lst;
  lst.append( _source );

  doCopy( lst, _dest, true );
}


void FtpProtocol::slotMove( QStringList& _source, const char *_dest )
{
  doCopy( _source, _dest, false, true );
}


void FtpProtocol::slotMove( const char* _source, const char *_dest )
{
  QStringList lst;
  lst.append( _source );

  doCopy( lst, _dest, true, true );
}


void FtpProtocol::doCopy( QStringList& _source, const char *_dest,
			  bool _rename, bool _move )
{
  if ( _rename )
    assert( _source.count() == 1 );

  kDebugInfo( 7102, "Making copy to %s", _dest );

  // Check whether the URLs are wellformed
  QStringList::Iterator soit = _source.begin();
  for( ; soit != _source.end(); ++soit ) {
    kDebugInfo( 7102, "Checking %s", (*soit).ascii() );
    KURL usrc( *soit );
    if ( usrc.isMalformed() ) {
      error( ERR_MALFORMED_URL, (*soit) );
      m_cmd = CMD_NONE;
      return;
    }
    if ( usrc.host().isEmpty() ) {
      error( ERR_MALFORMED_URL, (*soit) ); // !!! create a new error type ?
      m_cmd = CMD_NONE;
      return;
    }
    if ( strcmp( usrc.protocol(), "ftp" ) != 0L ) {
      error( ERR_INTERNAL, "kio_ftp got non ftp file as source in copy command" );
      m_cmd = CMD_NONE;
      return;
    }
  }

  kDebugInfo( 7102, "All URLs ok %s", _dest );

  // Make a copy of the parameter. if we do IPC calls from here, then we overwrite
  // our argument. This is tricky! ( but saves memory and speeds things up )
  QString dest = _dest;

  // Check wellformedness of the destination
  KURL udest( dest );
  if ( udest.isMalformed() ) {
    error( ERR_MALFORMED_URL, dest );
    m_cmd = CMD_NONE;
    return;
  }

  kDebugInfo( 7102, "Dest ok %s", dest.ascii() );

  // Find IO server for destination
  QString exec = KProtocolManager::self().executable( udest.protocol() );

  if ( exec.isEmpty() ) {
    error( ERR_UNSUPPORTED_PROTOCOL, udest.protocol() );
    m_cmd = CMD_NONE;
    return;
  }

  // Is the right most protocol a filesystem protocol ?
  if ( KProtocolManager::self().outputType( udest.protocol() ) != KProtocolManager::T_FILESYSTEM )
  {
    error( ERR_PROTOCOL_IS_NOT_A_FILESYSTEM, udest.protocol() );
    m_cmd = CMD_NONE;
    return;
  }

  kDebugInfo( 7102, "IO server ok %s", dest.ascii() );

  // Connect to the ftp server
  KURL usrc( _source.first() );
  if ( ! ftp.ftpConnect( usrc ) ) {
    error( ftp.error(), ftp.errorText() );
    ftp.ftpDisconnect( true );
    m_cmd = CMD_NONE;
    return;
  }

  kDebugInfo( 7102, "connected to a server" );

  // Find out, whether we are logged anonymously or not
  // Authorization has been already checked with ftp.ftpConnect( usrc )
  bool b_user = false;
  if ( usrc.hasUser() ) {
    b_user = true;
  }

  // Get a list of all source files and directories
  QValueList<Copy> files;
  QValueList<CopyDir> dirs;
  int size = 0;
  kDebugInfo( 7102, "Iterating" );

  soit = _source.begin();
  kDebugInfo( 7102, "Looping" );
  for( ; soit != _source.end(); ++soit ) {
    kDebugInfo( 7102, "Executing %s", (*soit).ascii() );
    KURL usrc( *soit );
    kDebugInfo( 7102, "Parsed URL" );
    // Did an error occur ?
    int s;
    if ( ( s = listRecursive( usrc, files, dirs, _rename ) ) == -1 ) {
      ftp.ftpDisconnect();
      // Error message is already sent
      m_cmd = CMD_NONE;
      return;
    }
    // Sum up the total amount of bytes we have to copy
    size += s;
  }

  kDebugInfo( 7102, "Recursive 1 %s", dest.ascii() );

  /*
  // Check whether we do not copy a directory in itself or one of its subdirectories
  struct stat buff2;
  if ( udest.isLocalFile() && stat( udest.path(), &buff2 ) == 0 ) {
    bool b_error = false;
    for( soit = _source.begin(); soit != _source.end(); ++soit ) {
      KURL usrc( *soit );

      struct stat buff1;
      // Can we stat both the source, too ? ( Should always be the case )
      if ( stat( usrc.path(), &buff1 ) == 0 ) {
	bool b_error = false;
	// Are source and dest equal ? => error
	if ( buff1.st_ino == buff2.st_ino )
	  b_error = true;
      }
    }

    if ( !b_error ) {
      // Iterate over all subdirectories
      QValueList<CopyDir>::iterator it = dirs.begin();
      for( ; it != dirs.end() && !b_error; it++ )
	if ( buff2.st_ino == (*it).m_ino )
	  b_error = true;
    }

    // Do we have a cylic copy now ? => error
    if ( b_error ) {
      error( ERR_CYCLIC_COPY, *soit );
      m_cmd = CMD_NONE;
      return;
    }
  }
  */

  kDebugInfo( 7102, "Recursive ok %s", dest.ascii() );

  m_cmd = CMD_GET;

  // Start a server for the destination protocol
  KIOSlave slave( exec );
  if ( !slave.isRunning() ) {
    error( ERR_CANNOT_LAUNCH_PROCESS, exec );
    ftp.ftpDisconnect( true );
    m_cmd = CMD_NONE;
    return;
  }

  // Put a protocol on top of the job
  FtpIOJob job( &slave, this );

  kDebugInfo( 7102, "Job started ok %s", dest.ascii() );

  // Tell our client what we 'r' gonna do
  totalSize( size );
  totalFiles( files.count() );
  totalDirs( dirs.count() );

  int processed_files = 0;
  int processed_dirs = 0;
  int processed_size = 0;

  // Replace the relative destinations with absolut destinations
  // by prepending the destinations path
  QString tmp1 = udest.path( 1 );
  // Strip '/'
  QString tmp1_stripped = udest.path( -1 );

  QValueList<CopyDir>::Iterator dit = dirs.begin();
  for( ; dit != dirs.end(); dit++ ) {
    QString tmp2 = (*dit).m_strRelDest;
    if ( _rename )
      (*dit).m_strRelDest = tmp1_stripped;
    else
      (*dit).m_strRelDest = tmp1;
    (*dit).m_strRelDest += tmp2;
  }
  QValueList<Copy>::Iterator fit = files.begin();
  for( ; fit != files.end(); fit++ ) {
    QString tmp2 = (*fit).m_strRelDest;
    if ( _rename ) // !!! && (*fit).m_strRelDest == "" )
      (*fit).m_strRelDest = tmp1_stripped;
    else
      (*fit).m_strRelDest = tmp1;
    (*fit).m_strRelDest += tmp2;
  }

  kDebugInfo( 7102, "Destinations ok %s", debugString(dest) );

  /*****
   * Make directories
   *****/

  m_bIgnoreJobErrors = true;
  bool overwrite_all = false;
  bool auto_skip = false;
  bool resume_all = false;
  QStringList skip_list, overwrite_list;

  // Create all directories
  dit = dirs.begin();
  for( ; dit != dirs.end(); dit++ ) {
    // Repeat until we got no error
    do {
      job.clearError();

      KURL ud( udest );
      ud.setPath( (*dit).m_strRelDest);
      QString d = ud.url();

      // Is this URL on the skip list ?
      bool skip = false;
      QStringList::Iterator sit = skip_list.begin();
      for( ; sit != skip_list.end() && !skip; sit++ )
	// Is d a subdirectory of *sit ?
	if ( *sit==d )
	  skip = true;

      if ( skip )
	continue;

      // Is this URL on the overwrite list ?
      bool overwrite = false;
      QStringList::Iterator oit = overwrite_list.begin();
      for( ; oit != overwrite_list.end() && !overwrite; oit++ )
	if ( *oit == d )
	  overwrite = true;

      if ( overwrite )
	continue;

      // Tell what we are doing
      makingDir( d.latin1() );

      // kDebugInfo( 7102, "Making remote dir %s", d );
      // Create the directory
      job.mkdir( d.latin1(), (*dit).m_access );
      while( !job.hasFinished() )
	job.dispatch();

      // Did we have an error ?
      if ( job.hasError() ) {
	// Can we prompt the user and ask for a solution ?
	if ( /* m_bGUI && */ job.errorId() == ERR_DOES_ALREADY_EXIST ) {
	  QString old_path = udest.path( 1 );
	  QString old_url = udest.url( 1 );
	  // Should we skip automatically ?
	  if ( auto_skip ) {
	    job.clearError();
	    // We dont want to copy files in this directory, so we put it on the skip list.
	    skip_list.append( old_url );
	    continue;
	  } else if ( overwrite_all ) {
	    job.clearError();
	    continue;
	  }

	  /* RenameDlg_Mode m = (RenameDlg_Mode)( M_SINGLE | M_OVERWRITE );
	  if ( dirs.count() > 1 )
	    m = (RenameDlg_Mode)(M_MULTI | M_SKIP | M_OVERWRITE ); */
	  RenameDlg_Mode m = (RenameDlg_Mode)( M_MULTI | M_SKIP | M_OVERWRITE );
	  QString tmp2 = udest.url(), n;
	  bool sn = (*dit).m_mtime >= ftp.ftpStat( usrc, true )->date;

	  RenameDlg_Result r = open_RenameDlg( (*dit).m_strAbsSource.path(), tmp2,
					       m, sn, n );
	  if ( r == R_CANCEL ) {
	    ftp.ftpDisconnect();
	    error( ERR_USER_CANCELED, "" );
	    m_cmd = CMD_NONE;
	    return;
	  } else if ( r == R_RENAME ) {
	    KURL u( n );
	    // The Dialog should have checked this.
	    if ( u.isMalformed() )
	      assert( 0 );
	    // The new path with trailing '/'
	    QString tmp3 = u.path( 1 );
	    ///////
	    // Replace old path with tmp3
	    ///////
	    QValueList<CopyDir>::Iterator dit2 = dit;
	    // Change the current one and strip the trailing '/'
	    (*dit2).m_strRelDest = u.path( -1 );
	    // Change the name of all subdirectories
	    dit2++;
	    for( ; dit2 != dirs.end(); dit2++ )
	      if ( strncmp((*dit2).m_strRelDest, old_path, old_path.length())==0 )
		(*dit2).m_strRelDest.replace( 0, old_path.length(), tmp3 );
	    // Change all filenames
	    QValueList<Copy>::Iterator fit2 = files.begin();
	    for( ; fit2 != files.end(); fit2++ )
	      if ( strncmp( (*fit2).m_strRelDest, old_path, old_path.length() ) == 0 )
		(*fit2).m_strRelDest.replace( 0, old_path.length(), tmp3 );
	    // Dont clear error => we will repeat the current command
	  } else if ( r == R_SKIP ) {
	    // Skip all files and directories that start with 'old_url'
	    skip_list.append( old_url );
	    // Clear the error => The current command is not repeated => skipped
	    job.clearError();
	  } else if ( r == R_AUTO_SKIP ) {
	    // Skip all files and directories that start with 'old_url'
	    skip_list.append( old_url );
	    // Clear the error => The current command is not repeated => skipped
	    job.clearError();
	    auto_skip = true;
	  } else if ( r == R_OVERWRITE ) {
	    // Dont bother for subdirectories
	    overwrite_list.append( old_url );
	    // Clear the error => The current command is not repeated => we will
	    // overwrite every file in this directory or any of its subdirectories
	    job.clearError();
	  } else if ( r == R_OVERWRITE_ALL ) {
	    job.clearError();
	    overwrite_all = true;
	  } else
	    assert( 0 );
	}
	// No need to ask the user, so raise an error
	else {
	  error( job.errorId(), job.errorText() );
	  ftp.ftpDisconnect();
	  m_cmd = CMD_NONE;
	  return;
	}
      }
    }
    while( job.hasError() );

    processedDirs( ++processed_dirs );
  }

  kDebugInfo( 7102, "Created directories %s", debugString(dest) );


  /*****
   * Server check - supports resuming ?
   *****/

  if ( !ftp.ftpResume( 0 ) )
    m_bCanResume = false;

  /*****
   * Copy files
   *****/

  time_t t_start = time( 0L );
  time_t t_last = t_start;

  fit = files.begin();
  for( ; fit != files.end(); fit++ ) {

    bool overwrite = false;
    bool skip_copying = false;
    bool resume = false;
    unsigned long offset = 0;

    // Repeat until we got no error
    do
    {
      job.clearError();

      KURL ud( dest );
      ud.setPath( (*fit).m_strRelDest);
      QString d = ud.url();

      // Is this URL on the skip list ?
      bool skip = false;
      QStringList::Iterator sit = skip_list.begin();
      for( ; sit != skip_list.end() && !skip; sit++ )
	// Is 'd' a file in directory '*sit' or one of its subdirectories ?
	if ( *sit==d )
	  skip = true;

      if ( skip )
	continue;

      // this will emit sigCanResume( m_bCanResume )
      canResume( m_bCanResume );

      KURL realpath ( (*fit).m_strAbsSource );
      copyingFile( realpath.url().ascii(), d.ascii() );

      // kDebugInfo( 7102, "Writing to %s", d );

      // Is this URL on the overwrite list ?
      QStringList::Iterator oit = overwrite_list.begin();
      for( ; oit != overwrite_list.end() && !overwrite; oit++ )
	if (*oit==d)
	  overwrite = true;

      // implicitly set permissions rw-r--r-- for anonymous ftp
      int md = -1;
      // but when it's not anonymous ftp, set permissions as in original source
      if ( b_user )
	md = (*fit).m_access;

      job.put( d, md, overwrite_all || overwrite,
               resume_all || resume, (*fit).m_size );

      while( !job.isReady() && !job.hasFinished() )
	job.dispatch();

      // Did we have an error ?
      if ( job.hasError() ) {
	int currentError = job.errorId();

	kDebugError( 7102, "################# COULD NOT PUT %d",currentError);
	// if ( /* m_bGUI && */ job.errorId() == ERR_WRITE_ACCESS_DENIED )
	if ( /* m_bGUI && */  currentError != ERR_DOES_ALREADY_EXIST &&
			      currentError != ERR_DOES_ALREADY_EXIST_FULL )
	{
	  // Should we skip automatically ?
	  if ( auto_skip ) {
	    job.clearError();
	    skip_copying = true;
	    continue;
	  }
	  QString tmp2 = ud.url();
	  SkipDlg_Result r;
	  r = open_SkipDlg( tmp2, ( files.count() > 1 ) );
	  if ( r == S_CANCEL ) {
	    error( ERR_USER_CANCELED, "" );
	    ftp.ftpDisconnect();
	    m_cmd = CMD_NONE;
	    return;
	  } else if ( r == S_SKIP ) {
	    // Clear the error => The current command is not repeated => skipped
	    job.clearError();
	    skip_copying = true;
	    continue;
	  } else if ( r == S_AUTO_SKIP ) {
	    // Clear the error => The current command is not repeated => skipped
	    job.clearError();
	    skip_copying = true;
	    continue;
	  } else
	    assert( 0 );
	}
	// Can we prompt the user and ask for a solution ?
	else if ( /* m_bGUI && */ currentError == ERR_DOES_ALREADY_EXIST ||
		  currentError == ERR_DOES_ALREADY_EXIST_FULL )
	{
	  // Should we skip automatically ?
	  if ( auto_skip ) {
	    job.clearError();
	    continue;
	  }

	  RenameDlg_Result r;
	  QString n;

	  if ( KProtocolManager::self().autoResume() && m_bCanResume &&
		    currentError != ERR_DOES_ALREADY_EXIST_FULL ) {
	    r = R_RESUME_ALL;
	  } else {
	    RenameDlg_Mode m;

	    // ask for resume only if transfer can be resumed and if it is not
	    // already fully downloaded
	    if ( files.count() > 1 ){
	      if ( m_bCanResume && currentError != ERR_DOES_ALREADY_EXIST_FULL )
		m = (RenameDlg_Mode)(M_MULTI | M_SKIP | M_OVERWRITE | M_RESUME);
	      else
		m = (RenameDlg_Mode)(M_MULTI | M_SKIP | M_OVERWRITE);
	    } else {
	      if ( m_bCanResume && currentError != ERR_DOES_ALREADY_EXIST_FULL )
		m = (RenameDlg_Mode)( M_SINGLE | M_OVERWRITE | M_RESUME);
	      else
		m = (RenameDlg_Mode)( M_SINGLE | M_OVERWRITE);
	    }

	    QString tmp2 = ud.url();
	    bool sn = (*dit).m_mtime >= ftp.ftpStat( usrc, true )->date;
	    r = open_RenameDlg( (*fit).m_strAbsSource.path(), tmp2, m, sn, n );
	  }

	  if ( r == R_CANCEL ) {
	    error( ERR_USER_CANCELED, "" );
	    ftp.ftpDisconnect();
	    m_cmd = CMD_NONE;
	    return;
	  } else if ( r == R_RENAME ) {
	    KURL u( n );
	    // The Dialog should have checked this.
	    if ( u.isMalformed() )
	      assert( 0 );

	    renamed( u.path( -1 ) ); // emit signal

	    // Change the destination name of the current file
	    (*fit).m_strRelDest = u.path( -1 );
	    // Dont clear error => we will repeat the current command
	  } else if ( r == R_SKIP ) {
	    // Clear the error => The current command is not repeated => skipped
	    job.clearError();
	  } else if ( r == R_AUTO_SKIP ) {
	    // Clear the error => The current command is not repeated => skipped
	    job.clearError();
	    auto_skip = true;
	  } else if ( r == R_OVERWRITE ) {
	    overwrite = true;
	    // Dont clear error => we will repeat the current command
	  } else if ( r == R_OVERWRITE_ALL ) {
	    overwrite_all = true;
	    // Dont clear error => we will repeat the current command
	  } else if ( r == R_RESUME ) {
	    resume = true;
	    offset = getOffset( ud.url() );
	    // Dont clear error => we will repeat the current command
	  } else if ( r == R_RESUME_ALL ) {
	    resume_all = true;
	    offset = getOffset( ud.url() );
	    // Dont clear error => we will repeat the current command
	  } else
	    assert( 0 );
	}
	// No need to ask the user, so raise an error
	else {
	  error( currentError, job.errorText() );
	  ftp.ftpDisconnect();
	  m_cmd = CMD_NONE;
	  return;
	}
      }
    }
    while( job.hasError() );

    if ( skip_copying )
      continue;

    // add the offset to processed size
    if ( offset > 0 ) {
      processed_size += offset;
      kDebugInfo( 7102, "Offset = %ld", offset );
    }

    KURL tmpurl ( (*fit).m_strAbsSource );

    kDebugInfo( 7102, "Opening %s", tmpurl.url().ascii() );

    if ( !ftp.ftpOpen( tmpurl, Ftp::READ, offset ) ) {
      error( ftp.error(), ftp.errorText() );
      ftp.ftpDisconnect();
      m_cmd = CMD_NONE;
      return;
    }

    // we want to know the processed size even before we read something
    // especially in case we overwrite file => this cleans size to zero
    processedSize( processed_size );

    char buffer[ 4096 ];
    int n;
    int read_size = 0; // this variable is needed for a correct speed calculation
    do {
//       setup_alarm( KProtocolManager::self().readTimeout() ); // start timeout
      n = ftp.read( buffer, 2048 );

      // !!! slow down loop for local testing
      // for ( int tmpi = 0; tmpi < 1000000; tmpi++ ) ;

      job.data( buffer, n );
      processed_size += n;
      read_size += n;

      time_t t = time( 0L );
      if ( t - t_last >= 1 ) {
	processedSize( processed_size );
	speed( read_size / ( t - t_start ) );
	t_last = t;
      }

      // Check parent
      while ( check( connection() ) ) {
	dispatch();
      }

      // Check for error messages from slave
      while ( check( &slave ) ) {
	job.dispatch();
      }

      // An error ?
      if ( job.hasFinished() ) {
	ftp.ftpDisconnect();
	ftp.ftpClose();
	finished();
	m_cmd = CMD_NONE;
	return;
      }
    } while ( n > 0 );

    // check whether file was fully copied
    int saved_size = read_size + offset;
    if ( saved_size< (*fit).m_size ) {
      kDebugInfo( 7102, "Sizes don't match." );
      ftp.ftpClose();
      error( ERR_UNKNOWN_INTERRUPT, (*fit).m_strRelDest ); // correct URL for message ?
      m_cmd = CMD_NONE;
      return;
    }

    job.dataEnd();

    ftp.ftpClose();

    while( !job.hasFinished() ) {
      job.dispatch();
    }

    time_t t = time( 0L );

    processedSize( processed_size );
    if ( t - t_start >= 1 ) {
      speed( read_size / ( t - t_start ) );
      t_last = t;
    }
    processedFiles( ++processed_files );
  }

  kDebugInfo( 7102, "Copied files %s", debugString(dest) );

  // slotDel() handles disconnecting by itself
  if ( _move ) {
    slotDel( _source );
  } else
    ftp.ftpDisconnect();

  finished();
  m_cmd = CMD_NONE;
}


void FtpProtocol::slotGet( const char *_url )
{

  kDebugInfo( 7102, "slotGet( %s )", _url );

  KURL usrc( _url );
  if ( usrc.isMalformed() ) {
    error( ERR_MALFORMED_URL, strdup(_url) );
    m_cmd = CMD_NONE;
    return;
  }

  if ( strcmp( usrc.protocol(), "ftp" ) != 0L ) {
    error( ERR_INTERNAL, "kio_ftp got non ftp file in get command" );
    m_cmd = CMD_NONE;
    return;
  }

  if ( !ftp.ftpConnect( usrc ) ) {
    error( ftp.error(), ftp.errorText() );
    ftp.ftpDisconnect( true );
    m_cmd = CMD_NONE;
    return;
  }

  FtpEntry *e = ftp.ftpStat( usrc, true );
  if ( !e ) {
    error( ERR_DOES_NOT_EXIST, strdup(_url) );
    m_cmd = CMD_NONE;
    return;
  }

  // Why is this connected ? (David)
/*  if ( S_ISDIR( e->type ) ) {
    error( ERR_IS_DIRECTORY, strdup(_url) );
    m_cmd = CMD_NONE;
    return;
  }*/

  m_cmd = CMD_GET;

  if ( !ftp.open( usrc, Ftp::READ ) ) {
    error( ERR_CANNOT_OPEN_FOR_READING, strdup(_url) );
    m_cmd = CMD_NONE;
    return;
  }

  ready();

  gettingFile( _url );

  totalSize( e->size );
  int processed_size = 0;
  time_t t_start = time( 0L );
  time_t t_last = t_start;

  char buffer[ 2048 ];
  while( !ftp.atEOF() ) {
    int n = ftp.read( buffer, 2048 );
    data( buffer, n );

    processed_size += n;
    time_t t = time( 0L );
    if ( t - t_last >= 1 ) {
      processedSize( processed_size );
      speed( processed_size / ( t - t_start ) );
      t_last = t;
    }
  }

  dataEnd();

  ftp.ftpClose();

  processedSize( e->size );
  time_t t = time( 0L );
  if ( t - t_start >= 1 )
    speed( processed_size / ( t - t_start ) );

  finished();
  m_cmd = CMD_NONE;
}


void FtpProtocol::slotGetSize( const char* _url ) {

  m_cmd = CMD_GET_SIZE;

  // Check whether URL is wellformed
  KURL usrc( _url );
  if ( usrc.isMalformed() ) {
      error( ERR_MALFORMED_URL, _url );
      m_cmd = CMD_NONE;
      return;
    }
  if ( strcmp( usrc.protocol(), "ftp" ) != 0L )
    {
      error( ERR_INTERNAL, "kio_ftp got non ftp file in getSize command" );
      m_cmd = CMD_NONE;
      return;
    }

  kDebugInfo( 7102, "URL is ok " );

  if ( !ftp.ftpConnect( usrc ) )
  {
    error( ftp.error(), ftp.errorText() );
    ftp.ftpDisconnect( true );
    m_cmd = CMD_NONE;
    return;
  }

  // Get a list of all source files and directories
  QValueList<Copy> files;
  QValueList<CopyDir> dirs;

  kDebugInfo( 7102, "Executing %s", _url );
  // Did an error occur ?
  int s;
  if ( ( s = listRecursive( usrc, files, dirs, false ) ) == -1 )
    {
      ftp.ftpDisconnect();
      // Error message is already sent
      m_cmd = CMD_NONE;
      return;
    }

  // Tell our client what we 'r' gonna do
  totalSize( s );

  ftp.ftpDisconnect();

  finished();
  m_cmd = CMD_NONE;
}


void FtpProtocol::slotPut( const char *_url, int _mode, bool _overwrite, bool _resume, int _size )
{
  QString url_orig = _url;
  QString url_part = url_orig + ".part";

  KURL udest_orig( url_orig );
  KURL udest_part( url_part );

  bool m_bMarkPartial = KProtocolManager::self().markPartial();

  if ( udest_orig.isMalformed() ) {
    error( ERR_MALFORMED_URL, url_orig );
    m_cmd = CMD_NONE;
    finished();
    return;
  }

  if ( strcmp( udest_orig.protocol(), "ftp" ) != 0L ) {
    error(ERR_INTERNAL,"kio_ftp got non ftp file as as destination in put command" );
    m_cmd = CMD_NONE;
    finished();
    return;
  }

  // Connect to the ftp server, only if we are not connected
  // this prevents connecting twice in recursive copying ( mkdir connects first time )
  if ( !ftp.isConnected() )
    if ( !ftp.ftpConnect( udest_orig ) )
      {
	error( ftp.error(), ftp.errorText() );
	ftp.ftpDisconnect( true );
	m_cmd = CMD_NONE;
	finished();
	return;
      }

  m_cmd = CMD_PUT;

  // Find out, whether we are logged anonymously or not
  // Authorization has been already checked with ftp.ftpConnect( usrc )
  bool b_user = false;
  if ( udest_orig.hasUser() )
    b_user = true;

  FtpEntry* e;

  if ( ( e = ftp.ftpStat( udest_orig, true ) ) ) { // original file exists
    if ( e->size == 0 ) {  // delete files with zero size
      ftp.ftpDelete( udest_orig.path() );
    } else if ( !_overwrite && !_resume ) {
      if ( e->size == _size ) {
	error( ERR_DOES_ALREADY_EXIST_FULL, udest_orig.path() );
      } else {
	error( ERR_DOES_ALREADY_EXIST, udest_orig.path() );
      }
      ftp.ftpDisconnect( true );
      m_cmd = CMD_NONE;
      finished();
      return;
    } else if ( m_bMarkPartial ) { // when using mark partial, append .part extension
      ftp.ftpRename( udest_orig.path(), udest_part.path() );
    }
  } else if ( ( e = ftp.ftpStat( udest_part, true ) ) ) { // file with extension .part exists
    if ( e->size == 0 ) {  // delete files with zero size
      ftp.ftpDelete( udest_part.path() );
    } else if ( !_overwrite && !_resume ) {
      if ( e->size == _size ) {
	error( ERR_DOES_ALREADY_EXIST_FULL, udest_orig.path() );
      } else {
	error( ERR_DOES_ALREADY_EXIST, udest_orig.path() );
      }

      ftp.ftpDisconnect( true );
      m_cmd = CMD_NONE;
      finished();
      return;
    } else if ( ! m_bMarkPartial ) { // when using mark partial, remove .part extension
      ftp.ftpRename( udest_part.path(), udest_orig.path() );
    }

  }

  KURL udest;

  // if we are using marking of partial downloads -> add .part extension
  if ( m_bMarkPartial ) {
    kDebugInfo( 7102, "Adding .part extension to %s", udest_orig.path().ascii() );
    udest = udest_part;
  } else
  udest = udest_orig;


  /* if ( access( udest.path(), W_OK ) == -1 )
  {
    kDebugError( 7102, "Write Access denied for '%s' %d",udest.path(), errno );

    error( ERR_WRITE_ACCESS_DENIED, url );
    m_cmd = CMD_NONE;
    finished();
    return;
  } */

  unsigned long offset = 0;

  // set the mode according to offset
  if ( _resume ) {
    offset = e->size;
    kDebugInfo( 7102, "Offset = %ld", offset );
  }

  if ( !ftp.ftpOpen( udest, Ftp::WRITE, offset ) ) {
    kDebugError( 7102, "########## COULD NOT WRITE %s", udest.path().ascii() );

    error( ftp.error(), ftp.errorText() );
    ftp.ftpDisconnect( true );
    m_cmd = CMD_NONE;
    finished();
    return;
  }

  // We are ready for receiving data
  ready();

  // Loop until we got 'dataEnd'
  while ( m_cmd == CMD_PUT && dispatch() );

  ftp.ftpClose(); // check order !!!
  ftp.ftpDisconnect( true );

  if ( (e = ftp.ftpStat( udest, true )) ) {
    if ( e->size == _size ) {
      // after full download rename the file back to original name
      if ( m_bMarkPartial ) {
	cerr << "kio_ftp renaming\n";
	if ( !ftp.ftpRename( udest.path(), udest_orig.path() ) )
	  {
	    error( ERR_CANNOT_RENAME, udest_orig.path() );
	    ftp.ftpDisconnect( true );
	    m_cmd = CMD_NONE;
	    finished();
	    return;
	  }
      }

      // do chmod only after full download
      if ( _mode != -1 ) {
	cerr << "kio_ftp chmoding\n";
	// set the desired attributes for dir  !!! how ?
	if ( !ftp.ftpChmod( udest_orig.path(), _mode ) )
	  {
	    error( ERR_CANNOT_CHMOD, udest_orig.path() );
	    ftp.ftpDisconnect( true );
	    m_cmd = CMD_NONE;
	    finished();
	    return;
	  }
      }
    } else if ( e->size < KProtocolManager::self().minimumKeepSize() ) {
      ftp.ftpDelete( udest.path() ); // if the size is less then minimum -> delete the file
    }
  }

  ftp.ftpDisconnect( true );

  // We have done our job => finish
  finished();
  m_cmd = CMD_NONE;
}


void FtpProtocol::slotDel( QStringList& _source )
{
  // Check whether the URLs are wellformed
  QStringList::Iterator soit = _source.begin();
  for( ; soit != _source.end(); ++soit ) {
    kDebugInfo( 7102, "Checking %s", (*soit).ascii() );
    KURL usrc( *soit );

    if ( usrc.isMalformed() ) {
      error( ERR_MALFORMED_URL, *soit);
      m_cmd = CMD_NONE;
      return;
    }

    if ( strcmp( usrc.protocol(), "ftp" ) != 0L ) {
      error(ERR_INTERNAL,"kio_ftp got non ftp file in delete command" );
      m_cmd = CMD_NONE;
      return;
    }
  }

  kDebugInfo( 7102, "All URLs ok" );

  // Get a list of all source files and directories
  QValueList<Copy> fs;
  QValueList<CopyDir> ds;
  int size = 0;
  kDebugInfo( 7102, "Iterating" );

  soit = _source.begin();
  kDebugInfo( 7102, "Looping" );
  for( ; soit != _source.end(); ++soit ) {
    kDebugInfo( 7102, "Executing %s", (*soit).ascii() );
    KURL usrc(*soit);
    kDebugInfo( 7102, "Parsed URL" );

    // Did an error occur ?
    int s;
    if ( ( s = listRecursive( usrc, fs, ds, false ) ) == -1 )
      {
 	// Error message is already sent
 	ftp.ftpDisconnect();
 	m_cmd = CMD_NONE;
 	return;
      }
    // Sum up the total amount of bytes we have to delete
    size += s;
  }

  kDebugInfo( 7102, "Recursive ok" );

  if ( fs.count() == 1 ) {
    m_cmd = CMD_DEL;
  } else {
    m_cmd = CMD_MDEL;
  }

  // Tell our client what we 'r' gonna do
  totalSize( size );
  totalFiles( fs.count() );
  totalDirs( ds.count() );

  /*****
   * Delete files
   *****/

  QValueList<Copy>::Iterator fit = fs.begin();
  for( ; fit != fs.end(); fit++ ) {

    QString filename = (*fit).m_strAbsSource.path();
    kDebugInfo( 7102, "Deleting file %s", filename.ascii() );

    deletingFile( filename );

    if ( !ftp.ftpDelete( filename ) )
      {
	error( ERR_CANNOT_DELETE, filename );
	ftp.ftpDisconnect();
	m_cmd = CMD_NONE;
	return;
      }
  }

  /*****
   * Delete empty directories
   *****/

  QValueList<CopyDir>::Iterator dit = ds.begin();
  for( ; dit != ds.end(); dit++ ) {

    QString dirname = (*dit).m_strAbsSource.path();
    kDebugInfo( 7102, "Deleting directory %s", dirname.ascii() );

    deletingFile( dirname );

    if ( !ftp.ftpRmdir( dirname ) ) {
      error( ERR_COULD_NOT_RMDIR, dirname );
      ftp.ftpDisconnect();
      m_cmd = CMD_NONE;
      return;
    }
  }

  finished();

  m_cmd = CMD_NONE;
}


void FtpProtocol::slotData( void *_p, int _len )
{
  switch( m_cmd ) {
    case CMD_PUT:
      ftp.write( _p, _len );
      break;
  }
}


void FtpProtocol::slotDataEnd()
{
  switch( m_cmd ) {
    case CMD_PUT:
      m_cmd = CMD_NONE;
  }
}


long FtpProtocol::listRecursive( const KURL & url, QValueList<Copy>&
				_files, QValueList<CopyDir>& _dirs, bool _rename )
{
  // Should be checked before, but who knows
  if ( url.isMalformed() )
    assert( 0 );
  m_bAutoSkip = false;

  // Check whether we have to copy the complete directory tree beginning by its root.
  if ( !url.hasPath() || url.path()=="/" ) {
    KURL url2 ( url );
    url2.setPath( "/" );
    CopyDir c;
    c.m_strAbsSource = url2;
    if ( _rename )
      c.m_strRelDest = "";
    else
      c.m_strRelDest = "";
    c.m_access = S_IRWXU | S_IRWXO | S_IRWXG;
    c.m_type = S_IFDIR;
    c.m_mtime = ::time(0L);
    _dirs.append( c );

    return listRecursive2( url2, c.m_strRelDest, _files, _dirs );
  }

  kDebugInfo( 7102, "########## RECURSIVE LISTING %s", url.url().ascii() );

  FtpEntry* e = ftp.ftpStat( url, true );
  if ( !e )  {
    error( ERR_DOES_NOT_EXIST, url.url());
    kDebugInfo( 7102, "ftpStat FAILED !" );
    return -1;
  }

  // Is the source not a directory ? => so just copy it and we are done.
  if ( !S_ISDIR( e->type ) ) {
    kDebugInfo( 7102, "not a dir" );
    QString fname;
    if ( _rename )
      fname = "";
    else {
      fname = url.filename();
      // Should be impossible, but who knows ...
      if ( fname.isEmpty() )
	assert( 0 );
    }

    Copy c;
    c.m_strAbsSource = url;
    c.m_strRelDest = fname;
    c.m_access = e->access;
    c.m_type = e->type;
    c.m_mtime = e->date;
    c.m_size = e->size;
    _files.append( c );
    return e->size;
  }

  // The source is a directory. So we have to go into recursion here.
  KURL tmp1(url);
  if ( _rename )
    tmp1.setPath( url.path( 0 ) );
  else {
    tmp1.setPath( url.directory( true ) + "/" );
  }
  QString tmp2;
  if ( _rename )
    tmp2 = "";
  else
    tmp2 = url.filename();
  CopyDir c;
  c.m_strAbsSource = url;
  c.m_strRelDest = tmp2;
  c.m_access = e->access;
  c.m_mtime = e->date;
  c.m_type = e->type;
  _dirs.append( c );
  kDebugInfo( 7102, "########### STARTING RECURSION with %s and %s",tmp1.url().ascii(), tmp2.ascii() );

  return listRecursive2( tmp1, tmp2, _files, _dirs );
}


long FtpProtocol::listRecursive2( const KURL & _abs_path, const char *_rel_path,
				  QValueList<Copy>& _files, QValueList<CopyDir>& _dirs )
{
  long size = 0;

  cerr << "listRecursive2 " << _abs_path.url().ascii() << "  " << _rel_path << endl;
  KURL p ( _abs_path) ;
  p.cd( _rel_path );

  scanningDir( p.path() );

  if ( !ftp.ftpOpenDir( p ) ) {
    if ( m_bAutoSkip )
      return 0;

    SkipDlg_Result result = open_SkipDlg( p.path(), true );
    if ( result == S_CANCEL ) {
      // error( ERR_CANNOT_ENTER_DIRECTORY, p );
      return -1;
    } else if ( result == S_AUTO_SKIP )
      m_bAutoSkip = true;
    return 0;
  }

  QStringList recursion;

  kDebugInfo( 7102, "##Listing" );

  FtpEntry *e;
  while ( ( e = ftp.readdir() ) != 0L ) {
    kDebugInfo( 7102, "#%s", e->name.ascii() );

    if ( e->name == "." || e->name == ".." )
      continue;

    // TODO : avoid reparsing
    QString p2 = p.url();
    p2 += "/";
    p2 += e->name;

    QString tmp = _rel_path;
    tmp += "/";
    tmp += e->name;

    if ( !S_ISDIR( e->type ) ) {
      kDebugInfo( 7102, "Appending '%s' '%s'", p2.ascii(), tmp.ascii() );
      Copy c;
      c.m_strAbsSource = KURL(p2); //
      c.m_strRelDest = tmp;
      c.m_access = e->access;
      c.m_type = e->type;
      c.m_mtime = e->date;
      c.m_size = e->size;
      _files.append( c );
      size += e->size;
    } else {
      CopyDir c;
      c.m_strAbsSource = KURL(p2); //
      c.m_strRelDest = tmp;
      c.m_access = e->access;
      c.m_type = e->type;
      c.m_mtime = e->date;
      _dirs.append( c );

      recursion.append( tmp );
    }
  }

  if ( !ftp.ftpCloseDir() ) {
    // error( ERR_COULD_NOT_CLOSEDIR, p );
    return -1;
  }

  QStringList::Iterator it = recursion.begin();
  for( ; it != recursion.end(); ++it ) {
    long s;
    if ( ( s = listRecursive2( _abs_path, (*it).ascii(), _files, _dirs ) ) == -1 )
      return -1;
    size += s;
  }

  return size;
}


void FtpProtocol::slotListDir( const char *_url )
{
  kDebugInfo( 7102, "=============== LIST %s ===============", _url  );

  KURL usrc( _url );
  if ( usrc.isMalformed() ) {
    error( ERR_MALFORMED_URL, strdup(_url) );
    m_cmd = CMD_NONE;
    return;
  }

  if ( strcmp( usrc.protocol(), "ftp" ) != 0L ) {
    error( ERR_INTERNAL, "kio_ftp got non ftp file in list command" );
    m_cmd = CMD_NONE;
    return;
  }

  /*
  struct stat buff;
  if ( stat( usrc.path(), &buff ) == -1 ) {
    error( ERR_DOES_NOT_EXIST, url);
    m_cmd = CMD_NONE;
    return;
  }

  if ( !S_ISDIR( buff.st_mode ) ) {
    error( ERR_IS_FILE, url );
    m_cmd = CMD_NONE;
    return;
  } */

  m_cmd = CMD_LIST;

  if ( !ftp.opendir( usrc ) ) {
    error( ERR_CANNOT_ENTER_DIRECTORY, strdup(_url) );
    m_cmd = CMD_NONE;
    return;
  }

  FtpEntry* e;
  while ( ( e = ftp.readdir() ) )
  {
    //Return those as well
    //if ( e->name == "." || e->name == ".." )
    //  continue;

    kDebugInfo( 7102, "Listing %s", e->name.ascii() );

    KUDSEntry entry;
    KUDSAtom atom;
    atom.m_uds = UDS_NAME;
    atom.m_str = e->name;
    entry.append( atom );

    atom.m_uds = UDS_FILE_TYPE;
    atom.m_long = e->type;
    entry.append( atom );

    atom.m_uds = UDS_SIZE;
    atom.m_long = e->size;
    entry.append( atom );

    atom.m_uds = UDS_MODIFICATION_TIME;
    atom.m_long = e->date;
    entry.append( atom );

    atom.m_uds = UDS_ACCESS;
    atom.m_long = e->access;
    entry.append( atom );

    atom.m_uds = UDS_USER;
    atom.m_str = e->owner;
    entry.append( atom );

    atom.m_uds = UDS_GROUP;
    atom.m_str = e->group;
    entry.append( atom );

    atom.m_uds = UDS_LINK_DEST;
    atom.m_str = e->link;
    entry.append( atom );

    /* atom.m_uds = UDS_ACCESS_TIME;
    atom.m_long = buff.st_atime;
    entry.append( atom );

    atom.m_uds = UDS_CREATION_TIME;
    atom.m_long = buff.st_ctime;
    entry.append( atom ); */

    listEntry( entry );
  }

  kDebugInfo( 7102, "============= COMPLETED LIST ============" );

  ftp.closedir();

  m_cmd = CMD_NONE;

  finished();

  kDebugInfo( 7102, "=============== BYE ===========" );
}


void FtpProtocol::slotTestDir( const char *_url )
{
  KURL usrc( _url );
  if ( usrc.isMalformed() ) {
    error( ERR_MALFORMED_URL, strdup(_url) );
    m_cmd = CMD_NONE;
    return;
  }

  if ( strcmp( usrc.protocol(), "ftp" ) != 0L ) {
    error( ERR_INTERNAL, "kio_ftp got non ftp file as source in copy command" );
    m_cmd = CMD_NONE;
    return;
  }
  kDebugInfo( 7102, "=============== slotTestDir ==============" );
  FtpEntry* e;
  if ( !( e = ftp.stat( usrc, false ) ) ) {
    kDebugInfo( 7102, " =========== ERROR ========" );
    error( ERR_DOES_NOT_EXIST, strdup(_url) );
    m_cmd = CMD_NONE;
    return;
  }

  if ( S_ISDIR( e->type ) ) {
    kDebugInfo( 7102, "========== DONE DIR ========= %s", _url );
    isDirectory();
  } else {
    kDebugInfo( 7102, "========== DONE FILE ========= %s", _url );
    isFile();
  }

  finished();
}


void FtpProtocol::jobError( int _errid, const char *_txt )
{
  if ( !m_bIgnoreJobErrors )
    error( _errid, _txt );
}


/*************************************
 *
 * FtpIOJob
 *
 *************************************/

FtpIOJob::FtpIOJob( KIOConnection *_conn, FtpProtocol *_Ftp ) : KIOJobBase( _conn )
{
  m_pFtp = _Ftp;
}


void FtpIOJob::slotError( int _errid, const char *_txt )
{
  KIOJobBase::slotError( _errid, _txt );
  m_pFtp->jobError( _errid, _txt );
}

/*************************************
 *
 * Utilities
 *
 *************************************/

int check( KIOConnection *_con )
{
  int err;
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  fd_set rfds;
  FD_ZERO( &rfds );
  FD_SET( _con->inFD(), &rfds );

again:
  if ( ( err = select( _con->inFD(), &rfds, 0L, 0L, &tv ) ) == -1 && errno == EINTR )
    goto again;

  // No error and something to read ?
  if ( err != -1 && err != 0 )
    return 1;

  return 0;
}

