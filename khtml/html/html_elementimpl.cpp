/**
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
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
 *
 * $Id$
 */
// -------------------------------------------------------------------------
//#define DEBUG
//#define DEBUG_LAYOUT
//#define PAR_DEBUG
//#define EVENT_DEBUG
#include "html_elementimpl.h"

#include "html_documentimpl.h"
#include "htmltokenizer.h"

#include "htmlhashes.h"
#include "khtmlview.h"
#include "khtml_part.h"

#include "rendering/render_object.h"
#include "rendering/render_replaced.h"
#include "css/css_valueimpl.h"
#include "css_stylesheetimpl.h"
#include "css/cssproperties.h"
#include "xml/dom_textimpl.h"
#include "xml/dom2_eventsimpl.h"

#include <kdebug.h>

using namespace DOM;
using namespace khtml;

HTMLElementImpl::HTMLElementImpl(DocumentImpl *doc) : ElementImpl(doc)
{
}

HTMLElementImpl::~HTMLElementImpl()
{
}

void HTMLElementImpl::parseAttribute(AttrImpl *attr)
{
    DOMString indexstring;
    switch( attr->attrId )
    {
// the core attributes...
    case ATTR_ID:
	// unique id
	setHasID();
	break;
    case ATTR_CLASS:
	// class
	setHasClass();
	break;
    case ATTR_STYLE:
	// ### we need to remove old style info in case there was any!
	// ### the inline sheet ay contain more than 1 property!
	// stylesheet info
	setHasStyle();
	addCSSProperty(attr->value());
	break;
    case ATTR_TITLE:
	// additional title for the element, may be displayed as tooltip
	setHasTooltip();
	break;
    case ATTR_TABINDEX:
	indexstring=getAttribute(ATTR_TABINDEX);
        if (indexstring.length())
	    setTabIndex(indexstring.toInt());
	break;
// i18n attributes
    case ATTR_LANG:
	break;
    case ATTR_DIR:
	addCSSProperty(CSS_PROP_DIRECTION, attr->value());
	break;
	// BiDi info
	break;
// standard events
    case ATTR_ONCLICK:
        removeHTMLEventListener(EventImpl::KHTML_CLICK_EVENT);
        addEventListener(EventImpl::KHTML_CLICK_EVENT,new HTMLEventListener(document->view()->part(),DOMString(attr->value()).string()),false);
	break;
    case ATTR_ONDBLCLICK:
        removeHTMLEventListener(EventImpl::KHTML_DBLCLICK_EVENT);
        addEventListener(EventImpl::KHTML_DBLCLICK_EVENT,new HTMLEventListener(document->view()->part(),DOMString(attr->value()).string()),false);
	break;
    case ATTR_ONMOUSEDOWN:
        removeHTMLEventListener(EventImpl::MOUSEDOWN_EVENT);
        addEventListener(EventImpl::MOUSEDOWN_EVENT,new HTMLEventListener(document->view()->part(),DOMString(attr->value()).string()),false);
	break;
    case ATTR_ONMOUSEMOVE:
        removeHTMLEventListener(EventImpl::MOUSEMOVE_EVENT);
        addEventListener(EventImpl::MOUSEMOVE_EVENT,new HTMLEventListener(document->view()->part(),DOMString(attr->value()).string()),false);
	break;
    case ATTR_ONMOUSEOUT:
        removeHTMLEventListener(EventImpl::MOUSEOUT_EVENT);
        addEventListener(EventImpl::MOUSEOUT_EVENT,new HTMLEventListener(document->view()->part(),DOMString(attr->value()).string()),false);
	break;
    case ATTR_ONMOUSEOVER:
        removeHTMLEventListener(EventImpl::MOUSEOVER_EVENT);
        addEventListener(EventImpl::MOUSEOVER_EVENT,new HTMLEventListener(document->view()->part(),DOMString(attr->value()).string()),false);
	break;
    case ATTR_ONMOUSEUP:
        removeHTMLEventListener(EventImpl::MOUSEUP_EVENT);
        addEventListener(EventImpl::MOUSEUP_EVENT,new HTMLEventListener(document->view()->part(),DOMString(attr->value()).string()),false);
	break;
    case ATTR_ONKEYDOWN:
    case ATTR_ONKEYPRESS:
    case ATTR_ONKEYUP:
	setHasEvents();
	break;
// other misc attributes
    default:
	break;
    }
}

void HTMLElementImpl::addCSSProperty( const DOMString &property, const DOMString &value, bool nonCSSHint)
{
    if(!m_styleDecls) createDecl();
    m_styleDecls->setProperty(property, value, false, nonCSSHint);
}

void HTMLElementImpl::addCSSProperty(int id, const DOMString &value)
{
    if(!m_styleDecls) createDecl();
    m_styleDecls->setProperty(id, value, false, true);
}

void HTMLElementImpl::addCSSLength(int id, const DOMString &value)
{
    if(!m_styleDecls) createDecl();
    m_styleDecls->setLengthProperty(id, stripAttributeGarbage( value ), false, true);
}

void HTMLElementImpl::addCSSProperty(const DOMString &property)
{
    if(!m_styleDecls) createDecl();
    m_styleDecls->setProperty(property);
}

void HTMLElementImpl::removeCSSProperty(int id)
{
    if(!m_styleDecls)
	return;
    HTMLDocumentImpl *doc = static_cast<HTMLDocumentImpl *>(document);
    m_styleDecls->setParent(doc->elementSheet());
    m_styleDecls->removeProperty(id);
}

void HTMLElementImpl::removeCSSProperty( const DOMString &id )
{
    if(!m_styleDecls)
	return;
    HTMLDocumentImpl *doc = static_cast<HTMLDocumentImpl *>(document);
    m_styleDecls->setParent(doc->elementSheet());
    m_styleDecls->removeProperty(id);
}

DOMString HTMLElementImpl::getCSSProperty( const DOM::DOMString &prop )
{
    if(!m_styleDecls)
	return 0;
    return m_styleDecls->getPropertyValue( prop );
}

DOMString HTMLElementImpl::innerHTML() const
{
    return toHTML();
}

DOMString HTMLElementImpl::innerText() const
{
    DOMString text;

    NodeImpl *n = firstChild();
    // find the next text/image after the anchor, to get a position
    while(n) {
	if(n->firstChild())
	    n = n->firstChild();
	else if(n->nextSibling())
	    n = n->nextSibling();
	else {
	    NodeImpl *next = 0;
	    while(!next) {
		n = n->parentNode();
		if(!n || n == (NodeImpl *)this ) goto end;
		next = n->nextSibling();
	    }
	    n = next;
	}
	if(n->isTextNode() ) {
	    text += static_cast<TextImpl *>(n)->data();
	}
    }
 end:
    return text;
}

bool HTMLElementImpl::setInnerHTML( const DOMString &html )
{
    // the following is in accordance with the definition as used by IE
    if( endTag() == FORBIDDEN )
	return false;
    // IE disallows innerHTML on inline elements. I don't see why we should have this restriction, as our
    // dhtml engine can cope with it. Lars
    //if ( isInline() ) return false;
    switch( id() ) {
	case ID_COL:
	case ID_COLGROUP:
	case ID_FRAMESET:
	case ID_HEAD:
	case ID_HTML:
	case ID_STYLE:
	case ID_TABLE:
	case ID_TBODY:
	case ID_TFOOT:
	case ID_THEAD:
	case ID_TITLE:
	case ID_TR:
	    return false;
	default:
	    break;
    }
    if ( !ownerDocument()->isHTMLDocument() )
	return false;

    DocumentFragmentImpl *fragment = new DocumentFragmentImpl( ownerDocument() );
    HTMLTokenizer *tok = new HTMLTokenizer( static_cast<HTMLDocumentImpl *>(ownerDocument()), fragment );
    tok->begin();
    tok->write( html.string(), false );
    tok->end();
    delete tok;

    removeChildren();
    int ec = 0;
    appendChild( fragment, ec );
    if ( !ec )
	return true;
    return false;
}

bool HTMLElementImpl::setInnerText( const DOMString &text )
{
    // following the IE specs.
    if( endTag() == FORBIDDEN )
	return false;
    // IE disallows innerHTML on inline elements. I don't see why we should have this restriction, as our
    // dhtml engine can cope with it. Lars
    //if ( isInline() ) return false;
    switch( id() ) {
	case ID_COL:
	case ID_COLGROUP:
	case ID_FRAMESET:
	case ID_HEAD:
	case ID_HTML:
	case ID_TABLE:
	case ID_TBODY:
	case ID_TFOOT:
	case ID_THEAD:
	case ID_TR:
	    return false;
	default:
	    break;
    }

    removeChildren();

    TextImpl *t = new TextImpl( ownerDocument(), text );
    int ec = 0;
    appendChild( t, ec );
    if ( !ec )
	return true;
    return false;
}

DOMString HTMLElementImpl::stripAttributeGarbage( const DOMString &value )
{
    unsigned int realLength = value.length();
    unsigned int l = DOMStringImpl::stripAttributeGarbage( value.unicode(), realLength );

    if ( l == realLength )
        return value;

    DOMString res( value );
    return res.split( l );
}

// -------------------------------------------------------------------------
HTMLGenericElementImpl::HTMLGenericElementImpl(DocumentImpl *doc, ushort i)
    : HTMLElementImpl(doc)
{
    _id = i;
}

HTMLGenericElementImpl::~HTMLGenericElementImpl()
{
}

const DOMString HTMLGenericElementImpl::nodeName() const
{
    if (document->isHTMLDocument())
	return getTagName(_id);
    else
	return getTagName(_id).string().lower();
}



