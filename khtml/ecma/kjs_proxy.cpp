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

#include "kjs_proxy.h"

#include <kjs/kjs.h>
#include <kjs/object.h>

#include <khtml_part.h>
#include <html_element.h>
#include <html_head.h>
#include <html_inline.h>
#include <html_image.h>

#include "kjs_binding.h"
#include "kjs_dom.h"
#include "kjs_html.h"
#include "kjs_window.h"
#include "kjs_navigator.h"

using namespace KJS;

extern "C" {
  KJSProxy *kjs_html_init(KHTMLPart *khtml);
}

// initialize HTML module
KJSProxy *kjs_html_init(KHTMLPart *khtml)
{
  KJScript *script = kjs_create(khtml);

  // proxy class operating via callback functions
  KJSProxy *proxy = new KJSProxy(script, &kjs_create, &kjs_eval, &kjs_clear,
	  			 &kjs_special, &kjs_destroy);
  proxy->khtml = khtml;

  return proxy;
}

  // init the interpreter
  KJScript* kjs_create(KHTMLPart *khtml)
  {
    KJScript *script = new KJScript();
    script->enableDebug();

    KJS::Global global(Global::current());

    KJSO window(newWindow(khtml));
    global.put("window", window);
    // make "window" prefix implicit for shortcuts like alert()
    global.setFilter(window);
    global.setPrototype(window);

    return script;
  }

  // evaluate code. Returns the JS return value or an invalid QVariant
  // if there was none, an error occured or the type couldn't be converted.
  QVariant kjs_eval(KJScript *script, const QChar *c, unsigned int len,
		    const DOM::Node &n)
  {
    script->init(); // set a valid current interpreter
    KJS::KJSO thisNode = n.isNull() ?
			 KJS::Global::current().prototype() : getDOMNode(n);

    bool ret = script->evaluate(thisNode, c, len);

    // let's try to convert the return value
    QVariant res;
    if (ret && script->returnValue()) {
      KJS::KJSO retVal(script->returnValue());
      switch (retVal.type()) {
      case BooleanType:
	  res = QVariant(retVal.toBoolean().value(), 0);
	  break;
      case NumberType:
	  res = QVariant(retVal.toNumber().value());
	  break;
      case StringType:
	  res = QVariant(retVal.toString().value().qstring());
	  break;
      default:
	  break;
      }
    }
    return res;
  }
  // clear resources allocated by the interpreter
  void kjs_clear(KJScript *script)
  {
    script->clear();
    delete script;
    script = 0L;
  }
  // for later extensions.
  const char *kjs_special(KJScript *, const char *)
  {
    // return something like a version number for now
    return "1";
  }
  void kjs_destroy(KJScript *script)
  {
    delete script;
  }
