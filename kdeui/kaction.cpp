/* This file is part of the KDE libraries
    Copyright (C) 1999 Reginald Stadlbauer <reggie@kde.org>
              (C) 1999 Simon Hausmann <hausmann@kde.org>
              (C) 2000 Nicolas Hadacek <haadcek@kde.org>
              (C) 2000 Kurt Granroth <granroth@kde.org>
              (C) 2000 Michael Koch <koch@kde.org>

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
#include "kaction.h"

#include <ktoolbar.h>
#include <ktoolbarbutton.h>
#include <kmenubar.h>
#include <qobjectlist.h>
#include <kapp.h>
#include <kaccel.h>
#include <kconfig.h>
#include <kurl.h>
#include <qtl.h>
#include <qptrdict.h>
#include <qfontdatabase.h>

#include <kiconloader.h>

static QFontDatabase *fontDataBase = 0;

static void cleanupFontDatabase()
{
    delete fontDataBase;
    fontDataBase = 0;
}

static void get_fonts( QStringList &lst )
{
    if ( !fontDataBase ) {
	fontDataBase = new QFontDatabase();
	qAddPostRoutine( cleanupFontDatabase );
    }
    lst = fontDataBase->families();
}

static QValueList<int> get_standard_font_sizes()
{
    if ( !fontDataBase ) {
	fontDataBase = new QFontDatabase();
	qAddPostRoutine( cleanupFontDatabase );
    }
    return fontDataBase->standardSizes();
}

int KAction::getToolButtonID()
{
    static int toolbutton_no = -2;
    return toolbutton_no--;
}

class KAction::KActionPrivate
{
public:
  KActionPrivate()
  {
    m_kaccel    = 0;
    m_bIconSet  = false;
    m_enabled   = true;
    m_accel     = 0;
    m_component = 0;
  }
  ~KActionPrivate()
  {
  }
  QString m_iconName;
  KAccel *m_kaccel;

  QObject* m_component;

  QString m_text;
  QString m_whatsThis;
  QString m_groupText;
  QPixmap m_pixmap;
  QIconSet m_iconSet;
  bool m_bIconSet;
  QString m_group;
  int m_accel;
  QString m_toolTip;
  QString m_shortText;

  struct Container
  {
    Container() { m_container = 0; m_representative = 0; m_id = 0; }
    Container( const Container& s ) { m_container = s.m_container;
                                      m_id = s.m_id; m_representative = s.m_representative; }
    QWidget* m_container;
    int m_id;
    QWidget* m_representative;
  };

  QValueList<Container> m_containers;
  bool m_enabled;
};

KAction::KAction( const QString& text, int accel, QObject* parent,
                  const char* name )
 : QObject( parent, name )
{
    d = new KActionPrivate;

    setAccel( accel );
    setText( text );

    if ( parent && parent->inherits("KActionCollection") )
        ((KActionCollection*)parent)->insert( this );
}

KAction::KAction( const QString& text, int accel, const QObject* receiver,
                  const char* slot, QObject* parent, const char* name )
 : QObject( parent, name )
{
    d = new KActionPrivate;

    setAccel( accel );
    setText( text );

    if ( parent && parent->inherits("KActionCollection") )
        ((KActionCollection*)parent)->insert( this );

    connect( this, SIGNAL( activated() ), receiver, slot );
}

KAction::KAction( const QString& text, const QIconSet& pix, int accel,
                  QObject* parent, const char* name )
 : QObject( parent, name )
{
    d = new KActionPrivate;

    setAccel( accel );
    setText( text );
    setIconSet( pix );

    if ( parent && parent->inherits("KActionCollection") )
        ((KActionCollection*)parent)->insert( this );
}

KAction::KAction( const QString& text, const QString& pix, int accel,
                  QObject* parent, const char* name )
: QObject( parent, name )
{
    d = new KActionPrivate;

    setText( text );
    setAccel( accel );
    setIcon( pix );

    if ( parent && parent->inherits("KActionCollection") )
        ((KActionCollection*)parent)->insert( this );
}

KAction::KAction( const QString& text, const QIconSet& pix, int accel,
                  const QObject* receiver, const char* slot, QObject* parent,
                  const char* name )
 : QObject( parent, name )
{
    d = new KActionPrivate;

    setAccel( accel );
    setText( text );
    setIconSet( pix );

    if ( parent && parent->inherits("KActionCollection") )
        ((KActionCollection*)parent)->insert( this );

    connect( this, SIGNAL( activated() ), receiver, slot );
}

KAction::KAction( const QString& text, const QString& pix, int accel,
                  const QObject* receiver, const char* slot, QObject* parent,
                  const char* name )
  : QObject( parent, name )
{
    d = new KActionPrivate;

    setAccel( accel );
    setText( text );
    setIcon( pix );

    if ( parent && parent->inherits("KActionCollection") )
        ((KActionCollection*)parent)->insert( this );

    connect( this, SIGNAL( activated() ), receiver, slot );
}

KAction::KAction( QObject* parent, const char* name )
 : QObject( parent, name )
{
    d = new KActionPrivate;
}

KAction::~KAction()
{
    if ( parent() && parent()->inherits("KActionCollection") )
        ((KActionCollection*)parent())->take( this );

    delete d; d = 0;
}

bool KAction::isPlugged() const
{
  if (d->m_kaccel)
    return true;
  else
    return ( containerCount() > 0 );
}

bool KAction::isPlugged( QWidget *container, int id ) const
{
  int i = findContainer( container );

  if ( i == -1 )
    return false;

  if ( menuId( i ) != id )
    return false;

  return true;
}

bool KAction::isPlugged( QWidget *container, QWidget *_representative ) const
{
  int i = findContainer( container );

  if ( i == -1 )
    return false;

  if ( representative( i ) != _representative )
    return false;

  return true;
}

QObject* KAction::component()
{
    return d->m_component;
}

void KAction::setComponent( QObject* obj )
{
    d->m_component = obj;
}


void KAction::update()
{
    // By default do nothing interesting
}

void KAction::setAccel( int a )
{
  d->m_accel = a;

  KActionCollection *coll = parentCollection();
  if (coll)
  {
    QDict<KKeyEntry> *keys = coll->keyDict();
    KKeyEntry *entry = keys->find( name() );
    if (entry)
    {
      entry->aCurrentKeyCode = a;
      keys->replace( name(), entry );
    }
  }

  int len = containerCount();
  for( int i = 0; i < len; ++i )
    setAccel( i, a );
}

void KAction::setAccel( int i, int a )
{
  if (d->m_kaccel)
    d->m_kaccel->updateItem(name(), a);
  QWidget* w = container( i );
  if ( w->inherits( "QPopupMenu" ) )
    ((QPopupMenu*)w)->setAccel( a, menuId( i ) );
  else if ( w->inherits( "QMenuBar" ) )
    ((QMenuBar*)w)->setAccel( a, menuId( i ) );
}

int KAction::accel() const
{
  return d->m_accel;
}

void KAction::setGroup( const QString& grp )
{
  d->m_group = grp;

  int len = containerCount();
  for( int i = 0; i < len; ++i )
    setGroup( i, grp );
}

void KAction::setGroup( int, const QString& )
{
  // DO SOMETHING
}

QString KAction::group() const
{
  return d->m_group;
}

bool KAction::isEnabled() const
{
  return d->m_enabled;
}

void KAction::setToolTip( const QString& tt )
{
  d->m_toolTip = tt;

  int len = containerCount();
  for( int i = 0; i < len; ++i )
    setToolTip( i, tt );
}

void KAction::setToolTip( int, const QString& )
{
  // DO SOMETHING!
}

QString KAction::toolTip() const
{
  return d->m_toolTip;
}

void KAction::setShortText( const QString& st )
{
  d->m_shortText = st;

  int len = containerCount();
  for( int i = 0; i < len; ++i )
    setShortText( i, st );
}

void KAction::setShortText( int, const QString& )
{
  // do something
}

QString KAction::shortText() const
{
  return d->m_shortText;
}

int KAction::plug( QWidget *w, int index )
{
  KActionCollection *p = parentCollection();

  if ( w->inherits("QPopupMenu") )
  {
    QPopupMenu* menu = (QPopupMenu*)w;
    int id;
    if ( !d->m_pixmap.isNull() )
    {
      id = menu->insertItem( pixmap(), this, SLOT( slotActivated() ),
                            accel(), -1, index );
    }
    else
    {
      if ( d->m_bIconSet )
        id = menu->insertItem( iconSet(), plainText(), this,
                               SLOT( slotActivated() ), accel(),
                               -1, index );
      else
        id = menu->insertItem( plainText(), this, SLOT( slotActivated() ),
                               accel(), -1, index );
    }

    menu->setItemEnabled( id, isEnabled() );
    menu->setWhatsThis( id, whatsThis() );

    addContainer( menu, id );
    connect( menu, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    if ( p )
      p->connectHighlight( menu, this );

    return d->m_containers.count() - 1;
  }
  else if ( w->inherits( "KToolBar" ) )
  {
    KToolBar *bar = (KToolBar *)w;

    int id_ = getToolButtonID();
    if ( (d->m_iconName == QString::null) && d->m_bIconSet )
    {
      bar->insertButton( iconSet().pixmap(), id_, SIGNAL( clicked() ), this,
                         SLOT( slotActivated() ),
                         isEnabled(), plainText(), index );
    }
    else
    {
      bar->insertButton( iconName(), id_, SIGNAL( clicked() ), this,
                         SLOT( slotActivated() ), isEnabled(), plainText(),
                         index );
    }

    addContainer( bar, id_ );

    connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    if ( p )
      p->connectHighlight( bar, this );

    return containerCount() - 1;
  }

  return -1;
}

void KAction::unplug( QWidget *w )
{
  KActionCollection *p = parentCollection();
  if ( w->inherits("QPopupMenu") )
  {
    QPopupMenu* menu = (QPopupMenu*)w;
    int i = findContainer( menu );
    if ( i != -1 )
    {
      menu->removeItem( menuId( i ) );
      removeContainer( i );
      if ( p )
        p->disconnectHighlight( menu, this );
    }
  }
  else if ( w->inherits( "KToolBar" ) )
  {
    KToolBar *bar = (KToolBar *)w;

    int idx = findContainer( bar );

    if ( idx != -1 )
    {
      bar->removeItem( menuId( idx ) );
      removeContainer( idx );
      if ( p )
        p->disconnectHighlight( bar, this );
    }

    return;
  }
}

void KAction::plugAccel(KAccel *kacc, bool configurable)
{
  if (d->m_kaccel)
    unplugAccel();
  d->m_kaccel = kacc;
  d->m_kaccel->insertItem(plainText(), name(), accel(), configurable);
  d->m_kaccel->connectItem(name(), this, SLOT(slotActivated()));
  connect(d->m_kaccel, SIGNAL(destroyed()), this, SLOT(slotDestroyed()));
  connect(d->m_kaccel, SIGNAL(keycodeChanged()), this, SLOT(slotKeycodeChanged()));
}

void KAction::unplugAccel()
{
	if ( d->m_kaccel==0 )
    return;
	d->m_kaccel->removeItem(name());
	d->m_kaccel->disconnect(this);
	d->m_kaccel = 0;
}

void KAction::setEnabled(bool enable)
{
  if (d->m_kaccel)
    d->m_kaccel->setItemEnabled(name(), enable);

  if ( enable == d->m_enabled )
    return;

  d->m_enabled = enable;

  int len = containerCount();
  for( int i = 0; i < len; ++i )
    setEnabled( i, enable );

  emit enabled( d->m_enabled );
}

void KAction::setEnabled( int i, bool e )
{
    QWidget *w = container( i );

    if ( w->inherits("QPopupMenu") )
        ((QPopupMenu*)w)->setItemEnabled( menuId( i ), e );
    else if ( w->inherits("QMenuBar") )
        ((QMenuBar*)w)->setItemEnabled( menuId( i ), e );
    else if ( w->inherits( "KToolBar" ) )
      ((KToolBar *)w)->setItemEnabled( menuId( i ), e );
}

void KAction::setText( const QString& text )
{
  if (d->m_kaccel)
    d->m_kaccel->setDescription(name(), text);

  d->m_text = text;

  int len = containerCount();
  for( int i = 0; i < len; ++i )
    setText( i, text );
}

void KAction::setText( int i, const QString &text )
{
  QWidget *w = container( i );

  if ( w->inherits( "QPopupMenu" ) )
    ((QPopupMenu*)w)->changeItem( menuId( i ), text );
  else if ( w->inherits( "QMenuBar" ) )
    ((QMenuBar*)w)->changeItem( menuId( i ), text );	
  else if ( w->inherits( "KToolBar" ) )
  {
    QWidget *button = ((KToolBar *)w)->getWidget( menuId( i ) );
    if ( button->inherits( "KToolBarButton" ) )
      ((KToolBarButton *)button)->setText( text );
  }
}

QString KAction::text() const
{
  return d->m_text;
}

QString KAction::plainText() const
{
  QString stripped( d->m_text );
  int pos;
  while( ( pos = stripped.find( '&' ) ) != -1 )
    stripped.replace( pos, 1, QString::null );

  return stripped;
}

void KAction::setIcon( const QString &icon )
{
  d->m_iconName = icon;

  // We load the "Small" icon as the main one (for menu items)
  // and we let setIcon( int, QString ) deal with toolbars

  // We used to use SmallIcon for this, but that's wrong since the
  // Small group may *not* be 16x16 and we *need* 16x16
  KIconLoader *loader;
  if ( parent() && parent()->inherits( "KActionCollection" ) )
    loader = static_cast<KActionCollection*>(parent())->instance()->iconLoader();
  else
    loader = KGlobal::iconLoader();
  setIconSet( SmallIcon( icon, 16 ) );

  // now handle any toolbars
  int len = containerCount();
  for ( int i = 0; i < len; ++i )
    setIcon( i, icon );
}

void KAction::setIcon( int id, const QString &icon )
{
  QWidget* w = container( id );

  if ( w->inherits( "KToolBar" ) )
    ((KToolBar *)w)->setButtonIcon( menuId( id ), icon );
}

QString KAction::iconName() const
{
  return d->m_iconName;
}

void KAction::setIconSet( const QIconSet &iconset )
{
  d->m_iconSet  = iconset;
  d->m_bIconSet = true;

  int len = containerCount();
  for( int i = 0; i < len; ++i )
    setIconSet( i, iconset );
}

void KAction::setIconSet( int id, const QIconSet& iconset )
{
  QWidget *w = container( id );

  if ( w->inherits( "QPopupMenu" ) )
    ((QPopupMenu*)w)->changeItem( menuId( id ), iconset, d->m_text );
  else if ( w->inherits( "QMenuBar" ) )
    ((QMenuBar*)w)->changeItem( menuId( id ), iconset, d->m_text );
  else if ( w->inherits( "KToolBar" ) )
    ((KToolBar *)w)->setButtonPixmap( menuId( id ), iconset.pixmap() );
}

QIconSet KAction::iconSet() const
{
  return d->m_iconSet;
}

bool KAction::hasIconSet() const
{
  return d->m_bIconSet;
}

void KAction::setWhatsThis( const QString& text )
{
  d->m_whatsThis = text;

  int len = containerCount();
  for( int i = 0; i < len; ++i )
    setWhatsThis( i, text );
}

void KAction::setWhatsThis( int i, const QString& text )
{
  QPopupMenu* pm = popupMenu( i );
  if ( pm )
    pm->setWhatsThis( menuId( i ), text );
}

QString KAction::whatsThis() const
{
  return d->m_whatsThis;
}

QPixmap KAction::pixmap() const
{
  return d->m_pixmap;
}

QWidget* KAction::container( int index )
{
  return d->m_containers[ index ].m_container;
}

KToolBar* KAction::toolBar( int index ) const
{
  QWidget* w = d->m_containers[ index ].m_container;
  if ( !w || !w->inherits( "KToolBar" ) )
    return 0;

  return (KToolBar*)w;
}

QPopupMenu* KAction::popupMenu( int index ) const
{
  QWidget* w = d->m_containers[ index ].m_container;
  if ( !w || !w->inherits( "QPopupMenu" ) )
    return 0;

  return (QPopupMenu*)w;
}

QWidget* KAction::representative( int index ) const
{
  return d->m_containers[ index ].m_representative;
}

int KAction::menuId( int index ) const
{
  return d->m_containers[ index ].m_id;
}

int KAction::containerCount() const
{
  return d->m_containers.count();
}

void KAction::addContainer( QWidget* c, int id )
{
  KActionPrivate::Container p;
  p.m_container = c;
  p.m_id = id;
  d->m_containers.append( p );
}

void KAction::addContainer( QWidget* c, QWidget* w )
{
  KActionPrivate::Container p;
  p.m_container = c;
  p.m_representative = w;
  d->m_containers.append( p );
}

void KAction::slotActivated()
{
  emit activated();
}

void KAction::slotDestroyed()
{
	if ( sender() == d->m_kaccel )
    d->m_kaccel = 0;

  const QObject* o = sender();

  int i;
  do
  {
    i = findContainer( (QWidget*)o );
    if ( i != -1 )
      removeContainer( i );
  } while ( i != -1 );
}

int KAction::findContainer( QWidget* widget ) const
{
  int pos = 0;
  QValueList<KActionPrivate::Container>::Iterator it = d->m_containers.begin();
  while( it != d->m_containers.end() )
  {
    if ( (*it).m_representative == widget || (*it).m_container == widget )
      return pos;
    ++it;
    ++pos;
  }

  return -1;
}

void KAction::removeContainer( int index )
{
  int i = 0;
  QValueList<KActionPrivate::Container>::Iterator it = d->m_containers.begin();
  while( it != d->m_containers.end() )
  {
    if ( i == index )
    {
      d->m_containers.remove( it );
      return;
    }
    ++it;
    ++i;
  }
}

void KAction::slotKeycodeChanged()
{
  setAccel(d->m_kaccel->currentKey(name()));
}

KActionCollection *KAction::parentCollection() const
{
  QObject *p = parent();

  if ( !p )
    return 0;

  if ( !p->inherits( "KActionCollection" ) )
    return 0;

  return static_cast<KActionCollection *>( p );
}

class KToggleAction::KToggleActionPrivate
{
public:
  KToggleActionPrivate()
  {
    m_locked  = false;
    m_checked = false;
  }

  bool m_locked;
  bool m_checked;
  QString m_exclusiveGroup;
};

KToggleAction::KToggleAction( const QString& text, int accel, QObject* parent,
                              const char* name )
    : KAction( text, accel, parent, name )
{
  d = new KToggleActionPrivate;
}

KToggleAction::KToggleAction( const QString& text, int accel,
                              const QObject* receiver, const char* slot,
                              QObject* parent, const char* name )
  : KAction( text, accel, receiver, slot, parent, name )
{
  d = new KToggleActionPrivate;
}

KToggleAction::KToggleAction( const QString& text, const QIconSet& pix,
                              int accel, QObject* parent, const char* name )
  : KAction( text, pix, accel, parent, name )
{
  d = new KToggleActionPrivate;
}

KToggleAction::KToggleAction( const QString& text, const QString& pix,
                              int accel, QObject* parent, const char* name )
 : KAction( text, pix, accel, parent, name )
{
  d = new KToggleActionPrivate;
}

KToggleAction::KToggleAction( const QString& text, const QIconSet& pix,
                              int accel, const QObject* receiver,
                              const char* slot, QObject* parent,
                              const char* name )
  : KAction( text, pix, accel, receiver, slot, parent, name )
{
  d = new KToggleActionPrivate;
}

KToggleAction::KToggleAction( const QString& text, const QString& pix,
                              int accel, const QObject* receiver,
                              const char* slot, QObject* parent,
                              const char* name )
  : KAction( text, pix, accel, receiver, slot, parent, name )
{
  d = new KToggleActionPrivate;
}

KToggleAction::KToggleAction( QObject* parent, const char* name )
    : KAction( parent, name )
{
  d = new KToggleActionPrivate;
}

int KToggleAction::plug( QWidget* widget, int index )
{
  if ( !widget->inherits("QPopupMenu") && !widget->inherits("KToolBar") )
  {
    qDebug("Can not plug KToggleAction in %s", widget->className() );
    return -1;	
  }

  int _index = KAction::plug( widget, index );
  if ( _index == -1 )
    return _index;

  if ( widget->inherits("QPopupMenu") )
  {
    int id = menuId( _index );

    popupMenu( _index )->setItemChecked( id, d->m_checked );
    popupMenu( _index )->setFont(KGlobal::menuFont());
  } else if ( widget->inherits( "KToolBar" ) ) {
    KToolBar *bar = (KToolBar *)widget;

    bar->setToggle( menuId( _index ), TRUE );
    bar->setButton( menuId( _index ), isChecked() );
  }

  return _index;
}

void KToggleAction::setChecked( bool c )
{
  if ( c == d->m_checked )
    return;

  int len = containerCount();

  for( int i = 0; i < len; ++i )
    setChecked( i, c );

  if ( c && parent() && !exclusiveGroup().isEmpty() ) {
    const QObjectList *list = parent()->children();
    if ( list ) {
      QObjectListIt it( *list );
      for( ; it.current(); ++it ) {
        if ( it.current()->inherits( "KToggleAction" ) && it.current() != this &&
            ((KToggleAction*)it.current())->exclusiveGroup() == exclusiveGroup() ) {
          ((KToggleAction*)it.current())->setChecked( FALSE );
        }
      }
    }
  }

  d->m_checked = c;

  d->m_locked = false;
  emit activated();
  d->m_locked = false;
  emit toggled( isChecked() );
}

void KToggleAction::setChecked( int id, bool checked )
{
  QWidget *w = container( id );

  if ( w->inherits( "QPopupMenu" ) )
    ((QPopupMenu*)w)->setItemChecked( menuId( id ), checked );
  else if ( w->inherits( "QMenuBar" ) )
    ((QMenuBar*)w)->setItemChecked( menuId( id ), checked );
  else if ( w->inherits( "KToolBar" ) )
  {
    QWidget* r = ( (KToolBar*)w )->getButton( menuId( id ) );
    if ( r->inherits( "KToolBarButton" ) )
      ( (KToolBar*)w )->setButton( menuId( id ), checked );
  }
}

void KToggleAction::slotActivated()
{
  if ( d->m_locked )
    return;

  d->m_locked = true;
  setChecked( !isChecked() );
  d->m_locked = false;
}

bool KToggleAction::isChecked() const
{
  return d->m_checked;
}

void KToggleAction::setExclusiveGroup( const QString& name )
{
  d->m_exclusiveGroup = name;
}

QString KToggleAction::exclusiveGroup() const
{
  return d->m_exclusiveGroup;
}


KRadioAction::KRadioAction( const QString& text, int accel, QObject* parent, const char* name )
: KToggleAction( text, accel, parent, name )
{
}

KRadioAction::KRadioAction( const QString& text, int accel,
	                    const QObject* receiver, const char* slot, QObject* parent, const char* name )
: KToggleAction( text, accel, receiver, slot, parent, name )
{
}

KRadioAction::KRadioAction( const QString& text, const QIconSet& pix, int accel,
	                    QObject* parent, const char* name )
: KToggleAction( text, pix, accel, parent, name )
{
}

KRadioAction::KRadioAction( const QString& text, const QString& pix, int accel,
	                    QObject* parent, const char* name )
: KToggleAction( text, pix, accel, parent, name )
{
}

KRadioAction::KRadioAction( const QString& text, const QIconSet& pix, int accel,
			    const QObject* receiver, const char* slot, QObject* parent, const char* name )
: KToggleAction( text, pix, accel, receiver, slot, parent, name )
{
}

KRadioAction::KRadioAction( const QString& text, const QString& pix, int accel,
			    const QObject* receiver, const char* slot, QObject* parent, const char* name )
: KToggleAction( text, pix, accel, receiver, slot, parent, name )
{
}

KRadioAction::KRadioAction( QObject* parent, const char* name )
: KToggleAction( parent, name )
{
}

void KRadioAction::slotActivated()
{
  if ( isChecked() )
    return;

  KToggleAction::slotActivated();
}

class KSelectAction::KSelectActionPrivate
{
public:
  KSelectActionPrivate()
  {
    m_lock = false;
    m_edit = false;
    m_menu = 0;
    m_current = -1;
  }
  bool m_lock;
  bool m_edit;
  QPopupMenu *m_menu;
  int m_current;
  QStringList m_list;
};

KSelectAction::KSelectAction( const QString& text, int accel, QObject* parent,
                              const char* name )
  : KAction( text, accel, parent, name )
{
  d = new KSelectActionPrivate;
}

KSelectAction::KSelectAction( const QString& text, int accel,
                              const QObject* receiver, const char* slot,
                              QObject* parent, const char* name )
  : KAction( text, accel, receiver, slot, parent, name )
{
  d = new KSelectActionPrivate;
}

KSelectAction::KSelectAction( const QString& text, const QIconSet& pix,
                              int accel, QObject* parent, const char* name )
  : KAction( text, pix, accel, parent, name )
{
  d = new KSelectActionPrivate;
}

KSelectAction::KSelectAction( const QString& text, const QString& pix,
                              int accel, QObject* parent, const char* name )
  : KAction( text, pix, accel, parent, name )
{
  d = new KSelectActionPrivate;
}

KSelectAction::KSelectAction( const QString& text, const QIconSet& pix,
                              int accel, const QObject* receiver,
                              const char* slot, QObject* parent,
                              const char* name )
  : KAction( text, pix, accel, receiver, slot, parent, name )
{
  d = new KSelectActionPrivate;
}

KSelectAction::KSelectAction( const QString& text, const QString& pix,
                              int accel, const QObject* receiver,
                              const char* slot, QObject* parent,
                              const char* name )
  : KAction( text, pix, accel, receiver, slot, parent, name )
{
  d = new KSelectActionPrivate;
}

KSelectAction::KSelectAction( QObject* parent, const char* name )
  : KAction( parent, name )
{
  d = new KSelectActionPrivate;
}

KSelectAction::~KSelectAction()
{
  delete d; d = 0;
}

void KSelectAction::setCurrentItem( int id )
{
    if ( id < 0 )
        return;

    if ( id >= (int)d->m_list.count() )
        return;

    if ( d->m_menu )
    {
        if ( d->m_current >= 0 )
            d->m_menu->setItemChecked( d->m_current, FALSE );
        if ( id >= 0 )
            d->m_menu->setItemChecked( id, TRUE );
    }

    d->m_current = id;

    int len = containerCount();

    for( int i = 0; i < len; ++i )
        setCurrentItem( i, id );

    emit KAction::activated();
    emit activated( currentItem() );
    emit activated( currentText() );
}

QPopupMenu* KSelectAction::popupMenu()
{
  if ( !d->m_menu )
  {
    d->m_menu = new QPopupMenu;
    QStringList::ConstIterator it = d->m_list.begin();
    int id = 0;
    for( ; it != d->m_list.end(); ++it ) {
      if (!((*it).isEmpty())) {
        d->m_menu->insertItem( *it, this, SLOT( slotActivated( int ) ), 0, id++ );
      } else {
        d->m_menu->insertSeparator();
      }
    }
  }

  return d->m_menu;
}

void KSelectAction::changeItem( int index, const QString& text )
{
  if ( index < 0 || index >= (int)d->m_list.count() )
  {
    qDebug("KSelectAction::changeItem Index out of scope");
    return;
  }

  d->m_list[ index ] = text;

  if ( d->m_menu )
    d->m_menu->changeItem( index, text );

  int len = containerCount();
  for( int i = 0; i < len; ++i )
    changeItem( i, index, text );
}

void KSelectAction::changeItem( int, int, const QString& )
{
    //QWidget* w = container( i );
    // HANDLE KToolBar??
}

void KSelectAction::setItems( const QStringList &lst )
{
  d->m_list = lst;
  d->m_current = -1;

  if ( d->m_menu )
  {
    d->m_menu->clear();
    QStringList::ConstIterator it = d->m_list.begin();
    int id = 0;
    for( ; it != d->m_list.end(); ++it )
      d->m_menu->insertItem( *it, this, SLOT( slotActivated( int ) ), 0, id++ );
  }

  int len = containerCount();
  for( int i = 0; i < len; ++i )
    setItems( i, lst );
}

QStringList KSelectAction::items() const
{
  return d->m_list;
}

QString KSelectAction::currentText() const
{
  if ( currentItem() < 0 )
    return QString::null;

  return d->m_list[ currentItem() ];
}

int KSelectAction::currentItem() const
{
  return d->m_current;
}

void KSelectAction::setCurrentItem( int id, int index )
{
  QWidget* w = container( id );
  if ( w->inherits( "KToolBar" ) ) {
    QWidget* r = ( (KToolBar*)w )->getWidget( menuId( id ) );
    if ( r->inherits( "QComboBox" ) ) {
      QComboBox *b = (QComboBox*)r;
      b->setCurrentItem( index );
    }
  }
}

void KSelectAction::setItems( int id, const QStringList& lst )
{
  QWidget* w = container( id );
  if ( w->inherits( "KToolBar" ) ) {
    QWidget* r = ( (KToolBar*)w )->getWidget( menuId( id ) );
    if ( r->inherits( "QComboBox" ) ) {
      QComboBox *b = (QComboBox*)r;
      b->clear();
      QStringList::ConstIterator it = lst.begin();
      for( ; it != lst.end(); ++it )
        b->insertItem( *it );
      }
   }
}

int KSelectAction::plug( QWidget *widget, int index )
{
  if ( widget->inherits("QPopupMenu") )
  {
    // Create the PopupMenu and store it in m_menu
    (void)popupMenu();
    popupMenu()->setFont(KGlobal::menuFont());

    QPopupMenu* menu = (QPopupMenu*)widget;
    int id;
    if ( !pixmap().isNull() )
    {
      id = menu->insertItem( pixmap(), d->m_menu, -1, index );
    }
    else
    {
      if ( hasIconSet() )
        id = menu->insertItem( iconSet(), text(), d->m_menu, -1, index );
      else
        id = menu->insertItem( text(), d->m_menu, -1, index );
    }

    menu->setItemEnabled( id, isEnabled() );
    menu->setWhatsThis( id, whatsThis() );

    addContainer( menu, id );
    connect( menu, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }
  else if ( widget->inherits("KToolBar") )
  {
    KToolBar* bar = (KToolBar*)widget;
    int id_ = KAction::getToolButtonID();
    bar->insertCombo( items(), id_, isEditable(),
                      SIGNAL( activated( const QString & ) ), this,
                      SLOT( slotActivated( const QString & ) ), isEnabled(),
                      QString::null, 70, index );
    QComboBox *cb = bar->getCombo( id_ );
    if ( cb )
      cb->setMinimumWidth( cb->sizeHint().width() );

    addContainer( bar, id_ );

    connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }

  qDebug("Can not plug KAction in %s", widget->className() );
  return -1;
}

void KSelectAction::clear()
{
  if ( d->m_menu )
    d->m_menu->clear();

  int len = containerCount();
  for( int i = 0; i < len; ++i )
    clear( i );
}

void KSelectAction::clear( int id )
{
  QWidget* w = container( id );
  if ( w->inherits( "KToolBar" ) ) {
    QWidget* r = ( (KToolBar*)w )->getWidget( menuId( id ) );
    if ( r->inherits( "QComboBox" ) ) {
      QComboBox *b = (QComboBox*)r;
      b->clear();
    }
  }
}

void KSelectAction::slotActivated( int id )
{
  if ( d->m_current == id )
    return;

  if ( d->m_lock )
    return;

  d->m_lock = TRUE;

  setCurrentItem( id );

  d->m_lock = FALSE;
}

void KSelectAction::slotActivated( const QString &text )
{
  if ( d->m_lock )
    return;

  d->m_lock = true;
  if ( isEditable() )
  {
    QStringList lst = items();
    lst.append( text );
    setItems( lst );
  }

  setCurrentItem( items().findIndex( text ) );
  d->m_lock = false;
}

void KSelectAction::setEditable( bool edit )
{
  d->m_edit = edit;
}

bool KSelectAction::isEditable() const
{
  return d->m_edit;
}

class KListAction::KListActionPrivate
{
public:
  KListActionPrivate()
  {
    m_current = 0;
  }
  int m_current;
};

KListAction::KListAction( const QString& text, int accel, QObject* parent,
                          const char* name )
  : KSelectAction( text, accel, parent, name )
{
  d = new KListActionPrivate;
}

KListAction::KListAction( const QString& text, int accel,
                          const QObject* receiver, const char* slot,
                          QObject* parent, const char* name )
  : KSelectAction( text, accel, parent, name )
{
  d = new KListActionPrivate;
  connect( this, SIGNAL(activated(int)), receiver, slot );
}

KListAction::KListAction( const QString& text, const QIconSet& pix,
                          int accel, QObject* parent, const char* name )
  : KSelectAction( text, pix, accel, parent, name )
{
  d = new KListActionPrivate;
}

KListAction::KListAction( const QString& text, const QString& pix,
                            int accel, QObject* parent, const char* name )
  : KSelectAction( text, pix, accel, parent, name )
{
  d = new KListActionPrivate;
}

KListAction::KListAction( const QString& text, const QIconSet& pix,
                          int accel, const QObject* receiver,
                          const char* slot, QObject* parent,
                          const char* name )
  : KSelectAction( text, pix, accel, receiver, slot, parent, name )
{
  d = new KListActionPrivate;
  connect( this, SIGNAL(activated(int)), receiver, slot );
}

KListAction::KListAction( const QString& text, const QString& pix,
                          int accel, const QObject* receiver,
                          const char* slot, QObject* parent,
                          const char* name )
  : KSelectAction( text, pix, accel, receiver, slot, parent, name )
{
  d = new KListActionPrivate;
  connect( this, SIGNAL(activated(int)), receiver, slot );
}

KListAction::KListAction( QObject* parent, const char* name )
  : KSelectAction( parent, name )
{
  d = new KListActionPrivate;
}

KListAction::~KListAction()
{
  delete d; d = 0;
}

void KListAction::setCurrentItem( int index )
{
  d->m_current = index;

  emit KAction::activated();
  emit activated( currentItem() );
  emit activated( currentText() );
}

QString KListAction::currentText() const
{
  if ( currentItem() < 0 )
      return QString::null;

  return items()[ currentItem() ];
}

int KListAction::currentItem() const
{
  return d->m_current;
}

class KRecentFilesAction::KRecentFilesActionPrivate
{
public:
  KRecentFilesActionPrivate()
  {
    m_maxItems = 0;
  }
  uint m_maxItems;
};

KRecentFilesAction::KRecentFilesAction( const QString& text, int accel,
                                        QObject* parent, const char* name,
                                        unsigned int maxItems )
  : KListAction( text, accel, parent, name)
{
  d = new KRecentFilesActionPrivate;
  d->m_maxItems = maxItems;

  connect( this, SIGNAL( activated( const QString& ) ),
           this, SLOT( itemSelected( const QString& ) ) );
}

KRecentFilesAction::KRecentFilesAction( const QString& text, int accel,
                                        const QObject* receiver,
                                        const char* slot,
                                        QObject* parent, const char* name,
                                        uint maxItems )
  : KListAction( text, accel, parent, name)
{
  d = new KRecentFilesActionPrivate;
  d->m_maxItems = maxItems;

  connect( this, SIGNAL( activated( const QString& ) ),
           this, SLOT( itemSelected( const QString& ) ) );

  connect( this,     SIGNAL(urlSelected(const KURL&)),
           receiver, slot );
}

KRecentFilesAction::KRecentFilesAction( const QString& text,
                                        const QIconSet& pix, int accel,
                                        QObject* parent, const char* name,
                                        uint maxItems )
  : KListAction( text, pix, accel, parent, name)
{
  d = new KRecentFilesActionPrivate;
  d->m_maxItems = maxItems;

  connect( this, SIGNAL( activated( const QString& ) ),
           this, SLOT( itemSelected( const QString& ) ) );
}

KRecentFilesAction::KRecentFilesAction( const QString& text,
                                        const QString& pix, int accel,
                                        QObject* parent, const char* name,
                                        uint maxItems )
  : KListAction( text, pix, accel, parent, name)
{
  d = new KRecentFilesActionPrivate;
  d->m_maxItems = maxItems;

  connect( this, SIGNAL( activated( const QString& ) ),
           this, SLOT( itemSelected( const QString& ) ) );
}

KRecentFilesAction::KRecentFilesAction( const QString& text,
                                        const QIconSet& pix, int accel,
                                        const QObject* receiver,
                                        const char* slot,
                                        QObject* parent, const char* name,
                                        uint maxItems )
  : KListAction( text, pix, accel, parent, name)
{
  d = new KRecentFilesActionPrivate;
  d->m_maxItems = maxItems;

  connect( this, SIGNAL( activated( const QString& ) ),
           this, SLOT( itemSelected( const QString& ) ) );

  connect( this,     SIGNAL(urlSelected(const KURL&)),
           receiver, slot );
}

KRecentFilesAction::KRecentFilesAction( const QString& text,
                                        const QString& pix, int accel,
                                        const QObject* receiver,
                                        const char* slot,
                                        QObject* parent, const char* name,
                                        uint maxItems )
  : KListAction( text, pix, accel, parent, name)
{
  d = new KRecentFilesActionPrivate;
  d->m_maxItems = maxItems;

  connect( this, SIGNAL( activated( const QString& ) ),
           this, SLOT( itemSelected( const QString& ) ) );

  connect( this,     SIGNAL(urlSelected(const KURL&)),
           receiver, slot );
}

KRecentFilesAction::KRecentFilesAction( QObject* parent, const char* name,
                                        uint maxItems )
  : KListAction( parent, name )
{
  d = new KRecentFilesActionPrivate;
  d->m_maxItems = maxItems;

  connect( this, SIGNAL( activated( const QString& ) ),
           this, SLOT( itemSelected( const QString& ) ) );
}

KRecentFilesAction::~KRecentFilesAction()
{
  delete d; d = 0;
}

uint KRecentFilesAction::maxItems() const
{
    return d->m_maxItems;
}

void KRecentFilesAction::setMaxItems( uint maxItems )
{
    QStringList lst = items();
    uint oldCount   = lst.count();

    // set new maxItems
    d->m_maxItems = maxItems;

    // remove all items that are too much
    while( lst.count() > maxItems )
    {
        // remove last item
        lst.remove( lst.last() );
    }

    // set new list if changed
    if( lst.count() != oldCount )
        setItems( lst );
}

void KRecentFilesAction::addURL( const KURL& url )
{
    QString     file = url.url();
    QStringList lst = items();

    // remove file if already in list
    lst.remove( file );

    // remove las item if already maxitems in list
    if( lst.count() == d->m_maxItems )
    {
        // remove last item
        lst.remove( lst.last() );
    }

    // add file to list
    lst.prepend( file );
    setItems( lst );
}

void KRecentFilesAction::removeURL( const KURL& url )
{
    QStringList lst = items();
    QString     file = url.url();

    // remove url
    if( lst.count() > 0 )
    {
        lst.remove( file );
        setItems( lst );
    }
}

void KRecentFilesAction::clearURLList()
{
    clear();
}

void KRecentFilesAction::loadEntries( KConfig* config )
{
    QString     key;
    QString     value;
    QString     oldGroup;
    QStringList lst;

    oldGroup = config->group();

    config->setGroup( "RecentFiles" );

    // read file list
    for( unsigned int i = 1 ; i <= d->m_maxItems ; i++ )
    {
        key = QString( "File%1" ).arg( i );
        value = config->readEntry( key, QString::null );

	if (!value.isNull())
            lst.append( value );
    }

    // set file
    setItems( lst );

    config->setGroup( oldGroup );
}

void KRecentFilesAction::saveEntries( KConfig* config )
{
    QString     key;
    QString     value;
    QString     oldGroup;
    QStringList lst = items();

    oldGroup = config->group();

    config->setGroup( "RecentFiles" );

    // write file list
    for( unsigned int i = 1 ; i <= lst.count() ; i++ )
    {
        key = QString( "File%1" ).arg( i );
        value = lst[ i - 1 ];
        config->writeEntry( key, value );
    }

    config->setGroup( oldGroup );
}

void KRecentFilesAction::itemSelected( const QString& text )
{
    emit urlSelected( KURL( text ) );
}

class KFontAction::KFontActionPrivate
{
public:
  KFontActionPrivate()
  {
  }
  QStringList m_fonts;
};

KFontAction::KFontAction( const QString& text, int accel, QObject* parent,
                          const char* name )
  : KSelectAction( text, accel, parent, name )
{
    d = new KFontActionPrivate;
    get_fonts( d->m_fonts );
    KSelectAction::setItems( d->m_fonts );
    setEditable( TRUE );
}

KFontAction::KFontAction( const QString& text, int accel,
                          const QObject* receiver, const char* slot,
                          QObject* parent, const char* name )
    : KSelectAction( text, accel, receiver, slot, parent, name )
{
    d = new KFontActionPrivate;
    get_fonts( d->m_fonts );
    KSelectAction::setItems( d->m_fonts );
    setEditable( TRUE );
}

KFontAction::KFontAction( const QString& text, const QIconSet& pix, int accel,
                          QObject* parent, const char* name )
    : KSelectAction( text, pix, accel, parent, name )
{
    d = new KFontActionPrivate;
    get_fonts( d->m_fonts );
    KSelectAction::setItems( d->m_fonts );
    setEditable( TRUE );
}

KFontAction::KFontAction( const QString& text, const QString& pix, int accel,
                          QObject* parent, const char* name )
    : KSelectAction( text, pix, accel, parent, name )
{
    d = new KFontActionPrivate;
    get_fonts( d->m_fonts );
    KSelectAction::setItems( d->m_fonts );
    setEditable( TRUE );
}

KFontAction::KFontAction( const QString& text, const QIconSet& pix, int accel,
                          const QObject* receiver, const char* slot,
                          QObject* parent, const char* name )
    : KSelectAction( text, pix, accel, receiver, slot, parent, name )
{
    d = new KFontActionPrivate;
    get_fonts( d->m_fonts );
    KSelectAction::setItems( d->m_fonts );
    setEditable( TRUE );
}

KFontAction::KFontAction( const QString& text, const QString& pix, int accel,
                          const QObject* receiver, const char* slot,
                          QObject* parent, const char* name )
    : KSelectAction( text, pix, accel, receiver, slot, parent, name )
{
    d = new KFontActionPrivate;
    get_fonts( d->m_fonts );
    KSelectAction::setItems( d->m_fonts );
    setEditable( TRUE );
}


KFontAction::KFontAction( QObject* parent, const char* name )
  : KSelectAction( parent, name )
{
    d = new KFontActionPrivate;
    get_fonts( d->m_fonts );
    KSelectAction::setItems( d->m_fonts );
    setEditable( TRUE );
}

KFontAction::~KFontAction()
{
    delete d;
    d = 0;
}

void KFontAction::setFont( const QString &family )
{
  int i = d->m_fonts.findIndex( family.lower() );
  if ( i != -1 )
    setCurrentItem( i );
}

int KFontAction::plug( QWidget *w, int index )
{
  int container = KSelectAction::plug( w, index );

  if ( container != -1 && w->inherits( "KToolBar" ) )
    ((KToolBar *)w)->getCombo( menuId( container ) )->setAutoCompletion( TRUE );

  return container;
}

class KFontSizeAction::KFontSizeActionPrivate
{
public:
  KFontSizeActionPrivate()
  {
    m_lock = false;
  }
  bool m_lock;
};

KFontSizeAction::KFontSizeAction( const QString& text, int accel,
                                  QObject* parent, const char* name )
  : KSelectAction( text, accel, parent, name )
{
  init();
}

KFontSizeAction::KFontSizeAction( const QString& text, int accel,
                                  const QObject* receiver, const char* slot,
                                  QObject* parent, const char* name )
  : KSelectAction( text, accel, receiver, slot, parent, name )
{
  init();
}

KFontSizeAction::KFontSizeAction( const QString& text, const QIconSet& pix,
                                  int accel, QObject* parent, const char* name )
  : KSelectAction( text, pix, accel, parent, name )
{
  init();
}

KFontSizeAction::KFontSizeAction( const QString& text, const QString& pix,
                                  int accel, QObject* parent, const char* name )
  : KSelectAction( text, pix, accel, parent, name )
{
  init();
}

KFontSizeAction::KFontSizeAction( const QString& text, const QIconSet& pix,
                                  int accel, const QObject* receiver,
                                  const char* slot, QObject* parent,
                                  const char* name )
    : KSelectAction( text, pix, accel, receiver, slot, parent, name )
{
  init();
}

KFontSizeAction::KFontSizeAction( const QString& text, const QString& pix,
                                  int accel, const QObject* receiver,
                                  const char* slot, QObject* parent,
                                  const char* name )
  : KSelectAction( text, pix, accel, receiver, slot, parent, name )
{
  init();
}

KFontSizeAction::KFontSizeAction( QObject* parent, const char* name )
  : KSelectAction( parent, name )
{
  init();
}

KFontSizeAction::~KFontSizeAction()
{
    delete d;
    d = 0;
}

void KFontSizeAction::init()
{
    d = new KFontSizeActionPrivate;

    setEditable( TRUE );
    QValueList<int> sizes = get_standard_font_sizes();
    QStringList lst;
    for ( QValueList<int>::Iterator it = sizes.begin(); it != sizes.end(); ++it )
	lst.append( QString::number( *it ) );

    setItems( lst );
}

void KFontSizeAction::setFontSize( int size )
{
    if ( size == fontSize() ) {
	setCurrentItem( items().findIndex( QString::number( size ) ) );
	return;
    }

    if ( size < 1 || size > 128 ) {
	qDebug( "KFontSizeAction: Size %i is out of range", size );
	return;
    }

    int index = items().findIndex( QString::number( size ) );
    if ( index == -1 ) {
	QStringList lst = items();
	lst.append( QString::number( size ) );
	qHeapSort( lst );
	KSelectAction::setItems( lst );
	index = lst.findIndex( QString::number( size ) );
	setCurrentItem( index );
    }
    else
	setCurrentItem( index );


    emit KAction::activated();
    emit activated( index );
    emit activated( QString::number( size ) );
    emit fontSizeChanged( size );
}

int KFontSizeAction::fontSize() const
{
  return currentText().toInt();
}

void KFontSizeAction::slotActivated( int index )
{
  KSelectAction::slotActivated( index );

  emit fontSizeChanged( items()[ index ].toInt() );
}

void KFontSizeAction::slotActivated( const QString& size )
{
  if ( d->m_lock )
    return;

  if ( size.toInt() < 1 || size.toInt() > 128 )
  {
    qDebug( "KFontSizeAction: Size %s is out of range", size.latin1() );
    return;
  }

  d->m_lock = TRUE;
  setFontSize( size.toInt() );
  d->m_lock = FALSE;
}

class KActionMenu::KActionMenuPrivate
{
public:
  KActionMenuPrivate()
  {
    m_popup = new QPopupMenu;
    m_popup->setFont(KGlobal::menuFont());
  }
  ~KActionMenuPrivate()
  {
    delete m_popup; m_popup = 0;
  }
  QPopupMenu *m_popup;
};

KActionMenu::KActionMenu( QObject* parent, const char* name )
  : KAction( parent, name )
{
  d = new KActionMenuPrivate;
}

KActionMenu::KActionMenu( const QString& text, QObject* parent,
                          const char* name )
  : KAction( text, 0, parent, name )
{
  d = new KActionMenuPrivate;
}

KActionMenu::KActionMenu( const QString& text, const QIconSet& icon,
                          QObject* parent, const char* name )
  : KAction( text, icon, 0, parent, name )
{
  d = new KActionMenuPrivate;
}

KActionMenu::KActionMenu( const QString& text, const QString& icon,
                          QObject* parent, const char* name )
  : KAction( text, icon, 0, parent, name )
{
  d = new KActionMenuPrivate;
}

KActionMenu::~KActionMenu()
{
  delete d; d = 0;
}

void KActionMenu::popup( const QPoint& global )
{
  popupMenu()->popup( global );
}

QPopupMenu* KActionMenu::popupMenu()
{
  return d->m_popup;
}

void KActionMenu::insert( KAction* cmd, int index )
{
  if ( cmd )
    cmd->plug( d->m_popup, index );
}

void KActionMenu::remove( KAction* cmd )
{
  if ( cmd )
    cmd->unplug( d->m_popup );
}


int KActionMenu::plug( QWidget* widget, int index )
{
  if ( widget->inherits("QMenuBar") )
  {
    QMenuBar* bar = (QMenuBar*)widget;
    int id;
    id = bar->insertItem( text(), d->m_popup, -1, index );

    bar->setItemEnabled( id, isEnabled() );
	
    addContainer( bar, id );
    connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;	
  }
  else if ( widget->inherits("QPopupMenu") )
  {
    QPopupMenu* menu = (QPopupMenu*)widget;
    int id;
    if ( !pixmap().isNull() )
      id = menu->insertItem( pixmap(), d->m_popup, -1, index );	
    else
    {
      if ( hasIconSet() )
        id = menu->insertItem( iconSet(), text(), d->m_popup, -1, index );
      else
        id = menu->insertItem( text(), d->m_popup, -1, index );
    }

    menu->setItemEnabled( id, isEnabled() );

    addContainer( menu, id );
    connect( menu, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;	
  }
  else if ( widget->inherits( "KToolBar" ) )
  {
    KToolBar *bar = (KToolBar *)widget;

    int id_ = KAction::getToolButtonID();
    bar->insertButton( iconSet().pixmap(), id_, SIGNAL( clicked() ), this,
                       SLOT( slotActivated() ), isEnabled(), plainText(),
                       index );

    addContainer( bar, id_ );

    connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    bar->setDelayedPopup( id_, popupMenu() );

    return containerCount() - 1;
  }
  else if ( widget->inherits( "KMenuBar" ) )
  {
    KMenuBar *bar = (KMenuBar *)widget;

    int id;

    id = bar->insertItem( text(), popupMenu(), -1, index );
    bar->setItemEnabled( id, isEnabled() );

    addContainer( bar, id );
    connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }

  return -1;
}

void KActionMenu::unplug( QWidget* widget )
{
  if ( widget->inherits("QMenuBar") )
  {
    QMenuBar* bar = (QMenuBar*)widget;
    int i = findContainer( bar );
    if ( i != -1 )
    {
      bar->removeItem( menuId( i ) );
      removeContainer( i );
    }
  }
  if ( widget->inherits( "KToolBar" ) )
  {
    KToolBar *bar = (KToolBar *)widget;

    int idx = findContainer( bar );

    if ( idx != -1 )
    {
      bar->removeItem( menuId( idx ) );
      removeContainer( idx );
    }

    return;
  }
  else if ( widget->inherits( "KMenuBar" ) )
  {
    KMenuBar *bar = (KMenuBar *)widget;
    int i = findContainer( bar );
    if ( i != -1 )
    {
      bar->removeItem( menuId( i ) );
      removeContainer( i );
    }

    return;
  }
  else
    KAction::unplug( widget );
}

void KActionMenu::setEnabled( int id, bool b )
{
  QWidget *w = container( id );

  if ( w->inherits( "KToolBar" ) )
    ((KToolBar *)w)->setItemEnabled( menuId( id ), b );

  KAction::setEnabled( id, b );
}

void KActionMenu::setText( int id, const QString& text )
{
  QWidget *w = container( id );

  if ( w->inherits( "KToolBar" ) )
  {
    QWidget *button = ((KToolBar *)w)->getWidget( menuId( id ) );
    if ( button->inherits( "KToolBarButton" ) )
     ((KToolBarButton *)button)->setText( text );
  }

  KAction::setText( id, text );
}


void KActionMenu::setIconSet( int id, const QIconSet& iconSet )
{
  QWidget *w = container( id );

  if ( w->inherits( "KToolBar" ) )
    ((KToolBar *)w)->setButtonPixmap( menuId( id ), iconSet.pixmap() );

  KAction::setIconSet( id, iconSet );
}

KActionSeparator::KActionSeparator( QObject *parent, const char *name )
  : KAction( parent, name )
{
}

KActionSeparator::~KActionSeparator()
{
}

int KActionSeparator::plug( QWidget *widget, int index )
{
  if ( widget->inherits("QPopupMenu") )
  {
    QPopupMenu* menu = (QPopupMenu*)widget;

    int id = menu->insertSeparator( index );

    addContainer( menu, id );
    connect( menu, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }
  else if ( widget->inherits( "KMenuBar" ) )
  {
    KMenuBar *menuBar = (KMenuBar *)widget;

    int id = menuBar->insertSeparator( index );

    addContainer( menuBar, id );

    connect( menuBar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }
  else if ( widget->inherits( "KToolBar" ) )
  {
    KToolBar *toolBar = (KToolBar *)widget;

    int id = toolBar->insertSeparator( index );

    addContainer( toolBar, id );

    connect( toolBar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }

  return -1;
}

void KActionSeparator::unplug( QWidget *widget )
{
  if ( widget->inherits("QPopupMenu") )
  {
    QPopupMenu* menu = (QPopupMenu*)widget;
    int i = findContainer( menu );
    if ( i != -1 )
    {
      menu->removeItem( menuId( i ) );
      removeContainer( i );
    }
  }
  else if ( widget->inherits( "KMenuBar" ) )
  {
    KMenuBar *menuBar = (KMenuBar *)widget;

    int i = findContainer( menuBar );

    if ( i != -1 )
    {
      menuBar->removeItem( menuId( i ) );
      removeContainer( i );
    }
    return;
  }
  else if ( widget->inherits( "KToolBar" ) )
  {
    KToolBar *toolBar = (KToolBar *)widget;

    int i = findContainer( toolBar );

    if ( i != -1 )
    {
      toolBar->removeItem( menuId( i ) );
      removeContainer( i );
    }
    return;
  }

  return;
}

class KActionCollection::KActionCollectionPrivate
{
public:
  KActionCollectionPrivate()
  {
    m_dctHighlightContainers.setAutoDelete( true );
    m_highlight = false;
  }
  ~KActionCollectionPrivate()
  {
  }
  KInstance *m_instance;
  QList<KAction> m_actions;
  QPtrDict< QList<KAction> > m_dctHighlightContainers;
  bool m_highlight;
  QDict<KKeyEntry> m_keyDict;
};

KActionCollection::KActionCollection( QObject *parent, const char *name,
                                      KInstance *instance )
  : QObject( parent, name )
{
  d = new KActionCollectionPrivate;
  setInstance( instance );
}

KActionCollection::KActionCollection( const KActionCollection &copy )
    : QObject()
{
  d = new KActionCollectionPrivate;
  d->m_actions = copy.d->m_actions;
  setInstance( copy.instance() );
}

KActionCollection::~KActionCollection()
{
  delete d; d = 0;
}

void KActionCollection::childEvent( QChildEvent* ev )
{
  if ( ev->inserted() && ev->child()->inherits( "KAction" ) )
    insert( (KAction*)ev->child() );
  else if ( ev->removed() )
    // We can not emit a removed signal here since the
    // actions destructor did already run :-(
    d->m_actions.remove( (KAction*)ev->child() );
}

void KActionCollection::insert( KAction* action )
{
  uint len = d->m_actions.count();
  for( uint i = 0; i < len; ++i )
  {
    KAction* a = d->m_actions.at( i );
    if ( action == a )
      return;
  }

  d->m_actions.append( action );
  emit inserted( action );

  KKeyEntry *entry = new KKeyEntry;

  entry->aDefaultKeyCode = action->accel();
  entry->aCurrentKeyCode = action->accel();
  entry->aConfigKeyCode  = action->accel();
  entry->bConfigurable   = true;
  entry->descr           = action->plainText();

  entry->aAccelId = 0;
  entry->receiver = 0;
  entry->member   = 0;
  entry->menuId   = 0;
  entry->menu     = 0;

  d->m_keyDict.insert( action->name(), entry );
}

void KActionCollection::remove( KAction* action )
{
  uint len = d->m_actions.count();
  for( uint i = 0; i < len; ++i )
  {
    KAction* a = d->m_actions.at( i );
    if ( action == a )
    {
      d->m_actions.remove( i );
      emit removed( action );
      delete action;
      return;
    }
  }
}

KAction* KActionCollection::take( KAction* action )
{
  uint len = d->m_actions.count();
  for( uint i = 0; i < len; ++i )
  {
    KAction* a = d->m_actions.at( i );
    if ( action == a )
    {
      d->m_actions.remove( a );
      emit removed( action );
      return a;
    }
  }

  return 0;
}

KAction* KActionCollection::action( const char* name, const char* classname,
                                    QObject* component ) const
{
  QListIterator<KAction> it( d->m_actions );
  for( ; it.current(); ++it )
  {
    if ( ( !name || strcmp( it.current()->name(), name ) == 0 ) &&
        ( !classname || strcmp( it.current()->className(), classname ) == 0 ) &&
        ( !component || component == it.current()->component() ) )
      return it.current();
  }
  return 0;
}

KAction* KActionCollection::action( int index )
{
  return d->m_actions.at( index );
}

void KActionCollection::setKeyDict( QDict<KKeyEntry> entry )
{
  d->m_keyDict = entry;

  QDictIterator<KKeyEntry> it( entry );
  for( ; it.current(); ++it )
  {
    if (it.current()->aCurrentKeyCode != it.current()->aConfigKeyCode)
    {
      KAction *act = action( it.currentKey().latin1() );
      act->setAccel( it.current()->aConfigKeyCode );
    }
  }
}

QDict<KKeyEntry>* KActionCollection::keyDict()
{
  return &d->m_keyDict;
}

uint KActionCollection::count() const
{
  return d->m_actions.count();
}

QStringList KActionCollection::groups() const
{
  QStringList lst;

  QListIterator<KAction> it( d->m_actions );
  for( ; it.current(); ++it )
    if ( !it.current()->group().isEmpty() && !lst.contains( it.current()->group() ) )
      lst.append( it.current()->group() );

  return lst;
}

QValueList<KAction*> KActionCollection::actions( const QString& group ) const
{
  QValueList<KAction*> lst;

  QListIterator<KAction> it( d->m_actions );
  for( ; it.current(); ++it )
    if ( it.current()->group() == group )
      lst.append( it.current() );
    else if ( it.current()->group().isEmpty() && group.isEmpty() )
      lst.append( it.current() );

  return lst;
}

QValueList<KAction*> KActionCollection::actions() const
{
  QValueList<KAction*> lst;

  QListIterator<KAction> it( d->m_actions );
  for( ; it.current(); ++it )
    lst.append( it.current() );

  return lst;
}

KActionCollection KActionCollection::operator+(const KActionCollection &c ) const
{
  KActionCollection ret( *this );

  QValueList<KAction *> actions = c.actions();
  QValueList<KAction *>::ConstIterator it = actions.begin();
  QValueList<KAction *>::ConstIterator end = actions.end();
  for (; it != end; ++it )
    ret.insert( *it );

  return ret;
}

KActionCollection &KActionCollection::operator=( const KActionCollection &c )
{
  d->m_actions = c.d->m_actions;
  setInstance( c.instance() );
  return *this;
}

KActionCollection &KActionCollection::operator+=( const KActionCollection &c )
{
  QListIterator<KAction> it(c.d->m_actions);
  for ( ; it.current(); ++it )
    insert( it.current() );

  return *this;
}

void KActionCollection::setInstance( KInstance *instance )
{
  if ( instance )
    d->m_instance = instance;
  else
    d->m_instance = KGlobal::instance();
}

KInstance *KActionCollection::instance() const
{
  return d->m_instance;
}

void KActionCollection::connectHighlight( QWidget *container, KAction *action )
{
  if ( !d->m_highlight )
    return;

  QList<KAction> *actionList = d->m_dctHighlightContainers[ container ];

  if ( !actionList )
  {
    actionList = new QList<KAction>;

    connect( container, SIGNAL( highlighted( int ) ),
	     this, SLOT( slotHighlighted( int ) ) );

    connect( container, SIGNAL( destroyed() ),
	     this, SLOT( slotDestroyed() ) );

    d->m_dctHighlightContainers.insert( container, actionList );
  }

  actionList->append( action );
}

void KActionCollection::disconnectHighlight( QWidget *container, KAction *action )
{
  if ( !d->m_highlight )
    return;

  QList<KAction> *actionList = d->m_dctHighlightContainers[ container ];

  if ( !actionList )
    return;

  actionList->removeRef( action );

  if ( actionList->count() == 0 )
    d->m_dctHighlightContainers.remove( container );
}

void KActionCollection::slotHighlighted( int id )
{
  if ( !d->m_highlight )
    return;

  const QWidget *container = (const QWidget *)sender();

  QList<KAction> *actionList = d->m_dctHighlightContainers[ (void *)container  ];

  if ( !actionList )
    return;

  //  qDebug( "highlight -- id is %i", id );

  QListIterator<KAction> it( *actionList );
  for (; it.current(); ++it )
    if ( it.current()->isPlugged( (QWidget *)container, id ) )
    {
      // qDebug( "action highlighted: %s", it.current()->name() );
      emit actionHighlighted( it.current() );
      return;
    }
}

void KActionCollection::slotDestroyed()
{
  d->m_dctHighlightContainers.remove( (void *)sender() );
}

#include "kaction.moc"

