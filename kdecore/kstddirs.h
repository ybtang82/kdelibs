/*
* kstddirs.h -- Declaration of class KStandardDirs.
* Generated by newclass on Thu Mar  5 16:05:28 EST 1998.
*/

#ifndef SSK_KSTDDIRS_H
#define SSK_KSTDDIRS_H

#include <qstring.h>
#include <qdict.h>
#include <qstringlist.h>

/**
* Site-independent access to standard KDE directories.
*
* The various directory accessors allow you to access the various
* KDE FSSTND directories in a portable way.
*
* Since the KDE FSSTND is heirarchical, the accessors provide the
* ability to get the directory in the most to the least specific
* location (in order, Application/User/System);
*
* Additionally, each accessor has the ability to find the most
* specific directory that actually exists. The default is no check
* whether the directory exists.
* 
* Ordinarily, you will not want to manually instantiate this class.
* The global @ref KApplication object instantiates and provides const 
* access to a KStandardDirs object via the 
* @ref KApplication::dirs method.
*
* standard resources that kdelibs allocates are:
* @li exe - executables in $prefix/bin. @see findExe for a function 
*     that takes $PATH into account
* @li cgi - CGIs to run from kdehelp
* @li config - configuration files
* @li apps - applications menu (.desktop files)
* @li data - where applications store data (add application subdir)
* @li html - HTML documentation 
* @li icon - icons
* @li mini - miniature icons
* @li locale - translation files for KLocale
* @li mime - mime types
* @li services - services
* @li toolbar - toolbar pictures
* @li wallpaper - wallpapers
* @li sound - application sounds
* 
* @author Sirtaj Singh Kang <taj@kde.org>
* @version $Id$
*/
class KStandardDirs
{
public:
        /**
	 * KStandardDirs Constructor.
	 * @param appName The name of the application, which will be
	 *		used for searching the "apps" directory.
	 */
	KStandardDirs( const QString& appName = QString::null );

	/**
	 * KStandardDirs Destructor.
	 */
	virtual ~KStandardDirs();

	/** 
	 * The scope of a directory. App is the application-specific
	 * directory in the user's local KDE directory, User is the
	 * user's local KDE directory, SysApp is the application-specific
	 * directory in the system directory and System is the system
	 * directory in which KDE is installed.
	 *
	 * Closest is used for searching the first directory in the
	 * list that exists, in the order App, User, SysApp, System.
	 */
	enum DirScope { Closest, App, User, SysApp, System };

	/*
	 * This adds another search dir to front of the fsstnd list.
	 * @li when compiling kdelibs, the prefix is added to this.
	 * @li when compiling a separate app, kapp adds the new prefix
	 * @li additional dirs may be loaded from share/config/kdeglobals (?)
	 *
	 * @param tosave specifies whether the dir will be saved to kderc.
	 * Any hardcoded dirs should not be saved, so kapp would make this
	 * false.
	 */
	void addPrefix( QString dir, bool tosave = false );


	/*
	 * with this we add types
	 * all basic types ("html", "config", etc) are added by kdelibs.
	 *
	 * @param type specifies a short descriptive string to access
	 * files of this type.
	 * @param relativename specifies a directory relative to the root
	 * of the KFSSTND
	 */
	bool addResourceType( const QString& type,
			      const QString& relativename );


	/**
	 * Adds hard path to the front of the search path for
	 * particular types for example in case of icons where
	 * the user specifies extra paths.
	 * 
	 * @param type specifies a short descriptive string to access files 
	 * of this type.
	 * @param absdir points to directory where to look for this specific
	 * type. Non-existant directories may be saved but pruned.
	 * @param tosave is same as with addPrefix.
	 * 
	 */
	bool addResourceDir( const QString& type, 
			     const QString& absdir, 
			     bool tosave = false );

	/**
	 * Tries to find resource in the following order:
	 * @li all hard paths (most recent first)
	 * @li all PREFIX/<relativename> paths (most recent first)
	 *
	 * @return a full path to the filename specified in the second
	 *         argument.
	 */
	QString findResource( const QString& type, 
			      const QString& filename ) const;

	/**
	 * Tries to find all directories whose name consists of the
	 * specified type and a relative path.
	 *
	 * @param type the type of the base directory.
	 * @param reldir relative directory.
	 *
	 * @return a list of matching directories.
	 */
	QStringList findDirs( const QString& type, 
                              const QString& reldir ) const;

	/**
	 * Tries to find the directory the file is in.
	 * It works the same as findResource, but it doesn't
	 * return the filename but the name of the directory.
	 * This way the application can access a couple of files
	 * that have been installed into the same directory without
	 * having to look for each file.
	 *
	 * @return the directory where the file specified in the second
	 *         argument is located.
	 */
	QString findResourceDir( const QString& type,
				 const QString& filename) const;


	/**
	 * Tries to find all resources with the specified type.
	 * The function will look into all specified directories
	 * and returns all filenames in these directories.
	 *
	 * @param type the type of resource to locate directories for.
	 * @param recursive specifies if the function should decend
	 *        into subdirectories.
	 * @return a list of directories matching the resource specified.
	 */
	QStringList findAllResources( const QString& type, 
				      bool recursive = false) const;

	/** 
	 * Finds the executable in the system path. A valid executable must
	 * be a file and have its executable bit set.
	 *
	 * @see #findAllExe
	 * @param appname the name of the executable file for which to search.
	 * @param pathstr The path which will be searched. If this is 
	 * 		0 (default), the $PATH environment variable will 
	 *		be searched.
	 * @param ignoreExecBit	If true, an existing file will be returned
	 *			even if its executable bit is not set.
	 *
	 * @return The path of the executable. If it was not found, this string 
	 *	will be null.
	 */
	static QString findExe( const QString& appname, 
				const QString& pathstr=QString::null,
				bool ignoreExecBit=false );

	/** 
	 * Finds all occurences of an executable in the system path.
	 *
	 * @see	#findExe
	 *
	 * @param list	will be filled with the pathnames of all the
	 *		executables found. Will be empty if the executable
	 *		was not found.
	 * @param appname	The name of the executable for which to
	 *	 		search.
	 * @param pathstr	The path list which will be searched. If this
	 *		is 0 (default), the $PATH environment variable will
	 *		be searched.
	 * @param ignoreExecBit If true, an existing file will be returned
	 *			even if its executable bit is not set.
	 *
	 * @return The number of executables found, 0 if none were found.
	 */
	static int findAllExe( QStringList& list, const QString& appname,
			       const QString& pathstr=QString::null, 
			       bool ignoreExecBit=false );

	/**
	 * This function adds the defaults that are used by the current
	 * KDE version. It's a serie of addResourceTypes and addPrefix calls.
	 * You normally wouldn't call this function, it's called for you from
	 * KGlobal.
	 */
	void addKDEDefaults();


	/**
	 * returns the list of possible directories for the type 
	 * The functions updates the cache if possible
	 */
	QStringList getResourceDirs(const QString& type) const;

	static QString kde_data_relative();

 private:

	QStringList prefixes;

	// Directory dictionaries
	QDict<QStringList> absolutes;
	QDict<QStringList> relatives;
	
	mutable QDict<QStringList> dircache;

	// Disallow assignment and copy-construction
	KStandardDirs( const KStandardDirs& );
	KStandardDirs& operator= ( const KStandardDirs& );
     
};

/**
 * This function is just for convience. It simply calls 
 * KGlobal::dirs()->findResource(type, filename)
 **/
QString locate( const QString& type, const QString& filename );

#endif // SSK_KSTDDIRS_H
