	/*

    Copyright (C) 2001 Nikolas Zimmermann <wildfox@kde.org>

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
#include "kplayobject.moc"

using namespace Arts;

KPlayObject::KPlayObject() : QObject()
{
    m_playObject = PlayObject::null();
    m_isStream = false;
}

KPlayObject::KPlayObject(PlayObject playobject, bool isStream) : QObject()
{
    m_playObject = playobject;
    m_isStream = isStream;
}

KPlayObject::~KPlayObject()
{
}

void KPlayObject::play()
{
    if(!m_isStream)
	object().play();
}

void KPlayObject::seek(Arts::poTime newTime)
{
    if(!m_isStream)
	object().seek(newTime);
}

void KPlayObject::pause()
{
    if(!m_isStream)
	object().pause();
}

void KPlayObject::halt()
{
    if(!m_isStream)
	object().halt();
}

QString KPlayObject::description()
{
    if(!m_isStream)
	return QString::fromLatin1(object().description().c_str());
}

Arts::poTime KPlayObject::currentTime()
{
    if(!m_isStream)
	return object().currentTime();
}

Arts::poTime KPlayObject::overallTime()
{
    if(!m_isStream)
	return object().overallTime();
}

Arts::poCapabilities KPlayObject::capabilities()
{
    if(!m_isStream)
	return object().capabilities();
}

QString KPlayObject::mediaName()
{
    if(!m_isStream)
	return QString::fromLatin1(object().mediaName().c_str());
}

Arts::poState KPlayObject::state()
{
    if(!m_isStream)
	return object().state();
}

PlayObject KPlayObject::object()
{
    return m_playObject;
}

void KPlayObject::setObject(Arts::PlayObject playObject)
{
    m_playObject = playObject;
}

bool KPlayObject::stream()
{
    return m_isStream;
}