/* This file is part of the KDE libraries
    Copyright (c) 1997 Stephan Kulow <coolo@kde.org>
    Copyright (c) 1999 Preston Brown <pbrown@kde.org>
    Copyright (c) 1999-2000 Hans Petter Bieker <bieker@kde.org>

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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include <qtextcodec.h>
#include <qfile.h>
#include <qdatetime.h>

#include "kglobal.h"
#include "kstddirs.h"
#include "ksimpleconfig.h"
#include "kinstance.h"
#include "kconfig.h"
#include "kdebug.h"
#include "klocale.h"

/**
  * Stephan: I don't want to put this in an extra header file, since
  * this would let people think, they can use it within C files, but
  * this is not the case.
  **/

/* Look up MSGID in the DOMAINNAME message catalog for the current CATEGORY
   locale.  */
char *k_dcgettext (const char* __domainname, const char* __msgid,
		   const char* _language);

/* Specify that the DOMAINNAME message catalog will be found
   in DIRNAME rather than in the system locale data base.  */
char *k_bindtextdomain (const char* __domainname,
			const char* __dirname);

#define SYSTEM_MESSAGES "kdelibs"

static const char *maincatalogue = 0;

void KLocale::splitLocale(const QString& aStr,
			  QString& lang,
			  QString& country,
			  QString& chrset){

    QString str = aStr;

    // just in case, there is another language appended
    int f = str.find(':');
    if (f >= 0) {
	str.truncate(f);
    }

    country = QString::null;
    chrset = QString::null;
    lang = QString::null;

    f = str.find('.');
    if (f >= 0) {
	chrset = str.mid(f + 1);
	str.truncate(f);
    }

    f = str.find('_');
    if (f >= 0) {
	country = str.mid(f + 1);
	str.truncate(f);
    }

    lang = str;
}

QString KLocale::language() const
{
    ASSERT(_inited);
    return lang;
}

QString KLocale::country() const
{
    ASSERT(_inited);
    return _country;
}

QString KLocale::monthName(int i, bool _short) const
{
  if ( _short )
    switch ( i )
    {
      case 1:   return translate("January", "Jan");
      case 2:   return translate("February", "Feb");
      case 3:   return translate("March", "Mar");
      case 4:   return translate("April", "Apr");
      case 5:   return translate("May long", "May");
      case 6:   return translate("June", "Jun");
      case 7:   return translate("July", "Jul");
      case 8:   return translate("August", "Aug");
      case 9:   return translate("September", "Sep");
      case 10:  return translate("October", "Oct");
      case 11:  return translate("November", "Nov");
      case 12:  return translate("December", "Dec");
    }
  else
    switch (i)
    {
      case 1:   return translate("January");
      case 2:   return translate("February");
      case 3:   return translate("March");
      case 4:   return translate("April");
      case 5:   return translate("May long", "May");
      case 6:   return translate("June");
      case 7:   return translate("July");
      case 8:   return translate("August");
      case 9:   return translate("September");
      case 10:  return translate("October");
      case 11:  return translate("November");
      case 12:  return translate("December");
    }

  return QString::null;
}

QString KLocale::weekDayName (int i, bool _short) const
{
  if ( _short )
    switch ( i )
    {
      case 1:  return translate("Monday", "Mon");
      case 2:  return translate("Tuesday", "Tue");
      case 3:  return translate("Wednesday", "Wed");
      case 4:  return translate("Thursday", "Thu");
      case 5:  return translate("Friday", "Fri");
      case 6:  return translate("Saturday", "Sat");
      case 7:  return translate("Sunday", "Sun");
    }
  else
    switch ( i )
    {
      case 1:  return translate("Monday");
      case 2:  return translate("Tuesday");
      case 3:  return translate("Wednesday");
      case 4:  return translate("Thursday");
      case 5:  return translate("Friday");
      case 6:  return translate("Saturday");
      case 7:  return translate("Sunday");
    }

  return QString::null;
}

#ifdef ENABLE_NLS

KLocale::KLocale( const QString& _catalogue )
  : _inited(false), _codec( 0 )
{
    KConfig *config = KGlobal::instance()->config();
    if (config)
    {
      KConfigGroupSaver saver(config, QString::fromLatin1("Locale"));
      chset = config->readEntry(QString::fromLatin1("Charset"));
    }
    if (chset.isNull()) chset = QString::fromLatin1("iso-8859-1");

    catalogues = new QStrList(true);

    /*
     * Use the first non-null string.
     */
    QString catalogue = _catalogue;
    if (maincatalogue)
      catalogue = QString::fromLatin1(maincatalogue);

    initLanguage(config, catalogue);
    initFormat(config);
}

void KLocale::setEncodingLang(const QString &_lang)
{
  _codec = 0;
  QString location = locate("locale", _lang + QString::fromLatin1("/charset"));
  if (!location.isNull())
  {
    QFile f(location);
    if (f.open(IO_ReadOnly))
    {
      char *buf = new char[256];
      int l = f.readLine(buf, 256);
      if (l > 0)
      {
        if (buf[l - 1] == '\n') buf[l - 1] = 0;
        _codec = QTextCodec::codecForName( buf );
      }
      f.close();
      delete [] buf;
    }
  }
  // default to 8 bit unicode
  if (!_codec)
    _codec = QTextCodec::codecForName( "utf8" );
}

void KLocale::initLanguage(KConfig *config, const QString& catalogue)
{
  if (!config || _inited)
    return;

  KConfigGroupSaver saver(config, QString::fromLatin1("Locale"));

  QStringList langlist = config->readListEntry("Language", ':');

  langlist.prepend( getenv("KDE_LANG") );
  // same order as setlocale use
  langlist << getenv("LC_MESSAGES");
  langlist << getenv("LC_ALL");
  langlist << getenv("LANG");
  langlist << QString::fromLatin1("C");

  for ( QStringList::Iterator it = langlist.begin();
        it != langlist.end();
        ++it )
  {
    QString ln, ct, chrset;
    splitLocale(*it, ln, ct, chrset);	

    if (!ct.isEmpty()) {
      langlist.insert(it, ln + '_' + ct);
      if (!chrset.isEmpty())
	langlist.insert(it, ln + '_' + ct + '.' + chrset);
    }
  }

  for ( QStringList::Iterator it = langlist.begin();
        it != langlist.end();
        ++it)
  {
    lang = *it;

    if (lang.isEmpty()) continue;

    // This always exists
    if (lang == QString::fromLatin1("C")) break;

    QString path = QString::fromLatin1("%1/LC_MESSAGES/%2.mo").arg(lang);
    if (!locate("locale", path.arg(catalogue)).isNull() &&
	!locate("locale", path.arg(SYSTEM_MESSAGES)).isNull())
      break;
  }

  langs = langlist.join(QString::fromLatin1(":"));

  setEncodingLang(lang);

  insertCatalogue( catalogue );
  insertCatalogue( QString::fromLatin1(SYSTEM_MESSAGES) );

  _country = config->readEntry(QString::fromLatin1("Country"));

  _inited = true;
}

// init some stuff for format*()
void KLocale::initFormat(KConfig *config)
{
  if (!config)
    return;

  KConfigGroupSaver saver(config, QString::fromLatin1("Locale"));

  KSimpleConfig entry(locate("locale",
			     QString::fromLatin1("l10n/%1/entry.desktop")
			     .arg(_country)), true);
  entry.setGroup(QString::fromLatin1("KCM Locale"));

  // Numeric
  _decimalSymbol = config->readEntry(QString::fromLatin1("DecimalSymbol"));
  if (_decimalSymbol.isNull())
    _decimalSymbol = entry.readEntry(QString::fromLatin1("DecimalSymbol"),
				     QString::fromLatin1("."));

  _thousandsSeparator = config->readEntry(QString::fromLatin1("ThousandsSeparator"));
  if (_thousandsSeparator.isNull())
    _thousandsSeparator = entry.readEntry(QString::fromLatin1("ThousandsSeparator"), QString::fromLatin1(","));
  _thousandsSeparator.replace( QRegExp(QString::fromLatin1("$0")),
				       QString::null );

  _positiveSign = config->readEntry(QString::fromLatin1("PositiveSign"));
  if (_positiveSign.isNull())
    _positiveSign = entry.readEntry(QString::fromLatin1("PositiveSign"));

  _negativeSign = config->readEntry(QString::fromLatin1("NegativeSign"));
  if (_negativeSign.isNull())
    _negativeSign = entry.readEntry(QString::fromLatin1("NegativeSign"),
				    QString::fromLatin1("-"));

  // Monetary
  _currencySymbol = config->readEntry(QString::fromLatin1("CurrencySymbol"));
  if (_currencySymbol.isNull())
    _currencySymbol = entry.readEntry(QString::fromLatin1("CurrencySymbol"),
				      QString::fromLatin1("$"));

  _monetaryDecimalSymbol = config->readEntry(QString::fromLatin1("MonetaryDecimalSymbol"));
  if (_monetaryDecimalSymbol.isNull())
    _monetaryDecimalSymbol = entry.readEntry(QString::fromLatin1("MonetaryDecimalSymbol"), QString::fromLatin1("."));

  _monetaryThousandsSeparator = config->readEntry(QString::fromLatin1("MonetaryThousendSeparator"));
  if (_monetaryThousandsSeparator.isNull())
    _monetaryThousandsSeparator = entry.readEntry(QString::fromLatin1("MonetaryThousandsSeparator"), QString::fromLatin1(","));
  _monetaryThousandsSeparator.replace(QRegExp(QString::fromLatin1("$0")),
				      QString::null);

  _fracDigits = config->readNumEntry(QString::fromLatin1("FractDigits"), -1);
  if (_fracDigits == -1)
    _fracDigits = entry.readNumEntry(QString::fromLatin1("FractDigits"), 2);

  _positivePrefixCurrencySymbol = entry.readBoolEntry(QString::fromLatin1("PositivePrefixCurrencySymbol"), true);
  _positivePrefixCurrencySymbol = config->readNumEntry(QString::fromLatin1("PositivePrefixCurrencySymbol"), _positivePrefixCurrencySymbol);

  _negativePrefixCurrencySymbol = entry.readBoolEntry(QString::fromLatin1("NegativePrefixCurrencySymbol"), true);
  _negativePrefixCurrencySymbol = config->readNumEntry(QString::fromLatin1("NegativePrefixCurrencySymbol"), _negativePrefixCurrencySymbol);

  _positiveMonetarySignPosition = (SignPosition)config->readNumEntry(QString::fromLatin1("PositiveMonetarySignPosition"), -1);
  if (_positiveMonetarySignPosition == -1)
    _positiveMonetarySignPosition = (SignPosition)entry.readNumEntry(QString::fromLatin1("PositiveMonetarySignPosition"), BeforeQuantityMoney);

  _negativeMonetarySignPosition = (SignPosition)config->readNumEntry(QString::fromLatin1("NegativeMonetarySignPosition"), -1);
  if (_negativeMonetarySignPosition == -1)
    _negativeMonetarySignPosition = (SignPosition)entry.readNumEntry(QString::fromLatin1("NegativeMonetarySignPosition"), ParensAround);

  // Date and time
  _timefmt = config->readEntry(QString::fromLatin1("TimeFormat"));
  if (_timefmt.isNull())
    _timefmt = entry.readEntry(QString::fromLatin1("TimeFormat"),
			       QString::fromLatin1("%I:%M:%S %p"));

  _datefmt = config->readEntry(QString::fromLatin1("DateFormat"));
  if (_datefmt.isNull())
    _datefmt = entry.readEntry(QString::fromLatin1("DateFormat"),
				 QString::fromLatin1("%A %d %B %Y"));

  _datefmtshort = config->readEntry(QString::fromLatin1("DateFormatShort"));
  if (_datefmtshort.isNull())
    _datefmtshort = entry.readEntry(QString::fromLatin1("DateFormatShort"),
				    QString::fromLatin1("%m/%d/%y"));

  m_weekStartsMonday = entry.readBoolEntry(QString::fromLatin1("WeekStartsMonday"), true);
  m_weekStartsMonday = config->readBoolEntry(QString::fromLatin1("WeekStartsMonday"), m_weekStartsMonday);
}

void KLocale::setLanguage(const QString &_lang)
{
  lang = _lang;
  // k_dcgettext doesn't like NULL pointers, so default to C
  if (lang.isEmpty()) lang = QString::fromLatin1("C");

  setEncodingLang(lang);

  QStrList *cats = catalogues;
  catalogues = new QStrList;
  for (const char* catalogue = cats->first(); catalogue;
       catalogue = cats->next())
    insertCatalogue( QString::fromLatin1(catalogue) );
  delete cats;
}

void KLocale::insertCatalogue( const QString& catalogue )
{
  QString str = QString::fromLatin1("%1/LC_MESSAGES/%2.mo")
    .arg(lang)
    .arg(catalogue);
  k_bindtextdomain ( catalogue.ascii() ,
		     KGlobal::dirs()->findResourceDir("locale", str).ascii());
  
  catalogues->append(catalogue.ascii());
}

KLocale::~KLocale()
{
  delete catalogues;
}

QString KLocale::translate_priv(const char *msgid, const char *fallback) const
{
  ASSERT(_inited);

  if (!msgid || !msgid[0])
  {
    qDebug("KLocale: trying to look up \"\" in catalouge. Fix the program");
    return QString::null;
  }

  for (const char* catalogue = catalogues->first(); catalogue;
       catalogue = catalogues->next())
  {
    const char *text = k_dcgettext( catalogue, msgid, lang.ascii());
    if ( text != msgid) // we found it
      return _codec->toUnicode( text );
  }

  // Always use UTF-8 if the string was not found
  return QString::fromUtf8( fallback );
}

QString KLocale::translate(const char* msgid) const
{
  return translate_priv(msgid, msgid);
}

QString KLocale::translate( const char *index, const char *fallback) const
{
  if (!index || !index[0] || !fallback || !fallback[0])
  {
    kdDebug() << ("KLocale: trying to look up \"\" in catalouge. Fix the program");
    return QString::null;
  } 

  char *newstring = new char[strlen(index) + strlen(fallback) + 5];
  sprintf(newstring, "_: %s\n%s", index, fallback);
  // as copying QString is very fast, it looks slower as it is ;/
  QString r = translate_priv(newstring, fallback);
  delete [] newstring;

  return r;
}

bool KLocale::weekStartsMonday() const
{
  return m_weekStartsMonday;
}

QString KLocale::decimalSymbol() const
{
  return _decimalSymbol;
}

QString KLocale::thousandsSeparator() const
{
  return _thousandsSeparator;
}

QString KLocale::currencySymbol() const
{
  return _currencySymbol;
}

QString KLocale::monetaryDecimalSymbol() const
{
  return _monetaryDecimalSymbol;
}

QString KLocale::monetaryThousandsSeparator() const
{
  return _monetaryThousandsSeparator;
}

QString KLocale::positiveSign() const
{
  return _positiveSign;
}

QString KLocale::negativeSign() const
{
  return _negativeSign;
}

int KLocale::fracDigits() const
{
  return _fracDigits;
}

bool KLocale::positivePrefixCurrencySymbol() const
{
  return _positivePrefixCurrencySymbol;
}

bool KLocale::negativePrefixCurrencySymbol() const
{
  return _negativePrefixCurrencySymbol;
}

KLocale::SignPosition KLocale::positiveMonetarySignPosition() const
{
  return _positiveMonetarySignPosition;
}

KLocale::SignPosition KLocale::negativeMonetarySignPosition() const
{
  return _negativeMonetarySignPosition;
}

QString KLocale::formatMoney(double num, const QString &symbol, int precision) const
{
    // some defaults
    QString currency = symbol.isNull()
        ? currencySymbol()
        : symbol;
    if (precision < 0) precision = fracDigits();

    // the number itself
    bool neg = num < 0;
    QString res = QString::number(neg?-num:num, 'f', precision);
    int pos = res.find('.');
    if (pos == -1) pos = res.length();
    else res.replace(pos, 1, monetaryDecimalSymbol());

    while (0 < (pos -= 3))
        res.insert(pos, monetaryThousandsSeparator()); // thousend sep

    // set some variables we need later
    int signpos = neg?negativeMonetarySignPosition():positiveMonetarySignPosition();
    QString sign = neg?negativeSign():positiveSign();

    switch (signpos)
    {
    case KLocale::ParensAround:
        res.prepend('(');
        res.append (')');
        break;
    case KLocale::BeforeQuantityMoney:
        res.prepend(sign);
        break;
    case KLocale::AfterQuantityMoney:
        res.append(sign);
        break;
    case KLocale::BeforeMoney:
        currency.prepend(sign);
        break;
    case KLocale::AfterMoney:
        currency.append(sign);
        break;
    }

    if(neg?negativePrefixCurrencySymbol():
            positivePrefixCurrencySymbol())
    {
        res.prepend(' ');
        res.prepend(currency);
    } else {
        res.append (' ');
        res.append (currency);
    }

    return res;
}

QString KLocale::formatMoney(const QString &numStr) const
{
  return formatMoney(numStr.toDouble());
}

QString KLocale::formatNumber(double num, int precision) const
{
    bool neg = num < 0;
    if (precision == -1) precision = 2;
    QString res = QString::number(neg?-num:num, 'f', precision);
    int pos = res.find('.');
    if (pos == -1) pos = res.length();
    else res.replace(pos, 1, decimalSymbol());

    if (pos > 4)
      while (0 < (pos -= 3))
        res.insert(pos, thousandsSeparator()); // thousend sep

    // How can we know where we should put the sign?
    res.prepend(neg?negativeSign():positiveSign());

    return res;
}

QString KLocale::formatNumber(const QString &numStr) const
{
  return formatNumber(numStr.toDouble());
}

QString KLocale::formatDate(const QDate &pDate, bool shortfmt) const
{
    QString rst = shortfmt?_datefmtshort:_datefmt;

    int i = -1;
    while ( (i = rst.findRev('%', i)) != -1 )
        switch ( rst.at(i + 1).latin1() )
        {
        case 'Y':
                rst.replace(i, 2, QString().sprintf("%02d", pDate.year()));
		continue;
	case 'y':
                rst.replace(i, 2, QString().sprintf("%02d", pDate.year() % 100));
		continue;
	case 'n':
                rst.replace(i, 2, QString().sprintf("%2d", pDate.month()));
		continue;
	case 'm':
                rst.replace(i, 2, QString().sprintf("%02d", pDate.month()));
		continue;
	case 'b':
                rst.replace(i, 2, monthName(pDate.month(), true));
		continue;
	case 'B':
                rst.replace(i, 2, monthName(pDate.month(), false));
		continue;
	case 'd':
                rst.replace(i, 2, QString().sprintf("%02d", pDate.day() ));
		continue;
	case 'e':
                rst.replace(i, 2, QString().sprintf("%2d", pDate.day() ));
		continue;
	case 'a':
                rst.replace(i, 2, weekDayName(pDate.dayOfWeek(), true));
		continue;
	case 'A':
                rst.replace(i, 2, weekDayName(pDate.dayOfWeek(), false));
		continue;
	default:
                rst.remove(i, 1);
		i--;
		continue;
        }

    return rst;
}
#endif

////// Those methods don't depend on ENABLE_NLS /////

void KLocale::setMainCatalogue(const char *catalogue)
{
    maincatalogue = catalogue;
}

double KLocale::readNumber(const QString &str, bool * ok) const
{
    bool neg = str.find(negativeSign()) == 0;
    QString buf = decimalSymbol();
    int pos = str.find(buf);

    QString major = str.left(pos);
    QString minior;
    if (pos != -1) minior = str.mid(pos + buf.length());

    for (pos = major.length(); pos >= 0; pos--)
        if (!major.at(pos).isNumber()) major.remove(pos, 1);

    for (pos = minior.length(); pos >= 0; pos--)
        if (!minior.at(pos).isNumber()) minior.remove(pos, 1);

    QString tot;
    if (neg) tot = '-';
    tot += major + '.' + minior;
    return tot.toDouble(ok);
}

double KLocale::readMoney(const QString &str, bool * ok) const
{
    bool neg;
    if (negativeMonetarySignPosition() == ParensAround)
    {
        int i1 = str.find('(');
        neg = i1 != -1 && i1 < str.find(')');
    }
    else
        neg = str.find(negativeSign()) != -1;

    QString buf = monetaryDecimalSymbol();
    int pos = str.find(buf);

    QString major = str.left(pos);
    QString minior;
    if (pos != -1) minior = str.mid(pos + buf.length());

    for (pos = major.length(); pos >= 0; pos--)
        if (!major.at(pos).isNumber()) major.remove(pos, 1);

    for (pos = minior.length(); pos >= 0; pos--)
        if (!minior.at(pos).isNumber()) minior.remove(pos, 1);

    QString tot;
    if (neg) tot = '-';
    tot += major + '.' + minior;
    return tot.toDouble(ok);
}

/**
 * helper method to read integers
 * @param str
 * @param pos the position to start at. It will be updated when we parse it.
 * @return the integer read in the string, or -1 if no string
 */
static int readInt(const QString &str, uint &pos)
{
	if (!str.at(pos).isDigit()) return -1;
	int result = 0;
	for (; str.length() > pos && str.at(pos).isDigit(); pos++) {
		result *= 10;
		result += str.at(pos).digitValue();
	}
	return result;
}

QDate KLocale::readDate(const QString &intstr) const
{
	QDate date;
	QString str = intstr.simplifyWhiteSpace().lower();
	QString fmt = _datefmtshort.simplifyWhiteSpace();

	int day = 0, month = 0, year = 0;
	uint strpos = 0;
	uint fmtpos = 0;

	while (fmt.length() > fmtpos && str.length() > strpos) {
		QChar c = fmt.at(fmtpos++);

		if (c != '%') {
			if (c.isSpace())
				strpos++;
			else if (c != str.at(strpos++))
				goto error;
			continue;
		}

		// remove space at the begining
		if (str.length() > strpos && str.at(strpos).isSpace())
			strpos++;

		c = fmt.at(fmtpos++);
		switch (c) {
		case 'a':
		case 'A':
			// this will just be ignored
			for (int j = 1; j < 8; j++) {
				QString s = weekDayName(j, c == 'a').lower();
				int len = s.length();
				if (str.mid(strpos, len) == s)
					strpos += len;
			}
			break;
		case 'b':
		case 'B':
			for (int j = 1; j < 13; j++) {
				QString s = monthName(j, c == 'b').lower();
				int len = s.length();
				if (str.mid(strpos, len) == s) {
					month = j;
					strpos += len;
				}
			}
			break;
		case 'd':
		case 'e':
			day = readInt(str, strpos);
			if (day < 1 || day > 31)
				goto error;

			break;

		case 'n':
		case 'm':
			month = readInt(str, strpos);
			if (month < 1 || month > 12)
				goto error;

			break;

		case 'Y':
		case 'y':
			year = readInt(str, strpos);
			if (year < 0)
				goto error;
			if (c == 'y') {
				if (year < 69) year += 100;
				year += 1900;
			}

			break;
		}
	}
	date.setYMD(year, month, day);

error:
        // if there was an error date will be zero, if not it would be ok
	return date;
}

#ifdef ENABLE_NLS

QString KLocale::formatTime(const QTime &pTime, bool includeSecs) const
{
  QString rst(_timefmt);

  int i = -1;
  while ( (i = rst.findRev('%', i)) != -1 )
    switch ( rst.at(i + 1).latin1() )
      {
      case 'H':
	rst.replace(i, 2, QString().sprintf("%02d", pTime.hour()));
	continue;
      case 'k':
	rst.replace(i, 2, QString().sprintf("%2d", pTime.hour()));
	continue;
      case 'I':
	rst.replace(i, 2, QString().sprintf("%02d", (pTime.hour() + 11) % 12 + 1));
	continue;
      case 'l':
	rst.replace(i, 2, QString().sprintf("%2d",  (pTime.hour() + 11) % 12 + 1));
	continue;
      case 'M':
	rst.replace(i, 2, QString().sprintf("%02d", pTime.minute()));
	continue;
      case 'S':
	if (includeSecs)
	  rst.replace(i, 2, QString().sprintf("%02d", pTime.second()));
	else
	  rst.remove(--i, 3);
	continue;
      case 'p':
	rst.replace(i, 2, pTime.hour() >= 12?translate("pm"):translate("am"));
	continue;
      default:
	rst.remove(i--, 1);
	continue;
      }

  return rst;
}

bool KLocale::use12Clock() const
{
  return _timefmt.contains(QString::fromLatin1("%p")) > 0;
}

QString KLocale::languages() const
{
  return langs;
}

QStringList KLocale::languageList() const
{
  return QStringList::split(':', languages());
}

#else /* ENABLE_NLS */

KLocale::KLocale( const QString & ) : _inited(true), lang(0)
{
  _datefmtshort = QString::fromLatin1("%m/%d/%y");
}

KLocale::~KLocale()
{
}

QString KLocale::translate(const char* index, const char* fallback) const
{
  return QString::fromUtf8(fallback);
}

QString KLocale::translate(const char* msgid) const
{
  return QString::fromUtf8(msgid);
}

bool KLocale::weekStartsMonday() const
{
  return TRUE;
}

QString KLocale::decimalSymbol() const
{
  return QString::fromLatin1(".");
}

QString KLocale::thousandsSeparator() const
{
  return QString::fromLatin1(",");
}

QString KLocale::currencySymbol() const
{
  return QString::fromLatin1("$");
}

QString KLocale::monetaryDecimalSymbol() const
{
  return QString::fromLatin1(".");
}

QString KLocale::monetaryThousandsSeparator() const
{
  return QString::fromLatin1(",");
}

QString KLocale::positiveSign() const
{
  return QString::null;
}

QString KLocale::negativeSign() const
{
  return QString::fromLatin1("-");
}

int KLocale::fracDigits() const
{
  return 2;
}

bool KLocale::positivePrefixCurrencySymbol() const
{
  return true;
}

bool KLocale::negativePrefixCurrencySymbol() const
{
  return true;
}

KLocale::SignPosition KLocale::positiveMonetarySignPosition() const
{
  return BeforeQuantityMoney;
}

KLocale::SignPosition KLocale::negativeMonetarySignPosition() const
{
  return ParensAround;
}

QString KLocale::formatMoney(double num, const QString &symbol, int digits) const
{
  QString str = QString::number( num, 'f', digits==-1?2:digits );
  if (!symbol.isNull())
    str.prepend(symbol);

  return str;
}

QString KLocale::formatMoney(const QString &numStr) const
{
  return numStr;
}

QString KLocale::formatNumber(double num, int precision) const
{
  return QString::number( num, 'f', precision==-1?2:precision);
}

QString KLocale::formatNumber(const QString &numStr) const
{
  return numStr;
}

QString KLocale::formatDate(const QDate &pDate, bool) const
{
  QString buf;
  buf.sprintf("%.2d/%.2d/%.2d", pDate.month(), pDate.day(), pDate.year() % 100);
  return buf;
}

QString KLocale::formatTime(const QTime &pTime, bool includeSecs) const
{
  QString result = pTime.toString();
  if (!includeSecs)
     result = result.left(5);
  return result;
}

bool KLocale::use12Clock() const
{
  return true;
}

QString KLocale::languages() const
{
  return QString::fromLatin1("C");
}

QStringList KLocale::languageList() const
{
    return QStringList(languages());
}

void KLocale::insertCatalogue( const QString& )
{
}

void KLocale::setLanguage(const QString & /* _lang */)
{
}

#endif /* ENABLE_NLS */

QString KLocale::formatDateTime(const QDateTime &pDateTime) const
{
  return formatDate(pDateTime.date()) + ' ' + formatTime(pDateTime.time());
}

QString i18n(const char* text)
{
#ifdef ENABLE_NLS
  register KLocale *instance = KGlobal::locale();
  if (instance)
     return instance->translate(text);
#endif
  return QString::fromUtf8(text);
}

QString i18n(const char* index, const char *text)
{
#ifdef ENABLE_NLS
  register KLocale *instance = KGlobal::locale();
  if (instance)
     return instance->translate(index, text);
#endif
  return QString::fromUtf8(text);
}

void KLocale::initInstance()
{
  if (KGlobal::_locale)
     return;

  KInstance *app = KGlobal::instance();
  if (app)
    KGlobal::_locale = new KLocale(app->instanceName());
  else
    debug("no app name available using KLocale - nothing to do");
}
