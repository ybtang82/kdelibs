/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999 Harri Porten (porten@kde.org)
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

#include <kjs/kjs.h>
#include <kjs/object.h>
#include <kjs/function.h>
#include <kjs/operations.h>

#include <khtml_part.h>
#include <html_element.h>
#include <html_head.h>
#include <html_inline.h>
#include <html_image.h>
#include <dom_string.h>
#include <dom_exception.h>
#include <html_misc.h>

#include "kjs_binding.h"
#include "kjs_dom.h"
#include "kjs_html.h"
#include "kjs_text.h"
#include "kjs_window.h"
#include "kjs_navigator.h"

using namespace KJS;

KJSO DOMObject::get(const UString &p) const
{
  KJSO result;
  try {
    result = tryGet(p);
  }
  catch (DOM::DOMException e) {
    result = Undefined();
  }
  return result;
}

void DOMObject::put(const UString &p, const KJSO& v)
{
  try {
    tryPut(p,v);
  }
  catch (DOM::DOMException e) {
  }
}

KJSO DOMFunction::get(const UString &p) const
{
  KJSO result;
  try {
    result = tryGet(p);
  }
  catch (DOM::DOMException e) {
    result = Undefined();
  }
  return result;
}

Completion DOMFunction::execute(const List &args)
{
  Completion completion;
  try {
    completion = tryExecute(args);
  }
  catch (DOM::DOMException e) {
    completion = Completion(Normal,Undefined());
  }
  return completion;
}

UString::UString(const QString &d)
{
  unsigned int len = d.length();
  UChar *dat = new UChar[len];
  memcpy(dat, d.unicode(), len * sizeof(UChar));
  rep = UString::Rep::create(dat, len);
}

UString::UString(const DOM::DOMString &d)
{
  unsigned int len = d.length();
  UChar *dat = new UChar[len];
  memcpy(dat, d.unicode(), len * sizeof(UChar));
  rep = UString::Rep::create(dat, len);
}

DOM::DOMString UString::string() const
{
  return DOM::DOMString((QChar*) data(), size());
}

QString UString::qstring() const
{
  return QString((QChar*) data(), size());
}

QConstString UString::qconststring() const
{
  return QConstString((QChar*) data(), size());
}

DOM::Node KJS::toNode(const KJSO& obj)
{
  if (!obj.derivedFrom("Node")) {
    //    printf("Can't convert %s to Node.\n", obj.imp()->typeInfo()->name);
    return DOM::Node();
  }

  //  printf("Converting %s to Node.\n", obj.imp()->typeInfo()->name);
  const DOMNode *dobj = static_cast<const DOMNode*>(obj.imp());
  DOM::Node n = dobj->toNode();

  return n;
}

KJSO KJS::getString(DOM::DOMString s)
{
  if (s.isNull())
    return Null();
  else
    return String(s);
}




