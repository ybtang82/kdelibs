/*
 * This file is part of the KDE Libraries
 * Copyright (C) 2000 Espen Sand (espen@kde.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include <qvaluelist.h>
#include <qstring.h>

#ifndef _KABOUTDATA_H_
#define _KABOUTDATA_H_

/**
 * This structure is used to store information about a person or developer.
 * It can store the person's name, a task, an email address and a
 * link to a homepage. This class is intended for use in the
 * @ref KAboutProgram class, but it can be used elsewhere as well.
 * Normally you should at least define the person's name.
*/
class KAboutPerson
{
public:
    /**
     * Convenience constructor
     *
     * @param _name The name of the person.
     *
     * @param _task The task of this person. This string should be
     *              marked for translation, e.g.
     *              I18N_NOOP("Task description....")
     *
     * @param _emailAddress The email address of the person.
     *
     * @param _webAddress Homepage of the person.
     */
    KAboutPerson( const char *_name, const char *_task,
                  const char *_emailAddress, const char *_webAddress )
    {
      mName = _name;
      mTask = _task;
      mEmailAddress = _emailAddress;
      mWebAddress = _webAddress;
    }
    /**
     * @internal - don't use. Required by QValueList
     */
    KAboutPerson() {}

    /**
     * The person's name
     */
    QString name() const;

    /**
     * The person's task
     */
    QString task() const;

    /**
     * The person's email address
     */
    QString emailAddress() const;

    /**
     * The homepage or a relevant link
     */
    QString webAddress() const;

protected:
    const char *mName;
    const char *mTask;
    const char *mEmailAddress;
    const char *mWebAddress;
};

/**
 * This class is used to store information about a program. It can store
 * such values a version number, program name, homepage, email address
 * for bug reporting, multiple authors and contributors
 * (using @ref KAboutPerson), license and copyright information.
 *
 * Currently, the values set here are shown by the "About" box
 * (@see KAboutDialog), used by the bug report dialog (@see KBugReport),
 * and by the help shown on command line (@see KCmdLineArguments).
 *
 * @short Class holding information needed by the "About" box and other
 * classes.
 * @author Espen Sand (espen@kde.org), David Faure (faure@kde.org)
 * @version $Id$
 */
class KAboutData
{
  public:
    enum LicenseKey
    {
      GPL  = 1,
      LGPL = 2,
      BSD  = 3,
      Artistic = 4
    };

  public:
    /**
     * Constructor.
     *
     * @param appName The program name used internally. Example: "kedit"
     *
     * @param programName A displayable program name string. This string
     *        should be marked for translation. Example: I18N_NOOP("KEdit")
     *
     * @param version The program version string.
     *
     * @param shortDescription A short description of what the program does.
     *        This string should be marked for translation.
     *        Example: I18N_NOOP("A simple text editor.")
     *
     * @param licenseType The license identifier. Use @ref setLicenseText if
     *        you use a license not predefined here.
     *
     * @param copyrightStatement A copyright statement, that can look like this:
     *        "(c) 1999-2000, Name". The string specified here is not modified
     *        in any manner. The author information from @ref addAuthor is not
     *        used.
     *
     * @param webAddress The program homepage string.
     *        Start the address with "http://". "http://some.domain" is
     *        is correct, "some.domain" is not.
     *
     * @param text Some free form text, that can contain any kind of
     *        information. The text can contain newlines. This string
     *        should be marked for translation.
     *
     * @param bugsEmailAddress The bug report email address string.
     *
     */
    KAboutData( const char *appName,
                const char *programName,
		const char *version,
		const char *shortDescription = 0,
		int licenseType = 0,
		const char *copyrightStatement = 0,
		const char *text = 0,
		const char *homePageAddress = 0,
		const char *bugsEmailAddress = "submit@bugs.kde.org"
		);

    /**
     * Defines an author. You can call this function as many times you
     * need. Each entry is appended to a list. The person in the first entry
     * is assumed to be the leader of the project.
     *
     * @param name The developer's name.
     *
     * @param task What the person is resposible for. This text can contain
     *             newlines. It should be marked for translation like this:
     *             I18N_NOOP("Task description...")
     *
     * @param emailAddress An Email address when the person can be reached.
     *
     * @param webAddress The person's homepage or a relevant link.
     *        Start the address with "http://". "http://some.domain" is
     *        is correct, "some.domain" is not.
     *
     */
    void addAuthor( const char *name,
		    const char *task=0,
		    const char *emailAddress=0,
		    const char *webAddress=0 );

    /**
     * Defines a person that deserves credit. You can call this function
     * as many times you need. Each entry is appended to a list.
     *
     * @param name The person's name.
     *
     * @param task What the person has done to deserve the hounor. The
     *        text can contain newlines. It should be marked for 
     *        translation like this: I18N_NOOP("Task description...")
     *
     * @param emailAddress An Email address when the person can be reached.
     *
     * @param webAddress The person's homepage or a relevant link.
     *        Start the address with "http://". "http://some.domain" is
     *        is correct, "some.domain" is not.
     *
     */
    void addCredit( const char *name,
                    const char *task=0,
		    const char *emailAddress=0,
		    const char *webAddress=0 );

    /**
     * Defines a licence text. Normally you should set a
     * value in the constructor which provides a set of standard licenses.
     * If you do so, any text defined with this function is ignored.
     *
     * @param license The license text.
     */
    void setLicenseText( const char *license );

    /**
     * @return the internal program name.
     */
    const char *appName() const;

    /**
     * @return the program name (translated).
     */
    QString programName() const;

    /**
     * @return the version string.
     */
    QString version() const;

    /**
     * @return the short description (translated).
     */
    QString shortDescription() const;

    /**
     * @return the application homepage URL.
     */
    QString homepage() const;

    /**
     * @return the email address where to report bugs.
     */
    QString bugAddress() const;

    /**
     * @return author information (list of persons).
     */
    const QValueList<KAboutPerson> authors() const;

    /**
     * @return credit information (list of persons).
     */
    const QValueList<KAboutPerson> credits() const;

    /**
     * @return the free form text (translated). 
     */
    QString otherText() const;

    /**
     * Returns the license. If @ref setLicenseType has been
     * used, any text defined by @ref setLicenseText is ignored,
     * and the standard text for the chosen license will be returned.
     *
     * @return The license text.
     */
    QString license() const;

    /**
     * @return the copyright statement.
     */
    QString copyrightStatement() const { return( mCopyrightStatement ); }

  private:
    const char *mAppName;
    const char *mProgramName;
    const char *mVersion;
    const char *mShortDescription;
    int mLicenseKey;
    const char *mCopyrightStatement;
    const char *mOtherText;
    const char *mHomepageAddress;
    const char *mBugEmailAddress;
    QValueList<KAboutPerson> mAuthorList;
    QValueList<KAboutPerson> mCreditList;
    const char *mLicenseText;
};


#endif

