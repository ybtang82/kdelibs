/* This file is part of the KDE libraries
   Copyright (C) 1999 Ian Zepp (icszepp@islc.net)

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

#include "kstringhandler.h"

QString KStringHandler::word( const QString &text , uint pos )
{
    QStringList list = split( text , " " );

    if ( pos < list.count() )
        return list[ pos ];

    return "";
}

QString KStringHandler::word( const QString &text , const char *range )
{
    // Format in: START:END
    // Note index starts a 0 (zero)
    //
    // 0:        first word to end
    // 1:3        second to fourth words
    QStringList list = split( text , " " );
    QString tmp = "";
    QString r = range;

    if ( text.isEmpty() )
        return tmp;

    // do stuff here
    QRegExp reg;

    int at = 0;
    int pos = 0;
    int cnt = 0;

    if ( r.find(QRegExp("[0-9]+:[0-9]+")) != -1 )
    {
        at  = r.find(":");
        pos = atoi( r.left(at).ascii() );
        cnt = atoi( r.remove(0,at+1).ascii() );
    }
    else if ( r.find(QRegExp(":+[0-9]+")) != -1 )
    {
        at  = r.find(":");
        pos = 0;
        cnt = atoi( r.remove(0,at+1).ascii() );
    }
    else if ( r.find(QRegExp("[0-9]+:+")) != -1 )
    {
        at  = r.find(":");
        pos = atoi( r.left(at).ascii() );
        cnt = list.count(); // zero index
    }
    else if ( r.find(QRegExp("[0-9]+")) != -1 )
    {
        pos = atoi( r.ascii() );
        cnt = pos;
    }
    else
    {
        return tmp; // not found/implemented
    }

    //
    // Extract words
    //
    int wordsToExtract = cnt-pos+1;
    QStringList::Iterator it = list.at( pos);

    while ( (it != list.end()) && (wordsToExtract-- > 0))
    {
       tmp += *it;
       tmp += " ";
       it++;
    }

    return tmp.stripWhiteSpace();
}

//
// Insertion and removal routines
//
QString KStringHandler::insword( const QString &text , const QString &word , uint pos )
{
    QString tmp = "";

    if ( text.isEmpty() )
        return word;

    if ( word.isEmpty() )
        return text;

    // Split words and add into list
    QStringList list = split( text );

    if ( pos >= list.count() )
        list.append( word );
    else
        list.insert( list.at(pos) , word );

    // Rejoin
    return join( list );
}

QString KStringHandler::setword( const QString &text , const QString &word , uint pos )
{
    QString tmp = "";

    if ( text.isEmpty() )
        return word;

    if ( word.isEmpty() )
        return text;

    // Split words and add into list
    QStringList list = split( text );

    if ( pos >= list.count() )
        list.append( word );
    else
    {
        list.insert( list.remove( list.at(pos) ) , word );
    }

    // Rejoin
    return join( list );
}

QString KStringHandler::remrange( const QString &text , const char *range )
{
    // Format in: START:END
    // Note index starts a 0 (zero)
    //
    // 0:        first word to end
    // 1:3        second to fourth words
    QStringList list = split( text , " " );
    QString tmp = "";
    QString r = range;

    if ( text.isEmpty() )
        return tmp;

    // do stuff here
    QRegExp reg;

    int at = 0;
    int pos = 0;
    int cnt = 0;

    if ( r.find(QRegExp("[0-9]+:[0-9]+")) != -1 )
    {
        at  = r.find(':');
        pos = atoi( r.left(at).ascii() );
        cnt = atoi( r.remove(0,at+1).ascii() );
    }
    else if ( r.find(QRegExp(":+[0-9]+")) != -1 )
    {
        at  = r.find(':');
        pos = 0;
        cnt = atoi( r.remove(0,at+1).ascii() );
    }
    else if ( r.find(QRegExp("[0-9]+:+")) != -1 )
    {
        at  = r.find(':');
        pos = atoi( r.left(at).ascii() );
        cnt = list.count(); // zero index
    }
    else if ( r.find(QRegExp("[0-9]+")) != -1 )
    {
        pos = atoi( r.ascii() );
        cnt = pos;
    }
    else
    {
        return text; // not found/implemented
    }

    //
    // Remove that range of words
    //
    int wordsToDelete = cnt-pos+1;
    QStringList::Iterator it = list.at( pos);

    while ( (it != list.end()) && (wordsToDelete-- > 0))
       it = list.remove( it );

    return join( list);
}

QString KStringHandler::remword( const QString &text , uint pos )
{
    QString tmp = "";

    if ( text.isEmpty() )
        return tmp;

    // Split words and add into list
    QStringList list = split( text );

    if ( pos < list.count() )
        list.remove( list.at( pos ) );

    // Rejoin
    return join( list );
}

QString KStringHandler::remword( const QString &text , const QString &word )
{
    QString tmp = "";

    if ( text.isEmpty() )
        return tmp;

    if ( word.isEmpty() )
        return text;

    // Split words and add into list
    QStringList list = split( text );

    QStringList::Iterator it = list.find(word);

    if (it != list.end())
       list.remove( it );

    // Rejoin
    return join( list );
}

//
// Capitalization routines
//
QString KStringHandler::capwords( const QString &text )
{
    QString tmp = "";

    if ( text.isEmpty() )
        return tmp;

    QStringList list = split( text );
    
    return join( capwords( split( text )));
}

QStringList KStringHandler::capwords( const QStringList &list )
{
    QStringList tmp;
    QString word;

    if ( list.count() == 0 )
        return tmp;

    for ( QStringList::ConstIterator it= list.begin();
          it != list.end();
          it++)
    {
        word = *it;
        word = word.left(1).upper() + word.remove(0,1);

        tmp.append( word ); // blank list to start with
    }

    return tmp;
}

//
// Reverse routines
//
QString KStringHandler::reverse( const QString &text )
{
    QString tmp;

    if ( text.isEmpty() )
        return tmp;

    QStringList list;
    list = split( text );
    list = reverse( list );

    return join( list );
}

QStringList KStringHandler::reverse( const QStringList &list )
{
    QStringList tmp;

    if ( list.count() == 0 )
        return tmp;

    for ( QStringList::ConstIterator it= list.begin();
          it != list.end();
          it++)
        tmp.prepend( *it );

    return tmp;
}

//
// Left, Right, Center justification
//
QString KStringHandler::ljust( const QString &text , uint width )
{
    QString tmp = text;
    tmp = tmp.stripWhiteSpace(); // remove leading/trailing spaces
    
    if ( tmp.length() >= width )
        return tmp;

    for ( uint pos = tmp.length() ; pos < width ; pos++ )
        tmp.append(" ");

    return tmp;
}

QString KStringHandler::rjust( const QString &text , uint width )
{
    QString tmp = text;
    tmp = tmp.stripWhiteSpace(); // remove leading/trailing spaces
    
    if ( tmp.length() >= width )
        return tmp;

    for ( uint pos = tmp.length() ; pos < width ; pos++ )
        tmp.prepend(" ");

    return tmp;
}

QString KStringHandler::center( const QString &text , uint width )
{
    // Center is slightly different, in that it will add
    // spaces to the RIGHT side (left-justified) before
    // it adds a space to the LEFT side.

    QString tmp = text;
    tmp = tmp.stripWhiteSpace(); // remove leading/trailing spaces
    
    if ( tmp.length() >= width )
        return tmp;

    bool left = false; // start at right side.

    for ( uint pos = tmp.length() ; pos < width ; pos++ )
    {
        if ( left )
            tmp.prepend(" ");
        else
            tmp.append(" ");

        // Reverse bool
        left = !left;
    }

    return tmp;
}

//
// Joins words in a list together
//
QString KStringHandler::join( const QStringList &list , const char *sep )
{
    QString tmp = "";

    if ( list.count() == 0 )
        return tmp;
    
    QStringList::ConstIterator last = list.fromLast();
    // Add all items with a sep except the last one
    for ( QStringList::ConstIterator it = list.begin();
          it != last;
          it++)
    {
        tmp += *it;
        tmp += sep;
    }

    // Add the last item
    tmp += *last;
    return tmp;
}

//
// Splits words in a string apart
//
QStringList KStringHandler::split( const QString &text , const char *sep )
{
    QStringList tmp;
    
    if ( text.isEmpty() )
        return tmp;

    int pos = -1;
    int old = 0;

    // Append words up to (not including) last
    while ( (pos = text.find( sep , old )) != -1 )
    {
        tmp.append( text.mid(old,pos - old) );
        old = pos + 1;
    }

    // Append final word
    tmp.append( text.mid(old,text.length()) );
    
    return tmp;
}
