/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2000 Harri Porten (porten@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>

#include <dom_string.h>
#include <kmessagebox.h>
#include <klocale.h>

#include <operations.h>
#include "kjs_navigator.h"

using namespace KJS;

KJSO *Navigator::get(const CString &p) const
{
  UString str;

  if (p == "appCodeName")
    str = "Mozilla";
  else if (p == "appName")
    str = "Konqueror";
  else if (p == "appVersion")
    str = "1.0 (X11; Konqueror; Unix)";
  else if (p == "userAgent")
    str = "Mozilla/1.0 (X11; Konqueror; Unix)";
  else
    return new KJSUndefined();

  return new KJSString(str);
}
