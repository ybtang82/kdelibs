/*-
 * HCStyle (C)2000 Daniel M. Duley <mosfet@kde.org>
 * Animated menu code based on code by Mario Weilguni <mweilguni@kde.org>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */


#ifndef INCLUDE_MENUITEM_DEF
#define INCLUDE_MENUITEM_DEF
#endif

#include <qmenudata.h>
#include "hcstyle.h"
#include <kapp.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kdrawutil.h>
#include <kglobalsettings.h>
#include <kpixmapeffect.h>
#include <qpalette.h>
#include <qbitmap.h>
#include <qtabbar.h>
#include <qpopupmenu.h>
#include <qobjectlist.h>
#include <qimage.h>
#include <qtimer.h>
#include <kimageeffect.h>
#include <ktoolbar.h>
#include <qdrawutil.h>
#include <unistd.h>
#include "paperbits.h"

#include "bitmaps.h"

HCAniMenu::HCAniMenu(QPopupMenu *menu)
  : QObject(menu), mnu(menu), widget(0L)
{
  KConfig *config = KGlobal::config();
  QString oldGrp = config->group();
  config->setGroup("B2");
  animationDelay = config->readNumEntry("MenuAnimationDelay", 3);
  config->setGroup(oldGrp);

  menu->installEventFilter(this);

  // We should be cleaned up when the menu is destroyed..
//     connect(mnu, SIGNAL(destroyed()), this, SLOT(slotFinished()));
}

HCAniMenu::~HCAniMenu()
{
    if(widget)
        delete widget;
}

bool HCAniMenu::eventFilter(QObject *, QEvent *ev)
{
  if(ev->type() == QEvent::Show)
    scrollIn();
  else if(ev->type() == QEvent::Hide){
    scrollOut();
  }
  return(false);
}

void HCAniMenu::scrollIn()
{
  QTime t;
  int w = mnu->width();
  int h = mnu->height();
  int steps = QMIN(w, h) / 10;
  
  bgPix = QPixmap::grabWindow(QApplication::desktop()->winId(),
				    mnu->x(), mnu->y(), w, h);

  mnuPix.resize(w, h);
  mnuPix.fill(mnu->colorGroup().color(QColorGroup::Background));
  QPainter::redirect(mnu, &mnuPix);
  mnu->repaint(false);
  QPainter::redirect(mnu, 0);
  if(!widget)
    widget = new QWidget(0, 0, WStyle_Customize | WStyle_NoBorder |
			 WStyle_Tool | WType_Popup );
  widget->setFocusPolicy(QWidget::StrongFocus);
  widget->move(mnu->x(), mnu->y());
  widget->resize(w, h);
  widget->setBackgroundMode(QWidget::NoBackground);
  widget->show();
  bitBlt(widget, 0, 0, &bgPix);

  for(int x = 1; x <= steps; x++)
    {
      t.start();
      while(t.elapsed() <= animationDelay );
      
      int howMuch = (int)(float(x) / float(steps) * w);

      bitBlt(widget, 0, 0, &mnuPix, w - howMuch, 0, howMuch, h);
      kapp->syncX();
    }
  QTimer::singleShot(1, this, SLOT(slotDestroyFake()));
}

void HCAniMenu::scrollOut()
{
  QTime t;
  int w = mnu->width();
  int h = mnu->height();
  int steps = QMIN(w, h) / 10;
     
  if(!widget)
    widget = new QWidget(0, 0, WStyle_Customize | WStyle_NoBorder |
			 WStyle_Tool | WType_Popup );
  widget->move(mnu->x(), mnu->y());
  widget->resize(w, h);
  widget->setBackgroundMode(QWidget::NoBackground);
  widget->show();
  bitBlt(widget, 0, 0, &mnuPix);

  for(int x = steps; x >= 0; x--)
    {
      t.start();
      while(t.elapsed() <= animationDelay );

      int howMuch = (int)(float(x) / float(steps) * w);

      bitBlt(widget, 0 + howMuch, 0, &bgPix, 0 + howMuch, 0, w - howMuch, h);
      bitBlt(widget, 0, 0, &mnuPix, w - howMuch, 0, howMuch, h);
      kapp->syncX();
    }
  QTimer::singleShot(1, this, SLOT(slotDestroyFake()));
}

void HCAniMenu::slotDestroyFake()
{
    if(widget)
        delete(widget);
    widget = NULL;
}

void HCAniMenu::slotFinished()
{
    if(widget)
        delete(widget);
    // We'll be deleted when the menu is destroyed..
//     delete this;
}


HCStyle::HCStyle()
    :KStyle()
{
    KConfig *config = KGlobal::config();
    QString oldGrp = config->group();
    QPalette p = kapp->palette();
    setButtonDefaultIndicatorWidth(0);
    if(QPixmap::defaultDepth() > 8){
        vSmall = new KPixmap;
        vSmall->resize(18, 24);
        vMed = new KPixmap;
        vMed->resize(18, 34);
        vLarge = new KPixmap;
        vLarge->resize(18, 64);
        hMed = new KPixmap;
        hMed->resize(34, 18);
        hLarge = new KPixmap;
        hLarge->resize(52, 18);
    }
    else{
        vSmall = vMed = vLarge = NULL;
        hMed = hLarge = NULL;
    }
}

HCStyle::~HCStyle()
{
    if(vSmall){
        delete vSmall;
        delete vMed;
        delete vLarge;
        delete hMed;
        delete hLarge;
    }
}

void HCStyle::polish(QPalette &)
{
    KConfig *config = KGlobal::config();
    QString oldGrp = config->group();
    config->setGroup("B2");
    QPalette pal = QApplication::palette();

    // if the config has specific colors for items set use those and don't
    // worry about high color usage, otherwise use KDE standard colors for
    // everything except one color - the slider groove fill. That doesn't
    // really look good with any of the standard colors and one additional
    // color alloc shouldn't kill psudeocolor display users :P
    QColor tmpColor(0, 0, 192);
    if(config->hasKey("RadioOnColor")){
        tmpColor = config->readColorEntry("RadioOnColor", &tmpColor);
        radioOnGrp.setColor(QColorGroup::Mid, tmpColor);
        radioOnGrp.setColor(QColorGroup::Button, tmpColor);
        radioOnGrp.setColor(QColorGroup::Light, tmpColor.light(130));
        radioOnGrp.setColor(QColorGroup::Dark, tmpColor.dark(130));
    }
    else{
        radioOnGrp.setColor(QColorGroup::Mid, tmpColor);
        radioOnGrp.setColor(QColorGroup::Button, tmpColor);
        radioOnGrp.setColor(QColorGroup::Light, QColor(0, 0, 255));
        radioOnGrp.setColor(QColorGroup::Dark, QColor(0, 0, 128));
    }
    
    QColorGroup g = pal.active();
    tmpColor = g.background();
    if(vSmall){
        KPixmapEffect::gradient(*vSmall, g.midlight(),
                                g.mid(),
                                KPixmapEffect::VerticalGradient);
        KPixmapEffect::gradient(*vMed, g.midlight(),
                                g.mid(),
                                KPixmapEffect::VerticalGradient);
        KPixmapEffect::gradient(*vLarge, g.midlight(),
                                g.mid(),
                                KPixmapEffect::VerticalGradient);
        KPixmapEffect::gradient(*hMed, g.midlight(),
                                g.mid(),
                                KPixmapEffect::HorizontalGradient);
        KPixmapEffect::gradient(*hLarge, g.midlight(),
                                g.mid(),
                                KPixmapEffect::HorizontalGradient);
    }
    QString tmpStr = config->readEntry("CustomWallpaper", "");
    if(!tmpStr.isEmpty()){
        QPixmap wallPaper(tmpStr);
        if(!wallPaper.isNull())
            pal.setBrush(QColorGroup::Background,
                 QBrush(pal.color(QPalette::Active, QColorGroup::Background),
                        wallPaper));
        else
            warning("Highcolor B2: Unable to load wallpaper %s",
                    tmpStr.latin1());
    }
    else if(config->readBoolEntry("UseWallpaper", true)){
        QPixmap wallPaper;
        makeWallpaper(wallPaper,pal.color(QPalette::Active,
                                          QColorGroup::Background));
        pal.setBrush(QColorGroup::Background,
                     QBrush(pal.color(QPalette::Active, QColorGroup::Background),
                            wallPaper));
    }
    config->setGroup(oldGrp);
}

void HCStyle::polish(QWidget *w)
{
    /*if ( !w->isTopLevel() ) {
        if (w->inherits("QPushButton")
            || w->inherits("QComboBox")
            //|| w->inherits("QSlider")
            || w->inherits("QRadioButton")
            || w->inherits("QCheckBox"))
            w->setAutoMask(true);
            }*/
    if(w->isTopLevel())
        return;

    if(w->inherits("QMenuBar") || w->inherits("KToolBarButton")){
        w->setBackgroundMode(QWidget::NoBackground);
        return;
    }
    if(w->inherits("QLabel") || w->inherits("QButton") ||
       w->inherits("QComboBox")){
        if(w->parent() && !w->parent()->inherits("KToolBar") &&
           !w->parent()->inherits("KHTMLView"))
            w->setBackgroundOrigin(QWidget::ParentOrigin);
        else
            w->setAutoMask(true);
        return;
    }
    if(w->inherits("KToolBar")){
        w->installEventFilter(this);
        w->setBackgroundMode(QWidget::NoBackground);
        return;
    }
}
 
void HCStyle::unPolish(QWidget *w)
{
    if (w->isTopLevel())
        return;

    if(w->inherits("QMenuBar") || w->inherits("KToolBarButton")){
        w->setBackgroundMode(QWidget::PaletteBackground);
        return;
    }
    if(w->inherits("QLabel") || w->inherits("QButton") ||
       w->inherits("QComboBox")){
        if(w->parent() && !w->parent()->inherits("KToolBar") &&
           !w->parent()->inherits("KHTMLView"))
            w->setBackgroundOrigin(QWidget::WidgetOrigin);
        else
            w->setAutoMask(false);
        return;
    }
    if(w->inherits("KToolBar")){
        w->removeEventFilter(this);
        w->setBackgroundMode(QWidget::PaletteBackground);
        return;
    }
}


bool HCStyle::eventFilter(QObject *obj, QEvent *ev)
{
    if(ev->type() == QEvent::Resize){
        // must be a toolbar resize
        QObjectList *tbChildList = obj->queryList("KToolBarButton", NULL,
                                                  false, false);
        QObjectListIt it(*tbChildList);
        QObject *child;
        while((child = it.current()) != NULL){
            ++it;
            if(child->isWidgetType())
                ((QWidget *)child)->repaint(false);
        }
    }
    return(false);
}

void HCStyle::drawButton(QPainter *p, int x, int y, int w, int h,
                            const QColorGroup &g, bool sunken,
                            const QBrush *fill)
{
    if(vSmall){
        int x2 = x+w-1;
        int y2 = y+h-1;
        p->setPen(g.dark());
        p->drawLine(x+1, y, x2-1, y);
        p->drawLine(x+1, y2, x2-1, y2);
        p->drawLine(x, y+1, x, y2-1);
        p->drawLine(x2, y+1, x2, y2-1);

        if(!sunken){
            p->setPen(g.light());
            p->drawLine(x+2, y+2, x2-1, y+2);
            p->drawLine(x+2, y+3, x2-2, y+3);
            p->drawLine(x+2, y+4, x+2, y2-1);
            p->drawLine(x+3, y+4, x+3, y2-2);
        }
        else{
            p->setPen(g.mid());
            p->drawLine(x+2, y+2, x2-1, y+2);
            p->drawLine(x+2, y+3, x2-2, y+3);
            p->drawLine(x+2, y+4, x+2, y2-1);
            p->drawLine(x+3, y+4, x+3, y2-2);
        }
        p->setPen(sunken? g.light() : g.mid());
        p->drawLine(x2-1, y+2, x2-1, y2-1);
        p->drawLine(x+2, y2-1, x2-1, y2-1);

        p->setPen(g.mid());
        p->drawLine(x+1, y+1, x2-1, y+1);
        p->drawLine(x+1, y+2, x+1, y2-1);
        p->drawLine(x2-2, y+3, x2-2, y2-2);

        drawVGradient(p, g.brush(QColorGroup::Mid), x+4, y+4, w-6, h-6);
    }
    else
        kDrawBeButton(p, x, y, w, h, g, sunken, fill);
}

void HCStyle::drawPushButton(QPushButton *btn, QPainter *p)
{
    QRect r = btn->rect();
    bool sunken = btn->isOn() || btn->isDown();
    QColorGroup g = btn->colorGroup();
    
    drawButton(p, r.x(), r.y(), r.width(), r.height(), g,
               sunken, sunken ? &g.brush(QColorGroup::Mid) :
               btn->isDefault() ? &g.brush(QColorGroup::Midlight) :
               &g.brush(QColorGroup::Button));
    if(btn->isDefault()){
        p->setPen(Qt::black);
        p->drawLine(r.x()+1, r.y(), r.right()-1, r.y());
        p->drawLine(r.x()+1, r.bottom(), r.right()-1, r.bottom());
        p->drawLine(r.x(), r.y()+1, r.x(), r.bottom()-1);
        p->drawLine(r.right(), r.y()+1, r.right(), r.bottom()-1);
    }

}

void HCStyle::drawPushButtonLabel(QPushButton *btn, QPainter *p)
{
    int x1, y1, x2, y2;
    btn->rect().coords(&x1, &y1, &x2, &y2);

    bool act = btn->isOn() || btn->isDown();
    if(act){
        ++x1, ++y1;
    }

    if(act || btn->isDefault()){
        QFont font = btn->font();
        font.setBold(true);
        p->setFont(font);
        drawItem(p, x1+1, y1+1, btn->width(), btn->height(),
                 AlignCenter | ShowPrefix, btn->colorGroup(), btn->isEnabled(),
                 btn->pixmap(), btn->text(), -1, act ?
                 &btn->colorGroup().dark() : &btn->colorGroup().mid());

        drawItem(p, x1, y1, btn->width(), btn->height(),
                 AlignCenter | ShowPrefix, btn->colorGroup(), btn->isEnabled(),
                 btn->pixmap(), btn->text(), -1, act ?
                 &btn->colorGroup().light() : &btn->colorGroup().text());
    }
    else{
        drawItem(p, x1+act?1:0, y1+act?1:0, btn->width(), btn->height(),
                 AlignCenter | ShowPrefix, btn->colorGroup(), btn->isEnabled(),
                 btn->pixmap(), btn->text(), -1,
                 act ? &btn->colorGroup().light() : &btn->colorGroup().buttonText());
    }
}

void HCStyle::drawButtonMask(QPainter *p, int x, int y, int w, int h)
{
    int x2 = x+w-1;
    int y2 = y+h-1;
    p->fillRect(x, y, w, h, QBrush(color1, SolidPattern));
    p->setPen(color0);
    p->drawPoint(x, y);
    p->drawPoint(x2, y);
    p->drawPoint(x, y2);
    p->drawPoint(x2, y2);
}

void HCStyle::drawBevelButton(QPainter *p, int x, int y, int w, int h,
                              const QColorGroup &g, bool sunken,
                              const QBrush *fill)
{
    // I like black instead of dark() for the darkest shade
    int x2 = x+w-1;
    int y2 = y+h-1;
    p->setPen(sunken ? Qt::black : g.light());
    p->drawLine(x, y, x2, y);
    p->drawLine(x, y, x, y2);
    p->setPen(sunken ? g.light() : Qt::black);
    p->drawLine(x, y2, x2, y2);
    p->drawLine(x2, y, x2, y2);

    p->setPen(sunken ? g.dark() : g.midlight());
    p->drawLine(x+1, y+1, x2-1, y+1);
    p->drawLine(x+1, y+1, x+1, y2-1);
    p->setPen(sunken ? g.midlight() : g.dark());
    p->drawLine(x+1, y2-1, x2-1, y2-1);
    p->drawLine(x2-1, y+1, x2-1, y2-1);
    if(vSmall)
        drawVGradient(p, g.brush(QColorGroup::Mid), x+2, y+2, w-4, h-4);
    else
        p->fillRect(x+2, y+2, w-4, h-4, fill ? *fill :
                    g.brush(QColorGroup::Button));

}

QRect HCStyle::buttonRect(int x, int y, int w, int h)
{
    return(QRect(x+4, y+4, w-6, h-6));
}

void HCStyle::drawComboButton(QPainter *p, int x, int y, int w, int h,
                                 const QColorGroup &g, bool sunken,
                                 bool edit, bool, const QBrush *)
{
    int x2 = x+w-1;
    int y2 = y+h-1;

    p->setPen(g.dark());
    p->drawLine(x+1, y, x2-1, y);
    p->drawLine(x+1, y2, x2-1, y2);
    p->drawLine(x, y+1, x, y2-1);
    p->drawLine(x2, y+1, x2, y2-1);
    
    if(vSmall)
        drawVGradient(p, g.brush(QColorGroup::Mid), x+2, y+2, w-4, h-4);
    else
        p->fillRect(x+2, y+2, w-4, h-4, g.brush(QColorGroup::Background));

    p->setPen(sunken? g.light() : g.mid());
    p->drawLine(x2-1, y+2, x2-1, y2-1);
    p->drawLine(x+1, y2-1, x2-1, y2-1);

    p->setPen(sunken? g.mid() : g.light());
    p->drawLine(x+1, y+1, x2-1, y+1);
    p->drawLine(x+1, y+2, x+1, y2-2);

    p->setPen(g.dark());
    p->drawPoint(x+1, y+1);

    if(edit){
        x2 = x2-17;
        y2 = y2-3;
        p->setPen(g.dark());
        //p->drawRect(x+2, y+2, x2, y2);
        p->drawLine(x+3, y+2, x2, y+2);
        p->drawLine(x+2, y+3, x+2, y2);
    }
    
    int arrow_h = h / 3;
    int arrow_w = arrow_h;
    int arrow_x = w - arrow_w - 6;
    int arrow_y = (h - arrow_h) / 2;
    drawArrow(p, DownArrow, false, arrow_x, arrow_y, arrow_w, arrow_h, g, true);
}

void HCStyle::drawComboButtonMask(QPainter *p, int x, int y, int w, int h)
{
    int x2 = x+w-1;
    int y2 = y+h-1;
    p->fillRect(x, y, w, h, QBrush(color1, SolidPattern));
    p->setPen(color0);
    p->drawPoint(x, y);
    p->drawPoint(x, y+1);
    p->drawPoint(x+1, y);
    p->drawPoint(x2, y);
    p->drawPoint(x, y2);
    p->drawPoint(x2, y2);
}

QRect HCStyle::comboButtonRect(int x, int y, int w, int h)
{
    return(QRect(x+3, y+3, w - (h / 3) - 13, h-6));
}

QRect HCStyle::comboButtonFocusRect(int x, int y, int w, int h)
{
    return(QRect(x+3, y+3, w-(h/3)-13, h-6));
}


void HCStyle::drawScrollBarControls(QPainter *p, const QScrollBar *sb,
                                       int sliderStart, uint controls,
                                       uint activeControl)
{
    int sliderMin, sliderMax, sliderLength, buttonDim;
    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );
 
    if (sliderStart > sliderMax) 
        sliderStart = sliderMax;

    bool horiz = sb->orientation() == QScrollBar::Horizontal;
    QColorGroup g = sb->colorGroup();
    QRect addB, subHC, subB;
    QRect addPageR, subPageR, sliderR;
    int addX, addY, subX, subY;
    int len = horiz ? sb->width() : sb->height();
    int extent = horiz ? sb->height() : sb->width();
 
    if (horiz) {
        subY = addY = ( extent - buttonDim ) / 2;
        subX = 0;
        addX = len - buttonDim;
    }
    else {
        subX = addX = ( extent - buttonDim ) / 2;
        subY = 0;
        addY = len - buttonDim;
    }
    subB.setRect( subX,subY,buttonDim,buttonDim );
    addB.setRect( addX,addY,buttonDim,buttonDim );
    if(horiz)
        subHC.setRect(addX-buttonDim,addY,buttonDim,buttonDim );
    else
        subHC.setRect(addX,addY-buttonDim,buttonDim,buttonDim );
        
    int sliderEnd = sliderStart + sliderLength;
    int sliderW = extent;

    if (horiz) {
        subPageR.setRect( subB.right() + 1, 0,
                          sliderStart - subB.right() - 1 , sliderW );
        addPageR.setRect( sliderEnd, 0, addX - sliderEnd - buttonDim, sliderW );
        sliderR .setRect( sliderStart, 0, sliderLength, sliderW );
    }
    else {
        subPageR.setRect( 0, subB.bottom() + 1, sliderW,
                          sliderStart - subB.bottom() - 1 );
        addPageR.setRect( 0, sliderEnd, sliderW, addY - buttonDim - sliderEnd);
        sliderR .setRect( 0, sliderStart, sliderW, sliderLength );
    }
    
    bool maxed = sb->maxValue() == sb->minValue();
    
    if ( controls & AddLine ) {
        drawSBButton(p, addB, g, activeControl == AddLine);
        drawArrow( p, horiz ? RightArrow : DownArrow,
                   false, addB.x()+4, addB.y()+4,
                   addB.width()-8, addB.height()-8, g, !maxed);
    }
    if ( controls & SubLine ) {
        drawSBButton(p, subB, g, activeControl == SubLine);
        drawArrow( p, horiz ? LeftArrow : UpArrow,
                   false, subB.x()+4, subB.y()+4,
                   subB.width()-8, subB.height()-8, g, !maxed);
        drawSBButton(p, subHC, g, activeControl == SubLine);
        drawArrow( p, horiz ? LeftArrow : UpArrow,
                   false, subHC.x()+4, subHC.y()+4,
                   subHC.width()-8, subHC.height()-8, g, !maxed);
    }
    if(controls & AddPage){
        if(addPageR.width()){
            p->setPen(g.dark());
            if(horiz){
                if(vMed){
                    p->drawTiledPixmap(addPageR.x(), addPageR.y()+1,
                                       addPageR.width(), addPageR.height()-2,
                                       *vMed, 0, activeControl==AddPage ? 0 :
                                       vMed->height()-(addPageR.height()-2));
                    p->drawLine(addPageR.x(), addPageR.y(), addPageR.right(),
                                addPageR.y());
                    p->drawLine(addPageR.x(), addPageR.bottom(), addPageR.right(),
                                addPageR.bottom());
                }
                else{
                    p->fillRect(addPageR, activeControl == AddPage ?
                                g.brush(QColorGroup::Midlight) :
                                g.brush(QColorGroup::Mid));
                    p->drawLine(addPageR.x(), addPageR.y(), addPageR.right(),
                                addPageR.y());
                    p->drawLine(addPageR.x(), addPageR.bottom(), addPageR.right(),
                                addPageR.bottom());
                    p->setPen(activeControl==AddPage ? g.mid() : g.background());
                    p->drawLine(addPageR.x(), addPageR.y()+1, addPageR.right(),
                                addPageR.y()+1);
                }
            }
            else{
                if(hMed){
                    p->drawTiledPixmap(addPageR.x()+1, addPageR.y(),
                                       addPageR.width()-2, addPageR.height(),
                                       *hMed, activeControl==AddPage ? 0 :
                                       hMed->width()-(addPageR.width()-2),
                                       0);
                    p->drawLine(addPageR.x(), addPageR.y(), addPageR.x(),
                                addPageR.bottom());
                    p->drawLine(addPageR.right(), addPageR.y(), addPageR.right(),
                                addPageR.bottom());
                }
                else{
                    p->fillRect(addPageR, activeControl == AddPage ?
                                g.brush(QColorGroup::Midlight) :
                                g.brush(QColorGroup::Mid));
                    p->drawLine(addPageR.x(), addPageR.y(), addPageR.x(),
                                addPageR.bottom());
                    p->drawLine(addPageR.right(), addPageR.y(), addPageR.right(),
                                addPageR.bottom());
                    p->setPen(activeControl==AddPage ? g.mid() : g.background());
                    p->drawLine(addPageR.x()+1, addPageR.y(), addPageR.x()+1,
                                addPageR.bottom());
                }
            }
        }
    }
    if(controls & SubPage){
        if(subPageR.height()){
            p->setPen(g.dark());
            if(horiz){
                if(vMed){
                    p->drawTiledPixmap(subPageR.x(), subPageR.y()+1,
                                       subPageR.width(), subPageR.height()-2,
                                       *vMed, 0, activeControl==SubPage ? 0 :
                                       vMed->height()-(subPageR.height()-2));
                    p->drawLine(subPageR.x(), subPageR.y(), subPageR.right(),
                                subPageR.y());
                    p->drawLine(subPageR.x(), subPageR.bottom(), subPageR.right(),
                                subPageR.bottom());
                }
                else{
                    p->fillRect(subPageR, activeControl == SubPage ?
                                g.brush(QColorGroup::Midlight) :
                                g.brush(QColorGroup::Mid));
                    p->drawLine(subPageR.x(), subPageR.y(), subPageR.right(),
                                subPageR.y());
                    p->drawLine(subPageR.x(), subPageR.bottom(), subPageR.right(),
                                subPageR.bottom());
                    p->setPen(activeControl==SubPage ? g.mid() : g.background());
                    p->drawLine(subPageR.x(), subPageR.y()+1, subPageR.right(),
                                subPageR.y()+1);
                    p->drawLine(subPageR.x(), subPageR.y()+1, subPageR.x(),
                                subPageR.bottom()-1);
                }
            }
            else{
                if(hMed){
                    p->drawTiledPixmap(subPageR.x()+1, subPageR.y(),
                                       subPageR.width()-2, subPageR.height(),
                                       *hMed, activeControl==SubPage ? 0 :
                                       hMed->width()-(subPageR.width()-2),
                                       0);
                    p->drawLine(subPageR.x(), subPageR.y(), subPageR.x(),
                                subPageR.bottom());
                    p->drawLine(subPageR.right(), subPageR.y(), subPageR.right(),
                                subPageR.bottom());
                }
                else{
                    p->fillRect(subPageR, activeControl == SubPage ?
                                g.brush(QColorGroup::Midlight) :
                                g.brush(QColorGroup::Mid));
                    p->drawLine(subPageR.x(), subPageR.y(), subPageR.x(),
                                subPageR.bottom());
                    p->drawLine(subPageR.right(), subPageR.y(), subPageR.right(),
                                subPageR.bottom());
                    p->setPen(activeControl==SubPage ? g.mid() : g.background());
                    p->drawLine(subPageR.x()+1, subPageR.y(), subPageR.x()+1,
                                subPageR.bottom());
                    p->drawLine(subPageR.x()+1, subPageR.y(), subPageR.right()-1,
                                subPageR.y());
                }
            }
        }
    }
    if ( controls & Slider ) {
        drawSBButton(p, sliderR, g, activeControl == Slider, false);
        g.setColor(QColorGroup::Dark, g.mid());
        drawSBDeco(p, sliderR, g, horiz);
    }
}

void HCStyle::drawSBButton(QPainter *p, const QRect &r, const QColorGroup &g,
                           bool down, bool fast)
{
    p->setPen(g.dark());
    p->drawRect(r);
    if(vSmall){
        if(fast){
            p->drawPixmap(r.x()+1, r.y()+1, *vSmall, 0, 0, r.width()-2,
                          r.height()-2);
        }
        else{
            if(r.width() > r.height())
                drawVGradient(p, g.brush(QColorGroup::Mid), r.x()+1, r.y()+1,
                              r.width()-2, r.height()-2);
            else
                drawHGradient(p, g.brush(QColorGroup::Mid), r.x()+1, r.y()+1,
                              r.width()-2, r.height()-2);
        }

    }
    else
        p->fillRect(r.x()+1, r.y()+1, r.width()-2, r.height()-2,
                    g.brush(down ? QColorGroup::Midlight : QColorGroup::Background));

    p->setPen(down ? g.light() : g.mid());
    p->drawLine(r.x()+1, r.bottom()-1, r.right()-1, r.bottom()-1);
    p->drawLine(r.right()-1, r.top()+1, r.right()-1, r.bottom()-1);
    p->setPen(down ? g.mid() : g.light());
    p->drawLine(r.x()+1, r.y()+1, r.right()-1, r.y()+1);
    p->drawLine(r.x()+1, r.y()+1, r.x()+1, r.bottom()-1);
}

void HCStyle::drawSBDeco(QPainter *p, const QRect &r, const QColorGroup &g,
                         bool horiz)
{
    if(horiz){
        if(r.width() >= 15){
            int y = r.y()+3;
            int x = r.x() + (r.width()-7)/2;
            int y2 = r.bottom()-3;
            p->setPen(g.light());
            p->drawLine(x, y, x, y2);
            p->drawLine(x+3, y, x+3, y2);
            p->drawLine(x+6, y, x+6, y2);

            p->setPen(g.mid());
            p->drawLine(x+1, y, x+1, y2);
            p->drawLine(x+4, y, x+4, y2);
            p->drawLine(x+7, y, x+7, y2);
        }
    }
    else{
        if(r.height() >= 15){
            int x = r.x()+3;
            int y = r.y() + (r.height()-7)/2;
            int x2 = r.right()-3;
            p->setPen(g.light());
            p->drawLine(x, y, x2, y);
            p->drawLine(x, y+3, x2, y+3);
            p->drawLine(x, y+6, x2, y+6);

            p->setPen(g.mid());
            p->drawLine(x, y+1, x2, y+1);
            p->drawLine(x, y+4, x2, y+4);
            p->drawLine(x, y+7, x2, y+7);
        }
    }

}


void HCStyle::scrollBarMetrics(const QScrollBar *sb, int &sliderMin,
                                  int &sliderMax, int &sliderLength,
                                  int &buttonDim)
{

    int maxLength;
    int b = 0;
    bool horiz = sb->orientation() == QScrollBar::Horizontal;
    int length = horiz ? sb->width()  : sb->height();
    int extent = horiz ? sb->height() : sb->width();

    if ( length > ( extent - b*2 - 1 )*2 + b*2  )
	buttonDim = extent - b*2;
    else
	buttonDim = ( length - b*2 )/2 - 1;

    sliderMin = b + buttonDim;
    maxLength  = length - b*2 - buttonDim*3;

    if ( sb->maxValue() == sb->minValue() ) {
	sliderLength = maxLength;
    } else {
	sliderLength = (sb->pageStep()*maxLength)/
			(sb->maxValue()-sb->minValue()+sb->pageStep());
	uint range = sb->maxValue()-sb->minValue();
	if ( sliderLength < 9 || range > INT_MAX/2 )
	    sliderLength = 9;
	if ( sliderLength > maxLength )
	    sliderLength = maxLength;
    }
    sliderMax = sliderMin + maxLength - sliderLength;

}

QStyle::ScrollControl HCStyle::scrollBarPointOver(const QScrollBar *sb,
                                                     int sliderStart,
                                                     const QPoint &p)
{
    if ( !sb->rect().contains( p ) )
        return NoScroll;
    int sliderMin, sliderMax, sliderLength, buttonDim, pos;
    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );
    pos = (sb->orientation() == QScrollBar::Horizontal)? p.x() : p.y();
    if ( pos < sliderMin )
	return SubLine;
    if ( pos < sliderStart )
	return SubPage;
    if ( pos < sliderStart + sliderLength )
        return Slider;
    if ( pos < sliderMax + sliderLength)
        return AddPage;
    if(pos > sliderMax + sliderLength + 16)
        return AddLine;

    return SubLine;
}

#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)


QSize HCStyle::exclusiveIndicatorSize() const
{
    return(QSize(13,13));
}

void HCStyle::drawExclusiveIndicator(QPainter *p, int x, int y, int w,
                                     int h, const QColorGroup &g, bool on,
                                     bool down, bool)
{
    static QBitmap lightBmp(13, 13, radiooff_light_bits, true);
    static QBitmap grayBmp(13, 13, radiooff_gray_bits, true);
    static QBitmap dgrayBmp(13, 13, radiooff_dgray_bits, true);
    static QBitmap blueBmp(13, 13, radioon_blue_bits, true);
    static QBitmap lblueBmp(13, 13, radioon_lblue_bits, true);
    static QBitmap dblueBmp(13, 13, radioon_dblue_bits, true);

    p->fillRect(x, y, w, h, g.brush(QColorGroup::Background));
    kColorBitmaps(p, g, x, y, &lightBmp, &grayBmp,
                  NULL, &dgrayBmp);

    if(on || down){
        kColorBitmaps(p, radioOnGrp, x, y, &lblueBmp, &blueBmp,
                      NULL, &dblueBmp);
    }
}

void HCStyle::drawExclusiveIndicatorMask(QPainter *p, int x, int y, int w,
                                         int h, bool)
{
    static QBitmap maskBmp(13, 13, radiomask_bits, true);
    p->fillRect(x, y, w, h, Qt::color0);
    p->setPen(Qt::color1);
    p->drawPixmap(x, y, maskBmp);
}


QSize HCStyle::indicatorSize() const
{
    return(QSize(13, 13));
}

void HCStyle::drawIndicator(QPainter *p, int x, int y, int w, int h,
                            const QColorGroup &g, int state, bool down, bool)
{
    int x2 = x+w-1;
    int y2 = y+h-1;

    static unsigned char x_bits[] = {0x63, 0x77, 0x3e, 0x1c, 0x3e, 0x77, 0x63};
    static QBitmap xBmp(7, 7, x_bits, true);
    if(!xBmp.mask())
        xBmp.setMask(xBmp);

    
    p->setPen(g.mid());
    p->drawLine(x, y, x2, y);
    p->drawLine(x, y, x, y2);

    p->setPen(g.light());
    p->drawLine(x2, y+1, x2, y2);
    p->drawLine(x+1, y2, x2, y2);

    p->setPen(g.dark());
    p->drawLine(x+1, y+1, x2-1, y+1);
    p->drawLine(x+1, y+1, x+1, y2-1);

    p->setPen(g.midlight());
    p->drawLine(x2-1, y+2, x2-1, y2-1);
    p->drawLine(x+2, y2-1, x2-1, y2-1);

    p->fillRect(x+2, y+2, w-4, h-4, down ? g.brush(QColorGroup::Background) :
                g.brush(QColorGroup::Light));

    if(state != QButton::Off){
        if(state == QButton::On){
            p->setPen(g.highlight());
            p->drawPixmap(3, 3, xBmp);
        }
        else{
            p->setPen(g.dark());
            p->drawRect(x+2, y+2, w-4, h-4);
            p->setPen(Qt::black);
            p->drawLine(x+3, (y+h)/2-2, x+w-4, (y+h)/2-2);
            p->drawLine(x+3, (y+h)/2, x+w-4, (y+h)/2);
            p->drawLine(x+3, (y+h)/2+2, x+w-4, (y+h)/2+2);
        }
    }

}

void HCStyle::drawIndicatorMask(QPainter *p, int x, int y, int w, int h, int)
{
    // needed for some reason by KHtml, even tho it's all filled ;P
    p->fillRect(x, y, w, h, Qt::color1);
}

void HCStyle::drawSlider(QPainter *p, int x, int y, int w, int h,
                         const QColorGroup &g, Orientation orient,
                         bool, bool)
{
    int x2 = x+w-1;
    int y2 = y+h-1;
    if(vSmall){
        if(orient == Horizontal)
            drawVGradient(p, g.brush(QColorGroup::Mid), x, y, w, h);
        else
            drawHGradient(p, g.brush(QColorGroup::Mid), x, y, w, h);
    }
    else
        p->fillRect(x, y, w, h, g.background());
    p->setPen(g.mid());
    p->drawLine(x, y, x2, y);
    p->drawLine(x, y, x, y2);
    p->setPen(Qt::black);
    p->drawLine(x2, y, x2, y2);
    p->drawLine(x, y2, x2, y2);

    p->setPen(g.dark());
    p->drawLine(x2-1, y+1, x2-1, y2-1);
    p->drawLine(x+1, y2-1, x2-1, y2-1);
    p->setPen(g.light());
    p->drawLine(x+1, y+1, x2-1, y+1);
    p->drawLine(x+1, y+1, x+1, y2-1);

    p->setPen(g.mid());
    p->drawLine(x2-2, y+2, x2-2, y2-2);
    p->drawLine(x+2, y2-2, x2-2, y2-2);
    p->setPen(g.midlight());
    p->drawLine(x+2, y+2, x2-2, y+2);
    p->drawLine(x+2, y+2, x+2, y2-2);

    if (orient == Horizontal){
        p->setPen(g.light());
        p->drawLine(x+5, y+4, x+5, y2-4);
        p->drawLine(x+8, y+4, x+8, y2-4);
        p->drawLine(x+11, y+4, x+11, y2-4);
        p->setPen(g.dark());
        p->drawLine(x+6, y+4, x+6, y2-4);
        p->drawLine(x+9, y+4, x+9, y2-4);
        p->drawLine(x+12, y+4, x+12, y2-4);
    }
    else{
        p->setPen(g.light());
        p->drawLine(x+4, y+5, x2-4, y+5);
        p->drawLine(x+4, y+8, x2-4, y+8);
        p->drawLine(x+4, y+11, x2-4, y+11);
        p->setPen(g.dark());
        p->drawLine(x+4, y+6, x2-4, y+6);
        p->drawLine(x+4, y+9, x2-4, y+9);
        p->drawLine(x+4, y+12, x2-4, y+12);
    }
    p->setPen(g.background());
    p->drawPoint(x, y);
    p->drawPoint(x2, y);
    p->drawPoint(x, y2);
    p->drawPoint(x2, y2);
}

void HCStyle::drawSliderMask(QPainter *p, int x, int y, int w, int h,
                             Orientation, bool, bool)
{
    int x2 = x+w-1;
    int y2 = y+h-1;
    p->fillRect(x, y, w, h, Qt::color1);
    p->setPen(Qt::color0);
    p->drawPoint(x, y);
    p->drawPoint(x2, y);
    p->drawPoint(x, y2);
    p->drawPoint(x2, y2);
}

int HCStyle::sliderLength() const
{
    return(18);
}

void HCStyle::drawArrow(QPainter *p, Qt::ArrowType type, bool on, int x,
                            int y, int w, int h, const QColorGroup &g,
                            bool enabled, const QBrush *)
{
    static QBitmap up(8, 8, up_bits, true);
    static QBitmap down(8, 8, down_bits, true);
    static QBitmap left(8, 8, left_bits, true);
    static QBitmap right(8, 8, right_bits, true);

    if(!up.mask()){
        up.setMask(up);
        down.setMask(down);
        left.setMask(left);
        right.setMask(right);
    }
    
    p->setPen(enabled ? on ? g.light() : Qt::black : g.mid());
    if(w > 8){
        x = x + (w-8)/2;
        y = y + (h-8)/2;
    }
    switch(type){
    case Qt::UpArrow:
        p->drawPixmap(x, y, up);
        break;
    case Qt::DownArrow:
        p->drawPixmap(x, y, down);
        break;
    case Qt::LeftArrow:
        p->drawPixmap(x, y, left);
        break;
    default:
        p->drawPixmap(x, y, right);
        break;
    }

}

void HCStyle::drawKBarHandle(QPainter *p, int x, int y, int w, int h,
                             const QColorGroup &g, KToolBarPos,
                             QBrush *)
{
    if(h > w){
        if(vSmall)
            drawVGradient(p, g.brush(QColorGroup::Mid), x, y, w, h);
        else
            p->fillRect(x, y, w, h, g.brush(QColorGroup::Background));
        int x2 = x+w-1;
        int y2 = y+h-1;

        p->setPen(g.light());
        p->drawLine(x, y, x2, y);
        p->drawLine(x, y, x, y2);

        p->drawLine(x+3, y+4, x+3, y2-4);
        p->drawLine(x+5, y+4, x+5, y2-4);
        p->drawLine(x+7, y+4, x+7, y2-4);

        p->setPen(g.dark());
        p->drawLine(x, y2, x2, y2);

        p->setPen(g.mid());
        p->drawLine(x+4, y+4, x+4, y2-4);
        p->drawLine(x+6, y+4, x+6, y2-4);
        p->drawLine(x+8, y+4, x+8, y2-4);

    }
    else{
        int x2 = x+w-1;
        int y2 = y+h-1;
        if(hMed)
            drawHGradient(p, g.brush(QColorGroup::Mid), x, y, w, h);
        else
            p->fillRect(x, y, w, h, g.brush(QColorGroup::Background));

        p->setPen(g.light());
        p->drawLine(x, y, x2, y);
        p->drawLine(x, y, x, y2);
        p->drawLine(x2, y, x2, y2);

        p->drawLine(x+4, y+3, x2-4, y+3);
        p->drawLine(x+4, y+5, x2-4, y+5);
        p->drawLine(x+4, y+7, x2-4, y+7);
        
        p->setPen(g.dark());
        p->drawLine(x2, y, x2, y2);

        p->setPen(g.mid());
        p->drawLine(x+4, y+4, x2-4, y+4);
        p->drawLine(x+4, y+6, x2-4, y+6);
        p->drawLine(x+4, y+8, x2-4, y+8);
        
    }
        
}

void HCStyle::drawKMenuBar(QPainter *p, int x, int y, int w, int h,
                           const QColorGroup &g, bool mac, QBrush *)
{
    if(mac){
        p->setPen(g.dark());
        p->drawRect(x, y, w, h);
        ++x, ++y, w-=2, h-=2;
        int x2 = x+w-1;
        int y2 = y+h-1;
        p->setPen(g.light());
        p->drawLine(x, y, x2, y);
        p->drawLine(x, y, x, y2);
        p->setPen(g.mid());
        p->drawLine(x2, y, x2, y2);
        p->drawLine(x, y2, x2, y2);
        if(vSmall)
            drawVGradient(p, g.brush(QColorGroup::Mid), x+1, y+1, w-2, h-1);
        else
            p->fillRect(x+1, y+1, w-2, h-2, g.brush(QColorGroup::Midlight));
    }
    else{
        qDrawShadePanel(p, x, y, w, h, g, false, 1);
        if(vSmall)
            drawVGradient(p, g.brush(QColorGroup::Mid), x+1, y+1, w-2, h-1);
        else
            p->fillRect(x+1, y+1, w-2, h-2, g.brush(QColorGroup::Background));
    }
			
}

void HCStyle::drawKToolBar(QPainter *p, int x, int y, int w, int h,
                           const QColorGroup &g, KToolBarPos, QBrush *)
{
    if(vSmall){
        qDrawShadePanel(p, x, y, w, h, g, false, 1);
        if(w > h)
            drawVGradient(p, g.brush(QColorGroup::Mid), x+1, y+1, w-2, h-2);
        else
            drawHGradient(p, g.brush(QColorGroup::Mid), x+1, y+1, w-2, h-2);
    }
    else
        qDrawShadePanel(p, x, y, w, h, g, false, 1,
                        &g.brush(QColorGroup::Background));
}

void HCStyle::drawKToolBarButton(QPainter *p, int x, int y, int w, int h,
                                    const QColorGroup &g, bool sunken,
                                    bool raised, bool enabled, bool popup,
                                    KToolButtonType icontext,
                                    const QString& btext, const QPixmap *pixmap,
                                    QFont *font, QWidget *btn)
{
    int dx, dy;

    QFontMetrics fm(*font);

    if(raised || sunken){
        int x2 = x+w;
        int y2 = y+h;
 
        if(vSmall)
            drawVGradient(p, g.brush(QColorGroup::Mid), x, y, w, h);
        else
            p->fillRect(x, y, w, h, g.brush(QColorGroup::Midlight));
 
        p->setPen(g.dark());
        p->drawLine(x+1, y+1, x2-2, y+1);
        p->drawLine(x, y+2, x, y2-3);
        p->drawLine(x2-1, y+2, x2-1, y2-3);
        p->drawLine(x+1, y2-2, x2-2, y2-2);
 
        p->setPen(sunken ? g.mid() : g.light());
        p->drawLine(x+1, y+2, x2-2, y+2);
        p->drawLine(x+1, y+2, x+1, y2-3);
        p->setPen(sunken ? g.light() : g.mid());
        p->drawLine(x2-2, y+3, x2-2, y2-3);
        p->drawLine(x+2, y2-3, x2-2, y2-3);
    }
    else
    {
        if(btn->parent() && btn->parent()->isWidgetType()){
            QWidget *toolbar = (QWidget*)btn->parent();
            // horizontal toolbar
            if(toolbar->width() > toolbar->height()){
                // See if we are top row. Buttons are offset a few pixels
                // but not visibly.
                if(btn->y() <= 3){
                    if(toolbar->height() <= 24)
                        p->drawTiledPixmap(x, y, w, h, *vSmall);
                    else if(toolbar->height() <= 34)
                        p->drawTiledPixmap(x, y, w, h, *vMed);
                    else
                        p->drawTiledPixmap(x, y, w, h, *vLarge);

                }
                // See if we are in the gradient at all. Two rows always are
                // large.
                else if(btn->y() <= 52){
                    p->fillRect(x, y, w, h, g.mid());
                    p->drawTiledPixmap(x, y, w, 64-btn->y(),
                                       *vLarge, 0, btn->y());
                }
                // nope, we are not in the gradient
                else
                    p->fillRect(x, y, w, h, g.mid());
            }
            // vertical toolbar
            else{
                if(btn->x() <= 3){
                    if(toolbar->width() <= 34)
                        p->drawTiledPixmap(x, y, w, h, *hMed);
                    else
                        p->drawTiledPixmap(x, y, w, h, *hLarge);

                }
                else if(btn->x() <= 52){
                    p->fillRect(x, y, w, h, g.mid());
                    p->drawTiledPixmap(x, y, 52-btn->x(), h,
                                       *hLarge, btn->x(), 0);
                }
                else
                    p->fillRect(x, y, w, h, g.mid());
            }
        }
    }
    p->setPen(g.text());

    if (icontext == Icon){ // icon only
        if (pixmap){
            dx = ( w - pixmap->width() ) / 2;
            dy = ( h - pixmap->height() ) / 2;
            if ( sunken )
            {
                ++dx;
                ++dy;
            }
            p->drawPixmap( x+dx, y+dy, *pixmap );
        }
    }
    else if (icontext == IconTextRight){ // icon and text (if any)
        if (pixmap){
            dx = 4;
            dy = ( h - pixmap->height() ) / 2;
            if ( sunken ){
                ++dx;
                ++dy;
            }
            p->drawPixmap( x+dx, y+dy, *pixmap );
        }
        if (!btext.isNull()){
            int tf = AlignVCenter|AlignLeft;
            if (pixmap)
                dx= 4 + pixmap->width() + 2;
            else
                dx= 4;
            dy = 0;
            if ( sunken ){
                ++dx;
                ++dy;
            }
            if (font)
                p->setFont(*font);
            if(raised)
                p->setPen(KGlobalSettings::toolBarHighlightColor());
            p->drawText(x+dx, y+dy, w-dx, h, tf, btext);
        }
    }
    else if (icontext == Text){ // only text, even if there is a icon
        if (!btext.isNull()){
            int tf = AlignVCenter|AlignLeft;
            if (!enabled)
                p->setPen(g.dark());
            dx= (w - fm.width(btext)) / 2;
            dy= (h - fm.lineSpacing()) / 2;
            if ( sunken ){
                ++dx;
                ++dy;
            }
            if (font)
                p->setFont(*font);
            if(raised)
                p->setPen(KGlobalSettings::toolBarHighlightColor());
            p->drawText(x+dx, y+dy, fm.width(btext), fm.lineSpacing(), tf, btext);
        }
    }
    else if (icontext == IconTextBottom){
        if (pixmap){
            dx = (w - pixmap->width()) / 2;
            dy = (h - fm.lineSpacing() - pixmap->height()) / 2;
            if ( sunken ){
                ++dx;
                ++dy;
            }
            p->drawPixmap( x+dx, y+dy, *pixmap );
        }
        if (!btext.isNull()){
            int tf = AlignBottom|AlignHCenter;
            dy= pixmap->height();
            dx = 2;
            if ( sunken ){
                ++dx;
                ++dy;
            }
            if (font)
                p->setFont(*font);
            if(raised)
                p->setPen(KGlobalSettings::toolBarHighlightColor());
            p->drawText(x, y, w, h-3, tf, btext);
        }
    }
    if (popup){
        if (enabled)
            qDrawArrow (p, Qt::DownArrow, Qt::WindowsStyle, false, w-5, h-5, 0, 0,
                        g, true);
        else
            qDrawArrow (p, Qt::DownArrow, Qt::WindowsStyle, false, w-5, h-5,
                        0, 0, g, false);
    }
}


void HCStyle::drawKMenuItem(QPainter *p, int x, int y, int w, int h,
                            const QColorGroup &g, bool active, QMenuItem *mi,
                            QBrush *)
{
    if ( p->font() == KGlobal::generalFont() )
      p->setFont( KGlobal::menuFont() );

    if(active){
        qDrawShadePanel(p, x, y, w, h, g, true, 1,
                        &g.brush(QColorGroup::Midlight));
        QApplication::style().drawItem(p, x, y, w, h,
                                       AlignCenter|ShowPrefix|DontClip|SingleLine,
                                       g, mi->isEnabled(), mi->pixmap(), mi->text(),
                                       -1, &g.text());
    }
    else
        QApplication::style().drawItem(p, x, y, w, h,
                                       AlignCenter|ShowPrefix|DontClip|SingleLine,
                                       g, mi->isEnabled(), mi->pixmap(), mi->text(),
                                       -1, &g.text() );
}

void HCStyle::drawPopupMenuItem( QPainter* p, bool checkable, int maxpmw,
                                     int tab, QMenuItem* mi,
                                     const QPalette& pal, bool act,
                                     bool enabled, int x, int y, int w, int h)
{
static const int motifItemFrame         = 2;
static const int motifItemHMargin       = 3;
static const int motifItemVMargin       = 2;
static const int motifArrowHMargin      = 6;
static const int windowsRightBorder     = 12;
    maxpmw = QMAX( maxpmw, 20 );

    if ( p->font() == KGlobal::generalFont() )
      p->setFont( KGlobal::menuFont() );

    if(act){
        bool dis = !enabled;
        QColorGroup itemg = dis ? pal.disabled() : pal.active();
        
        int checkcol = maxpmw;

        qDrawShadePanel(p, x, y, w, h, itemg, true, 1,
                        &itemg.brush(QColorGroup::Midlight));

        if ( mi->iconSet() ) {
            QIconSet::Mode mode = dis? QIconSet::Disabled : QIconSet::Normal;
            if (!dis)
                mode = QIconSet::Active;
            QPixmap pixmap = mi->iconSet()->pixmap(QIconSet::Small, mode);
            int pixw = pixmap.width();
            int pixh = pixmap.height();
            QRect cr(x, y, checkcol, h);
            QRect pmr(0, 0, pixw, pixh);
            pmr.moveCenter( cr.center() );
            p->setPen(itemg.highlightedText());
            p->drawPixmap(pmr.topLeft(), pixmap );

        }
        else if(checkable) {
            int mw = checkcol + motifItemFrame;
            int mh = h - 2*motifItemFrame;
            if (mi->isChecked()){
                drawCheckMark( p, x + motifItemFrame,
                               y+motifItemFrame, mw, mh, itemg, act, dis );
            }
        }
        p->setPen(itemg.text());
        QColor discol;
        if (dis) {
            discol = itemg.text();
            p->setPen(discol);
        }
        int xm = motifItemFrame + checkcol + motifItemHMargin;
        QString s = mi->text();
        if (!s.isNull()) {
            int t = s.find( '\t' );
            int m = motifItemVMargin;
            const int text_flags = AlignVCenter|ShowPrefix | DontClip | SingleLine;
            if (t >= 0) {
                p->drawText(x+w-tab-windowsRightBorder-motifItemHMargin-motifItemFrame,
                            y+m, tab, h-2*m, text_flags, s.mid( t+1 ));
            }
            p->drawText(x+xm, y+m, w-xm-tab+1, h-2*m, text_flags, s, t);
        } else if (mi->pixmap()) {
            QPixmap *pixmap = mi->pixmap();
            if (pixmap->depth() == 1)
                p->setBackgroundMode(OpaqueMode);
            p->drawPixmap( x+xm, y+motifItemFrame, *pixmap);
            if (pixmap->depth() == 1)
                p->setBackgroundMode(TransparentMode);
        }
        if (mi->popup()) {
            int dim = (h-2*motifItemFrame) / 2;
            if (!dis)
                discol = itemg.text();
            QColorGroup g2(discol, itemg.highlight(),
                           white, white,
                           dis ? discol : white,
                           discol, white);
            drawArrow(p, RightArrow, true,
                      x+w - motifArrowHMargin - motifItemFrame - dim,  y+h/2-dim/2,
                      dim, dim, itemg, TRUE);
        }
    }
    else
        KStyle::drawPopupMenuItem(p, checkable, maxpmw, tab, mi, pal, act,
                                  enabled, x, y, w, h);
}

int HCStyle::popupMenuItemHeight(bool /*checkable*/, QMenuItem *mi,
                                 const QFontMetrics &fm)
{
    if (mi->isSeparator())
        return 2;

    int h = 0;
    if (mi->pixmap())
        h = mi->pixmap()->height();

    if (mi->iconSet())
        h = QMAX(mi->iconSet()->
                 pixmap(QIconSet::Small, QIconSet::Normal).height(), h);

    h = QMAX(fm.height() + 4, h);

    // we want a minimum size of 18
    h = QMAX(h, 18);

    return h;
}

void HCStyle::drawKProgressBlock(QPainter *p, int x, int y, int w, int h,
                                    const QColorGroup &g, QBrush *)
{
    p->fillRect(x, y, w, h, g.brush(QColorGroup::Text));
}

void HCStyle::drawFocusRect(QPainter *p, const QRect &r,
                            const QColorGroup &g, const QColor *c,
                            bool atBorder)
{
    /*
    p->setPen(g.foreground());
    p->setBrush(NoBrush);
    if ( atBorder )
        p->drawWinFocusRect( QRect( r.x()+1, r.y()+1, r.width()-2, r.height()-2 ) );
    else
    p->drawWinFocusRect( r ); */
    KStyle::drawFocusRect(p, r, g, c, atBorder);
}

void HCStyle::polishPopupMenu(QPopupMenu *mnu)
{
    KConfig *config = KGlobal::config();
    QString oldGrp = config->group();
    config->setGroup("B2");

    KStyle::polishPopupMenu(mnu);
    if (config->readBoolEntry("AnimateMenus", false))
      (void)new HCAniMenu(mnu); 
    config->setGroup(oldGrp);
}

/*
void HCStyle::drawTab(QPainter *p, const QTabBar *tabBar, QTab *tab,
                      bool selected)
{
    if(tabBar->shape() != QTabBar::RoundedAbove){
        KStyle::drawTab(p, tabBar, tab, selected);
        return;
    }

    QRect r = tab->rect();
    QColorGroup g = tabBar->colorGroup();
    if(selected){
        p->setPen(g.light());
        p->drawLine(r.x()+2, r.y(), r.right()-8, r.y());
        p->drawPoint(r.x()+1, r.y()+1);
        p->drawLine(r.x(), r.y()+2, r.x(), r.bottom());
        p->drawPoint(r.right()-7, r.y()+1);

        p->setPen(Qt::black);
        p->drawLine(r.right()-5, r.y()+3, r.right()-5, r.bottom()-5);
        p->drawLine(r.right()-4, r.bottom()-4, r.right(), r.bottom());
        p->setPen(g.dark());
        p->drawLine(r.right()-6, r.y()+2, r.right()-6, r.bottom()-5);
        p->drawLine(r.right()-5, r.bottom()-4, r.right()-1, r.bottom());

        p->fillRect(r.x()+2, r.y()+2, r.width()-10, r.height()-2,
                    g.button());
        p->setPen(g.button());
        p->drawLine(r.x()+2, r.y()+1, r.right()-8, r.y()+1);
        p->drawLine(r.x()+1, r.y()+2, r.x()+1, r.bottom());
        p->drawLine(r.right()-7, r.y()+2, r.right()-7, r.bottom());

        p->drawLine(r.right()-6, r.bottom()-4, r.right()-6, r.bottom());
        p->drawLine(r.right()-5, r.bottom()-3, r.right()-5, r.bottom());
        p->drawLine(r.right()-4, r.bottom()-2, r.right()-4, r.bottom());
        p->drawLine(r.right()-3, r.bottom()-1, r.right()-3, r.bottom());
        p->drawPoint(r.right()-2, r.bottom());
    }
    else{
        r.setY(r.y()+3);
        p->fillRect(r.x()+2, r.y()+2, r.width()-9, r.height()-2,
                    g.brush(QColorGroup::Button));

        p->setPen(g.light());
        p->drawLine(r.x()+2, r.y(), r.right()-8, r.y());
        p->drawPoint(r.x()+1, r.y()+1);
        p->drawLine(r.x(), r.y()+2, r.x(), r.bottom());
        p->drawPoint(r.right()-7, r.y()+1);


        p->setPen(g.midlight());
        p->drawLine(r.x()+1, r.y()+2, r.x()+1, r.bottom());
        p->drawLine(r.x()+2, r.y()+1, r.right()-8, r.y()+1);

        p->setPen(g.dark());
        p->drawLine(r.right()-6, r.y()+2, r.right()-6, r.bottom());
        p->drawLine(r.x(), r.bottom()-1, r.right()-5, r.bottom()-1);
        p->setPen(Qt::black);
        p->drawLine(r.right()-5, r.y()+3, r.right()-5, r.bottom());
        p->drawLine(r.x(), r.bottom(), r.right()-5, r.bottom());

    }
}

void HCStyle::drawTabMask(QPainter *p, const QTabBar*, QTab *tab,
                          bool )

{
    p->fillRect(tab->rect(), Qt::color1);
}

void HCStyle::tabbarMetrics(const QTabBar *t, int &hFrame, int &vFrame,
                            int &overlap)
{
    if(t->shape() == QTabBar::RoundedAbove){
        overlap = 5;
        hFrame = 24;
        vFrame = 10;
    }
    else
        KStyle::tabbarMetrics(t, hFrame, vFrame, overlap);
}
*/


void HCStyle::drawSplitter(QPainter *p, int x, int y, int w, int h,
                           const QColorGroup &g, Orientation)
{
    int x2 = x+w-1;
    int y2 = y+h-1;
    p->setPen(g.dark());
    p->drawRect(x, y, w, h);
    p->setPen(g.background());
    p->drawPoint(x, y);
    p->drawPoint(x2, y);
    p->drawPoint(x, y2);
    p->drawPoint(x2, y2);
    p->setPen(g.light());
    p->drawLine(x+1, y+1, x+1, y2-1);
    p->drawLine(x+1, y+1, x2-1, y+1);
    p->setPen(g.midlight());
    p->drawLine(x+2, y+2, x+2, y2-2);
    p->drawLine(x+2, y+2, x2-2, y+2);
    p->setPen(g.mid());
    p->drawLine(x2-1, y+1, x2-1, y2-1);
    p->drawLine(x+1, y2-1, x2-1, y2-1);
    p->fillRect(x+3, y+3, w-5, h-5, g.brush(QColorGroup::Background));
}

void HCStyle::drawPanel(QPainter *p, int x, int y, int w, int h,
                        const QColorGroup &g, bool sunken,
                        int lineWidth, const QBrush *fill)
{
    if(lineWidth != 2 || !sunken)
        KStyle::drawPanel(p, x, y, w, h, g, sunken, lineWidth, fill);
    else{
        QPen oldPen = p->pen();
        int x2 = x+w-1;
        int y2 = y+h-1;
        p->setPen(g.light());
        p->drawLine(x, y2, x2, y2);
        p->drawLine(x2, y, x2, y2);
        p->setPen(g.mid());
        p->drawLine(x, y, x2, y);
        p->drawLine(x, y, x, y2);

        p->setPen(g.midlight());
        p->drawLine(x+1, y2-1, x2-1, y2-1);
        p->drawLine(x2-1, y+1, x2-1, y2-1);
        p->setPen(g.dark());
        p->drawLine(x+1, y+1, x2-1, y+1);
        p->drawLine(x+1, y+1, x+1, y2-1);
        p->setPen(oldPen);
        if(fill){
            // I believe here we are only supposed to fill if there is a
            // specified fill brush...
            p->fillRect(x+2, y+2, w-4, h-4, *fill);
        }
    }
}

// no check here, make sure your highcolor beforehand ;-)
void HCStyle::drawVGradient(QPainter *p, const QBrush &fill, int x, int y,
                            int w, int h)
{
    if(h <= 24){
        p->drawTiledPixmap(x, y, w, h, *vSmall);
    }
    else if(h <= 34){
        p->drawTiledPixmap(x, y, w, h, *vMed);
    }
    else if(h <= 64){
        p->drawTiledPixmap(x, y, w, h, *vLarge);
    }
    else{
        p->fillRect(x, y+vLarge->height(), w, h-vLarge->height(), fill);
        p->drawTiledPixmap(x, y, w, vLarge->height(), *vLarge);
    }
}

void HCStyle::drawHGradient(QPainter *p, const QBrush &fill, int x, int y,
                            int w, int h)
{
    if(w <= 34){
        p->drawTiledPixmap(x, y, w, h, *hMed);
    }
    else if(w <= 52){
        p->drawTiledPixmap(x, y, w, h, *hLarge);
    }
    else{
        p->fillRect(x+hLarge->width(), y, w-hLarge->width(), h, fill);
        p->drawTiledPixmap(x, y, hLarge->width(), h, *hLarge);
    }
}

void HCStyle::drawKickerAppletHandle(QPainter *p, int x, int y, int w, int h,
                                      const QColorGroup &g, QBrush *)
{
    if(h > w){
        int y2 = y+h-1;

        p->setPen(g.light());

        p->drawLine(x+1, y+2, x+1, y2-2);
        p->drawLine(x+4, y+2, x+4, y2-2);

        p->setPen(g.dark());
        p->drawLine(x+2, y+2, x+2, y2-2);
        p->drawLine(x+5, y+2, x+5, y2-2);

    }
    else{
        int x2 = x+w-1;

        p->setPen(g.light());

        p->drawLine(x+2, y+1, x2-2, y+1);
        p->drawLine(x+2, y+4, x2-2, y+4);

        p->setPen(g.dark());
        p->drawLine(x+2, y+2, x2-2, y+2);
        p->drawLine(x+2, y+5, x2-2, y+5);
    }
        
}

void HCStyle::drawKickerTaskButton(QPainter *p, int x, int y, int w, int h,
                                  const QColorGroup &g,
                                  const QString &text, bool sunken,
                                  QPixmap *pixmap, QBrush *)
{
    int x2 = x+w-1;
    int y2 = y+h-1;
    if(sunken)
        p->fillRect(x+1, y+1, w-2, h-2, g.brush(QColorGroup::Mid));
    else
        p->drawTiledPixmap(x+1, y+1, w-2, h-2, sunken ? *vLarge : *vSmall);
        
    p->setPen(sunken ? Qt::black : g.light());
    p->drawLine(x, y, x2-1, y);
    p->drawLine(x, y, x, y2-1);
    p->setPen(sunken ? g.midlight() : g.mid());
    p->drawLine(x+1, y2-1, x2-1, y2-1);
    p->drawLine(x2-1, y+1, x2-1, y2-1);
    p->setPen(sunken ? g.light() : Qt::black);
    p->drawLine(x, y2, x2, y2);
    p->drawLine(x2, y, x2, y2);

    const int pxWidth = 20;
    QRect br(buttonRect(x, y, w, h));

    if ( pixmap && !pixmap->isNull() ) {
        int dx = ( pxWidth - pixmap->width() ) / 2;
        int dy = ( h - pixmap->height() ) / 2;
        if (sunken) {
            dx++;
            dy++;
        }
        p->drawPixmap( br.x()+dx, dy, *pixmap );
    }
    p->setPen(sunken ? g.light() : g.buttonText());

    if (!text.isNull()){
        QString s2 = text;
        if (p->fontMetrics().width(s2) > br.width()-pxWidth){
            while (s2.length()>0 &&
                   p->fontMetrics().width(s2) > br.width() - pxWidth
                   - p->fontMetrics().width("...")) {
                s2.truncate( s2.length() - 1 );
            }
            s2.append("...");
        }
        p->drawText(br.x()+ pxWidth, 0, w-pxWidth, h,
                    AlignLeft|AlignVCenter, s2);
    }
}



void HCStyle::makeWallpaper(QPixmap &dest, const QColor &base)
{
    static QBitmap paper1(45, 45, paper_1_bits, true);
    static QBitmap paper2(45, 45, paper_2_bits, true);
    static QBitmap paper3(45, 45, paper_3_bits, true);

    if(!paper1.mask()){
        paper1.setMask(paper1);
        paper2.setMask(paper2);
        paper3.setMask(paper3);
    }
    dest.resize(45, 45);
    dest.fill(base);
    QPainter p;
    p.begin(&dest);
    p.setPen(base.dark(104));
    p.drawPixmap(0, 0, paper1);
    p.setPen(base.light(104));
    p.drawPixmap(0, 0, paper2);
    p.setPen(base.light(106));
    p.drawPixmap(0, 0, paper3);
    p.end();
}
    



                
#include "hcstyle.moc"


