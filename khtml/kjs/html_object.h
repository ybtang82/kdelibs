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

#ifndef _HTML_OBJECT_H_
#define _HTML_OBJECT_H_

#include "global.h"
#include "object.h"

class KHTMLWidget;
class HTMLElement;
class HTMLCollection;

namespace KJS {

  enum { IDDocWrite };

  class HTMLDocFunction : public KJSInternalFunction {
  public:
    HTMLDocFunction(KHTMLWidget *w, int i) : htmlw(w), id(i) { };
    KJSO *execute(KJSContext *);
  private:
    KHTMLWidget *htmlw;
    int id;
  };

  class HTMLDocument : public HostObject {
  public:
    HTMLDocument(KHTMLWidget *w) : htmlw(w) { }
    virtual KJSO *get(const CString &p) const;
  private:
    KHTMLWidget *htmlw;
  };

  class HTMLElement : public HostObject {
  public:
    HTMLElement(DOM::HTMLElement e) : element(e) { }
    virtual KJSO *get(const CString &p) const;
  private:
    DOM::HTMLElement element;
  };

  class HTMLCollection : public HostObject {
  public:
    HTMLCollection(DOM::HTMLCollection c) : collection(c) { }
    virtual KJSO *get(const CString &p) const;
  private:
    DOM::HTMLCollection collection;
  };

}; // namespace

#endif
