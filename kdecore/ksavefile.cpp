/*
  This file is part of the KDE libraries
  Copyright (c) 1999 Waldo Bastian <bastian@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License version 2 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
*/

#include "config.h"

#include <sys/types.h>

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_TEST
#include <test.h>
#endif

#include <stdlib.h>

#include <qdatetime.h>
#include "kapp.h"
#include "ksavefile.h"

KSaveFile::KSaveFile(const QString &filename, int mode)
 : mTempFile(true) 
{
   if (!checkAccess(filename, W_OK)) 
   {
      mTempFile.setError(EACCES);
      return;
   }

   if (mTempFile.create(filename, ".new", mode))
   {
      mFileName = filename; // Set filename upon success
   }
   return;
}

KSaveFile::~KSaveFile()
{
   close();
}

QString 
KSaveFile::name()
{
   return mFileName;
}

void
KSaveFile::abort()
{
   mTempFile.unlink();
   mTempFile.close();
}

bool
KSaveFile::close()
{
   if (mTempFile.close())
   {
      int result = rename( mTempFile.name().ascii(), mFileName.ascii());
      if ( result == 0 )
      {
         return true; // Success!
      }
      mTempFile.setError(errno);
   }
    
   // Something went wrong, make sure to delete the interim file.
   mTempFile.unlink();
   return false;
}

