/* This file is part of the KDE libraries
   Copyright (C) 2000 Max Judin <novaprint@mtu-net.ru>

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
#ifndef KDOCKWIDGET_PRIVATE_H
#define KDOCKWIDGET_PRIVATE_H

#include <qwidget.h>
#include <qpushbutton.h>
#include <netwm_def.h>

class QFrame;

/**
 * Like QSplitter but specially designed for dockwidgets stuff.
 * @internal
 *
 * @author Max Judin.
 * @version $Id$
*/
class KDockSplitter : public QWidget
{
  Q_OBJECT
public:
  KDockSplitter(QWidget *parent= 0, const char *name= 0, Orientation orient= Vertical, int pos= 50);
  virtual ~KDockSplitter(){};

  void activate(QWidget *c0, QWidget *c1 = 0L);
  void deactivate();

  int separatorPos();
  void setSeparatorPos(int pos, bool do_resize = true);

  virtual bool eventFilter(QObject *, QEvent *);
  virtual bool event( QEvent * );

  QWidget* getFirst(){ return child0; }
  QWidget* getLast(){ return child1; }
  QWidget* getAnother( QWidget* );
  void updateName();

protected:
  int checkValue( int );
  virtual void resizeEvent(QResizeEvent *);

private:
  void setupMinMaxSize();

  QWidget *child0, *child1;
  Orientation orientation;
  bool initialised;
  QFrame* divider;
  int xpos;
};

/**
 * A mini-button usually placed in the dockpanel.
 * @internal
 *
 * @author Max Judin.
 * @version $Id$
*/
class KDockButton_Private : public QPushButton
{
  Q_OBJECT
public:
  KDockButton_Private( QWidget *parent=0, const char *name=0 );
  ~KDockButton_Private();

protected:
  virtual void drawButton( QPainter * );
  virtual void enterEvent( QEvent * );
  virtual void leaveEvent( QEvent * );

private:
  bool moveMouse;
};

/**
 * additional KDockWidget stuff (private)
*/
class KDockWidgetPrivate : public QObject
{
  Q_OBJECT
public:
  KDockWidgetPrivate();
  ~KDockWidgetPrivate();

public:
  int index;
  int splitPosInPercent;
  bool pendingFocusInEvent;
  bool blockHasUndockedSignal;
  NET::WindowType windowType;
  QWidget *_parent;
  bool transient;
};

#endif
