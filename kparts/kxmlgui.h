
#ifndef __kxmlgui_h__
#define __kxmlgui_h__

#include <qobject.h>
#include <qaction.h>
#include <qdom.h>

class KXMLGUIBuilder;

/**
 * Abstract interface for serving actions and xml to the GUI factory
 */
class KXMLGUIServant
{
 public:
  
  virtual QAction *action( const QDomElement &element ) = 0;

  virtual QString xml() = 0;

};

/**
 * Implementation of the servant interface that serves nothing.
 * Used when no part is active.
 */
class KNullGUIServant : public KXMLGUIServant
{
 public:
  KNullGUIServant() {}

  virtual QAction *action( const QDomElement & ) { return 0L; }
  virtual QString xml() { return QString::null; }
};

/**
 * In order to make the GUI-merging very flexible, it is a bit "abstract".
 * Let's try to explain a bit the servants/factory/builder concept :
 *
 * The usual case for GUI merging is merging actions for a "shell" (main
 * window) with actions for a "part" (embedded component)
 * In this case we have :
 *  shell servant + part servant => factory (who merges) => builder (who creates)
 *
 * "servants" are objects that serve actions to the factory, upon request
 * the "factory" is responsible for the actual merging
 * the "builder" is the class that creates the elements
 *
 * Servants and Buidler are interfaces, so that the factory can be 'plugged'
 * to a lot of different inputs and outputs.
 */
class KXMLGUIFactory
{
 public:
  static QString readConfigFile( const QString &filename );

  static void createGUI( KXMLGUIServant *shell, KXMLGUIServant *part, KXMLGUIBuilder *builder );

 private:
  KXMLGUIFactory( KXMLGUIServant *shellServant, KXMLGUIServant *partServant, KXMLGUIBuilder *builder );

  void buildRecursive( const QDomElement &shellElement,
                       const QDomElement &partElement,
                       QObject *parent = 0L );

  QDomElement findMatchingElement( const QDomElement &shellElement, const QDomElement &partElement );

  KXMLGUIServant *m_shellServant;
  KXMLGUIServant *m_partServant;
  KXMLGUIBuilder *m_builder;

};

#endif
