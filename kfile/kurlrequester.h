/* This file is part of the KDE libraries
    Copyright (C) 1999,2000 Carsten Pfeiffer <pfeiffer@kde.org>

    library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/


#ifndef KURLREQUESTER_H
#define KURLREQUESTER_H

#include <qhbox.h>

#include <kurl.h>

class KFileDialog;
class KLineEdit;
class QString;
class QTimer;
class QToolButton;


/**
 * This class is a widget showing a lineedit and a button, which invokes a
 * filedialog. File completion is available in the lineedit.
 *
 * The defaults for the filedialog are to ask for one existing local file, i.e.
 * KFileDialog::setMode( KFile::File | KFile::ExistingOnly | KFile::LocalOnly )
 * The default filter is "*", i.e. show all files, and the start directory is
 * the current working directory, or the last directory where a file has been
 * selected.
 *
 * You can change this behavior by obtaining a pointer to the dialog 
 * (@ref fileDialog()) and calling the appropriate methods.
 *
 * @short A widget to request a filename/url from the user
 * @author Carsten Pfeiffer <pfeiffer@kde.org>
 */
class KURLRequester : public QHBox
{
    Q_OBJECT

public:
    /**
     * Constructs a KURLRequester widget
     * @param modal specifies whether the filedialog should be opened as modal
     * or not.
     */
    KURLRequester( QWidget *parent=0, const char *name=0, bool modal = true );
    
    /**
     * Constructs a KURLRequester widget with the default URL @param url
     * @param modal specifies whether the filedialog should be opened as modal
     * or not.
     */
    KURLRequester( const KURL& url, QWidget *parent=0, const char *name=0,
		   bool modal = true );
    
    /**
     * Destructs the KURLRequester
     */
    ~KURLRequester();

    /**
     * Sets the url in the lineedit to @param url. Depending on the state of
     * @ref showLocalProtocol(), file:/ on local files will be shown or not.
     */
    void 		setURL(const KURL& url);
    
    /**
     * @returns the current url in the lineedit. May be malformed, if the user
     * entered something weird.
     */
    KURL 		url() const;

    /**
     * Enables/disables showing file:/ in the lineedit, when a local file has
     * been selected in the filedialog or was set via @ref setURL().
     * Default is false, not showing file:/
     * @see #showLocalProtocol
     */
    void 		setShowLocalProtocol( bool b );
    
    /**
     * @returns whether local files will be prefixed with file:/ in the 
     * lineedit
     * @see #setShowLocalProtocol
     */
    bool 		showLocalProtocol() const { return myShowLocalProt; }

    /**
     * @returns a pointer to the filedialog
     * You can use this to customize the dialog, e.g. to specify a filter.
     * Never returns 0L.
     */
    KFileDialog *	fileDialog() const;


signals:
    // forwards from LineEdit
    /**
     * Emitted when the text in the lineedit changes.
     * The parameter contains the contents of the lineedit.
     */
    void 		textChanged(const QString&);
    
    /**
     * Emitted when return or enter was pressed in the lineedit.
     */
    void 		returnPressed();

    /**
     * Emitted when return or enter was pressed in the lineedit.
     * The parameter contains the contents of the lineedit.
     */
    void 		returnPressed( const QString& );


protected:
    void		init();

    KLineEdit *		myEdit;
    QToolButton *	myButton;
    bool 		myModal;


private:
    KURL 		myURL;
    bool 		myShowLocalProt;
    mutable KFileDialog * myFileDialog;


protected slots:
    /**
     * Called when the button is pressed to open the filedialog.
     * Also called when @ref KStdAccel::Open (default is Ctrl-O) is pressed.
     */
    void 		slotOpenDialog();
    
};

#endif // KURLREQUESTER_H
