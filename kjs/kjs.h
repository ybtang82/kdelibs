/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
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
 */

#ifndef _KJS_H_
#define _KJS_H_

class QChar;
class KJScriptLock;

namespace KJS {
  class UChar;
  class UString;
  class Lexer;
  class Context;
  class Global;
  class ProgramNode;
  class Error;
};

/**
 * @short ECMAScript interpreter
 *
 * This library implements ECMAScript. Currently its main aim is to add
 * JavaScript support to KHTMLView.
 */
class KJScript {
  friend KJScriptLock;
public:
  /**
   * Create a new ECMAScript interpreter. You can later ask it to interprete
   * code by pass it via @ref #evaluate.
   */
  KJScript();
  /**
   *  Destructor
   */
  ~KJScript();
  /**
   * Asks the interpreter to evaluate a piece of code. If called more than
   * once the state (global variables, functions etc.) will be preserved
   * between each call.
   * @param code is a string containing the code to be executed.
   */
  bool evaluate(const char *code);
  /**
   * Same as above. Only differing in the arguments accepted.
   * @param code is a pointer to an Unicode string containing the code to
   * be executed.
   * @param length number of characters.
   */
  bool evaluate(const QChar *code, unsigned int length);
  /**
   * Added for convenience in case you have the code in available in
   * internal representation already.
   * @param code is an Unicode string containing the code to be executed.
   */
  bool evaluate(const KJS::UString &code);
  /**
   * Clear the interpreter's memory. Otherwise, function declarations
   * and global variables will be remembered after each invokation of
   * @ref KJScript::evaluate.
   */
  void clear();
private:
  /**
   * Initialize global object and context. For internal use only.
   */
  void init();
public:
  static KJScript *current() { return curr; }
  void setCurrent(KJScript *c) { curr = c; }

  static KJS::Error *error() { return current()->err; }
  static void setError(KJS::Error *e) { current()->err = e; }

  static KJS::Context *context() { return current()->con; }
  static void setContext(KJS::Context *c) { current()->con = c; }

  static KJS::Global *global() { return current()->glob; }
  static void setGlobal(KJS::Global *g) { current()->glob = g; }

  static KJS::Lexer *lexer() { return current()->lex; }
  static void setLexer(KJS::Lexer *l) { current()->lex = l; }
private:
  bool initialized;
  static KJScript *curr;
  KJS::Lexer *lex;
  KJS::Error *err;
  KJS::Context *con;
  KJS::Global *glob;
};

// callback functions for KJSProxy
typedef bool (KJSEvalFunc)(KJScript *script, const QChar *, unsigned int);
typedef void (KJSClearFunc)(KJScript *script);
typedef const char* (KJSSpecialFunc)(KJScript *script, const char *);
typedef void (KJSDestroyFunc)(KJScript *script);
extern "C" {
  KJSEvalFunc kjs_eval;
  KJSClearFunc kjs_clear;
  KJSSpecialFunc kjs_special;
  KJSDestroyFunc kjs_destroy;
}

/**
 * @short Proxy class serving as interface when being dlopen'ed.
 */
class KJSProxy {
public:
  KJSProxy(KJScript *s, KJSEvalFunc e, KJSClearFunc c,
	   KJSSpecialFunc sp, KJSDestroyFunc d)
    : script(s), eval(e), clr(c), spec(sp), destr(d) {};
  ~KJSProxy() { (*destr)(script); }
  bool evaluate(const QChar *c, unsigned int l) {
    return (*eval)(script, c, l);
  }
  const char *special(const char *c) {
    return (*spec)(script, c);
  }
  void clear() {
    (*clr)(script);
  }
private:
  KJScript *script;
  KJSEvalFunc *eval;
  KJSClearFunc *clr;
  KJSSpecialFunc *spec;
  KJSDestroyFunc *destr;
};

#endif
