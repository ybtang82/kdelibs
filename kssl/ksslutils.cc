/* This file is part of the KDE project
 *
 * Copyright (C) 2000 George Staikos <staikos@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


#include "ksslutils.h"

#include <qstring.h>
#include <kglobal.h>
#include <klocale.h>

#ifdef HAVE_SSL
// This code is mostly taken from OpenSSL v0.9.5a
// by Eric Young
QString ASN1_UTCTIME_QString(ASN1_UTCTIME *tm) {
QString qstr;
char *v;
int gmt=0;
int i;
int y=0,M=0,d=0,h=0,m=0,s=0;
KLocale kl;
 
  i = tm->length;
  v = (char *)tm->data;
 
  if (i < 10) goto auq_err;
  if (v[i-1] == 'Z') gmt=1;
  for (i=0; i<10; i++)
          if ((v[i] > '9') || (v[i] < '0')) goto auq_err;
  y = (v[0]-'0')*10+(v[1]-'0');
  if (y < 50) y+=100;
  M = (v[2]-'0')*10+(v[3]-'0');
  if ((M > 12) || (M < 1)) goto auq_err;
  d = (v[4]-'0')*10+(v[5]-'0');
  h = (v[6]-'0')*10+(v[7]-'0');
  m =  (v[8]-'0')*10+(v[9]-'0');
  if (    (v[10] >= '0') && (v[10] <= '9') &&
          (v[11] >= '0') && (v[11] <= '9'))
          s = (v[10]-'0')*10+(v[11]-'0');
 
  // did I just defeat the purpose of klocale by calling ascii()?  No time to
  // check this right now.
  qstr.sprintf("%s %2d %02d:%02d:%02d %d%s", kl.monthName(M,true).ascii(),d,h,m,s,y+1900,(gmt)?" GMT":""); 
  return qstr;
auq_err:
  qstr = "(Bad time value)";
  return qstr;
}
#endif

