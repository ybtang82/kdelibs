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

#ifndef _EXECREQUEST_H
#define _EXECREQUEST_H

#include "artsflow.h"

class ExecRequest {
	ArtsServer *_Server;
	vector<ModuleDesc *> _Modules;
	long _ServerID,_StructureID,mid;
	string _structureName;

public:
	ExecRequest(ArtsServer *Server, long StructureID, const string&
																structureName);

	//const Arts::ModuleDescSeq& Modules();		// FIXME: remove me?
	long ServerID();
	long StructureID();
	long MID();		// FIXME: remove me
	const char *structureName();

	bool createModule(ModuleDesc *desc);
	bool localConnectModules();
	bool remoteConnectModules(ExecRequest *remote);
	bool finalizeModules();
   	void startModules();
   	void deleteModules();

	ArtsServer *Server();
};
#endif
