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

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include "object.h"
#include "factory.h"
#include <list>

class ObjectManager {
protected:
	static ObjectManager *_instance;
	list<Factory *> factories;
	list<string> referenceFiles;

public:
	ObjectManager();
	~ObjectManager();

	static ObjectManager *the();
	Object_skel *create(string name);

	bool addGlobalReference(Object_skel *object, string name);
	string getGlobalReference(string name);

	void registerFactory(Factory *factory);
	void removeFactory(Factory *factory);
};

#endif /* OBJECTMANAGER_H */
