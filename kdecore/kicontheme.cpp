/* vi: ts=8 sts=4 sw=4
 *
 * $Id$
 *
 * This file is part of the KDE project, module kdecore.
 * Copyright (C) 2000 Geert Jansen <jansen@kde.org>
 *
 * This is free software; it comes under the GNU Library General
 * Public License, version 2. See the file "COPYING.LIB" for the
 * exact licensing terms.
 *
 * kicontheme.cpp: Lowlevel icon theme handling.
 */

#include <stdlib.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qlist.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qimage.h>
#include <qfileinfo.h>
#include <qdir.h>

#include <kdebug.h>
#include <kstddirs.h>
#include <kglobal.h>
#include <kconfig.h>
#include <ksimpleconfig.h>
#include <kinstance.h>

#include "kicontheme.h"

class KIconThemePrivate
{
public:
    QString example;
    QString screenshot;
};

/**
 * A subdirectory in an icon theme.
 */
class KIconThemeDir
{
public:
    KIconThemeDir(const QString& dir, const KConfigBase *config);

    QString iconPath(const QString& name) const;
    QStringList iconList() const;
    QString dir() const { return mDir; }

    int context() const { return mContext; }
    int type() const { return mType; }
    int size() const { return mSize; }
    int minSize() const { return mMinSize; }
    int maxSize() const { return mMaxSize; }

private:
    bool mbValid;
    int mType, mSize, mContext;
    int mMinSize, mMaxSize;

    QString mDir;
};


/*** KIconTheme ***/

KIconTheme::KIconTheme(const QString& name, const QString& appName)
{
    d = new KIconThemePrivate;
    bool isApp = !appName.isEmpty();
    if (isApp && (name != "hicolor") && (name != "locolor"))
    {
	kdDebug(264) << "Only hicolor and locolor themes can be local.\n";
	return;
    }
    QStringList icnlibs = KGlobal::dirs()->resourceDirs("icon");
    QStringList::ConstIterator it;
    for (it=icnlibs.begin(); it!=icnlibs.end(); it++)
    {
	QFileInfo fi(*it + name + "/index.desktop");
	if (fi.exists())
	    break;
    }
    if (it == icnlibs.end())
    {
	kdDebug(264) << "Icon theme " << name << " not found.\n";
	return;
    }
    mDir = *it + name + "/";
    KConfig *cfg = new KSimpleConfig(mDir + "index.desktop");
    cfg->setGroup("KDE Icon Theme");
    mName = cfg->readEntry("Name");
    mDesc = cfg->readEntry("Description");
    mDepth = cfg->readNumEntry("DisplayDepth", 32);
    mInherits = cfg->readListEntry("Inherits");
    d->example = cfg->readEntry("Example");
    d->screenshot = cfg->readEntry("ScreenShot");

    if (isApp)
    {
	// We just read the global theme description file. This is intended
	// for app specific themes too.
	icnlibs = KGlobal::dirs()->resourceDirs("data");
	for (it=icnlibs.begin(); it!=icnlibs.end(); it++)
	{
	    QFileInfo fi(*it + appName + "/icons/" + name);
	    if (fi.exists())
		break;
	}
	if (it == icnlibs.end())
	    return;
	mDir = *it + appName + "/icons/" + name + "/";
	mName += "-";
	mName += appName;
    }

    QStringList dirs = cfg->readListEntry("Directories");
    mDirs.setAutoDelete(true);
    for (it=dirs.begin(); it!=dirs.end(); it++)
    {
	cfg->setGroup(*it);
	mDirs.append(new KIconThemeDir(mDir + *it, cfg));
    }

    // Expand available sizes for scalable icons to their full range
    int i;
    QMap<int,QValueList<int> > scIcons;
    for (KIconThemeDir *dir=mDirs.first(); dir!=0L; dir=mDirs.next())
    {
	if ((dir->type() == KIcon::Scalable) && !scIcons.contains(dir->size()))
	{
	    QValueList<int> lst;
	    for (i=dir->minSize(); i<=dir->maxSize(); i++)
		lst += i;
	    scIcons[dir->size()] = lst;
	}
    }

    QStringList groups;
    groups += "Desktop";
    groups += "Toolbar";
    groups += "MainToolbar";
    groups += "Small";
    cfg->setGroup("KDE Icon Theme");
    for (it=groups.begin(), i=0; it!=groups.end(); it++, i++)
    {
	mDefSize[i] = cfg->readNumEntry(*it + "Default", 32);
	QValueList<int> lst = cfg->readIntListEntry(*it + "Sizes"), exp;
	QValueList<int>::ConstIterator it2;
	for (it2=lst.begin(); it2!=lst.end(); it2++)
	{
	    if (scIcons.contains(*it2))
		exp += scIcons[*it2];
	    else
		exp += *it2;
	}
	mSizes[i] = exp;
    }

    delete cfg;
}

KIconTheme::~KIconTheme()
{
    delete d;
}

bool KIconTheme::isValid() const
{
    return !mDirs.isEmpty();
}

QString KIconTheme::example() const { return d->example; }

QString KIconTheme::screenshot() const { return d->screenshot; }

int KIconTheme::defaultSize(int group) const
{
    if ((group < 0) || (group >= KIcon::LastGroup))
    {
	kdDebug(264) << "Illegal icon group: " << group << "\n";
	return -1;
    }
    return mDefSize[group];
}

QValueList<int> KIconTheme::querySizes(int group) const
{
    QValueList<int> empty;
    if ((group < 0) || (group >= KIcon::LastGroup))
    {
	kdDebug(264) << "Illegal icon group: " << group << "\n";
	return empty;
    }
    return mSizes[group];
}
	
QStringList KIconTheme::queryIcons(int size, int context) const
{
    int delta = 1000, dw;

    QListIterator<KIconThemeDir> dirs(mDirs);
    KIconThemeDir *dir;

    // Try to find exact match
    for ( ; dirs.current(); ++dirs)
    {
	dir = dirs.current();
	if ((context != KIcon::Any) && (context != dir->context()))
	    continue;
	if ((dir->type() == KIcon::Fixed) && (dir->size() == size))
	    return dir->iconList();
	if ((dir->type() == KIcon::Scalable) &&
		(size >= dir->minSize()) && (size <= dir->maxSize()))
	    return dir->iconList();
    }

    dirs.toFirst();

    // Find close match
    KIconThemeDir *best = 0L;
    for ( ; dirs.current(); ++dirs)
    {
	dir = dirs.current();
	if ((context != KIcon::Any) && (context != dir->context()))
	    continue;
	dw = dir->size() - size;
	if ((dw > 6) || (abs(dw) >= abs(delta)))
	    continue;
	delta = dw;
	best = dir;
    }
    if (best == 0L)
	return QStringList();

    return best->iconList();
}

KIcon KIconTheme::iconPath(const QString& name, int size, int match) const
{
    KIcon icon;
    QString path;
    int delta = 1000;
    KIconThemeDir *dir;

    kdDebug(264) << "iconPath " << name <<  " " << mDirs.count() << endl;
    QListIterator<KIconThemeDir> dirs(mDirs);
    for ( ; dirs.current(); ++dirs)
    {
	dir = dirs.current();

	kdDebug(264) << dir->dir() << " " << size << " " << dir->size() << " " << match << endl;
	if (match == KIcon::MatchExact)
	{
	    if ((dir->type() == KIcon::Fixed) && (dir->size() != size))
		continue;
	    if ((dir->type() == KIcon::Scalable) &&
		((size < dir->minSize()) || (size > dir->maxSize())))
	      continue;
	} else
	{
	    int dw = dir->size() - size;
	    if ((dw > 6) || (abs(dw) >= abs(delta)))
		continue;

            delta = dw;
	}

	path = dir->iconPath(name);
	if (path.isEmpty())
	    continue;
	icon.path = path;
	icon.size = dir->size();
	icon.type = dir->type();
	icon.context = dir->context();

	// if we got in MatchExact that far, we find no better
	if (match == KIcon::MatchExact)
	  return icon;
    }
    return icon;
}

// static
QString KIconTheme::current()
{
    KConfig *config = KGlobal::config();
    KConfigGroupSaver saver(config, "Icons");
    QString theme = config->readEntry("Theme");
    if (theme.isEmpty())
    {
	if (QPixmap::defaultDepth() > 8)
	    theme = QString::fromLatin1("hicolor");
	else
	    theme = QString::fromLatin1("locolor");
    }
    return theme;
}

// static
QStringList KIconTheme::list()
{
    QStringList result;
    QStringList icnlibs = KGlobal::dirs()->resourceDirs("icon");
    QStringList::ConstIterator it;
    for (it=icnlibs.begin(); it!=icnlibs.end(); it++)
    {
        QDir dir(*it);
        if (!dir.exists())
            continue;
        QStringList lst = dir.entryList(QDir::Dirs);
        QStringList::ConstIterator it2;
        QFileInfo fi;
        for (it2=lst.begin(); it2!=lst.end(); it2++)
        {
            if ((*it2 == ".") || (*it2 == ".."))
                continue;
            fi.setFile(*it + *it2 + "/index.desktop");
            if (!fi.exists())
                continue;
            if (!result.contains(*it2))
		result += *it2;
        }
    }
    return result;
}


/*** KIconThemeDir ***/

KIconThemeDir::KIconThemeDir(const QString& dir, const KConfigBase *config)
{
    mbValid = false;
    mDir = dir;
    mSize = config->readNumEntry("Size");
    if (mSize == 0)
	return;

    QString tmp = config->readEntry("Context");
    if (tmp == "Devices")
	mContext = KIcon::Device;
    else if (tmp == "MimeTypes")
	mContext = KIcon::MimeType;
    else if (tmp == "FileSystems")
	mContext = KIcon::FileSystem;
    else if (tmp == "Applications")
	mContext = KIcon::Application;
    else if (tmp == "Actions")
	mContext = KIcon::Action;
    else {
	kdDebug(264) << "Invalid Context= line for icon theme: " << mDir << "\n";
	return;
    }
    tmp = config->readEntry("Type");
    if (tmp == "Fixed")
	mType = KIcon::Fixed;
    else if (tmp == "Scalable")
	mType = KIcon::Scalable;
    else {
	kdDebug(264) << "Invalid Type= line for icon theme: " <<  mDir << "\n";
	return;
    }
    if (mType == KIcon::Scalable)
    {
	mMinSize = config->readNumEntry("MinSize", mSize);
	mMaxSize = config->readNumEntry("MaxSize", mSize);
    }
    mbValid = true;
}

QString KIconThemeDir::iconPath(const QString& name) const
{
    if (!mbValid)
	return QString::null;
    kdDebug(264) << "trying " << mDir << "/" << name << endl;
    QString file = mDir + "/" + name;
    if (KStandardDirs::exists(file)) {
      kdDebug(264) << "that worked" << endl;
      return file;
    }
    return QString::null;
}

QStringList KIconThemeDir::iconList() const
{
    QDir dir(mDir);
    QStringList lst = dir.entryList("*.png;*.xpm", QDir::Files);
    QStringList result;
    QStringList::ConstIterator it;
    for (it=lst.begin(); it!=lst.end(); it++)
	result += mDir + "/" + *it;
    return result;
}
