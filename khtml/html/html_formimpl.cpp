/*
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
#include "html_formimpl.h"

#include "khtmlview.h"
#include "khtml_part.h"
#include "html_documentimpl.h"

#include "htmlhashes.h"

#include "css/cssstyleselector.h"
#include "css/cssproperties.h"

#include "rendering/render_form.h"

#include <kdebug.h>
#include <kmimetype.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <netaccess.h>
#include <qfile.h>

using namespace DOM;
using namespace khtml;

//template class QList<khtml::RenderFormElement>;

HTMLFormElementImpl::HTMLFormElementImpl(DocumentImpl *doc)
    : HTMLElementImpl(doc)
{
    m_post = false;
    m_multipart = false;
    m_enctype = "application/x-www-form-urlencoded";
    m_boundary = "----------0xKhTmLbOuNdArY";
}

HTMLFormElementImpl::~HTMLFormElementImpl()
{
    QListIterator<HTMLGenericFormElementImpl> it(formElements);
    for (; it.current(); ++it)
	it.current()->setForm(0);
}

ushort HTMLFormElementImpl::id() const
{
    return ID_FORM;
}

long HTMLFormElementImpl::length() const
{
    // ###
    return 0;
}

QCString HTMLFormElementImpl::encodeByteArray(const QByteArray& e)
{
    static const char *safe = "$-._!*(),"; /* RFC 1738 */
    unsigned pos = 0;
    QCString encoded;

    while ( pos < e.size() )
    {
        char c = e[pos];

        if ( (( c >= 'A') && ( c <= 'Z')) ||
             (( c >= 'a') && ( c <= 'z')) ||
             (( c >= '0') && ( c <= '9')) ||
             (strchr(safe, c))
            )
            encoded += c;
        else if ( c == ' ' )
            encoded += '+';
        else if ( c == '\n' )
            encoded += "%0D%0A";
        else if ( c != '\r' )
        {
            encoded += '%';
            unsigned int h = c / 16;
            h += (h > 9) ? ('A' - 10) : '0';
            encoded += h;

            unsigned int l = c % 16;
            l += (l > 9) ? ('A' - 10) : '0';
            encoded += l;
        }
        pos++;
    }

    return encoded;
}


QByteArray HTMLFormElementImpl::formData()
{
    kdDebug( 6030 ) << "form: formData()" << endl;

    QByteArray form_data(0);
    bool first = true;
    QCString enc_string = ""; // used for non-multipart data

    HTMLGenericFormElementImpl *current = formElements.first();
    for( ; current; current = formElements.next() )
    {
        khtml::encodingList lst;

        if (current->encoding(lst)) {

            khtml::encodingList::Iterator it;
            for( it = lst.begin(); it != lst.end(); ++it )
            {
                QByteArray enc(*it);

                if (!m_multipart)
                {
                    if(!enc.size())
                        continue;
                    if(!first)
                        enc_string += '&';

                    enc_string += HTMLFormElementImpl::encodeByteArray(current->name().string().local8Bit());
                    enc_string += "=";
                    enc_string += HTMLFormElementImpl::encodeByteArray(enc);

                    first = false;
                }
                else
                {
                    // if there is no name to this part, we skip it
                    if ( current->name() == 0 )
                        continue;

                    QCString str(("--" + m_boundary.string() + "\r\n").ascii());
                    str += "Content-Disposition: form-data; ";
                    str += ("name=\"" + current->name().string() + "\"").ascii();

                    // if the current type is FILE, then we also need to
                    // include the filename *and* the file type

                    if (current->nodeType() == Node::ELEMENT_NODE && current->id() == ID_INPUT &&
                        static_cast<HTMLInputElementImpl*>(current)->inputType() == HTMLInputElementImpl::FILE)
                    {
                        QString path = static_cast<HTMLInputElementImpl*>(current)->filename().string();
                        QString onlyfilename = path.mid(path.findRev('/')+1);

                        str += ("; filename=\"" + onlyfilename + "\"\r\n").ascii();
                        if(!static_cast<HTMLInputElementImpl*>(current)->filename().isEmpty())
                        {
                            str += "Content-Type: ";
                            KMimeType::Ptr ptr = KMimeType::findByURL(KURL(path));
                            str += ptr->name().ascii();
                        }
                    }

                    str += "\r\n\r\n";

                    // this is where it gets ugly.. we have to memcpy the
                    // text part to the form.. then memcpy the (possibly
                    // binary) data.  yuck!
                    unsigned int old_size = form_data.size();
                    form_data.resize( old_size + str.size() + enc.size());
                    memcpy(form_data.data() + old_size, str.data(), str.length());
                    memcpy(form_data.data() + old_size + str.length(), enc, enc.size());
                    form_data[form_data.size()-2] = '\r';
                    form_data[form_data.size()-1] = '\n';
                }
            }
        }
    }
    if (m_multipart)
        enc_string = ("--" + m_boundary.string() + "--\r\n").ascii();

    int old_size = form_data.size();
    form_data.resize( form_data.size() + enc_string.length() );
    memcpy(form_data.data() + old_size, enc_string.data(), enc_string.length() );

    return form_data;
}

void HTMLFormElementImpl::setEnctype( const DOMString& type )
{
    m_enctype = type;
    if ( strcasecmp( type, "multipart/form-data" ) == 0 )
        m_multipart = true;
}

void HTMLFormElementImpl::setBoundary( const DOMString& bound )
{
    m_boundary = bound;
}

void HTMLFormElementImpl::submit(  )
{
    kdDebug( 6030 ) << "submit pressed!" << endl;
    if(!view) return;

    DOMString script = getAttribute(ATTR_ONSUBMIT);
    if (!script.isNull())
	view->part()->executeScript(script.string());

    QByteArray form_data = formData();

    // formdata is not null-terminated, so this will cause insure to scream
    // kdDebug( 6030 ) << "formdata = " << form_data.data() << endl << "m_post = " << m_post << endl << "multipart = " << m_multipart << endl;

    if(m_post)
    {
        view->part()->submitForm( "post", url.string(), form_data,
                                  target.string(),
                                  enctype().string(),
                                  boundary().string() );
    }
    else
        view->part()->submitForm( "get", url.string(), form_data,
                                  target.string() );
}

void HTMLFormElementImpl::reset(  )
{
    kdDebug( 6030 ) << "reset pressed!" << endl;

    DOMString script = getAttribute(ATTR_ONRESET);
    if (!script.isNull())
	view->part()->executeScript(script.string());

    HTMLGenericFormElementImpl *current = formElements.first();
    while(current)
    {
	current->reset();
	current = formElements.next();
    }
    if (document->isHTMLDocument())
	static_cast<HTMLDocumentImpl*>(document)->updateRendering();
}

void HTMLFormElementImpl::parseAttribute(AttrImpl *attr)
{
    switch(attr->attrId)
    {
    case ATTR_ACTION:
	url = attr->value();
	break;
    case ATTR_TARGET:
	target = attr->value();
	break;
    case ATTR_METHOD:
	if ( strcasecmp( attr->value(), "post" ) == 0 )
	    m_post = true;
	break;
    case ATTR_ENCTYPE:
        setEnctype( attr->value() );
	break;
    case ATTR_ACCEPT_CHARSET:
    case ATTR_ACCEPT:
	// ignore these for the moment...
	break;
    default:
	HTMLElementImpl::parseAttribute(attr);
    }
}

void HTMLFormElementImpl::attach(KHTMLView *_view)
{
    view = _view;
    HTMLElementImpl::attach(_view);
}

void HTMLFormElementImpl::detach()
{
    view = 0;
    HTMLElementImpl::detach();
}

void HTMLFormElementImpl::radioClicked( HTMLGenericFormElementImpl *caller )
{
    // ### make this work for form elements that don't have renders
    if(!view) return;

    HTMLGenericFormElementImpl *current;
    for (current = formElements.first(); current; current = formElements.next())
    {
	if (current->id() == ID_INPUT &&
	    static_cast<HTMLInputElementImpl*>(current)->inputType() == HTMLInputElementImpl::RADIO &&
	    current != caller && current->name() == caller->name()) {
	    static_cast<HTMLInputElementImpl*>(current)->setChecked(false);
	}
    }
}

void HTMLFormElementImpl::maybeSubmit()
{
    if(!view) return;

    int le = 0;
    int total = 0;

    // count number of LineEdits / total number

    HTMLGenericFormElementImpl *current;
    for (current = formElements.first(); current; current = formElements.next()) {
	if (!current->disabled() && !current->readOnly()) {
	    if (current->id() == ID_INPUT &&
	        (static_cast<HTMLInputElementImpl*>(current)->inputType() == HTMLInputElementImpl::TEXT ||
	         static_cast<HTMLInputElementImpl*>(current)->inputType() == HTMLInputElementImpl::PASSWORD))
		le++;

            // we're not counting hidden input's here, as they're not enabled (### check this)
	    total++;
	}
    }

    // if there's only one lineedit or only one possibly successful one, submit
    if (le < 2 || total < 2)
        submit();
}


void HTMLFormElementImpl::registerFormElement(HTMLGenericFormElementImpl *e)
{
    formElements.append(e);
}

void HTMLFormElementImpl::removeFormElement(HTMLGenericFormElementImpl *e)
{
    // ### make sure this get's called, when formElements get removed from
    // the document tree
    formElements.remove(e);
}


// -------------------------------------------------------------------------

HTMLGenericFormElementImpl::HTMLGenericFormElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f)
    : HTMLElementImpl(doc)
{
    _form = f;
    if (_form)
	_form->registerFormElement(this);

    view = 0;
    m_disabled = m_readOnly = false;

}

HTMLGenericFormElementImpl::HTMLGenericFormElementImpl(DocumentImpl *doc)
    : HTMLElementImpl(doc)
{
    _form = getForm();
    if (_form)
	_form->registerFormElement(this);

    view = 0;
    m_disabled = m_readOnly = false;
}

HTMLGenericFormElementImpl::~HTMLGenericFormElementImpl()
{
    if (_form)
	_form->removeFormElement(this);
}

void HTMLGenericFormElementImpl::parseAttribute(AttrImpl *attr)
{
    switch(attr->attrId)
    {
    case ATTR_NAME:
	_name = attr->value();
	break;
    case ATTR_DISABLED:
	m_disabled = attr->val() != 0;
	break;
    case ATTR_READONLY:
        m_readOnly = attr->val() != 0;
    default:
	HTMLElementImpl::parseAttribute(attr);
    }
}

HTMLFormElementImpl *HTMLGenericFormElementImpl::getForm() const
{
    NodeImpl *p = parentNode();
    while(p)
    {
	if( p->id() == ID_FORM )
	    return static_cast<HTMLFormElementImpl *>(p);
	p = p->parentNode();
    }
    kdDebug( 6030 ) << "couldn't find form!" << endl;
    return 0;
}


void HTMLGenericFormElementImpl::attach(KHTMLView *_view)
{
    view = _view;
    HTMLElementImpl::attach(_view);
}

void HTMLGenericFormElementImpl::detach()
{
    view = 0;
    HTMLElementImpl::detach();
}

void HTMLGenericFormElementImpl::onBlur()
{
    DOMString script = getAttribute(ATTR_ONBLUR);
    if (!script.isEmpty())
	view->part()->executeScript(script.string());
}

void HTMLGenericFormElementImpl::onFocus()
{
    DOMString script = getAttribute(ATTR_ONFOCUS);
    if (!script.isEmpty())
	view->part()->executeScript(script.string());
}

void HTMLGenericFormElementImpl::onSelect()
{
    DOMString script = getAttribute(ATTR_ONSELECT);
    if (!script.isEmpty())
	view->part()->executeScript(script.string());
}

void HTMLGenericFormElementImpl::onChange()
{
    DOMString script = getAttribute(ATTR_ONCHANGE);
    if (!script.isEmpty())
	view->part()->executeScript(script.string());


void HTMLGenericFormElementImpl::blur()
{
    static_cast<RenderFormElement*>(m_render)->blur();
//    onBlur(); // ### enable this - but kjs needs to support re-entry
}

void HTMLGenericFormElementImpl::focus()
{
    static_cast<RenderFormElement*>(m_render)->focus();
//    onFocus(); // ### enable this - but kjs needs to support re-entry
}

// -------------------------------------------------------------------------

HTMLButtonElementImpl::HTMLButtonElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f)
    : HTMLGenericFormElementImpl(doc, f)
{
    _clicked = false;
    _type = SUBMIT;
    dirty = true;
}

HTMLButtonElementImpl::HTMLButtonElementImpl(DocumentImpl *doc)
    : HTMLGenericFormElementImpl(doc)
{
    _clicked = false;
    _type = SUBMIT;
    dirty = true;
}

HTMLButtonElementImpl::~HTMLButtonElementImpl()
{
}

const DOMString HTMLButtonElementImpl::nodeName() const
{
    return "BUTTON";
}

ushort HTMLButtonElementImpl::id() const
{
    return ID_BUTTON;
}

long HTMLButtonElementImpl::tabIndex() const
{
    // ###
    return 0;
}

void HTMLButtonElementImpl::setTabIndex( long  )
{
}

DOMString HTMLButtonElementImpl::type() const
{
    // ###
    return DOMString();
}

void HTMLButtonElementImpl::parseAttribute(AttrImpl *attr)
{
    switch(attr->attrId)
    {
    case ATTR_TYPE:
	if ( strcasecmp( attr->value(), "submit" ) == 0 )
	    _type = SUBMIT;
	else if ( strcasecmp( attr->value(), "reset" ) == 0 )
	    _type = RESET;
	else if ( strcasecmp( attr->value(), "button" ) == 0 )
	    _type = BUTTON;
	break;
    case ATTR_VALUE:
	_value = attr->value();
	currValue = _value.string();
	break;
    case ATTR_TABINDEX:
    case ATTR_ACCESSKEY:
    case ATTR_ONFOCUS:
    case ATTR_ONBLUR:
	// ignore for the moment
	break;
//    case ATTR_NAME:
	// handled by parent class...
    default:
	HTMLGenericFormElementImpl::parseAttribute(attr);
    }
}

void HTMLButtonElementImpl::attach(KHTMLView *_view)
{
    view = _view;
    HTMLElementImpl::attach(_view);
}

// -------------------------------------------------------------------------

HTMLFieldSetElementImpl::HTMLFieldSetElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f)
    : HTMLGenericFormElementImpl(doc, f)
{
}

HTMLFieldSetElementImpl::HTMLFieldSetElementImpl(DocumentImpl *doc)
    : HTMLGenericFormElementImpl(doc)
{
}

HTMLFieldSetElementImpl::~HTMLFieldSetElementImpl()
{
}

const DOMString HTMLFieldSetElementImpl::nodeName() const
{
    return "FIELDSET";
}

ushort HTMLFieldSetElementImpl::id() const
{
    return ID_FIELDSET;
}

// -------------------------------------------------------------------------

HTMLInputElementImpl::HTMLInputElementImpl(DocumentImpl *doc)
    : HTMLGenericFormElementImpl(doc)
{
    _type = TEXT;
    m_checked = false;
    _maxLen = -1;
    _size = 20;
    _clicked = false;
    m_filename = "";

    view = 0;
}

HTMLInputElementImpl::HTMLInputElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f)
    : HTMLGenericFormElementImpl(doc, f)
{
    _type = TEXT;
    m_checked = false;
    _maxLen = -1;
    _size = 20;
    _clicked = false;
    m_filename = "";

    view = 0;
}

HTMLInputElementImpl::~HTMLInputElementImpl()
{
    ownerDocument()->removeElement(this);
}

const DOMString HTMLInputElementImpl::nodeName() const
{
    return "INPUT";
}

ushort HTMLInputElementImpl::id() const
{
    return ID_INPUT;
}

long HTMLInputElementImpl::tabIndex() const
{
    // ###
    return 0;
}

DOMString HTMLInputElementImpl::type() const
{
    // HTML DTD suggests this is supposed to be uppercase
    switch (_type) {
    case TEXT: return "TEXT";
    case PASSWORD: return "PASSWORD";
    case CHECKBOX: return "CHECKBOX";
    case RADIO: return "RADIO";
    case SUBMIT: return "SUBMIT";
    case RESET: return "RESET";
    case FILE: return "FILE";
    case HIDDEN: return "HIDDEN";
    case IMAGE: return "IMAGE";
    case BUTTON: return "BUTTON";
    default: return "";
    }
}

QString HTMLInputElementImpl::state( )
{
    switch (_type) {
    case TEXT:
    case PASSWORD:
        return m_value.string()+'.'; // Make sure the string is not empty!
    case CHECKBOX:
        return QString::fromLatin1(m_checked ? "on" : "off");
    case RADIO:
        return QString::fromLatin1(m_checked ? "on" : "off");
    case FILE:
        return m_filename.string()+'.';
    default:
        return QString::null;
    }
}



void HTMLInputElementImpl::restoreState(const QString &state)
{
    switch (_type) {
    case TEXT:
    case PASSWORD:
        m_value = DOMString(state.left(state.length()-1));
        break;
    case CHECKBOX:
    case RADIO:
        m_checked = state == QString::fromLatin1("on");
        break;
    case FILE:
        m_value = m_filename = DOMString(state.left(state.length()-1));
        break;
    default:
	break;
    }
    setChanged(true);
}


void HTMLInputElementImpl::blur(  )
{
    if (_type != IMAGE)
	HTMLGenericFormElementImpl::blur();
}

void HTMLInputElementImpl::focus(  )
{
    if (_type != IMAGE)
	HTMLGenericFormElementImpl::focus();
}

void HTMLInputElementImpl::select(  )
{
    if (_type == TEXT || _type == PASSWORD)
	static_cast<RenderLineEdit*>(m_render)->select();
    else if (_type == FILE)
	static_cast<RenderFileButton*>(m_render)->select();
}

void HTMLInputElementImpl::click(  )
{
    // ###
    kdDebug( 6030 ) << " HTMLInputElementImpl::click(  )" << endl;
}

void HTMLInputElementImpl::parseAttribute(AttrImpl *attr)
{
    // ### IMPORTANT: check that the type can't be changed after the first time
    // otherwise a javascript programmer may be able to set a text field's value
    // to something like /etc/passwd and then change it to a file field
    // ### also check that input defaults to something - text perhaps?
    switch(attr->attrId)
    {
    case ATTR_TYPE:
	if ( strcasecmp( attr->value(), "text" ) == 0 )
	    _type = TEXT;
	else if ( strcasecmp( attr->value(), "password" ) == 0 )
	    _type = PASSWORD;
	else if ( strcasecmp( attr->value(), "checkbox" ) == 0 )
	    _type = CHECKBOX;
	else if ( strcasecmp( attr->value(), "radio" ) == 0 )
	    _type = RADIO;
	else if ( strcasecmp( attr->value(), "submit" ) == 0 )
	    _type = SUBMIT;
	else if ( strcasecmp( attr->value(), "reset" ) == 0 )
	    _type = RESET;
	else if ( strcasecmp( attr->value(), "file" ) == 0 )
	    _type = FILE;
	else if ( strcasecmp( attr->value(), "hidden" ) == 0 )
	    _type = HIDDEN;
	else if ( strcasecmp( attr->value(), "image" ) == 0 )
	    _type = IMAGE;
	else if ( strcasecmp( attr->value(), "button" ) == 0 )
	    _type = BUTTON;
	break;
    case ATTR_VALUE:
	m_value = attr->value();
	break;
    case ATTR_CHECKED:
	setChecked(attr->val() != 0);
	break;
    case ATTR_MAXLENGTH:
	_maxLen = attr->val() ? attr->val()->toInt() : -1;
	break;
    case ATTR_SIZE:
	_size = attr->val() ? attr->val()->toInt() : 20;
	break;
    case ATTR_SRC:
	_src = attr->value();
	break;
    case ATTR_ALT:
    case ATTR_USEMAP:
    case ATTR_TABINDEX:
    case ATTR_ACCESSKEY:
	// ### ignore for the moment
	break;
    case ATTR_ALIGN:
	addCSSProperty(CSS_PROP_TEXT_ALIGN, attr->value(), false);
	break;
//    case ATTR_NAME:
	// handled by parent class...
    case ATTR_WIDTH:
	addCSSProperty(CSS_PROP_WIDTH, attr->value(), false);
	break;
    case ATTR_HEIGHT:
	addCSSProperty(CSS_PROP_HEIGHT, attr->value(), false);
	break;
	
    default:
	HTMLGenericFormElementImpl::parseAttribute(attr);
    }
}

void HTMLInputElementImpl::attach(KHTMLView *_view)
{
    m_style = document->styleSelector()->styleForElement(this);
    view = _view;

    khtml::RenderObject *r = _parent->renderer();
    if(r)
    {
	switch(_type)
	{
        case TEXT:
            m_render = new RenderLineEdit(view, this);
            break;
        case PASSWORD:
            m_render = new RenderLineEdit(view, this);
            break;
        case CHECKBOX:
            m_render = new RenderCheckBox(view, this);
            break;
        case RADIO:
            m_render = new RenderRadioButton(view, this);
            break;
        case SUBMIT:
            m_render = new RenderSubmitButton(view, this);
            break;
        case IMAGE:
        {
            RenderImageButton *i = new RenderImageButton(this);
//		i->setImageUrl(_src, static_cast<HTMLDocumentImpl *>(document)->URL(),
//	                   static_cast<HTMLDocumentImpl *>(document)->docLoader());
            m_render = i;
            setHasEvents();
            break;
        }
        case RESET:
            m_render = new RenderResetButton(view, this);
            break;
        case FILE:
            m_render = new RenderFileButton(view, this);
            break;
        case HIDDEN:
            m_render = 0;
            break;
        case BUTTON:
            m_render = new RenderPushButton(view, this);
            break;
	}
	
	if (m_render)
	{
	    m_render->setStyle(m_style);
	    m_render->ref();
            kdDebug( 6030 ) << "adding " << m_render->renderName() << " as child of " << r->renderName() << endl;
            QString state = document->registerElement(this);
            if ( !state.isEmpty())
            {
                kdDebug( 6030 ) << "Restoring InputElem name=" << _name.string() <<
                    " state=" << state << endl;
                restoreState( state );
            }

	    r->addChild(m_render, _next ? _next->renderer() : 0);
	}
    }
    NodeBaseImpl::attach(_view);
    if (_type == IMAGE) {
	static_cast<RenderImageButton*>(m_render)->setImageUrl(_src,
                                                               static_cast<HTMLDocumentImpl *>(document)->URL(),
                                                               static_cast<HTMLDocumentImpl *>(document)->docLoader());

    }
}


bool HTMLInputElementImpl::encoding(khtml::encodingList& encoding)
{
    if (_name.isEmpty()) return false;

    switch (_type) {
    case TEXT:
    case PASSWORD:
    case HIDDEN:

        if(!m_value.isEmpty())
        {
            encoding += m_value.string().local8Bit();
            return true;
        }

    case CHECKBOX:

        if( checked() )
        {
            encoding += ( m_value.isEmpty() ? QCString("on") : m_value.string().local8Bit() );
            return true;
        }

    case RADIO:

        if( checked() )
        {
            encoding += m_value.string().local8Bit();
            return true;
        }

    case RESET:
        // a reset button is never successful
        return false;
    case SUBMIT:

        if (m_render && static_cast<RenderSubmitButton*>(m_render)->clicked())
        {
            QCString enc_str = m_value.isNull() ?
                static_cast<RenderSubmitButton*>(m_render)->defaultLabel().local8Bit() : value().string().local8Bit();

            if (m_render)
                static_cast<RenderSubmitButton*>(m_render)->setClicked(false);

            if(!enc_str.isEmpty())
            {
                encoding += enc_str;
                return true;
            }
        }

    case FILE:
    {
        QString local;

        // if no filename at all is entered, return successful, however empty
        if(m_filename.isEmpty())
            return true;

        if ( KIO::NetAccess::download(KURL(m_filename.string()), local) )
        {
            QFile file(local);
            if (file.open(IO_ReadOnly))
            {
                QByteArray filearray(file.size()+1);
                file.readBlock( filearray.data(), file.size());
                // not really necessary, but makes it easier
                filearray[filearray.size()-1] = '\0';
                file.close();

                encoding += filearray;
                KIO::NetAccess::removeTempFile( local );

                return true;
            }
            else
            {
                KMessageBox::error(0L, i18n("Cannot open downloaded file.\nSubmit a bugreport"));
                KIO::NetAccess::removeTempFile( local );
                return false;
            }
        }
        else {
            KMessageBox::sorry(0L, i18n("Error downloading file:\n%1").arg(KIO::NetAccess::lastErrorString()));
            return false;
        }
    }
    }
    return false;
}

void HTMLInputElementImpl::saveDefaults()
{
    _defaultValue = getAttribute(ATTR_VALUE);
    _defaultChecked = (getAttribute(ATTR_CHECKED) != 0);
}

void HTMLInputElementImpl::reset()
{
    setAttribute(ATTR_VALUE,_defaultValue);
    setAttribute(ATTR_CHECKED,_defaultChecked);
    if ((_type == SUBMIT || _type == RESET || _type == BUTTON || _type == IMAGE) && m_render)
	static_cast<RenderSubmitButton*>(m_render)->setClicked(false);
}

void HTMLInputElementImpl::setChecked(bool _checked)
{
    m_checked = _checked;
    if (_type == RADIO && _form && m_render && m_checked)
	_form->radioClicked(this);
    setChanged(true);
}

void HTMLInputElementImpl::setValue(DOMString val)
{
    switch (_type) {
    case TEXT:
    case PASSWORD:
        m_value = val;
        setChanged(true);
        break;
    case FILE:
        // sorry, can't change this!
        m_value = m_filename;
    default:
        setAttribute(ATTR_VALUE,val);
    }
}

bool HTMLInputElementImpl::mouseEvent( int _x, int _y, int button, MouseEventType type,
                                       int _tx, int _ty, DOMString &url,
                                       NodeImpl *&innerNode, long &offset )
{
    bool wasPressed = pressed();
    bool ret = HTMLGenericFormElementImpl::mouseEvent(_x,_y,button,type,_tx,_ty,url,innerNode,offset);
    if (_type == IMAGE && (type == MouseClick || ((type == MouseRelease) && wasPressed))) {
	// ### if the above mouse event called a javascript which deleted us, then
	// we will probably crash here
	// ### submit co-ordinates clicked on as specified in html specs
	_form->submit();
	return true;
    }
    return ret;
}

// -------------------------------------------------------------------------

HTMLLabelElementImpl::HTMLLabelElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f)
    : HTMLGenericFormElementImpl(doc, f)
{
}

HTMLLabelElementImpl::HTMLLabelElementImpl(DocumentImpl *doc)
    : HTMLGenericFormElementImpl(doc)
{
}

HTMLLabelElementImpl::~HTMLLabelElementImpl()
{
}

const DOMString HTMLLabelElementImpl::nodeName() const
{
    return "LABEL";
}

ushort HTMLLabelElementImpl::id() const
{
    return ID_LABEL;
}

// -------------------------------------------------------------------------

HTMLLegendElementImpl::HTMLLegendElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f)
    : HTMLGenericFormElementImpl(doc, f)
{
}

HTMLLegendElementImpl::HTMLLegendElementImpl(DocumentImpl *doc)
    : HTMLGenericFormElementImpl(doc)
{
}

HTMLLegendElementImpl::~HTMLLegendElementImpl()
{
}

const DOMString HTMLLegendElementImpl::nodeName() const
{
    return "LEGEND";
}

ushort HTMLLegendElementImpl::id() const
{
    return ID_LEGEND;
}

// -------------------------------------------------------------------------

HTMLSelectElementImpl::HTMLSelectElementImpl(DocumentImpl *doc)
    : HTMLGenericFormElementImpl(doc)
{
    m_multiple = false;
    view = 0;
    // 0 means invalid (i.e. not set)
    m_size = 0;
    m_selectedIndex = -1;
}

HTMLSelectElementImpl::HTMLSelectElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f)
    : HTMLGenericFormElementImpl(doc, f)
{
    m_multiple = false;
    view = 0;
    // 0 means invalid (i.e. not set)
    m_size = 0;
    m_selectedIndex = -1;
}

ushort HTMLSelectElementImpl::id() const
{
    return ID_SELECT;
}

DOMString HTMLSelectElementImpl::type() const
{
    return (m_multiple ? "select-multiple" : "select-one");
}

long HTMLSelectElementImpl::selectedIndex() const
{
    return m_selectedIndex;
}

void HTMLSelectElementImpl::setSelectedIndex( long  index )
{
    m_selectedIndex = index;
    setChanged(true);
}

long HTMLSelectElementImpl::length() const
{
    int len = 0;
    NodeImpl *child;
    for (child = firstChild(); child; child = child->nextSibling())
	len++;
    return len;
}

void HTMLSelectElementImpl::add( const HTMLElement &/*element*/, const HTMLElement &/*before*/ )
{
    // ###
}

void HTMLSelectElementImpl::remove( long /*index*/ )
{
    // ###
}

QString HTMLSelectElementImpl::state( )
{
    return m_render ? static_cast<RenderSelect*>(m_render)->state() : QString::null;
}

void HTMLSelectElementImpl::restoreState(const QString &state)
{
    if (m_render)
	static_cast<RenderSelect*>(m_render)->restoreState(state);
    setChanged(true);
}

NodeImpl *HTMLSelectElementImpl::insertBefore ( NodeImpl *newChild, NodeImpl *refChild )
{
    NodeImpl *result = HTMLGenericFormElementImpl::insertBefore(newChild,refChild);
    if (m_render)
	static_cast<RenderSelect*>(m_render)->setOptionsChanged(true);
    return result;
}

NodeImpl *HTMLSelectElementImpl::replaceChild ( NodeImpl *newChild, NodeImpl *oldChild )
{
    NodeImpl *result = HTMLGenericFormElementImpl::replaceChild(newChild,oldChild);
    if (m_render)
	static_cast<RenderSelect*>(m_render)->setOptionsChanged(true);
    return result;
}

NodeImpl *HTMLSelectElementImpl::removeChild ( NodeImpl *oldChild )
{
    NodeImpl *result = HTMLGenericFormElementImpl::removeChild(oldChild);
    if (m_render)
	static_cast<RenderSelect*>(m_render)->setOptionsChanged(true);
    return result;
}

NodeImpl *HTMLSelectElementImpl::appendChild ( NodeImpl *newChild )
{
    NodeImpl *result = HTMLGenericFormElementImpl::appendChild(newChild);
    if (m_render)
	static_cast<RenderSelect*>(m_render)->setOptionsChanged(true);
    setChanged(true);
    return result;
}

void HTMLSelectElementImpl::parseAttribute(AttrImpl *attr)
{
    switch(attr->attrId)
    {
    case ATTR_SIZE:
	m_size = attr->val()->toInt();
	break;
    case ATTR_MULTIPLE:
	m_multiple = (attr->val() != 0);
	break;
    case ATTR_TABINDEX:
    case ATTR_ACCESSKEY:
	// ### ignore for the moment
	break;
    case ATTR_ONFOCUS:
    case ATTR_ONBLUR:
    case ATTR_ONSELECT:
    case ATTR_ONCHANGE:
	// ###
	break;
    default:
	HTMLGenericFormElementImpl::parseAttribute(attr);
    }
}

void HTMLSelectElementImpl::attach(KHTMLView *_view)
{
    m_style = document->styleSelector()->styleForElement(this);
    view = _view;

    khtml::RenderObject *r = _parent->renderer();
    if(r)
    {
        RenderSelect *f = new RenderSelect(view, this);
	if (f)
	{
    	    m_render = f;
	    m_render->setStyle(m_style);
	    m_render->ref();
	    r->addChild(m_render, _next ? _next->renderer() : 0);
	}
    }
    NodeBaseImpl::attach(_view);
}


bool HTMLSelectElementImpl::encoding(khtml::encodingList& encoded_value)
{
    return static_cast<RenderSelect*>(m_render)->encoding(encoded_value);
}


// -------------------------------------------------------------------------

HTMLOptGroupElementImpl::HTMLOptGroupElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f)
    : HTMLGenericFormElementImpl(doc, f)
{
}

HTMLOptGroupElementImpl::HTMLOptGroupElementImpl(DocumentImpl *doc)
    : HTMLGenericFormElementImpl(doc)
{
}

HTMLOptGroupElementImpl::~HTMLOptGroupElementImpl()
{
}

ushort HTMLOptGroupElementImpl::id() const
{
    return ID_OPTGROUP;
}

NodeImpl *HTMLOptGroupElementImpl::insertBefore ( NodeImpl *newChild, NodeImpl *refChild )
{
    NodeImpl *result = HTMLGenericFormElementImpl::insertBefore(newChild,refChild);
    recalcSelectOptions();
    return result;
}

NodeImpl *HTMLOptGroupElementImpl::replaceChild ( NodeImpl *newChild, NodeImpl *oldChild )
{
    NodeImpl *result = HTMLGenericFormElementImpl::replaceChild(newChild,oldChild);
    recalcSelectOptions();
    return result;
}

NodeImpl *HTMLOptGroupElementImpl::removeChild ( NodeImpl *oldChild )
{
    NodeImpl *result = HTMLGenericFormElementImpl::removeChild(oldChild);
    recalcSelectOptions();
    return result;
}

NodeImpl *HTMLOptGroupElementImpl::appendChild ( NodeImpl *newChild )
{
    NodeImpl *result = HTMLGenericFormElementImpl::appendChild(newChild);
    recalcSelectOptions();
    return result;
}

void HTMLOptGroupElementImpl::parseAttribute(AttrImpl *attr)
{
    HTMLGenericFormElementImpl::parseAttribute(attr);
    recalcSelectOptions();
}

void HTMLOptGroupElementImpl::recalcSelectOptions()
{
    NodeImpl *select = parentNode();
    while (select && select->id() != ID_SELECT)
	select = select->parentNode();
    if (select) {
	// ### also do this for other changes to optgroup and options
	select->setChanged(true);
	if (select->renderer())
	    static_cast<RenderSelect*>(select->renderer())->setOptionsChanged(true);
    }
}

// -------------------------------------------------------------------------

HTMLOptionElementImpl::HTMLOptionElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f)
    : HTMLGenericFormElementImpl(doc, f)
{
    m_selected = false;
}

HTMLOptionElementImpl::HTMLOptionElementImpl(DocumentImpl *doc)
    : HTMLGenericFormElementImpl(doc)
{
    m_selected = false;
}

const DOMString HTMLOptionElementImpl::nodeName() const
{
    return "OPTION";
}

ushort HTMLOptionElementImpl::id() const
{
    return ID_OPTION;
}

DOMString HTMLOptionElementImpl::text() const
{
    if(firstChild() && firstChild()->nodeType() == Node::TEXT_NODE) {
	return firstChild()->nodeValue();
    }
    else
	return DOMString();
}

long HTMLOptionElementImpl::index() const
{
    // ###
    return 0;
}

void HTMLOptionElementImpl::setIndex( long  )
{
    // ###
}

void HTMLOptionElementImpl::parseAttribute(AttrImpl *attr)
{
    switch(attr->attrId)
    {
    case ATTR_SELECTED:
        m_selected = true;
        break;
    case ATTR_VALUE:
        m_value = attr->value();
        break;
    default:
	HTMLGenericFormElementImpl::parseAttribute(attr);
    }
}

bool HTMLOptionElementImpl::selected() const
{
    return m_selected;
}

// -------------------------------------------------------------------------

HTMLTextAreaElementImpl::HTMLTextAreaElementImpl(DocumentImpl *doc)
    : HTMLGenericFormElementImpl(doc)
{
    // DTD requires rows & cols be specified, but we will provide reasonable defaults
    m_rows = 3;
    m_cols = 60;
    m_wrap = ta_Virtual;
}


HTMLTextAreaElementImpl::HTMLTextAreaElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f)
    : HTMLGenericFormElementImpl(doc, f)
{
    // DTD requires rows & cols be specified, but we will provide reasonable defaults
    m_rows = 3;
    m_cols = 60;
    m_wrap = ta_Virtual;
}

ushort HTMLTextAreaElementImpl::id() const
{
    return ID_TEXTAREA;
}

DOMString HTMLTextAreaElementImpl::type() const
{
    // ###
    return DOMString();
}

QString HTMLTextAreaElementImpl::state( )
{
    // Make sure the string is not empty!
    return m_value.string()+'.';
}

void HTMLTextAreaElementImpl::restoreState(const QString &state)
{
    m_value = DOMString(state.left(state.length()-1));
    setChanged(true);
}

void HTMLTextAreaElementImpl::select(  )
{
    if (m_render)
	static_cast<RenderTextArea*>(m_render)->select();
//    onSelect(); // ### enable this - but kjs needs to support re-entry
}

void HTMLTextAreaElementImpl::parseAttribute(AttrImpl *attr)
{
    switch(attr->attrId)
    {
    case ATTR_ROWS:
        m_rows = attr->val() ? attr->val()->toInt() : 3;
        break;
    case ATTR_COLS:
        m_cols = attr->val() ? attr->val()->toInt() : 60;
        break;
    case ATTR_WRAP:
        if ( strcasecmp( attr->value(), "virtual" ) == 0 )
            m_wrap = ta_Virtual;
        else if ( strcasecmp ( attr->value(), "physical" ) == 0)
            m_wrap = ta_Physical;
        else
            m_wrap = ta_NoWrap;
        break;
    case ATTR_TABINDEX:
    case ATTR_ACCESSKEY:
	// ignore for the moment
	break;
    case ATTR_ONFOCUS:
    case ATTR_ONBLUR:
    case ATTR_ONSELECT:
    case ATTR_ONCHANGE:
	// no need to parse
	break;
//    case ATTR_NAME:
	// handled by parent class...
    default:
	HTMLGenericFormElementImpl::parseAttribute(attr);
    }
}

void HTMLTextAreaElementImpl::attach(KHTMLView *_view)
{
    m_style = document->styleSelector()->styleForElement(this);
    view = _view;

    khtml::RenderObject *r = _parent->renderer();
    if(r)
    {
	RenderTextArea *f = new RenderTextArea(view, this);

	if (f)
	{
    	    m_render = f;
	    m_render->setStyle(m_style);
	    m_render->ref();
	    r->addChild(m_render, _next ? _next->renderer() : 0);
	
            QString state = document->registerElement(this);
            if (!state.isEmpty())
		restoreState( state );
	}
    }
    NodeBaseImpl::attach(_view);
}

bool HTMLTextAreaElementImpl::encoding(khtml::encodingList& encoding)
{
    if (_name.isEmpty() || !m_render) return false;

    // ### make this work independent of render
    encoding += static_cast<RenderTextArea*>(m_render)->text().local8Bit();

    return true;
}

void HTMLTextAreaElementImpl::reset()
{
    // ### check that this uses default value as specified in HTML file
    setValue(m_value);
}

void HTMLTextAreaElementImpl::setValue(DOMString _value)
{
    m_value = _value;
    setChanged(true);
}


void HTMLTextAreaElementImpl::saveDefaults()
{
    _defaultValue = getAttribute(m_value);
}









// -------------------------------------------------------------------------




