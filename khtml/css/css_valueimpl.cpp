/**
 * This file is part of the DOM implementation for KDE.
 *
 * (C) 1999 Lars Knoll (knoll@kde.org)
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
#include "css_valueimpl.h"
#include "css_value.h"
#include "css_ruleimpl.h"
#include "css_stylesheetimpl.h"
#include "cssparser.h"
#include "dom_exception.h"
#include "dom_string.h"
#include "dom_stringimpl.h"

using namespace DOM;

CSSStyleDeclarationImpl::CSSStyleDeclarationImpl(CSSRuleImpl *parent)
    : StyleBaseImpl(parent)
{
    m_lstValues = 0;
}

CSSStyleDeclarationImpl::CSSStyleDeclarationImpl(CSSRuleImpl *parent, QList<CSSProperty> *lstValues)
    : StyleBaseImpl(parent)
{
    m_lstValues = lstValues;
}

CSSStyleDeclarationImpl::~CSSStyleDeclarationImpl()
{
    if(m_lstValues) delete m_lstValues;
}

DOMString CSSStyleDeclarationImpl::getPropertyValue( const DOMString &propertyName )
{
    // ###
    return 0;
}

CSSValueImpl *CSSStyleDeclarationImpl::getPropertyCSSValue( const DOMString &propertyName )
{
    int id = getPropertyID(propertyName.string().ascii(), propertyName.length());
    return getPropertyCSSValue(id);
}

CSSValueImpl *CSSStyleDeclarationImpl::getPropertyCSSValue( int propertyID )
{
    int i = 0;
    while(i < m_lstValues->count())
    {
	if(propertyID == m_lstValues->at(i)->m_id) return m_lstValues->at(i)->value();
	i++;
    }
    return 0;
}


DOMString CSSStyleDeclarationImpl::removeProperty( const DOMString &propertyName )
{
    int id = getPropertyID(propertyName.string().ascii(), propertyName.length());
    int i = 0;
    while(i < m_lstValues->count())
    {
	if(id == m_lstValues->at(i)->m_id)
	{
	    m_lstValues->remove(i);
	    // ###
	    return 0;
	}
	i++;
    }
    return 0;
}

DOMString CSSStyleDeclarationImpl::getPropertyPriority( const DOMString &propertyName )
{
    int id = getPropertyID(propertyName.string().ascii(), propertyName.length());
    if(getPropertyPriority(id))
	return DOMString("important");
    return DOMString();
}

bool CSSStyleDeclarationImpl::getPropertyPriority( int propertyID )
{
    int i = 0;
    while(i < m_lstValues->count())
    {
	if(propertyID == m_lstValues->at(i)->m_id ) return m_lstValues->at(i)->m_bImportant;
	i++;
    }
    return false;
}

void CSSStyleDeclarationImpl::setProperty( const DOMString &propertyName, const DOMString &value, const DOMString &priority )
{
    // ###
}

unsigned long CSSStyleDeclarationImpl::length() const
{
    if(!m_lstValues) return 0;
    return m_lstValues->count();
}

DOMString CSSStyleDeclarationImpl::item( unsigned long index )
{
    // ###
    //return m_lstValues->at(index);
    return 0;
}

CSSRuleImpl *CSSStyleDeclarationImpl::parentRule() const
{
    if( !m_parent ) return 0;
    if( m_parent->isRule() ) return static_cast<CSSRuleImpl *>(m_parent);
    return 0;
}

DOM::DOMString CSSStyleDeclarationImpl::cssText() const
{
  return DOM::DOMString();
    // ###
}

void CSSStyleDeclarationImpl::setCssText(DOM::DOMString str)
{
    // ###
}

bool CSSStyleDeclarationImpl::parseString( const DOMString &string )
{
    return false;
    // ###
}


// --------------------------------------------------------------------------------------

CSSValueImpl::CSSValueImpl()
    : StyleBaseImpl()
{
}

CSSValueImpl::~CSSValueImpl()
{
}

DOM::DOMString CSSValueImpl::cssText() const
{
  return DOM::DOMString();
    // ###
}

void CSSValueImpl::setCssText(DOM::DOMString str)
{
    // ###
}

// ----------------------------------------------------------------------------------------

CSSValueListImpl::CSSValueListImpl()
    : CSSValueImpl()
{
}

CSSValueListImpl::~CSSValueListImpl()
{
}

unsigned long CSSValueListImpl::length() const
{
  // TODO
  return 0;
}

CSSValueImpl *CSSValueListImpl::item( unsigned long index )
{
  // TODO
  return 0;
}

unsigned short CSSValueListImpl::valueType() const
{
    return CSSValue::CSS_VALUE_LIST;
}

// -------------------------------------------------------------------------------------

CSSPrimitiveValueImpl::CSSPrimitiveValueImpl()
    : CSSValueImpl()
{
    m_type = 0;
}

CSSPrimitiveValueImpl::CSSPrimitiveValueImpl(int ident)
    : CSSValueImpl()
{
    m_value.ident = ident;
    m_type = CSSPrimitiveValue::CSS_IDENT;
}

CSSPrimitiveValueImpl::CSSPrimitiveValueImpl(float num, CSSPrimitiveValue::UnitTypes type)
{
    m_value.num = num;
    m_type = type;
}

CSSPrimitiveValueImpl::CSSPrimitiveValueImpl(const DOMString &str, CSSPrimitiveValue::UnitTypes type)
{
    m_value.string = str.implementation();
    m_value.string->ref();
    m_type = type;
}

CSSPrimitiveValueImpl::CSSPrimitiveValueImpl(const Counter &c)
{
    m_value.counter = new Counter(c);
    m_type = CSSPrimitiveValue::CSS_COUNTER;
}

CSSPrimitiveValueImpl::CSSPrimitiveValueImpl(const Rect &r)
{
    m_value.rect = new Rect(r);
    m_type = CSSPrimitiveValue::CSS_RECT;
}

CSSPrimitiveValueImpl::CSSPrimitiveValueImpl(const RGBColor &rgb)
{
    m_value.rgbcolor = new RGBColor(rgb);
    m_type = CSSPrimitiveValue::CSS_RGBCOLOR;
}

CSSPrimitiveValueImpl::CSSPrimitiveValueImpl(const QColor &color)
{
    m_value.rgbcolor = new RGBColor(color);
    m_type = CSSPrimitiveValue::CSS_RGBCOLOR;
}

CSSPrimitiveValueImpl::~CSSPrimitiveValueImpl()
{
    if(m_type < CSSPrimitiveValue::CSS_STRING || m_type != CSSPrimitiveValue::CSS_IDENT)
    { }
    else if(m_type < CSSPrimitiveValue::CSS_COUNTER)
	m_value.string->deref();
    else if(m_type == CSSPrimitiveValue::CSS_COUNTER)
	delete m_value.counter;
    else if(m_type == CSSPrimitiveValue::CSS_RECT)
	delete m_value.rect;
    else if(m_type == CSSPrimitiveValue::CSS_RGBCOLOR)
	delete m_value.rgbcolor;
}

unsigned short CSSPrimitiveValueImpl::primitiveType() const
{
    return m_type;
}

void CSSPrimitiveValueImpl::setFloatValue( unsigned short unitType, float floatValue )
{
    // ### check if property supports this type
    if(m_type > CSSPrimitiveValue::CSS_DIMENSION) throw CSSException(CSSException::SYNTAX_ERR);
    //if(m_type > CSSPrimitiveValue::CSS_DIMENSION) throw DOMException(DOMException::INVALID_ACCESS_ERR);
    m_value.num = floatValue;
    m_type = unitType;
}

float CSSPrimitiveValueImpl::getFloatValue( unsigned short unitType )
{
    if(m_type > CSSPrimitiveValue::CSS_DIMENSION) throw CSSException(CSSException::SYNTAX_ERR);
    //if(m_type > CSSPrimitiveValue::CSS_DIMENSION) throw DOMException(DOMException::INVALID_ACCESS_ERR);
    return m_value.num;
}

void CSSPrimitiveValueImpl::setStringValue( unsigned short stringType, const DOMString &stringValue )
{
    //if(m_type < CSSPrimitiveValue::CSS_STRING) throw DOMException(DOMException::INVALID_ACCESS_ERR);
    //if(m_type > CSSPrimitiveValue::CSS_ATTR) throw DOMException(DOMException::INVALID_ACCESS_ERR);
    if(m_type < CSSPrimitiveValue::CSS_STRING) throw CSSException(CSSException::SYNTAX_ERR);
    if(m_type > CSSPrimitiveValue::CSS_ATTR) throw CSSException(CSSException::SYNTAX_ERR);
    if(stringType != CSSPrimitiveValue::CSS_IDENT)
    {
	m_value.string = stringValue.implementation();
	m_value.string->ref();
	m_type = stringType;
    }
    // ### parse ident
}

DOMStringImpl *CSSPrimitiveValueImpl::getStringValue(  )
{
    if(m_type < CSSPrimitiveValue::CSS_STRING) return 0;
    if(m_type > CSSPrimitiveValue::CSS_ATTR) return 0;
    if(m_type == CSSPrimitiveValue::CSS_IDENT)
    {
	// ###
	return 0;
    }
    return m_value.string;
}

Counter *CSSPrimitiveValueImpl::getCounterValue(  )
{
    if(m_type != CSSPrimitiveValue::CSS_COUNTER) return 0;
    return m_value.counter;
}

Rect *CSSPrimitiveValueImpl::getRectValue(  )
{
    if(m_type != CSSPrimitiveValue::CSS_RECT) return 0;
    return m_value.rect;
}

RGBColor *CSSPrimitiveValueImpl::getRGBColorValue(  )
{
    if(m_type != CSSPrimitiveValue::CSS_RGBCOLOR) return 0;
    return m_value.rgbcolor;
}

unsigned short CSSPrimitiveValueImpl::valueType() const
{
    return CSSValue::CSS_PRIMITIVE_VALUE;
}

bool CSSPrimitiveValueImpl::parseString( const DOMString &string )
{
    // ###
    return false;
}

int CSSPrimitiveValueImpl::getIdent()
{
    if(m_type != CSSPrimitiveValue::CSS_IDENT) return 0;
    return m_value.ident;
}
