
#include "kxmlgui.h"

#include <qfile.h>

QString KXMLGUIFactory::readConfigFile( const QString &filename )
{
  QFile file( filename );
  if ( !file.open( IO_ReadOnly ) )
    return QString::null;

  uint size = file.size();
  char* buffer = new char[ size + 1 ];
  file.readBlock( buffer, size );
  buffer[ size ] = 0;
  file.close();

  QString text = QString::fromUtf8( buffer, size );
  delete[] buffer;

  return text;
}

void KXMLGUIFactory::createGUI( KXMLGUIServant *shell, KXMLGUIServant *part, KXMLGUIBuilder *builder )
{

  QDomDocument shellXML;
  shellXML.setContent( shell->xml() );
  QDomElement shellDoc = shellXML.documentElement();

  QDomDocument partXML;
  partXML.setContent( part->xml() );
  QDomElement partDoc = partXML.documentElement();

  qDebug( "starting recursive build" );

  KXMLGUIFactory factory( shell, part, builder );

  factory.buildRecursive( shellDoc, partDoc );
}

KXMLGUIFactory::KXMLGUIFactory( KXMLGUIServant *shellServant, KXMLGUIServant *partServant, KXMLGUIBuilder *builder )
{
  m_shellServant = shellServant;
  m_partServant = partServant;
  m_builder = builder;
}

void KXMLGUIFactory::buildRecursive( const QDomElement &shellElement,
                                     const QDomElement &partElement,
                                     QObject *parent )
{

  QDomElement servantElement = shellElement;
  KXMLGUIServant *servant = m_shellServant;

  QStringList shellElementChildNames;

  if ( servantElement.isNull() )
  {
    qDebug( "switch to part servant" );
    servantElement = partElement;
    servant = m_partServant;
  }
  else
  {
    QDomElement e = shellElement.firstChild().toElement();
    for (; !e.isNull(); e = e.nextSibling().toElement() )
      shellElementChildNames.append( e.tagName() );
  }

  QDomElement e = servantElement.firstChild().toElement();
  for (; !e.isNull(); e = e.nextSibling().toElement() )
  {
    qDebug( "parsing tag %s with possible name %s", e.tagName().ascii(), e.attribute( "name" ).ascii() );
    if ( e.tagName() == "Part" && servant == m_shellServant )
    {
      QDomElement p = findMatchingElement( e.parentNode().toElement(), 
                                           partElement.parentNode().toElement().firstChild().toElement() );

      buildRecursive( QDomElement(), p, parent );
    }
    else if ( e.tagName() == "Action" )
    {
      QAction *action = servant->action( e );
     
      if ( action && parent && parent->inherits( "QWidget" ) )
        action->plug( (QWidget *)parent );

      if ( !action )
        qDebug( "cannot find action %s", e.attribute( "name" ).ascii() );

    }
    else
    {
      QWidget *container = m_builder->createContainer( parent, e );

      if ( !container )
        continue;

      if ( servant == m_shellServant )
        buildRecursive( e,
                        findMatchingElement( e, partElement.firstChild().toElement() ), 
                        container );
      else
	buildRecursive( QDomElement(), e, container );
      
    }
  
  }  

  if ( shellElementChildNames.count() > 0 && !parent )
  {
    QDomElement e = partElement.firstChild().toElement();
    for (; !e.isNull(); e = e.nextSibling().toElement() )
      if ( !shellElementChildNames.contains( e.tagName() ) )
      {
	QWidget *container = m_builder->createContainer( 0, e );

	if ( !container )
	  return;

	buildRecursive( QDomElement(), e );
      }
  }

}

QDomElement KXMLGUIFactory::findMatchingElement( const QDomElement &shellElement, const QDomElement &partElement )
{
  QDomElement p;
  QDomElement i = partElement;

  QString name = shellElement.attribute( "name" ); 

  if ( !name.isEmpty() )
  {
    for (; !i.isNull(); i = i.nextSibling().toElement() )
      if ( i.attribute( "name" ) == name )
      {
        p = i;
        break;
      }
    return p;
  }

  name = shellElement.tagName();

  for (; !i.isNull(); i = i.nextSibling().toElement() )
    if ( i.tagName() == name )
      {
	p = i;
	break;
      }

  return p;
}
