/* This file is part of the KDE libraries
   Copyright (C) 2000 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kglobalsettings.h"

#include <qdir.h>
#include <kconfig.h>
#include <kapp.h>
#include <kipc.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kcharsets.h>
#include <kaccel.h>
#include <klocale.h>
#include <qfontinfo.h>

QString* KGlobalSettings::s_desktopPath = 0;
QString* KGlobalSettings::s_autostartPath = 0;
QString* KGlobalSettings::s_trashPath = 0;
QFont *KGlobalSettings::_generalFont = 0;
QFont *KGlobalSettings::_fixedFont = 0;
QFont *KGlobalSettings::_toolBarFont = 0;
QFont *KGlobalSettings::_menuFont = 0;
QColor *KGlobalSettings::kde2Blue = 0;
QColor *KGlobalSettings::kde2Gray = 0;


int KGlobalSettings::dndEventDelay()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, "General" );
    return c->readNumEntry("DndDelay", 2);
}

bool KGlobalSettings::singleClick()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, "KDE" );
    return c->readBoolEntry("SingleClick", KDE_DEFAULT_SINGLECLICK);
}

bool KGlobalSettings::insertTearOffHandle()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, "KDE" );
    return c->readBoolEntry("InsertTearOffHandle", KDE_DEFAULT_INSERTTEAROFFHANDLES);
}

bool KGlobalSettings::changeCursorOverIcon()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, "KDE" );
    return c->readBoolEntry("ChangeCursor", KDE_DEFAULT_CHANGECURSOR);
}

bool KGlobalSettings::visualActivate()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, "KDE" );
    return c->readBoolEntry("VisualActivate", KDE_DEFAULT_VISUAL_ACTIVATE);
}

unsigned int KGlobalSettings::visualActivateSpeed()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, "KDE" );
    return
        c->readNumEntry(
            "VisualActivateSpeed",
            KDE_DEFAULT_VISUAL_ACTIVATE_SPEED
        );
}



int KGlobalSettings::autoSelectDelay()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, "KDE" );
    return c->readNumEntry("AutoSelectDelay", KDE_DEFAULT_AUTOSELECTDELAY);
}

KGlobalSettings::Completion KGlobalSettings::completionMode()
{
    int completion;
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, "General" );
    completion = c->readNumEntry("completionMode", -1);
    if ((completion < (int) CompletionNone) ||
        (completion > (int) CompletionShell))
      {
        completion = (int) CompletionAuto; // Default
      }
  return (Completion) completion;
}

bool KGlobalSettings::showContextMenusOnPress ()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs (c, "ContextMenus");

    return cgs.config()->readBoolEntry("ShowOnPress", true);
}

int KGlobalSettings::contextMenuKey ()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs (c, "Keys");

    return KAccel::stringToKey (cgs.config()->readEntry ("PopupContextMenu", "Menu"));
}

bool KGlobalSettings::honorGnome()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, "General" );
    return c->readBoolEntry("honorGnome", false);
}

QColor KGlobalSettings::toolBarHighlightColor()
{
    initColors();
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("Toolbar style") );
    return c->readColorEntry("HighlightColor", kde2Blue);
}

QColor KGlobalSettings::inactiveTitleColor()
{
    if (!kde2Gray)
        kde2Gray = new QColor(220, 220, 220);
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("WM") );
    return c->readColorEntry( "inactiveBackground", kde2Gray );
}

QColor KGlobalSettings::inactiveTextColor()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("WM") );
    return c->readColorEntry( "inactiveForeground", &Qt::darkGray );
}

QColor KGlobalSettings::activeTitleColor()
{
    initColors();
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("WM") );
    return c->readColorEntry( "activeBackground", kde2Blue);
}

QColor KGlobalSettings::activeTextColor()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("WM") );
    return c->readColorEntry( "activeForeground", &Qt::white );
}

int KGlobalSettings::contrast()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("KDE") );
    return c->readNumEntry( "contrast", 7 );
}

// IMPORTANT:
//  This function should be get in sync with
//   KApplication::kdisplaySetPalette()
QColor KGlobalSettings::baseColor()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("General") );
    return c->readColorEntry( "windowBackground", &Qt::white );
}

// IMPORTANT:
//  This function should be get in sync with
//   KApplication::kdisplaySetPalette()
QColor KGlobalSettings::textColor()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("General") );
    return c->readColorEntry( "windowForeground", &Qt::black );
}

// IMPORTANT:
//  This function should be get in sync with
//   KApplication::kdisplaySetPalette()
QColor KGlobalSettings::highlightedTextColor()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("General") );
    return c->readColorEntry( "selectForeground", &Qt::white );
}

// IMPORTANT:
//  This function should be get in sync with
//   KApplication::kdisplaySetPalette()
QColor KGlobalSettings::highlightColor()
{
    initColors();
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("General") );
    return c->readColorEntry( "selectBackground", kde2Blue );
}

QColor KGlobalSettings::linkColor()
{
    initColors();
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("General") );
    return c->readColorEntry( "linkColor", kde2Blue );
}

QColor KGlobalSettings::visitedLinkColor()
{
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("General") );
    return c->readColorEntry( "visitedLinkColor", &Qt::magenta );
}

QFont KGlobalSettings::generalFont()
{
    if (_generalFont)
        return *_generalFont;

    _generalFont = new QFont("helvetica", 12, QFont::Normal);
    KCharsets *charsets = KGlobal::charsets();
    charsets->setQFont(*_generalFont, charsets->charsetForLocale());

    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("General") );
    *_generalFont = c->readFontEntry("font", _generalFont);
    return *_generalFont;
}

QFont KGlobalSettings::fixedFont()
{
    if (_fixedFont)
        return *_fixedFont;

    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("General") );
    _fixedFont = new QFont(c->readFontEntry("fixed"));

    if(!QFontInfo(*_fixedFont).fixedPitch() ) {
        *_fixedFont = QFont("fixed",
                            _fixedFont->pointSize(), QFont::Normal);
        _fixedFont->setStyleHint(QFont::Courier);
        _fixedFont->setFixedPitch(true);
        KCharsets *charsets = KGlobal::charsets();
        charsets->setQFont(*_fixedFont, charsets->charsetForLocale());
    }
    return *_fixedFont;
}

QFont KGlobalSettings::toolBarFont()
{
    if(_toolBarFont)
        return *_toolBarFont;

    _toolBarFont = new QFont("helvetica", 10, QFont::Normal);
    KCharsets *charsets = KGlobal::charsets();
    charsets->setQFont(*_toolBarFont, charsets->charsetForLocale());

    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("General") );
    *_toolBarFont = c->readFontEntry(QString::fromLatin1("toolBarFont"), _toolBarFont);
    return *_toolBarFont;
}

QFont KGlobalSettings::menuFont()
{
    if(_menuFont)
        return *_menuFont;

    _menuFont = new QFont("helvetica", 12);
    KCharsets *charsets = KGlobal::charsets();
    charsets->setQFont(*_menuFont, charsets->charsetForLocale());

    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("General") );
    *_menuFont = c->readFontEntry(QString::fromLatin1("menuFont"), _menuFont);
    return *_menuFont;
}

void KGlobalSettings::initStatic() // should be called initPaths(). Don't put anything else here.
{
    if ( s_desktopPath != 0 )
        return;
    kdDebug() << "KGlobalSettings::initStatic" << endl;

    s_desktopPath = new QString();
    s_autostartPath = new QString();
    s_trashPath = new QString();

    KConfig *config = KGlobal::config();
    bool dollarExpansion = config->isDollarExpansion();
    config->setDollarExpansion(true);
    KConfigGroupSaver cgs( config, "Paths" );

    // Desktop Path
    *s_desktopPath = QDir::homeDirPath() + "/" + "Desktop" + "/";
    *s_desktopPath = config->readEntry( "Desktop", *s_desktopPath);
    if ( (*s_desktopPath)[0] != '/' )
      s_desktopPath->prepend( QDir::homeDirPath() + "/" );
    *s_desktopPath = QDir::cleanDirPath( *s_desktopPath );
    if ( s_desktopPath->right(1) != "/")
        *s_desktopPath += "/";

    // Trash Path
    *s_trashPath = *s_desktopPath + "Trash" + "/";
    *s_trashPath = config->readEntry( "Trash" , *s_trashPath);
    *s_trashPath = QDir::cleanDirPath( *s_trashPath );
    if ( s_trashPath->right(1) != "/")
        *s_trashPath += "/";

    // Autostart Path
    *s_autostartPath = KGlobal::dirs()->localkdedir() + "Autostart" + "/";
    *s_autostartPath = config->readEntry( "Autostart" , *s_autostartPath);
    if ( (*s_autostartPath)[0] != '/' )
      s_autostartPath->prepend( QDir::homeDirPath() + "/" );
    *s_autostartPath = QDir::cleanDirPath( *s_autostartPath );
    if ( s_autostartPath->right(1) != "/")
        *s_autostartPath += "/";

    config->setDollarExpansion(dollarExpansion);

    // Make sure this app gets the notifications about those paths
    kapp->addKipcEventMask(KIPC::SettingsChanged);
}

void KGlobalSettings::initColors()
{
    if (!kde2Blue)
        kde2Blue = new QColor(0, 128, 128);
}

void KGlobalSettings::rereadFontSettings()
{
    delete _generalFont;
    _generalFont = 0L;
    delete _fixedFont;
    _fixedFont = 0L;
    delete _menuFont;
    _menuFont = 0L;
    delete _toolBarFont;
    _toolBarFont = 0L;
}

void KGlobalSettings::rereadPathSettings()
{
    kdDebug() << "KGlobalSettings::rereadPathSettings" << endl;
    delete s_autostartPath;
    s_autostartPath = 0L;
    delete s_trashPath;
    s_trashPath = 0L;
    delete s_desktopPath;
    s_desktopPath = 0L;
}
