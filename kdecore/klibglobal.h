/*
* klibglobal.h -- Declaration of class KGlobal.
* Generated by newclass on Sat May  1 02:08:43 EST 1999.
*/
#ifndef _KLIBGLOBAL_H
#define _KLIBGLOBAL_H

class KApplication;
class KStandardDirs;
class KConfig;
class KLocale;
class KIconLoader;
class KCharsets;
class QFont;

#include <qstring.h>


/**
* Accessors to KDE global objects for use in shared libraries.
*
* @author Torben Weis
* @version $Id$
*/
class KLibGlobal
{
public:

  KLibGlobal( const QString& libname );
  /**
   * Destructor
   */
  ~KLibGlobal();

  /**
   *  Retrieves the application standard dirs object.
   */
  KStandardDirs	*dirs();
	
  /**
   *  Retrieves the general config object.
   */
  KConfig *config();
	
  /**
   *  Retrieves an iconloader object.
   */
  KIconLoader	*iconLoader();

  KLocale		*locale();
  KCharsets	*charsets();

  QFont            generalFont();
  QFont            fixedFont();

  int             dndEventDelay();

    QString name() const;

protected:
    /**
     *  Copy Constructor is not allowed
     */
  KLibGlobal( const KLibGlobal& ) { }

private:
    KStandardDirs	*_dirs;

    KConfig		*_config;
    KIconLoader	*_iconLoader;
    
    QString _name;
};

#endif // SSK_KGLOBAL_H
