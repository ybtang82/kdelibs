/*
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
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
#ifndef HTML_FORMIMPL_H
#define HTML_FORMIMPL_H

#include "html_elementimpl.h"
#include "html_element.h"

#include <qlist.h>
#include <qvaluelist.h>
#include <qarray.h>

class KHTMLView;
class QTextCodec;

namespace khtml
{
    class RenderFormElement;
    class RenderTextArea;
    class RenderSelect;

    typedef QValueList<QByteArray> encodingList;
}

namespace DOM {

class HTMLFormElement;
class DOMString;
class HTMLGenericFormElementImpl;
class HTMLOptionElementImpl;

class HTMLFormElementImpl : public HTMLElementImpl
{
public:
    HTMLFormElementImpl(DocumentImpl *doc);
    virtual ~HTMLFormElementImpl();

    virtual const DOMString nodeName() const { return "FORM"; }
    virtual ushort id() const;

    virtual tagStatus startTag() { return FORMStartTag; }
    virtual tagStatus endTag() { return FORMEndTag; }

    long length() const;
    void prepareSubmit();
    void reset (  );

    QByteArray formData( );

    DOMString enctype() const { return m_enctype; }
    void setEnctype( const DOMString & );

    DOMString boundary() const { return m_boundary; }
    void setBoundary( const DOMString & );

    virtual void parseAttribute(AttrImpl *attr);

    virtual void attach(KHTMLView *w);
    virtual void detach();

    void radioClicked( HTMLGenericFormElementImpl *caller );
    void maybeSubmit();

    void registerFormElement(khtml::RenderFormElement *);
    void removeFormElement(khtml::RenderFormElement *);

    void registerFormElement(HTMLGenericFormElementImpl *);
    void removeFormElement(HTMLGenericFormElementImpl *);

    /*

     * state() and restoreState() are complimentary functions.
     */
    virtual QString state() { return QString::null; }
    virtual void restoreState(const QString &) { };

protected:
    static QCString encodeByteArray( const QByteArray& e );

    void submit (  );

    friend class HTMLFormElement;

    QList<HTMLGenericFormElementImpl> formElements;
    DOMString m_url;
    DOMString m_target;
    DOMString m_enctype;
    DOMString m_boundary;
    DOMString m_acceptcharset;
    KHTMLView *view;
    bool m_post;
    bool m_multipart;
};

// -------------------------------------------------------------------------

class HTMLGenericFormElementImpl : public HTMLElementImpl
{
    friend class HTMLFormElementImpl;

public:
    HTMLGenericFormElementImpl(DocumentImpl *doc);
    HTMLGenericFormElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);
    virtual ~HTMLGenericFormElementImpl();

    HTMLFormElementImpl *form() { return _form; }

    virtual NodeImpl *addChild(NodeImpl *newChild) { return HTMLElementImpl::addChild(newChild); }

    virtual void parseAttribute(AttrImpl *attr);

    virtual void attach(KHTMLView *w);
    virtual void detach();

    virtual void reset() {}

    void onBlur();
    void onFocus();
    void onSelect();
    void onChange();


    bool disabled() const { return m_disabled; }
    void setDisabled(bool _disabled) { m_disabled = _disabled; }

    bool readOnly() const { return m_readOnly; }
    void setReadOnly(bool _readOnly) { m_readOnly = _readOnly; }

    const DOMString &name() const { return _name; }
    void setForm(HTMLFormElementImpl *f) { _form = f; }

    /*
     * override in derived classes to get the encoded name=value pair
     * for submitting
     * return true for a successful control (see 17.13.2)
     */
    virtual bool encoding(const QTextCodec*, khtml::encodingList&) { return false; }

    virtual void blur();
    virtual void focus();

protected:
    HTMLFormElementImpl *getForm() const;

    DOMString _name;
    HTMLFormElementImpl *_form;
    KHTMLView *view;
    bool m_disabled, m_readOnly;
};

// -------------------------------------------------------------------------

class HTMLButtonElementImpl : public HTMLGenericFormElementImpl
{
public:
    HTMLButtonElementImpl(DocumentImpl *doc);
    HTMLButtonElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);

    virtual ~HTMLButtonElementImpl();

    enum typeEnum {
        SUBMIT,
        RESET,
        BUTTON
    };

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    virtual tagStatus startTag() { return BUTTONStartTag; }
    virtual tagStatus endTag() { return BUTTONEndTag; }

    DOMString type() const;

    void parseAttribute(AttrImpl *attr);

    virtual void attach(KHTMLView *w);

    virtual NodeImpl *addChild(NodeImpl *newChild) { return HTMLElementImpl::addChild(newChild); }

protected:
    DOMString _value;
    QString currValue;
    typeEnum _type;
    bool dirty;
    bool _clicked;
};

// -------------------------------------------------------------------------

class HTMLFieldSetElementImpl : public HTMLGenericFormElementImpl
{
public:
    HTMLFieldSetElementImpl(DocumentImpl *doc);
    HTMLFieldSetElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);

    virtual ~HTMLFieldSetElementImpl();

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    virtual tagStatus startTag() { return FIELDSETStartTag; }
    virtual tagStatus endTag() { return FIELDSETEndTag; }
};

// -------------------------------------------------------------------------

class HTMLInputElementImpl : public HTMLGenericFormElementImpl
{
public:
    enum typeEnum {
        TEXT,
        PASSWORD,
        CHECKBOX,
        RADIO,
        SUBMIT,
        RESET,
        FILE,
        HIDDEN,
        IMAGE,
        BUTTON
    };

    HTMLInputElementImpl(DocumentImpl *doc);
    HTMLInputElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);
    virtual ~HTMLInputElementImpl();

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    virtual tagStatus startTag() { return INPUTStartTag; }
    virtual tagStatus endTag() { return INPUTEndTag; }

    bool checked() { return m_checked; }
    void setChecked(bool _checked);
    long maxLength() const { return _maxLen; }
    int size() const { return _size; }
    DOMString type() const;

    DOMString value() const;
    void setValue(DOMString val);

    DOMString filename() const { return m_filename; }
    void setFilename(DOMString _filename) { m_filename = _filename; }

    virtual QString state();
    virtual void restoreState(const QString &);

    virtual void blur();
    virtual void focus();
    void select();
    void click();

    virtual void parseAttribute(AttrImpl *attr);

    virtual void attach(KHTMLView *w);

    virtual bool encoding(const QTextCodec*, khtml::encodingList&);
    typeEnum inputType() { return _type; }
    virtual void reset();

    // for images
    virtual bool mouseEvent( int _x, int _y, int button, MouseEventType type,
                             int _tx, int _ty, DOMString &url,
                             NodeImpl *&innerNode, long &offset );

    virtual void setOwnerDocument(DocumentImpl *_document);
    // used in case input type=image was clicked.
    int clickX() const { return xPos; }
    int clickY() const { return yPos; }

protected:

    DOMString m_value;
    DOMString m_filename;
    DOMString _src;
    DOMString _defaultValue;
    typeEnum _type;
    int _maxLen;
    int _size;
    int xPos, yPos;
    bool _clicked;
    bool _defaultChecked;
    bool m_checked;
    bool m_haveType;
};

// -------------------------------------------------------------------------

class HTMLLabelElementImpl : public HTMLGenericFormElementImpl
{
public:
    HTMLLabelElementImpl(DocumentImpl *doc);
    HTMLLabelElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);
    virtual ~HTMLLabelElementImpl();

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    virtual tagStatus startTag() { return LABELStartTag; }
    virtual tagStatus endTag() { return LABELEndTag; }
};

// -------------------------------------------------------------------------

class HTMLLegendElementImpl : public HTMLGenericFormElementImpl
{
public:
    HTMLLegendElementImpl(DocumentImpl *doc);
    HTMLLegendElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);
    virtual ~HTMLLegendElementImpl();

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    virtual tagStatus startTag() { return LEGENDStartTag; }
    virtual tagStatus endTag() { return LEGENDEndTag; }
};


// -------------------------------------------------------------------------

class HTMLSelectElementImpl : public HTMLGenericFormElementImpl
{
public:
    HTMLSelectElementImpl(DocumentImpl *doc);
    HTMLSelectElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);

    virtual const DOMString nodeName() const { return "SELECT"; }
    virtual ushort id() const;

    virtual tagStatus startTag() { return SELECTStartTag; }
    virtual tagStatus endTag() { return SELECTEndTag; }

    DOMString type() const;

    long selectedIndex() const;
    void setSelectedIndex( long index );

    long length() const;

    long size() const { return m_size; }

    bool multiple() { return m_multiple; }

    void add ( const HTMLElement &element, const HTMLElement &before );
    void remove ( long index );

    virtual QString state();
    virtual void restoreState(const QString &);

    virtual NodeImpl *insertBefore ( NodeImpl *newChild, NodeImpl *refChild );
    virtual NodeImpl *replaceChild ( NodeImpl *newChild, NodeImpl *oldChild );
    virtual NodeImpl *removeChild ( NodeImpl *oldChild );
    virtual NodeImpl *appendChild ( NodeImpl *newChild );


    virtual void parseAttribute(AttrImpl *attr);

    virtual void attach(KHTMLView *w);
    virtual bool encoding(const QTextCodec*, khtml::encodingList&);

    // get the actual listbox index of the optionIndexth option
    int optionToListIndex(int optionIndex) const;
    // reverse of optionToListIndex - get optionIndex from listboxIndex
    int listToOptionIndex(int listIndex) const;
    void recalcListItems();
    QArray<HTMLGenericFormElementImpl*> listItems() { return m_listItems; }
    virtual void reset();
    void notifyOptionSelected(HTMLOptionElementImpl *selectedOption, bool selected);

protected:
    QArray<HTMLGenericFormElementImpl*> m_listItems;
    int m_size;
    bool m_multiple;
};


// -------------------------------------------------------------------------

class HTMLOptGroupElementImpl : public HTMLGenericFormElementImpl
{
public:
    HTMLOptGroupElementImpl(DocumentImpl *doc);
    HTMLOptGroupElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);
    virtual ~HTMLOptGroupElementImpl();

    virtual const DOMString nodeName() const { return "OPTGROUP"; }
    virtual ushort id() const;

    virtual tagStatus startTag() { return OPTGROUPStartTag; }
    virtual tagStatus endTag() { return OPTGROUPEndTag; }

    virtual NodeImpl *insertBefore ( NodeImpl *newChild, NodeImpl *refChild );
    virtual NodeImpl *replaceChild ( NodeImpl *newChild, NodeImpl *oldChild );
    virtual NodeImpl *removeChild ( NodeImpl *oldChild );
    virtual NodeImpl *appendChild ( NodeImpl *newChild );
    virtual void parseAttribute(AttrImpl *attr);
    void recalcSelectOptions();
};


// ---------------------------------------------------------------------------

class HTMLOptionElementImpl : public HTMLGenericFormElementImpl
{
public:
    HTMLOptionElementImpl(DocumentImpl *doc);
    HTMLOptionElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    virtual tagStatus startTag() { return OPTIONStartTag; }
    virtual tagStatus endTag() { return OPTIONEndTag; }

    DOMString text();

    long index() const;
    void setIndex( long );
    virtual void parseAttribute(AttrImpl *attr);
    DOMString value() { return m_value; }

    bool selected() const { return m_selected; }
    void setSelected(bool _selected);

    HTMLSelectElementImpl *getSelect();


protected:
    DOMString m_value;
    bool m_selected;
};


// -------------------------------------------------------------------------

class HTMLTextAreaElementImpl : public HTMLGenericFormElementImpl
{
public:
    enum WrapMethod {
        ta_NoWrap,
        ta_Virtual,
        ta_Physical
    };

    HTMLTextAreaElementImpl(DocumentImpl *doc);
    HTMLTextAreaElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);

    virtual const DOMString nodeName() const { return "TEXTAREA"; }
    virtual ushort id() const;

    virtual tagStatus startTag() { return TEXTAREAStartTag; }
    virtual tagStatus endTag() { return TEXTAREAEndTag; }

    long cols() const { return m_cols; }

    long rows() const { return m_rows; }

    WrapMethod wrap() { return m_wrap; }

    DOMString type() const;

    virtual QString state();
    virtual void restoreState(const QString &);

    void select (  );

    virtual void parseAttribute(AttrImpl *attr);
    virtual void attach(KHTMLView *w);
    virtual bool encoding(const QTextCodec*, khtml::encodingList&);
    virtual void reset();
    DOMString value();
    void setValue(DOMString _value);
    DOMString defaultValue();
    void setDefaultValue(DOMString _defaultValue);


protected:
    int m_rows;
    int m_cols;
    WrapMethod m_wrap;
    DOMString m_value;

    friend class khtml::RenderTextArea;
};

// -------------------------------------------------------------------------

class HTMLIsIndexElementImpl : public HTMLInputElementImpl
{
public:
    HTMLIsIndexElementImpl(DocumentImpl *doc);
    HTMLIsIndexElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);

    ~HTMLIsIndexElementImpl();

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    virtual tagStatus startTag() { return ISINDEXStartTag; }
    virtual tagStatus endTag() { return ISINDEXEndTag; }

    virtual void parseAttribute(AttrImpl *attr);
    virtual void attach(KHTMLView *w);

protected:
    DOMString m_prompt;
};


}; //namespace

#endif
