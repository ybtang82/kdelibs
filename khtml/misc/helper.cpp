/*
 * This file is part of the CSS implementation for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
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
#include "helper.h"
#include <qmap.h>
#include <qpainter.h>
#include <dom/dom_string.h>
// ### remove me...
#include <xml/dom_stringimpl.h>
#include <qlist.h>
#include <khtmllayout.h>
#include <kstaticdeleter.h>
//#include <kdebug.h>
using namespace DOM;
using namespace khtml;

// ### make it const if possible...
 struct HTMLColors {
    QMap<QString,QColor> map;
    HTMLColors()
    {
        map["black"] = "#000000";
        map["green"] = "#008000";
        map["silver"] = "#c0c0c0";
        map["lime"] = "#00ff00";
        map["gray"] = "#808080";
        map["olive"] = "#808000";
        map["white"] = "#ffffff";
        map["yellow"] = "#ffff00";
        map["maroon"] = "#800000";
        map["navy"] = "#000080";
        map["red"] = "#ff0000";
        map["blue"] = "#0000ff";
        map["purple"] = "#800080";
        map["teal"] = "#008080";
        map["fuchsia"] = "#ff00ff";
        map["aqua"] = "#00ffff";
    };
};

static HTMLColors *htmlColors = 0L;

static KStaticDeleter<HTMLColors> hcsd;

void khtml::setNamedColor(QColor &color, const QString &_name)
{
    if( !htmlColors )
        htmlColors = hcsd.setObject( new HTMLColors );

    QString name = _name.lower(); 
    int len = name.length();
    char ch = name[0].latin1();

    if(len == 0 || (len == 11 && name == "transparent"))
    {
        color = QColor(); // invalid color == transparent
        return;
    }

    // also recognize "color=ffffff"
    if (len == 6)
    {
        bool ok;
        int val = name.toInt(&ok, 16);
        if(ok)
        {
            color.setRgb((0xff << 24) | val);
            return;
        }
    }

    if ( len > 4 && ch == 'r' && name[1].latin1() == 'g' &&
         name[2].latin1() == 'b' && name[3].latin1() == '(' &&
         name[len-1].latin1() == ')')
    {
        // CSS like rgb(r, g, b) style
        DOMString rgb = name.mid(4, name.length()-5);
        QList<Length> *l = rgb.implementation()->toLengthList();
        if(l->count() != 3)
        {
            color.setRgb(0, 0, 0);
            delete l;
            return;
        }
        int r = l->at(0)->isUndefined() ? 0 : l->at(0)->width(255);
        if(r < 0) r = 0;
        if(r > 255) r = 255;
        int g = l->at(1)->isUndefined() ? 0 : l->at(1)->width(255);
        if(g < 0) g = 0;
        if(g > 255) g = 255;
        int b = l->at(2)->isUndefined() ? 0 : l->at(2)->width(255);
        if(b < 0) b = 0;
        if(b > 255) b = 255;

        color.setRgb(r, g, b);
        delete l;
    }
    else
    {
        const QColor& tc = htmlColors->map[name];
        if (tc.isValid())
            color = tc;
        else
            color.setNamedColor(name);
    }
}

static QPainter *printpainter = 0;

void khtml::setPrintPainter( QPainter *printer )
{
    printpainter = printer;
}

QFontMetrics khtml::fontMetrics( const QFont &f )
{
    if(printpainter) {
	printpainter->setFont( f );
	return printpainter->fontMetrics();
    }
    return QFontMetrics( f );
}
