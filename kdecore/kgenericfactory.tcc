/*
 * The Type2Type template and the Inheritance Detector are from
 * <http://www.cuj.com/experts/1810/alexandr.htm>
 * (c) Andrei Alexandrescu <andrei@metalanguage.com> and 
 * free for any use.
 *
 * The rest is:
 * Copyright (C) 2001 Simon Hausmann <hausmann@kde.org>
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
 */
// -*- mode: c++ -*-
//
//  W A R N I N G
//  -------------
//
// This file is not part of the KDE API.  It exists for the convenience
// of KGenericFactory. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qmetaobject.h>

namespace KParts
{
    class Part;
};

namespace KDEPrivate
{
    template <class Base>
    struct InheritanceDetector
    {
        typedef char ConversionExists;
        struct ConversionDoesNotExist { char bleh[ 2 ]; };
        static ConversionExists test( Base * );
        static ConversionDoesNotExist test( ... );
    };

    /* Simon: KCC doesn't eat the generic InheritanceDetector<Base>. 
       Instead we have to use concrete instantiations :-(

    template <class Base, class Derived>
    struct InheritanceTest
    {
        typedef Derived * DerivedPtr;
        enum { Result = sizeof( InheritanceDetector<Base>::test( DerivedPtr() ) ) ==
                        sizeof( InheritanceDetector<Base>::ConversionExists ) };
    };
    */

    template <class Derived>
    struct QWidgetInheritanceTest
    {
        typedef Derived * DerivedPtr;
        enum { Result = sizeof( InheritanceDetector<QWidget>::test( DerivedPtr() ) ) ==
                        sizeof( InheritanceDetector<QWidget>::ConversionExists ) };
    };

    template <class Derived>
    struct PartInheritanceTest
    {
        typedef Derived * DerivedPtr;
        enum { Result = sizeof( InheritanceDetector<KParts::Part>::test( DerivedPtr() ) ) ==
                        sizeof( InheritanceDetector<KParts::Part>::ConversionExists ) };
    };


    template <bool condition, typename Then, typename Else>
    struct If
    {
        typedef Else Result;
    };

    template <typename Then, typename Else>
    struct If<true, Then, Else>
    {
        typedef Then Result;
    };

    // a small helper template, to ease the overloading done in ConcreteFactory
    // to choose the right constructor for the given class.
    template <class T>
    struct Type2Type
    {
        typedef T OriginalType;
    };

    // this template is called from the MultiFactory one. It instantiates
    // the given class if the className matches. Instantiating is done by 
    // calling the right constructor (a parentwidget/widgetname/parent/name 
    // one for Parts, a parentwidget/widgetname one for widgets and last
    // but not least the standard default constructor of parent/name .
    // the choice of the right constructor is done using an ordered inheritance
    // test.
    template <class T>
    class ConcreteFactory
    {
    public:
        // select the right base type using inheritance tests. 
        // (can't use the generic InheritanceTest because kcc doesn't get it)
        typedef typename If< PartInheritanceTest< T >::Result,
                             KParts::Part,
                             typename If< QWidgetInheritanceTest< T >::Result,
                                          QWidget, QObject >::Result >::Result BaseType;
 
        static inline T *create( QWidget *parentWidget, const char *widgetName,
                                 QObject *parent, const char *name, const char *className,
                                 const QStringList &args )
        {
            QMetaObject *metaObject = T::staticMetaObject();
            while ( metaObject )
            {
                if ( !qstrcmp( className, metaObject->className() ) )
                    // instantiate using the right constructor.
                    return create( parentWidget, widgetName,
                                   parent, name, args, Type2Type<BaseType>() );
                metaObject = metaObject->superClass();
            }
            return 0;
        }
    private:
        static inline T *create( QWidget *parentWidget, const char *widgetName,
                                 QObject *parent, const char *name,
                                 const QStringList &args, Type2Type<KParts::Part> )
        { 
            // ### disabled args. enable next friday.
            //return new T( parentWidget, widgetName, parent, name, args ); 
            return new T( parentWidget, widgetName, parent, name ); 
        }

        static inline T *create( QWidget *parentWidget, const char *widgetName,
                                 QObject *parent, const char *name,
                                 const QStringList &args, Type2Type<QWidget> )
        {
            /* ### disabled for now. breaks source compat with regexp editor. 
               enable next friday!
            QWidget *p = dynamic_cast<QWidget *>( parent );
            if ( !p )
                p = parentWidget;
            if ( !p )
                return 0;
            return new T( p, name, args );
            */
            return new T( parent, name );
        }

        static inline T *create( QWidget *parentWidget, const char *widgetName,
                                 QObject *parent, const char *name,
                                 const QStringList &args, Type2Type<QObject> )
        { 
            // ### args disabled for now. enable next friday.
            //return new T( parent, name, args ); 
            return new T( parent, name ); 
        }
    };

    // this template is used to iterate through the typelist and call the
    // concrete factory for each type. the specializations of this template
    // are the ones actually being responsible for iterating, in fact.
    template <class T>
    class MultiFactory
    {
    public:
        inline static QObject *create( QWidget *parentWidget, const char *widgetName,
                                       QObject *parent, const char *name,
                                       const char *className, 
                                       const QStringList &args )
        {
            return ConcreteFactory<T>::create( parentWidget, widgetName,
                                               parent, name, className, args );
        }
 
    };

    // this specialized template we 'reach' at the end of a typelist
    // (the last item in a typelist is the NullType)
    template <>
    class MultiFactory<KDE::NullType>
    {
    public:
        inline static QObject *create( QWidget *, const char *, QObject *, 
                                       const char *, const char *, 
                                       const QStringList & )
        { return 0; }
    };
 
    template <class T1, class T2>
    class MultiFactory< KTypeList<T1, T2> >
    {
    public:
        inline static QObject *create( QWidget *parentWidget, const char *widgetName,
                                       QObject *parent, const char *name,
                                       const char *className, 
                                       const QStringList &args )
        {
            // try with the head of the typelist first. the head is always
            // a concrete type.
            QObject *object = MultiFactory<T1>::create( parentWidget, widgetName,
                                                        parent, name, className, args );

            // if that failed continue by advancing the typelist, calling this
            // template specialization recursively (with T2 being a typelist) .
            // at the end we reach the nulltype specialization.
            if ( !object )
                object = MultiFactory<T2>::create( parentWidget, widgetName,
                                                   parent, name, className, args );

            return object;
        }
    };
};

/*
 * vim: et sw=4
 */
