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

    Permission is also granted to link this program with the Qt
    library, treating Qt like a library that normally accompanies the
    operating system kernel, whether or not that is in fact the case.

    */

#include "object.h"
#include "dispatcher.h"
#include "flowsystem.h"
#include <stdio.h>
#include <iostream.h>

using namespace std;

struct Object::ObjectStreamInfo {
	string name;
	long flags;
	void *ptr;
};

struct Object_skel::MethodTableEntry {
	DispatchFunction dispatcher;
	OnewayDispatchFunction onewayDispatcher;
	void *object;
	MethodDef methodDef;
};

/*
 * Object: common for every object
 */

long Object::_staticObjectCount = 0;

Object::Object() :  _deleteOk(false), _scheduleNode(0), _nextNotifyID(1),
							_refCnt(1)
{
	_staticObjectCount++;
}

void Object::_destroy()
{
	_deleteOk = true;

	if(_scheduleNode)
	{
		if(_scheduleNode->remoteScheduleNode())
		{
			delete _scheduleNode;
		}
		else
		{
			FlowSystem_impl *fs = Dispatcher::the()->flowSystem();
			assert(fs);

			fs->removeObject(_scheduleNode);
		}
	}
	delete this;
}

Object::~Object()
{
	if(!_deleteOk)
	{
		cerr << "error: reference counting violation - you may not" << endl;
		cerr << "       call delete manually - use _release() instead" << endl;
	}
	assert(_deleteOk);

	/* clear stream list */
	list<ObjectStreamInfo *>::iterator osii;
	for(osii = _streamList.begin(); osii != _streamList.end(); osii++)
		delete (*osii);

	_staticObjectCount--;
}

ScheduleNode *Object::_node()
{
	if(!_scheduleNode)
	{
		switch(_location())
		{
			case objectIsLocal:
				{
					FlowSystem_impl *fs = Dispatcher::the()->flowSystem();
					assert(fs);
					_scheduleNode = fs->addObject(_skel());

					/* init streams */

					list<ObjectStreamInfo *>::iterator osii;
					for(osii = _streamList.begin(); osii != _streamList.end(); osii++)
					{
						_scheduleNode->initStream((*osii)->name,(*osii)->ptr,(*osii)->flags);
					}
				}
				break;

			case objectIsRemote:
					_scheduleNode=new RemoteScheduleNode(_stub());
				break;
		}

		assert(_scheduleNode);
	}
	return _scheduleNode;
}

bool Object::_error()
{
	// no error as default ;)
	return false;
}

Object_skel *Object::_skel()
{
	assert(false);
	return 0;
}

Object::ObjectLocation Object::_location()
{
	assert(false);
	return objectIsLocal;
}

Object_stub *Object::_stub()
{
	assert(false);
	return 0;
}

Object_stub *Object_stub::_stub()
{
	return this;
}

Object::ObjectLocation Object_stub::_location()
{
	return objectIsRemote;
}

Object_skel *Object_skel::_skel()
{
	return this;
}

Object::ObjectLocation Object_skel::_location()
{
	return objectIsLocal;
}

void Object_skel::_initStream(string name, void *ptr, long flags)
{
	ObjectStreamInfo *osi = new ObjectStreamInfo;
	osi->name = name;
	osi->ptr = ptr;
	osi->flags = flags;
	_streamList.push_back(osi);
}

void Object::calculateBlock(unsigned long)
{
}

string Object::_interfaceName()
{
	assert(0); // derived classes *must* override this
	return "";
}

Buffer *Object::_allocCustomMessage(long /*handlerID*/)
{
	assert(0);
	return 0;
}

void Object::_sendCustomMessage(Buffer *buffer)
{
	assert(0);
	delete buffer;
}

/*
 * Stuff for object skeletons
 */

Object_skel::Object_skel() :_remoteSendCount(0), _remoteSendUpdated(false)
{
	_objectID = Dispatcher::the()->addObject(this);
	_methodTableInit = false;

	char ioid[1024];
	sprintf(ioid,"SKEL:%p",this);
	_internalObjectID = ioid;
}

Object_skel::~Object_skel()
{
	Dispatcher::the()->removeObject(_objectID);
}

// flow system

FlowSystem_base *Object_skel::_flowSystem()
{
	FlowSystem_base *fs = Dispatcher::the()->flowSystem();
	if(fs)
		return fs->_copy();
	else
		return 0;
}

// reference counting

void Object_skel::_release()
{
	assert(_refCnt > 0);
	_refCnt--;
	if((_refCnt+_remoteSendCount) == 0) _destroy();
}

void Object_skel::_copyRemote()
{
	// cout << "_copyRemote();" << endl;

	_remoteSendCount++;
	_remoteSendUpdated = true;
}

void Object_skel::_releaseRemote()
{
	//cout << "_releaseRemote();" << endl;

	Connection *conn = Dispatcher::the()->activeConnection();
	list<Connection *>::iterator i;
	bool found = false;

	for(i=_remoteUsers.begin(); !found && i != _remoteUsers.end(); i++)
	{
		found = (*i) == conn;
		if(found) _remoteUsers.erase(i);
	}
	assert(found);
	_release();
}

void Object_skel::_useRemote()
{
	//cout << "_useRemote();" << endl;

	Connection *conn = Dispatcher::the()->activeConnection();
	if(_remoteSendCount == 0)
	{
		cerr << "warning: _useRemote without prior _copyRemote() -"
					" this might fail sometimes" << endl;
	}
	else
	{
		_remoteSendCount--;
	}
	_refCnt++;
	_remoteUsers.push_back(conn);
}

void Object_skel::_disconnectRemote(Connection *conn)
{
	//cout << "_disconnectRemote();" << endl;

	int rcount = 0;
	list<Connection *>::iterator i;

	i=_remoteUsers.begin();
	while(i != _remoteUsers.end())
	{
		if((*i) == conn)
		{
			_remoteUsers.erase(i);
			i = _remoteUsers.begin();
			rcount++;
		}
		else i++;
	}

	while(rcount) {
		cerr << "client disconnected: dropped one object reference" << endl;
		rcount--;
		_release();
	}
	/* warning: object may not exist any longer here */
}

void Object_skel::_referenceClean()
{
	if(_remoteSendCount > 0)
	{
		if(_remoteSendUpdated)
		{
			// this ensures that every client gets at least five
			// seconds to connect
			_remoteSendUpdated = false;
		}
		else
		{
			cerr << "_referenceClean: found unused object"
			        " marked by _copyRemote => releasing" << endl;
			_remoteSendCount = 0;
			_refCnt++;
			_release();
		}
		/* warning: object may be gone here */
	}
}

string Object_skel::_toString()
{
	return Dispatcher::the()->objectToString(_objectID);
}

string Object_skel::_interfaceName()
{
	assert(0); // derived classes *must* override this
	return "";
}

string Object_skel::_interfaceNameSkel()
{
	assert(0); // derived classes *must* override this
	return "";
}


InterfaceDef* Object_skel::_queryInterface(const string& name)
{
	return Dispatcher::the()->interfaceRepo()->queryInterface(name);
}

TypeDef* Object_skel::_queryType(const string& name)
{
	return Dispatcher::the()->interfaceRepo()->queryType(name);
}

void Object_skel::_addMethod(DispatchFunction disp, void *obj,
                                               const MethodDef& md)
{
	MethodTableEntry me;
	me.dispatcher = disp;
	me.object = obj;
	me.methodDef = md;
	_methodTable.push_back(me);
}

void Object_skel::_addMethod(OnewayDispatchFunction disp, void *obj,
                                               const MethodDef& md)
{
	MethodTableEntry me;
	me.onewayDispatcher = disp;
	me.object = obj;
	me.methodDef = md;
	_methodTable.push_back(me);
}

long Object_skel::_addCustomMessageHandler(OnewayDispatchFunction handler,
																	void *obj)
{
	if(!_methodTableInit)
	{
		// take care that the object base methods are at the beginning
		Object_skel::_buildMethodTable();
		_buildMethodTable();
		_methodTableInit = true;
	}
	MethodTableEntry me;
	me.onewayDispatcher = handler;
	me.object = obj;
	me.methodDef.name = "_userdefined_customdatahandler";
	_methodTable.push_back(me);
	return _methodTable.size()-1;
}

void Object_skel::_dispatch(Buffer *request, Buffer *result,long methodID)
{
	if(!_methodTableInit)
	{
		// take care that the object base methods are at the beginning
		Object_skel::_buildMethodTable();
		_buildMethodTable();
		_methodTableInit = true;
	}
	_methodTable[methodID].dispatcher(_methodTable[methodID].object,
														request,result);
}

void Object_skel::_dispatch(Buffer *request,long methodID)
{
	if(!_methodTableInit)
	{
		// take care that the object base methods are at the beginning
		Object_skel::_buildMethodTable();
		_buildMethodTable();
		_methodTableInit = true;
	}
	_methodTable[methodID].onewayDispatcher(_methodTable[methodID].object,
																	request);
}

long Object_skel::_lookupMethod(const MethodDef& md)
{
	long mcount = 0;

	assert(_methodTableInit);

	vector<MethodTableEntry>::iterator i;
	for(i=_methodTable.begin(); i != _methodTable.end(); i++)
	{
		MethodDef& mdm = i->methodDef;
		if(mdm.name == md.name && mdm.type == md.type)
		{
		/*	TODO: compare signature
			vector<ParamDef *>::const_iterator j,k;
			j = md.signature.begin();
			k = mdm.signature.begin();
			while(j != md.signature.end() && (*j) == (*k))
			{
				j++;
				k++;
			}

			if(j == md.signature.end() && k == mdm.signature.end())
			*/
				return mcount;
		}
		mcount++;
	}
	return -1;
}

// _lookupMethod
static void _dispatch_Object_00(void *object, Buffer *request, Buffer *result)
{
	MethodDef methodDef(*request);
	result->writeLong(((Object_skel *)object)->_lookupMethod(methodDef));
}

// _interfaceName
static void _dispatch_Object_01(void *object, Buffer *, Buffer *result)
{
	result->writeString(((Object_skel *)object)->_interfaceName());
}

// _queryInterface
static void _dispatch_Object_02(void *object, Buffer *request, Buffer *result)
{
	string name;
	request->readString(name);
	InterfaceDef *_returnCode = ((Object_skel *)object)->_queryInterface(name);
	_returnCode->writeType(*result);
	delete _returnCode;
}

// _queryType
static void _dispatch_Object_03(void *object, Buffer *request, Buffer *result)
{
	string name;
	request->readString(name);
	TypeDef *_returnCode = ((Object_skel *)object)->_queryType(name);
	_returnCode->writeType(*result);
	delete _returnCode;
}

// _toString
static void _dispatch_Object_04(void *object, Buffer *, Buffer *result)
{
	result->writeString(((Object_skel *)object)->_toString());
}

// _copyRemote
static void _dispatch_Object_05(void *object, Buffer *, Buffer *)
{
	((Object_skel *)object)->_copyRemote();
}

// _useRemote
static void _dispatch_Object_06(void *object, Buffer *, Buffer *)
{
	((Object_skel *)object)->_useRemote();
}

// _releaseRemote
static void _dispatch_Object_07(void *object, Buffer *, Buffer *)
{
	((Object_skel *)object)->_releaseRemote();
}

// _get__flowSystem
static void _dispatch_Object_08(void *object, Buffer *, Buffer *result)
{
	FlowSystem_base *returnCode = ((Object_skel *)object)->_flowSystem();
	writeObject(*result,returnCode);
	if(returnCode) returnCode->_release();
}

void Object_skel::_buildMethodTable()
{
	Buffer m;
	m.fromString("MethodTable:0e0000005f6c6f6f6b75704d6574686f6400050000006c6f6e670000000000010000000a0000004d6574686f64446566000a0000006d6574686f64446566000f0000005f696e746572666163654e616d650007000000737472696e67000000000000000000100000005f7175657279496e74657266616365000d000000496e7465726661636544656600000000000100000007000000737472696e6700050000006e616d65000b0000005f71756572795479706500080000005479706544656600000000000100000007000000737472696e6700050000006e616d65000a0000005f746f537472696e670007000000737472696e670000000000000000000c0000005f636f707952656d6f74650005000000766f69640000000000000000000b0000005f75736552656d6f74650005000000766f69640000000000000000000f0000005f72656c6561736552656d6f74650005000000766f6964000000000000000000110000005f6765745f5f666c6f7753797374656d000b000000466c6f7753797374656d000000000000000000","MethodTable");
	_addMethod(_dispatch_Object_00,this,MethodDef(m));
	_addMethod(_dispatch_Object_01,this,MethodDef(m));
	_addMethod(_dispatch_Object_02,this,MethodDef(m));
	_addMethod(_dispatch_Object_03,this,MethodDef(m));
	_addMethod(_dispatch_Object_04,this,MethodDef(m));
	_addMethod(_dispatch_Object_05,this,MethodDef(m));
	_addMethod(_dispatch_Object_06,this,MethodDef(m));
	_addMethod(_dispatch_Object_07,this,MethodDef(m));
	_addMethod(_dispatch_Object_08,this,MethodDef(m));
}

/*
 * Stuff for object stubs
 */

Object_stub::Object_stub()
{
	assert(0); // never use this constructor, as it doesn't assign an object ID
}

Object_stub::Object_stub(Connection *connection, long objectID)
{
	_connection = connection;
	_connection->_copy();
	_objectID = objectID;
	_lookupCacheRandom = rand();

	char ioid[1024];
	sprintf(ioid,"STUB:%d:%p",_objectID,connection);
	_internalObjectID = ioid;
}

Object_stub::~Object_stub()
{
	/*
	 * invalidate method lookup cache entries of this object, as it might
	 * happen, that another Object_stub is created just at the same position
	 */
	if(_lookupMethodCache)
	{
		long c1 = (long)this,p;
		for(p=0;p<_lookupMethodCacheSize;p++)
		{
			long pos = 3*p;
			if(_lookupMethodCache[pos] == c1)
				_lookupMethodCache[pos] = 0;
		}
	}
	_connection->_release();
}

bool Object_stub::_error()
{
	/*
	 * servers are trustworthy - they don't do things wrong (e.g. send
	 * wrong buffers or things like that) - however, if the connection is
	 * lost, this indicates that something went terrible wrong (probably
	 * the remote server crashed, or maybe the network is dead), and you
	 * can't rely on results of invocations any longer
	 */
	return _connection->broken();
}

void Object_stub::_release()
{
	assert(_refCnt > 0);
	_refCnt--;
	if(_refCnt == 0)
	{
		_releaseRemote();
		_destroy();
	}
}

Object *Object::_create(const std::string& subClass)
{
	Object_skel *skel = ObjectManager::the()->create(subClass);
	assert(skel);
	return skel;
}

Object *Object::_fromString(string objectref)
{
	Object *result = 0;
	ObjectReference r;

	if(Dispatcher::the()->stringToObjectReference(r,objectref))
	{
		result = (Object *)Dispatcher::the()->connectObjectLocal(r,"Object");
		if(!result)
		{
			Connection *conn = Dispatcher::the()->connectObjectRemote(r);
			if(conn)
			{
				result = new Object_stub(conn,r.objectID);
				result->_useRemote();
			}
		}
	}
	return result;
}

Object *Object::_fromReference(ObjectReference r, bool needcopy)
{
	Object *result;
	result = (Object *)Dispatcher::the()->connectObjectLocal(r,"Object");
	if(!result)
	{
		Connection *conn = Dispatcher::the()->connectObjectRemote(r);
		if(conn)
		{
			result = new Object_stub(conn,r.objectID);
			if(needcopy) result->_copyRemote();
			result->_useRemote();
		}
	}
	return result;
}

string Object_stub::_interfaceName()
{
	long requestID;
	Buffer *request, *result;
	request = Dispatcher::the()->createRequest(requestID,_objectID,1);
	// methodID = 1  =>  _interfaceName (always)
	request->patchLength();
	_connection->qSendBuffer(request);

	result = Dispatcher::the()->waitForResult(requestID,_connection);
	if(!result) return ""; // error
	string returnCode;
	result->readString(returnCode);
	delete result;
	return returnCode;
}

InterfaceDef* Object_stub::_queryInterface(const string& name)
{
	long requestID;
	Buffer *request, *result;
	request = Dispatcher::the()->createRequest(requestID,_objectID,2);
	// methodID = 2  =>  _queryInterface (always)
	request->writeString(name);
	request->patchLength();
	_connection->qSendBuffer(request);

	result = Dispatcher::the()->waitForResult(requestID,_connection);
	if(!result) return new InterfaceDef(); // error
	InterfaceDef *_returnCode = new InterfaceDef(*result);
	delete result;
	return _returnCode;
}

TypeDef* Object_stub::_queryType(const string& name)
{
	long requestID;
	Buffer *request, *result;
	request = Dispatcher::the()->createRequest(requestID,_objectID,3);
	// methodID = 3  =>  _queryType (always)
	request->writeString(name);
	request->patchLength();
	_connection->qSendBuffer(request);

	result = Dispatcher::the()->waitForResult(requestID,_connection);
	if(!result) return new TypeDef(); // error
	TypeDef *_returnCode = new TypeDef(*result);
	delete result;
	return _returnCode;
}

long Object_stub::_lookupMethod(const MethodDef& methodDef)
{
	long requestID;
	Buffer *request, *result;
	request = Dispatcher::the()->createRequest(requestID,_objectID,0);
	// methodID = 0  =>  _lookupMethod (always)
	methodDef.writeType(*request);
	request->patchLength();
	_connection->qSendBuffer(request);

	result = Dispatcher::the()->waitForResult(requestID,_connection);
	if(!result) return 0; // error
	long returnCode = result->readLong();
	delete result;
	return returnCode;
}

long *Object_stub::_lookupMethodCache = 0;

long Object_stub::_lookupMethodFast(const char *method)
{
	long c1 = (long)this;
	long c2 = (long)method;
	long pos = 3*((c1 ^ c2 ^ _lookupCacheRandom)%_lookupMethodCacheSize);

	if(!_lookupMethodCache)
	  _lookupMethodCache=(long *)calloc(_lookupMethodCacheSize,3*sizeof(long));
	
	if(_lookupMethodCache[pos] == c1 && _lookupMethodCache[pos+1] == c2)
		return _lookupMethodCache[pos+2];
	
	Buffer _methodBuffer;
	_methodBuffer.fromString(method,"method");
	long methodID = _lookupMethod(MethodDef(_methodBuffer));
	//cout << "methodID = " << methodID << endl;

	_lookupMethodCache[pos] = c1;
	_lookupMethodCache[pos+1] = c2;
	_lookupMethodCache[pos+2] = methodID;
	return methodID;
}

// other (normal) methods without fixed location

string Object_stub::_toString()
{
	long methodID = _lookupMethodFast("method:0a0000005f746f537472696e670007000000737472696e67000000000000000000");
	long requestID;
	Buffer *request, *result;
	request = Dispatcher::the()->createRequest(requestID,_objectID,methodID);
	// methodID = 7  =>  _toString
	request->patchLength();
	_connection->qSendBuffer(request);

	result = Dispatcher::the()->waitForResult(requestID,_connection);
	if(!result) return ""; // error
	string returnCode;
	result->readString(returnCode);
	delete result;
	return returnCode;
}

void Object_stub::_copyRemote()
{
	long methodID = _lookupMethodFast("method:0c0000005f636f707952656d6f74650005000000766f6964000000000000000000");
	long requestID;
	Buffer *request, *result;
	request = Dispatcher::the()->createRequest(requestID,_objectID,methodID);
	// methodID = 8  =>  _copyRemote
	request->patchLength();
	_connection->qSendBuffer(request);

	result = Dispatcher::the()->waitForResult(requestID,_connection);
	if(result) delete result;
}

void Object_stub::_useRemote()
{
	long methodID = _lookupMethodFast("method:0b0000005f75736552656d6f74650005000000766f6964000000000000000000");
	long requestID;
	Buffer *request, *result;
	request = Dispatcher::the()->createRequest(requestID,_objectID,methodID);
	// methodID = 9  =>  _useRemote
	request->patchLength();
	_connection->qSendBuffer(request);

	result = Dispatcher::the()->waitForResult(requestID,_connection);
	if(result) delete result;
}

void Object_stub::_releaseRemote()
{
	long methodID = _lookupMethodFast("method:0f0000005f72656c6561736552656d6f74650005000000766f6964000000000000000000");
	long requestID;
	Buffer *request, *result;
	request = Dispatcher::the()->createRequest(requestID,_objectID,methodID);
	// methodID = 10  =>  _releaseRemote
	request->patchLength();
	_connection->qSendBuffer(request);

	result = Dispatcher::the()->waitForResult(requestID,_connection);
	if(result) delete result;
}

FlowSystem_base *Object_stub::_flowSystem()
{
	long methodID = _lookupMethodFast("method:110000005f6765745f5f666c6f7753797374656d000b000000466c6f7753797374656d000000000000000000");
	long requestID;
	Buffer *request, *result;
	request = Dispatcher::the()->createRequest(requestID,_objectID,methodID);
	// methodID = 11  =>  _get__flowSystem
	request->patchLength();
	_connection->qSendBuffer(request);

	result = Dispatcher::the()->waitForResult(requestID,_connection);
	if(!result) return 0; // error
	FlowSystem_base* returnCode;
	readObject(*result,returnCode);
	delete result;
	return returnCode;
}

/*
 * custom messaging
 */

Buffer *Object_stub::_allocCustomMessage(long handlerID)
{
	return Dispatcher::the()->createOnewayRequest(_objectID,handlerID);
}

void Object_stub::_sendCustomMessage(Buffer *buffer)
{
	buffer->patchLength();
	_connection->qSendBuffer(buffer);
}
