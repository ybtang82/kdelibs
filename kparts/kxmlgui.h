/* This file is part of the KDE libraries
    Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>

    This library is free software; you can redistribute it and/or
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

#ifndef __kxmlgui_h__
#define __kxmlgui_h__

#include <qobject.h>
#include <qaction.h>
#include <qdom.h>
#include <qmap.h>
#include <qguardedptr.h>

namespace KParts
{

class Part;
class XMLGUIFactoryPrivate;

/**
 * This file contains all the "kernel" of KParts.
 * You shouldn't have to look in here unless you want to hack KParts.
 */

/**
 * Abstract interface for a "GUI builder", used by the GUIFactory
 */
class XMLGUIBuilder
{
 public:

  XMLGUIBuilder() {}
  virtual ~XMLGUIBuilder() {}

  /**
   * Create a container (menubar/menu/toolbar/statusbar/separator/...) from an
   * element in the XML file
   * @param parent the parent for the container
   * @param index the index where the container should be inserted into the parent container/widget
   * @param element the element from the DOM tree describing the container (use it to access
   *                container specified attributes or child elements)
   * @param containerStateBuffer a buffer which possibibly contains previously saved container state
   *        information, return via @ref removeContainer .
   */
  virtual QObject *createContainer( QWidget *parent, int index, const QDomElement &element, const QByteArray &containerStateBuffer ) = 0;

  /**
   * Remove the given (and previously via @ref createContainer ) created container.
   * @return a buffer containing state information about the deleted container (like the last position of
   *         a toolbar container for example) . The buffer is passed again to @ref createContainer when
   *         the same container is about to be created again later.
   */
  virtual QByteArray removeContainer( QObject *container, QWidget *parent ) = 0;
};

class XMLGUIServantPrivate;
class XMLGUIFactory;

/**
 * Abstract interface for serving actions and xml to the GUI factory
 */
class XMLGUIServant
{
 public:
  XMLGUIServant();
  virtual ~XMLGUIServant();

  virtual QAction *action( const QDomElement &element ) = 0;

  virtual QDomDocument document() = 0;

  /**
   * default implementation, storing the given data in an internal map. Called from
   * KXMLGUIFactory when removing containers which were owned by the servant.
   */
  virtual void storeContainerStateBuffer( const QString &key, const QByteArray &data );
  /**
   * default implementation, returning a previously via @ref storeContainerStateBuffer saved
   * data. Called from KXMLGUIFactory when creating a new container.
   */
  virtual QByteArray takeContainerStateBuffer( const QString &key );

  void setFactory( XMLGUIFactory *factory );
  XMLGUIFactory *factory() const;

 private:
  XMLGUIServantPrivate *d;
};

class XMLGUIContainerNode;

class XMLGUIFactory
{
 public:
  XMLGUIFactory( XMLGUIBuilder *builder );
  ~XMLGUIFactory();

  // XXX move to somewhere else? (Simon)
  static QString readConfigFile( const QString &filename );

  /**
   * Creates the GUI described by the QDomDocument of the servant, using the servant's actions, and
   * merges it with the previously created GUI.
   */
  void addServant( XMLGUIServant *servant );

  /**
   * Removes the GUI described by the servant, by unplugging all provided actions and removing all owned
   * containers (and storing container state information in the given servant)
   */
  void removeServant( XMLGUIServant *servant );

 private:
  void buildRecursive( const QDomElement &element, XMLGUIContainerNode *parentNode );
  bool removeRecursive( XMLGUIContainerNode *node );

  static XMLGUIContainerNode *findContainer( XMLGUIContainerNode *node, const QDomElement &element, const QList<QObject> &excludeList );

  XMLGUIServant *m_servant;
  XMLGUIBuilder *m_builder;

  XMLGUIFactoryPrivate *d;
};

};

#endif
