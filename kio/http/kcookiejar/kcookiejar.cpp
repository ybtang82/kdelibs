/*
This file is part of the KDE File Manager

   Copyright (C) 1998-2000 Waldo Bastian (bastian@kde.org)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
//----------------------------------------------------------------------------
//
// KDE File Manager -- HTTP Cookies
// $Id$

//
// The cookie protocol is a mess. RFC2109 is a joke since nobody seems to
// use it. Apart from that it is badly written.
// We try to implement Netscape Cookies and try to behave us according to
// RFC2109 as much as we can.
//

#include <config.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <qstring.h>
#include <qstrlist.h>
#include <qlist.h>
#include <qdict.h>

#include <kurl.h>
#include <krfcdate.h>
#include <kconfig.h>
#include <ksavefile.h>
#include <kdebug.h>

#include "kcookiejar.h"

#define READ_BUFFER_SIZE 8192

template class QList<KHttpCookie>;
template class QDict<KHttpCookieList>;

static QString adviceToStr(KCookieAdvice _advice)
{
    switch( _advice )
    {
    case KCookieAccept: return "Accept";
    case KCookieReject: return "Reject";
    case KCookieAsk: return "Ask";
    default: return "Dunno";
    }
}

static KCookieAdvice strToAdvice(const QString &_str)
{
    if (_str.isEmpty())
        return KCookieDunno;

    QString advice = _str.lower();

    if (advice == "accept")
		return KCookieAccept;
    else if (advice == "reject")
		return KCookieReject;
    else if (advice == "ask")
		return KCookieAsk;

    return KCookieDunno;	
}

// KHttpCookie
///////////////////////////////////////////////////////////////////////////

//
// Cookie constructor
//
KHttpCookie::KHttpCookie(const QString &_host,
                 const QString &_domain,
                 const QString &_path,
                 const QString &_name,
                 const QString &_value,
                 time_t _expireDate,
                 int _protocolVersion) :
       mHost(_host),
       mDomain(_domain),
       mPath(_path),
       mName(_name),
       mValue(_value),
       mExpireDate(_expireDate),
       mProtocolVersion(_protocolVersion)
{
    nextCookie = 0;
}

//
// Checks if a cookie has been expired
//
bool    KHttpCookie::isExpired(time_t currentDate)
{
    return (mExpireDate != 0) && (mExpireDate < currentDate);
}

//
// Returns a string for a HTTP-header
//
QString KHttpCookie::cookieStr(void)
{
    QString result;

    if (mProtocolVersion != 0)
    {
        result.sprintf("$Version=\"%d\"; ", mProtocolVersion);
        result += mName + "=\"" + mValue + "\"";
        if (!mPath.isEmpty())
            result += "; $Path=\""+ mPath + "\"";
        if (!mDomain.isEmpty())
            result += "; $Domain=\""+ mDomain + "\"";
    }
    else
    {
        result = mName + "=" + mValue;
    }
    return result;
}

//
// Returns whether this cookie should be send to this location.
bool KHttpCookie::match(const QStringList &domains, const QString &fqdn,
                    const QString &path)
{
    if (!mDomain.isEmpty())
    {
        // Cookie has a domain set
        if (!domains.contains(mDomain))
        {
            if (mDomain[0] == '.')
               return false; 
	    // Maybe the domain needs an extra dot.
            QString domain = "."+mDomain;
            if (!domains.contains(domain))
                return false; // Domain of cookie does not match with host of URL
        }
    }
    else
    {
        // Cookie has no domain set
        if (fqdn != mHost)
            return false; // Host of cookie does not match with host of URL
    }

    if (!mPath.isEmpty())
    {
        // Cookie has a path set
        if (path.find(mPath) != 0)
            return false; // Path of URL does not start with cookie-path
    }
    return true;
}

// KHttpCookieList
///////////////////////////////////////////////////////////////////////////

int KHttpCookieList::compareItems( void * item1, void * item2)
{
    int pathLen1 = ((KHttpCookie *)item1)->path().length();
    int pathLen2 = ((KHttpCookie *)item2)->path().length();
    if (pathLen1 > pathLen2)
        return 1;
    if (pathLen1 < pathLen2)
        return -1;
    return 0;
}


// KCookieJar
///////////////////////////////////////////////////////////////////////////

//
// Constructs a new cookie jar
//
// One jar should be enough for all cookies.
//
KCookieJar::KCookieJar()
{
    cookieDomains.setAutoDelete( true );
    globalAdvice = KCookieDunno;
    configChanged = false;
    cookiesChanged = false;
}

//
// Destructs the cookie jar
//
// Poor little cookies, they will all be eaten by the cookie monster!
//
KCookieJar::~KCookieJar()
{
    // Not much to do here
}

//
// Looks for cookies in the cookie jar which are appropriate for _url.
// Returned is a string containing all appropriate cookies in a format
// which can be added to a HTTP-header without any additional processing.
//
QString KCookieJar::findCookies(const QString &_url)
{
    QString cookieStr;
    QStringList domains;
    QString fqdn;
    QString path;
    KHttpCookiePtr cookie;
    int protVersion = 1;
    int cookieCount = 0;

    if (!parseURL(_url, fqdn, path))
    {
        return cookieStr;
    }

    extractDomains(fqdn, domains);

    for(QStringList::ConstIterator it = domains.begin();
        it != domains.end();
        ++it)
    {
       QString domain = *it;
       KHttpCookieList *cookieList = cookieDomains[domain];

       if (!cookieList)
          continue; // No cookies for this domain

       for ( cookie=cookieList->first(); cookie != 0; cookie=cookieList->next() )
       {
          if (!cookie->match( domains, fqdn, path))
             continue;

          // Use first cookie to determine protocol version
          if (cookieCount == 0)
          {
	     protVersion = cookie->protocolVersion();
	  }
	  if (protVersion == 0)
	  {
             if (cookieCount == 0)
                cookieStr += "Cookie: ";
             else
                cookieStr += "; ";
             cookieStr += cookie->cookieStr();
	  }
	  else
	  {
             if (cookieCount > 0)
                cookieStr += "\r\n";
             cookieStr += "Cookie: ";
             cookieStr += cookie->cookieStr();
          }
          cookieCount++;
       }
    }
    return cookieStr;
}

//
// This function parses a string like 'my_name="my_value";' and returns
// 'my_name' in Name and 'my_value' in Value.
//
// A pointer to the end of the parsed part is returned.
// This pointer points either to:
// '\0' - The end of the string has reached.
// ';'  - Another my_name="my_value" pair follows
// ','  - Another cookie follows
// '\n' - Another header follows
static const char * parseNameValue(const char *header,
                                  QString &Name,
                                  QString &Value)
{
    const char *s = header;

    // Skip any whitespace
    for(; (*s == ' ') || (*s == '\t'); s++)
    {
        if ((*s=='\0') || (*s==';') || (*s=='\n'))
        {
            // End of header
            Name = "";
            Value = "";
            return (s);
        }
    }

    header = s;

    // Parse 'my_name' part
    for(; (*s != '=') && (*s != ' ') && (*s != '\t'); s++)
    {
        if ((*s=='\0') || (*s==';') || (*s=='\n'))
        {
            // End of Name
            Value = "";
            Name = header;
            Name.truncate( s - header );
            return (s);
        }
    }

    Name = header;
    Name.truncate( s - header );

    // Skip any whitespace
    for(; (*s != '='); s++)
    {
        if ((*s=='\0') || (*s==';') || (*s=='\n'))
        {
            // End of Name
            Value = "";
            return (s);
        }
    }

    // *s == '='
    s++;

    // Skip any whitespace
    for(; (*s == ' ') || (*s == '\t'); s++)
    {
        if ((*s=='\0') || (*s==';') || (*s=='\n'))
        {
            // End of Name
            Value = "";
            return (s);
        }
    }

    if (*s == '\"')
    {
        // Parse '"my_value"' part (quoted value)
        s++;  // skip "
        header = s;
        for(;(*s != '\"');s++)
        {
            if ((*s=='\0') || (*s=='\n'))
            {
                // End of Name
                Value = header;
                Value.truncate(s - header);
                return (s);
            }
        }
        Value = header;
        Value.truncate( s - header );

        // *s == '\"';
        s++;
        // Skip any remaining garbage
        for(;; s++)
        {
            if ((*s=='\0') || (*s==';') || (*s=='\n'))
                break;
        }
    }
    else
    {
        // Parse 'my_value' part (unquoted value)
        header = s;
        while ((*s != '\0') && (*s != ';') && (*s != '\n'))
            s++;
        // End of Name
        Value = header;
        Value.truncate( s - header );
    }
    return (s);

}

static void stripDomain(const QString &_fqdn, QString &_domain)
{
   QStringList domains;
   KCookieJar::extractDomains(_fqdn, domains);
   _domain = domains[0];
}

static QString stripDomain( KHttpCookiePtr cookiePtr)
{
    QString domain; // We file the cookie under this domain.
    if (cookiePtr->domain().isEmpty() || 
        (cookiePtr->host() == cookiePtr->domain()))
       stripDomain( cookiePtr->host(), domain);
    else
       domain = cookiePtr->domain();
    return domain;
}

bool KCookieJar::parseURL(const QString &_url,
                          QString &_fqdn,
                          QString &_path)
{
    KURL kurl(_url);
    if (kurl.isMalformed())
       return false;

    _fqdn = kurl.host().lower();
    _path = kurl.path();
    if (_path.isEmpty())
       _path = "/";
    return true;
}

bool KCookieJar::extractDomains(const QString &_fqdn,
                                QStringList &_domains)
{
    // Use fqdn only if the fqdn consists of numbers.
    if ((_fqdn[0] >= '0') && (_fqdn[0] <= '9'))
    {
       _domains.append(_fqdn);
       return true;
    }

    QStringList partList = QStringList::split('.', _fqdn, false);
    partList.remove(partList.begin()); // Remove hostname
    while(partList.count())
    {
       if (partList.count() == 1)
         break; // We only have a TLD left.
       if (partList.count() == 2)
       {
          // If this is a TLD, we should stop. (e.g. co.uk)
          // We assume this is a TLD if it ends with .xx.yy or .x.yy
          if ((partList[0].length() <= 2) &&
              (partList[1].length() == 2))
             break; // This is a TLD.
       }
       QString domain = "."+partList.join(".");
       _domains.append(domain);
       partList.remove(partList.begin()); // Remove part
    }
    // If we have no domains, use the fqdn only.
    // Do not forget to append a preceeding ".".  Otherwise,
    // there will be an incorrect mis-match when checking for
    // cookies under some circumstances.  For example, a cookie
    // set by "linuxtoday.com" won't be avaliable to "www.linuxtoday.com"
    // and vise-versa.
    if (_domains.isEmpty())
       _domains.append( "." + _fqdn);
    return true;
}

//
// This function parses cookie_headers and returns a linked list of
// KHttpCookie objects for all cookies found in cookie_headers.
// If no cookies could be found 0 is returned.
//
// cookie_headers should be a concatenation of all lines of a HTTP-header
// which start with "Set-Cookie". The lines should be separated by '\n's.
//
KHttpCookiePtr KCookieJar::makeCookies(const QString &_url,
                                       const QCString &cookie_headers,
                                       long windowId)
{
    KHttpCookiePtr cookieChain = 0;
    KHttpCookiePtr lastCookie = 0;
    const char *cookieStr = cookie_headers.data();
    QString Name;
    QString Value;
    QString fqdn;
    QString path;

    if (!parseURL(_url, fqdn, path))
    {
        // Error parsing _url
        return 0;
    }

    //  The hard stuff :)
    for(;;)
    {
        // check for "Set-Cookie"
        if (strncasecmp(cookieStr, "Set-Cookie:", 11) == 0)
        {
            cookieStr = parseNameValue(cookieStr+11, Name, Value);

	        if (Name.isEmpty())
	            continue;

            // Host = FQDN
            // Default domain = ""
            // Default path = ""
            lastCookie = new KHttpCookie(fqdn, QString::null, QString::null,
                                     Name, Value );
            lastCookie->mWindowId = windowId;

            // Insert cookie in chain
            lastCookie->nextCookie = cookieChain;
            cookieChain = lastCookie;
        }
        else if (lastCookie && (strncasecmp(cookieStr, "Set-Cookie2: ", 13) == 0))
        {
            // What the fuck is this?
            // Does anyone invent his own headers these days?
            // Read the fucking RFC guys! This header is not there!
            cookieStr +=12;
            // Continue with lastCookie
        }
        else
        {
            // This is not the start of a cookie header, skip till next line.
            while (*cookieStr && *cookieStr != '\n')
                cookieStr++;

            if (*cookieStr == '\n')
            	cookieStr++;

            if (!*cookieStr)
                break; // End of cookie_headers
            else
                continue; // end of this header, continue with next.
        }

        while ((*cookieStr == ';') || (*cookieStr == ' '))
        {
            cookieStr++;

            // Name-Value pair follows
            cookieStr = parseNameValue(cookieStr, Name, Value);

            Name = Name.lower();

            if (Name == "domain")
            {
                lastCookie->mDomain = Value.lower();
            }
            else if (Name == "max-age")
            {
                int max_age = Value.toInt();
                if (max_age == 0)
                    lastCookie->mExpireDate = 1;
                else
                    lastCookie->mExpireDate = time(0)+max_age;
            }
            else if (Name == "expires")
            {
                // Parse brain-dead netscape cookie-format
                lastCookie->mExpireDate = KRFCDate::parseDate(Value);
            }
            else if (Name == "path")
            {
                lastCookie->mPath = Value;
            }
            else if (Name == "version")
            {
                lastCookie->mProtocolVersion = Value.toInt();
            }
        }
        // We have to default the path if the server did not
        // provide one according to RFC 2109 sec 4.3.1 as well
        // as the original Netscape spec.  Otherwise, we will
        // definitetly send cookies where we should not in terms
        // of the path. (DA)
        if( lastCookie && lastCookie->mPath.isEmpty() )
        {
            QString dir = path;
            if( dir != "/" )
            {
                int last_slash = dir.findRev('/');
                if( last_slash != 0 )
                    dir.truncate( last_slash );
            }
            lastCookie->mPath = dir;
            kdDebug(7104) << "Set-Cookie did not specify path.  Setting path to: " << dir << endl;
        }

        if (*cookieStr == '\0')
            break; // End of header

        // Skip ';' or '\n'
        cookieStr++;
    }

    return cookieChain;
}

//
// This function hands a KHttpCookie object over to the cookie jar.
//
// On return cookiePtr is set to 0.
//
void KCookieJar::addCookie(KHttpCookiePtr &cookiePtr)
{
    QString domain = stripDomain(cookiePtr); // We file the cookie under this domain.

    KHttpCookieList *cookieList = cookieDomains[domain];

    if (!cookieList)
    {
        // Make a cookie list for domain

        // Make a new cookie list
        cookieList = new KHttpCookieList();

        // All cookies whose domain is not already
        // known to us should be added with KCookieDunno.
        // KCookieDunno means that we use the global policy.
        cookieList->setAdvice( KCookieDunno );
        cookieDomains.insert( domain, cookieList);
        // Update the list of domains
        domainList.append(domain);
    }

    // Look for matching existing cookies
    // They are removed
    for ( KHttpCookiePtr cookie=cookieList->first(); cookie != 0; )
    {
        if ((cookie->name() == cookiePtr->name()) &&
            (cookie->domain() == cookiePtr->domain()) &&
            (cookie->path() == cookiePtr->path()))
        {
            KHttpCookiePtr old_cookie = cookie;
            cookie = cookieList->next();
            cookieList->removeRef( old_cookie );
            cookiesChanged = true;
        }
        else
        {
            cookie = cookieList->next();
        }
    }

    // Add the cookie to the cookie list
    // The cookie list is sorted 'longest path first'
    if (!cookiePtr->isExpired(time(0)))
    {
        cookieList->inSort( cookiePtr );
        cookiesChanged = true;
    }
    else
    {
        delete cookiePtr;
    }
    cookiePtr = 0;
}

//
// This function advices whether a single KHttpCookie object should
// be added to the cookie jar.
//
KCookieAdvice KCookieJar::cookieAdvice(KHttpCookiePtr cookiePtr)
{
    QStringList domains;
    if (!extractDomains(cookiePtr->host(), domains))
       return KCookieReject;

    if (!cookiePtr->domain().isEmpty())
    {
       // Cookie specifies a domain. Check whether it is valid.
       bool valid = false;

       if (cookiePtr->domain() == cookiePtr->host())
          valid = true;
       
       if (!valid)
       {
          if (domains.contains(cookiePtr->domain()))
             valid = true;
       }
       
       if (!valid)
       {
          // Maybe the domain doesn't start with a "."
          QString domain = "."+cookiePtr->domain();
          if (domains.contains(domain))
             valid = true;
       }

       if (!valid)
       {
          // Maybe the host.foo tries to set cookies for .host.foo
          if ("."+cookiePtr->host() == cookiePtr->domain())
             valid = true;
       }

       if (!valid)
       {
          qWarning("WARNING: Host %s tries to set cookie for domain %s",
                    cookiePtr->host().latin1(), cookiePtr->domain().latin1());

          return KCookieReject;
       }
    }

    if ((cookiePtr->name().find('\"') != -1) ||
        (cookiePtr->value().find('\"') != -1))
    {
        qWarning("WARNING: Host %s tries to set a suspicious cookie for domain %s",
              cookiePtr->host().latin1(), cookiePtr->domain().latin1());
        return KCookieReject;
    }

    QString domain; // We file the cookie under this domain.
    if (cookiePtr->domain().isEmpty() || 
        (cookiePtr->host() == cookiePtr->domain()))
       domain = domains[0];
    else
       domain = cookiePtr->domain();

    KHttpCookieList *cookieList = cookieDomains[domain];
    KCookieAdvice advice;

    if (cookieList)
    {
        kdDebug(7104) << "Found domain..." << endl;
        advice = cookieList->getAdvice();
        if (advice == KCookieDunno)
        {
           kdDebug(7104) << "No domain specific advice, defaulting to global policy." << endl;
           advice = globalAdvice;
        }
        else
           kdDebug(7104) << "Using domain specific advice." << endl;
    }
    else
    {
        kdDebug(7104) << "No domain specific policy found for. Use the global policy instead..." << endl;
        advice = globalAdvice;
    }
    kdDebug(7104) << "Domain name: " << domain << "\tCookie Advice: " << adviceToStr( advice ) << endl;
    return advice;
}

//
// This function gets the advice for all cookies originating from
// _domain.
//
KCookieAdvice KCookieJar::getDomainAdvice(const QString &_domain)
{
    KHttpCookieList *cookieList = cookieDomains[_domain];
    KCookieAdvice advice;

    if (cookieList)
    {
        advice = cookieList->getAdvice();
    }
    else
    {
        advice = KCookieDunno;
    }

    return advice;
}

//
// This function sets the advice for all cookies originating from
// _domain.
//
void KCookieJar::setDomainAdvice(const QString &_domain, KCookieAdvice _advice)
{
    QString domain(_domain);
    KHttpCookieList *cookieList = cookieDomains[domain];

    kdDebug(7104) << "Set cookie policy for => " << _domain.latin1() << ": " << adviceToStr( _advice ).latin1() << endl;
    if (cookieList)
    {
        if (cookieList->getAdvice() != _advice);
        {
           configChanged = true;
           // domain is already known
           cookieList->setAdvice( _advice);
        }

        if ((cookieList->isEmpty()) &&
            (_advice == KCookieDunno))
        {
            // This deletes cookieList!
            kdDebug(7104) << "Deleting cookie from the list ==> " << _domain.latin1() << ": " << adviceToStr( _advice ).latin1() << endl;
            cookieDomains.remove(domain);			
            domainList.remove(domain);
        }
    }
    else
    {
        // domain is not yet known
        if (_advice != KCookieDunno)
        {
            // We should create a domain entry
            configChanged = true;
            // Make a new cookie list
            cookieList = new KHttpCookieList();
            cookieList->setAdvice( _advice);
            cookieDomains.insert( domain, cookieList);
            // Update the list of domains
            domainList.append( domain);
        }
    }

    kdDebug(7104) << "Number of items in the domain cookie policy list : " << cookieDomains.count() << endl;
}

//
// This function sets the advice for all cookies originating from
// the same domain as _cookie
//
void KCookieJar::setDomainAdvice(KHttpCookiePtr cookiePtr, KCookieAdvice _advice)
{
    QString domain = stripDomain(cookiePtr); // We file the cookie under this domain.

    setDomainAdvice(domain, _advice);
}

//
// This function sets the global advice for cookies
//
void KCookieJar::setGlobalAdvice(KCookieAdvice _advice)
{
    if (globalAdvice != _advice)
       configChanged = true;
    globalAdvice = _advice;
}

//
// Get a list of all domains known to the cookie jar.
//
const QStringList *KCookieJar::getDomainList()
{
    return &domainList;
}

//
// Get a list of all cookies in the cookie jar originating from _domain.
//
const KHttpCookieList *KCookieJar::getCookieList(const QString &_domain)
{
    return cookieDomains[_domain];
}

//
// Eat a cookie out of the jar.
// cookiePtr should be one of the cookies returned by getCookieList()
//
void KCookieJar::eatCookie(KHttpCookiePtr cookiePtr)
{
    QString domain = stripDomain(cookiePtr); // We file the cookie under this domain.
    KHttpCookieList *cookieList = cookieDomains[domain];

    if (cookieList)
    {
        // This deletes cookiePtr!
        if (cookieList->removeRef( cookiePtr ))
           cookiesChanged = true;

        if ((cookieList->isEmpty()) &&
            (cookieList->getAdvice() == KCookieDunno))
        {
            // This deletes cookieList!
            cookieDomains.remove(domain);

            domainList.remove(domain);
        }
    }
}

void KCookieJar::eatCookies( KHttpCookieList* cookieList )
{
    if (cookieList)
    {
        KHttpCookiePtr cookiePtr = cookieList->first();
        QString domain = stripDomain(cookiePtr); // We file the cookie under this domain.

        for ( ; cookieList != 0; cookieList->next() )
            cookieList->removeRef( cookiePtr );

        // Delete cookie list.
        cookieDomains.remove(domain);
        // Remove from the domain list.
        domainList.remove(domain);
        // set the cookies changed flag.
        cookiesChanged = true;
    }
}

void KCookieJar::eatAllCookies()
{
    for ( QStringList::Iterator it=domainList.begin();
    	  it != domainList.end();
    	  it++)
    {
        const QString &domain = *it;

	KHttpCookieList *cookieList = cookieDomains[domain];
        KHttpCookiePtr cookie=cookieList->first();

	for (; cookie != 0;)
        {
           // Delete expired cookies
           KHttpCookiePtr old_cookie = cookie;
           cookie = cookieList->next();
           cookieList->removeRef( old_cookie );
	}
    }
    cookiesChanged = true;
}

//
// Saves all cookies to the file '_filename'.
// On succes 'true' is returned.
// On failure 'false' is returned.
bool KCookieJar::saveCookies(const QString &_filename)
{
    KSaveFile saveFile(_filename);

    if (saveFile.status() != 0)
       return false;

    FILE *fStream = saveFile.fstream();

    time_t curTime = time(0);

    fprintf(fStream, "# KDE Cookie File\n#\n");

    fprintf(fStream, "%-20s %-20s %-12s %-9s %-4s %-10s %s\n",
    	"# Host", "Domain", "Path", "Exp.date", "Prot", "Name", "Value");

    for ( QStringList::Iterator it=domainList.begin();
    	  it != domainList.end();
    	  it++)
    {
        const QString &domain = *it;
        bool domainPrinted = false;

	KHttpCookieList *cookieList = cookieDomains[domain];
        KHttpCookiePtr cookie=cookieList->first();

	for (; cookie != 0;)
        {
            if (cookie->isExpired(curTime))
	    {
	    	// Delete expired cookies
                KHttpCookiePtr old_cookie = cookie;
                cookie = cookieList->next();
                cookieList->removeRef( old_cookie );
	    }
	    else if (cookie->expireDate() != 0)
            {
                if (!domainPrinted)
                {
                    domainPrinted = true;
                    fprintf(fStream, "[%s]\n", domain.latin1());
                }
                // Store persistent cookies
                QString path("\"");
                path += cookie->path();
                path += "\"";
                QString domain("\"");
                domain += cookie->domain();
                domain += "\"";
                fprintf(fStream, "%-20s %-20s %-12s %9lu   %2d %-10s \"%s\"\n",
		    cookie->host().latin1(), domain.latin1(), path.latin1(),
		    (unsigned long) cookie->expireDate(),
		    cookie->protocolVersion(),
		    cookie->name().latin1(), cookie->value().latin1());
		cookie = cookieList->next();
	    }
	    else
	    {
	    	// Skip session-only cookies
		cookie = cookieList->next();
	    }
	}
    }

    return saveFile.close();
}

typedef char *charPtr;

static const char *parseField(charPtr &buffer)
{
    char *result;
    if (*buffer == '\"')
    {
    	// Find terminating "
    	buffer++;
    	result = buffer;
        while((*buffer != '\"') && (*buffer))
    	    buffer++;
    }
    else
    {
        // Find first white space
        result = buffer;
        while((*buffer != ' ') && (*buffer != '\t') && (*buffer != '\n') && (*buffer))
    	    buffer++;
    }	    	
    if (!*buffer)
    	return result; //
    *buffer++ = '\0';

    // Skip white-space
    while((*buffer == ' ') || (*buffer == '\t') || (*buffer == '\n'))
    	buffer++;

    return result;
}


//
// Reloads all cookies from the file '_filename'.
// On succes 'true' is returned.
// On failure 'false' is returned.
bool KCookieJar::loadCookies(const QString &_filename)
{
    FILE *fStream = fopen( _filename.latin1(), "r");
    if (fStream == 0)
    {
        return false;
    }

    time_t curTime = time(0);

    char *buffer = new char[READ_BUFFER_SIZE];

    bool err = false;
    err = (fgets(buffer, READ_BUFFER_SIZE, fStream) == 0);

    err = err || (strcmp(buffer, "# KDE Cookie File\n") != 0);

    if (!err)
    {
    	while(fgets(buffer, READ_BUFFER_SIZE, fStream) != 0)
    	{
            char *line = buffer;
    	    // Skip lines which begin with '#' or '['
    	    if ((line[0] == '#') || (line[0] == '['))
    	    	continue;
	
	    const char *host( parseField(line) );
	    const char *domain( parseField(line) );
	    const char *path( parseField(line) );
	    const char *expStr( parseField(line) );
	    if (!expStr) continue;
	    int expDate  = (time_t) strtoul(expStr, 0, 10);
	    const char *verStr( parseField(line) );
	    if (!verStr) continue;
	    int protVer  = (time_t) strtoul(verStr, 0, 10);
	    const char *name( parseField(line) );
	    const char *value( parseField(line) );

	    // Parse error
	    if (!value) continue;

	    // Expired or parse error
	    if ((expDate == 0) || (expDate < curTime))
	    	continue;
	    	
#if 0
            // Domain must be either hostname or domain of hostname
	    if ( strlen(domain) && (strcmp(host,domain) != 0))
	    {
                QString checkDomain;
                stripDomain( host, checkDomain);
                if (checkDomain != domain)
                    continue;
            }
#endif

	    KHttpCookie *cookie = new KHttpCookie(host, domain, path,
	    	name, value, expDate, protVer);
	    addCookie(cookie);	
    	}
    }
    delete [] buffer;
    cookiesChanged = false;

    fclose( fStream);
    return err;
}

//
// Save the cookie configuration
//

void KCookieJar::saveConfig(KConfig *_config)
{
    if (!configChanged)
        return;

    _config->setGroup(QString::null);
    _config->writeEntry("DefaultRadioButton", defaultRadioButton);

    QStringList domainSettings;
    _config->setGroup("Cookie Policy");
    _config->writeEntry("CookieGlobalAdvice", adviceToStr( globalAdvice));

    for ( QStringList::Iterator it=domainList.begin();
    	  it != domainList.end();
    	  it++)
    {
         const QString &domain = *it;
         KCookieAdvice advice = getDomainAdvice( domain);
         if (advice != KCookieDunno)
         {
             QString value(domain);
             value += ':';
             value += adviceToStr(advice);
             domainSettings.append(value);
         }
    }
    _config->writeEntry("CookieDomainAdvice", domainSettings);
    _config->sync();
}


//
// Load the cookie configuration
//

void KCookieJar::loadConfig(KConfig *_config, bool reparse )
{
    QString value;
    QStringList domainSettings;

    if( reparse )
        _config->reparseConfiguration();

    _config->setGroup(QString::null);
    defaultRadioButton = _config->readNumEntry("DefaultRadioButton", 0);

    // Read the old group name if we did not yet save to
    // the new group name.
    if( _config->hasGroup( "Browser Settings/HTTP" ) &&
		!_config->hasGroup( "Cookie Policy" ) )
		_config->setGroup( "Browser Settings/HTTP" );	
    else
		_config->setGroup("Cookie Policy");
    value = _config->readEntry("CookieGlobalAdvice", "Ask");
    globalAdvice = strToAdvice(value);
    domainSettings = _config->readListEntry("CookieDomainAdvice");

    // Reset current domain settings first. 
    for ( QStringList::Iterator it=domainList.begin(); it != domainList.end();)
    {
         // Make sure to update iterator before calling setDomainAdvice()
         // setDomainAdvice() might delete the domain from domainList.
         QString domain = *it++;
         setDomainAdvice(domain, KCookieDunno); 
    }

    // Now apply the
    for ( QStringList::Iterator it=domainSettings.begin();
    	  it != domainSettings.end();)
    {
        const QString &value = *it++;
        int sepPos = value.find(':');
        if (sepPos <= 0) { continue; }
        QString domain(value.left(sepPos));
        KCookieAdvice advice = strToAdvice( value.mid(sepPos + 1) );
        setDomainAdvice(domain, advice);
    }

    kdDebug(7104) << "********** COOKIE POLICY CONFIG **********\n" << endl;
    kdDebug(7104) << "Global Cookie Policy: " << adviceToStr(globalAdvice) << endl;
    kdDebug(7104) << "Domain Specific Policies: " << endl;
    for ( QStringList::Iterator it = domainSettings.begin(); it != domainSettings.end(); ++it )
        kdDebug(7104) << (*it) << endl;
    kdDebug(7104) << "\n****************************************" << endl;


}

