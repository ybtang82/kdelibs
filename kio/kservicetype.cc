#include "kservicetype.h"

#include <assert.h>
#include <qsmartptr.h>

QList<KServiceType>* KServiceType::s_lstServiceTypes = 0L;

void KServiceType::initStatic()
{
  if ( s_lstServiceTypes == 0 )
    s_lstServiceTypes = new QList<KServiceType>;
}

KServiceType::KServiceType( KSimpleConfig& _cfg )
{
  initStatic();
  s_lstServiceTypes->append( this );
  _cfg.setDesktopGroup();

  // Is this a bug or an intentionnal change ? Currently mimetype files
  // contain "MimeType=", not "Name=" !  (David)
  // m_strName = _cfg.readEntry( "Name" );

  // -> reverted to "MimeType=" temporarily, but seems wrong since
  // some servicetype are not mimetypes, if I got it correctly (any example, Torben ? ;) )
  m_strName = _cfg.readEntry( "MimeType" );

  m_strComment = _cfg.readEntry( "Comment" );
  m_strIcon = _cfg.readEntry( "Icon" );
  // TODO: Properties

  //QSmartPtr<KGroupIterator> git = _cfg.groupIterator();
  QValueListIterator<QString> git = _cfg.groupList().begin();
  for( ; git != _cfg.groupList().end(); ++git )
  {
    if( (*git).find( "Property::" ) == 0 )
    {
      _cfg.setGroup( *git );
      m_mapProps.insert( (*git).mid( 10 ),
			 _cfg.readPropertyEntry( "Value",
						 QProperty::nameToType( _cfg.readEntry( "Type" ) ) ) );
    }
  }

  git = _cfg.groupList().begin();
  for( ; git != _cfg.groupList().end(); ++git )
  {
    if( (*git).find( "PropertyDef::" ) == 0 )
    {
      _cfg.setGroup( *git );
      m_mapPropDefs.insert( (*git).mid( 13 ),
			    QProperty::nameToType( _cfg.readEntry( "Type" ) ) );
    }
  }

  m_bValid = !m_strName.isEmpty();
}

KServiceType::KServiceType( const QString& _type, const QString& _icon, const QString& _comment )
{
  initStatic();
  s_lstServiceTypes->append( this );

  m_strName = _type;
  m_strIcon = _icon;
  m_strComment = _comment;
  m_bValid = !m_strName.isEmpty();
}

KServiceType::~KServiceType()
{
  s_lstServiceTypes->removeRef( this );
}

KServiceType::PropertyPtr KServiceType::property( const QString& _name ) const
{
  QProperty* p = 0;

  if ( _name == "Name" )
    p = new QProperty( m_strName );
  if ( _name == "Icon" )
    p = new QProperty( m_strIcon );
  if ( _name == "Comment" )
    p = new QProperty( m_strComment );

  if ( p )
  {
    // We are not interestes in these
    p->deref();
    return p;
  }

  QMap<QString,QProperty>::ConstIterator it = m_mapProps.find( _name );
  if ( it == m_mapProps.end() )
    return (QProperty*)0;

  p = (QProperty*)(&(it.data()));

  return p;
}

QStringList KServiceType::propertyNames() const
{
  QStringList res;

  QMap<QString,QProperty>::ConstIterator it = m_mapProps.begin();
  for( ; it != m_mapProps.end(); ++it )
    res.append( it.key() );

  res.append( "Name" );
  res.append( "Comment" );
  res.append( "Icon" );

  return res;
}

QProperty::Type KServiceType::propertyDef( const QString& _name ) const
{
  QMap<QString,QProperty::Type>::ConstIterator it = m_mapPropDefs.find( _name );
  if ( it == m_mapPropDefs.end() )
    return QProperty::Empty;
  return it.data();
}

QStringList KServiceType::propertyDefNames() const
{
  QStringList l;

  QMap<QString,QProperty::Type>::ConstIterator it = m_mapPropDefs.begin();
  for( ; it != m_mapPropDefs.end(); ++it )
    l.append( it.key() );

  return l;
}

void KServiceType::load( QDataStream& _str )
{
  Q_INT8 b;
  _str >> m_strName >> m_strIcon >> m_strComment >> m_mapProps >> m_mapPropDefs >> b;
  m_bValid = b;
}

void KServiceType::save( QDataStream& _str ) const
{
  _str << m_strName << m_strIcon << m_strComment << m_mapProps << m_mapPropDefs << (Q_INT8)m_bValid;
}

KServiceType* KServiceType::serviceType( const QString& _name )
{
  assert( s_lstServiceTypes );
  QListIterator<KServiceType> it( *s_lstServiceTypes );
  for( ; it.current(); ++it )
    if ( it.current()->name() == _name )
      return it.current();

  return 0;
}

QDataStream& operator>>( QDataStream& _str, KServiceType& s )
{
  s.load( _str );
  return _str;
}

QDataStream& operator<<( QDataStream& _str, const KServiceType& s )
{
  s.save( _str );
  return _str;
}
