/*
 * kdiskfreesp.cpp
 *
 * Copyright (c) 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "kdiskfreesp.h"
#include <qfile.h>
#include <qtstream.h>

#include <kdebug.h>
#include <kprocess.h>
#include <kio/global.h>

#include "kdiskfreesp.moc"

#define DF_COMMAND    "df"
#define DF_ARGS       "-k"
#define NO_FS_TYPE    true

#define BLANK ' '
#define FULL_PERCENT 95.0

/***************************************************************************
  * constructor
**/
KDiskFreeSp::KDiskFreeSp(QObject *parent, const char *name)
    : QObject(parent,name)
{
    dfProc = new KProcess(); CHECK_PTR(dfProc);
    connect( dfProc, SIGNAL(receivedStdout(KProcess *, char *, int) ),
             this, SLOT (receivedDFStdErrOut(KProcess *, char *, int)) );
    connect(dfProc,SIGNAL(processExited(KProcess *) ),
            this, SLOT(dfDone() ) );

    readingDFStdErrOut=FALSE;
}


/***************************************************************************
  * destructor
**/
KDiskFreeSp::~KDiskFreeSp()
{
};

/***************************************************************************
  * is called, when the df-command writes on StdOut
**/
void KDiskFreeSp::receivedDFStdErrOut(KProcess *, char *data, int len)
{
  QCString tmp(data,len+1);  // adds a zero-byte
  dfStringErrOut.append(tmp);
}

/***************************************************************************
  * reads the df-commands results
**/
int KDiskFreeSp::readDF( const QString & mountPoint )
{
  if (readingDFStdErrOut || dfProc->isRunning())
    return -1;
  m_mountPoint = mountPoint;
  dfStringErrOut=""; // yet no data received
  dfProc->clearArguments();
  (*dfProc) << QString::fromLocal8Bit(DF_COMMAND) << QString::fromLocal8Bit(DF_ARGS);
  if (!dfProc->start( KProcess::NotifyOnExit, KProcess::AllOutput ))
     kdError() << "could not execute ["<< DF_COMMAND << "]" << endl;;
  return 1;
}


/***************************************************************************
  * is called, when the df-command has finished
**/
void KDiskFreeSp::dfDone()
{
  kdDebug() << "KDiskFreeSp::dfDone" << endl;
  readingDFStdErrOut=TRUE;

  QTextStream t (dfStringErrOut, IO_ReadOnly);
  QString s=t.readLine();
  if ( (s.isEmpty()) || ( s.left(10) != QString::fromLatin1("Filesystem") ) )
    kdError() << "Error running df command... got [" << s << "]" << endl;
  while ( !t.eof() ) {
    QString u,v;
    s=t.readLine();
    s=s.simplifyWhiteSpace();
    if ( !s.isEmpty() ) {
      kdDebug() << "GOT: [" << s << "]" << endl;

      if (s.find(BLANK)<0)      // devicename was too long, rest in next line
	if ( !t.eof() ) {       // just appends the next line
            v=t.readLine();
            s=s.append(v);
            s=s.simplifyWhiteSpace();
            kdDebug() << "SPECIAL GOT: [" << s << "]" << endl;
	 }//if silly linefeed

      kdDebug() << "EFFECTIVELY GOT " << s.length() << " chars: [" << s << "]" << endl;

      QString deviceName = s.left(s.find(BLANK));
      s=s.remove(0,s.find(BLANK)+1 );
      kdDebug() << "    DeviceName:    [" << deviceName << "]" << endl;

      if (!NO_FS_TYPE)
          s=s.remove(0,s.find(BLANK)+1 ); // eat fs type

      u=s.left(s.find(BLANK));
      unsigned long kBSize = u.toULong();
      s=s.remove(0,s.find(BLANK)+1 );
      kdDebug() << "    Size:       [" << kBSize << "]" << endl;

      u=s.left(s.find(BLANK));
      unsigned long kBUsed = u.toULong();
      s=s.remove(0,s.find(BLANK)+1 );
      kdDebug() << "    Used:       [" << kBUsed << "]" << endl;

      u=s.left(s.find(BLANK));
      unsigned long kBAvail = u.toULong();
      s=s.remove(0,s.find(BLANK)+1 );
      kdDebug() << "    Avail:       [" << kBAvail << "]" << endl;


      s=s.remove(0,s.find(BLANK)+1 );  // delete the capacity 94%
      QString mountPoint = s.left(s.find(BLANK));
      s=s.remove(0,s.find(BLANK)+1 );
      kdDebug() << "    MountPoint:       [" << mountPoint << "]" << endl;

      if ( mountPoint == m_mountPoint )
        emit foundMountPoint( mountPoint, kBSize, kBUsed, kBAvail );
    }//if not header
  }//while further lines available

  readingDFStdErrOut=FALSE;
  emit done();
  delete this;
}

KDiskFreeSp * KDiskFreeSp::findUsageInfo( const QString & path )
{
    KDiskFreeSp * job = new KDiskFreeSp;
    QString mountPoint = KIO::findPathMountPoint( path );
    job->readDF( mountPoint );
    return job;
}
