/*
Copyright (c) 2000 Waldo Bastian <bastian@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
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

#include <dcopserver.h>
#include <dcopsignals.h>

DCOPSignals::DCOPSignals()
{
   connections.setAutoDelete(true);
}

/**
 * Client "conn" emits the signal "fun" with "data" as arguments.
 *
 * If "excludeSelf" is true, signal is never send to "conn" itself.
 */
void
DCOPSignals::emitSignal( DCOPConnection *conn, const QCString &fun, const QByteArray &data, bool excludeSelf)
{
   DCOPSignalConnectionList *list = connections.find(fun);	 
   if (!list) return;
   for(DCOPSignalConnection *current = list->first(); current; current = list->next())
   {
      bool doSend = false;
      if (current->senderConn)
      {
         if (current->senderConn == conn) 
	    doSend = true;
      }
      else if (!current->sender.isEmpty())
      {
	 if (current->sender == conn->appId)
	    doSend = true;
      }
      else
      {
         doSend = true;
      }
      
      if (excludeSelf && (conn == current->recvConn))
         doSend = false;
      if (doSend)
      {
         the_server->sendMessage(current->recvConn, conn->appId, 
                                 current->recvConn->appId, current->recvObj,
                                 current->slot, data);
      }
   }
}

/**
 * Connects "signal" of the client named "sender" with the "slot" of 
 * "receiverObj" in the "conn" client.
 *
 * If "Volatile" is true the connection will be removed when either
 * "sender" or "conn" unregisters.
 *
 * If "Volatile" is false the connection will only be removed when
 * "conn" unregisters.
 */
bool 
DCOPSignals::connectSignal( const QCString &sender, const QCString &signal, 
                       DCOPConnection *conn, const QCString &receiverObj, 
                       const QCString &slot, bool Volatile)
{
   // TODO: Check if signal and slot match

   DCOPConnection *senderConn = 0;
   if (Volatile)
   {
      senderConn = the_server->findClient(sender);
      if (!senderConn)
         return false; // Sender does not exist.
   }
   DCOPSignalConnection *current = new DCOPSignalConnection;
   current->sender = sender;
   current->senderConn = senderConn;
   current->signal = signal;
   current->recvConn = conn;
   current->recvObj = receiverObj;
   current->slot = slot;

   DCOPSignalConnectionList *list = connections.find(signal);
   if (!list)
   {
      list = new DCOPSignalConnectionList;
      connections.insert(signal, list);
   }

   list->append( current );
   conn->signalConnectionList()->append(current);
   if (senderConn)
      senderConn->signalConnectionList()->append(current);
   return true;
}

/**
 * Disconnects "signal" of the client named "sender" from the "slot" of 
 * "receiverObj" in the "conn" client.
 */
bool 
DCOPSignals::disconnectSignal( const QCString &sender, const QCString &signal, 
                       DCOPConnection *conn, const QCString &receiverObj, 
                       const QCString &slot)
{
   DCOPSignalConnectionList *list = connections.find(signal);
   if (!list)
      return false; // Not found...
  
   DCOPSignalConnection *next = 0;
   bool result = false;

   for(DCOPSignalConnection *current = list->first(); current; current = next)
   {
      next = list->next();

      if (current->recvConn != conn)
         continue;

      if (current->senderConn)
      {
         if (current->senderConn->appId != sender) 
	    continue;
      }
      else if (current->sender != sender)
	    continue;

      if (!receiverObj.isEmpty() && 
          (current->recvObj != receiverObj))
         continue;

      if (!slot.isEmpty() && 
          (current->slot != slot))
         continue;
         
      result = true;
      list->removeRef(current);
      conn->signalConnectionList()->removeRef(current);
      if (current->senderConn)
         current->senderConn->signalConnectionList()->removeRef(current);
      delete current;
   }
   return result;
}

/**
 * Removes all connections related to the "conn" client.
 * This means:
 *   All connections for which "conn" is the receiver.
 *   All volatile connections for which "conn" is the sender.
 */
void 
DCOPSignals::removeConnections(DCOPConnection *conn)
{
   DCOPSignalConnectionList *list = conn->_signalConnectionList;
   if (!list)
      return; // Nothing to do...
  
   DCOPSignalConnection *next = 0;

   for(DCOPSignalConnection *current = list->first(); current; current = next)
   {
      next = list->next();

      if (current->senderConn && (current->senderConn != conn))
         current->senderConn->signalConnectionList()->removeRef(current);

      if (current->recvConn != conn)
         current->recvConn->signalConnectionList()->removeRef(current);

      DCOPSignalConnectionList *signalList = connections.find(current->signal);
      if (signalList)
      {
         signalList->removeRef(current);
         if (signalList->isEmpty())
            connections.remove(current->signal);
      }
      else
      {
         qDebug("Error: Signal Connection was not in signalList!\n");
      }
      list->removeRef(current);
      delete current;
   }
}


