    /*

    Copyright (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */

#ifndef FACTORY_H
#define FACTORY_H

#include <string>
#include "object.h"
#include "startupmanager.h"

class Factory : public StartupClass {
public:
	void startup();
	void shutdown();
	virtual Object_skel *createInstance() = 0;
	virtual string interfaceName() = 0;
};

#define REGISTER_IMPLEMENTATION(impl)                             \
  class impl ## _Factory : virtual public Factory {               \
  public:                                                         \
	string interfaceName() { return impl ## ::_interfaceNameSkel(); } \
	Object_skel *createInstance() { return new impl ; }           \
} The_ ## impl ## _Factory;

#endif
