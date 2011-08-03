/*
 * LTE Game Engine
 * Copyright (C) 2006-2008 SiberianSTAR <haxormail@gmail.com>
 * http://www.ltestudios.com
 *
 * The LTE Game Engine is based on Irrlicht 1.0
 * Irrlicht Engine is Copyright (C) 2002-2006 Nikolaus Gebhardt
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 */

#ifndef __engine__C_NETWORK_DRIVER__
#define __engine__C_NETWORK_DRIVER__

#include "INetworkDriver.h"
#include <sys/fd_set.h>

namespace engine
{
namespace net
{

class ISocketLow : public ISocket
{
public:
virtual void setInfo(void *drv, int ThisIndex);
virtual core::stringb* ret_buffer_in() ;
virtual core::stringb* ret_buffer_out() ;
virtual void close_fd();
virtual bool getConnectError() = 0;
virtual bool getCatchEvent() = 0;
virtual bool getConnected() = 0;
virtual void unsetCatchEvent() = 0;
virtual E_SOCK_ERROR_CODES getError() = 0;
virtual bool isPerformingConn() = 0;
virtual void unsetPerform() = 0;

virtual int getConnTimeout() = 0;
virtual void setConnTimeout(int c) = 0;

};

class CNetworkDriver : public INetworkDriver
{
public:
CNetworkDriver(bool InetModulesLoaded);

//! Initialize the wifi connections
/**
 \param ApctlIndex: apctl config index
 \param NetworkCallback: an object that extends the INetworkCallback interface
 \return Returns true if wifi was succesfully initialized */
virtual void initialize(INetworkCallback *NetworkCallback, int ApctlIndex = 1 );

//! Close the wifi connection
virtual void disconnectApctl();

//! Create a socket
/** \param HostName: hostname or ip address to connect to, if this param is ommitted the socket will not be connected
 \param Port: the port number
 \return Returns the created socket
   When the socket succesfully connects the event onConnect is raised, otherwise the event onError will be called.

 */
virtual ISocket* createSocket(c8 *HostName = 0, int Port = 0);

//! Get the socket with the index specified
//! \param Index: the socket index you want to get
//! \return Returns the socket class
virtual ISocket* getSocketByIndex(int Index);


//! Gets the local ip
//! \return Return the local ip address
virtual core::stringc& getLocalAddress();
void removeSocket(ISocketLow *socket);
void run();
bool ConnectionInitialized;
ISocketLow* searchFromSocket(int sock);
INetworkCallback*  cb;
private:
bool ModulesLoaded;
bool LastStatus;
core::stringc localip;

core::array<ISocketLow*> cache;
int socketIndexStart;
bool Connecting;
};

class CSocket : public ISocketLow
{

public:

CSocket();
~CSocket();

virtual void setInfo(void *drv, int ThisIndex);

//! Connect the socket
/** \param HostName: hostname or ip address to connect to, if this param is ommitted the socket will not be connected
 \param Port: the port number
   When the socket succesfully connects the event onConnect is raised, otherwise the event onError will be called.
 */
virtual void connectTo(c8* HostName, int Port);


//! Close the current connection
virtual void closeSocket();

//! Send data
virtual void write(c8* data);

//! Write line
virtual void writeLine(c8* data);

virtual void writeBytes(c8* data, int size);

//! Send data
virtual void writeBytes(core::stringb& data);

//! Send data
virtual void writeBytes(core::stringb* data);

//! Get received data
virtual bool getData(core::stringb* buffer);

//! Read line
virtual bool readLine(core::stringc* buffer);

//! Returns true if the socket is connected
virtual bool isConnected();

//! Get socket id
virtual int getSocketId();

//! Gets the socket index
virtual int getIndex();

virtual bool getConnectError();
virtual bool getCatchEvent() ;
virtual bool getConnected() ;
virtual void unsetCatchEvent();
virtual E_SOCK_ERROR_CODES getError() ;

virtual core::stringc& getRemoteIP() ;
virtual core::stringc& getRemoteHost() ;

virtual core::stringb* ret_buffer_in() ;
virtual core::stringb* ret_buffer_out() ;
virtual void close_fd();

virtual bool isPerformingConn();
virtual void unsetPerform();

virtual int getConnTimeout();
virtual void setConnTimeout(int c);

c8 szHostName[128];
u32 lPort;
bool Connected;
E_SOCK_ERROR_CODES error;
bool CatchEvent;
bool ConnectError;
int Socket;
CNetworkDriver *ntwrk;
bool PerformingConnection;
int ConnectionTimeout;
core::stringc rem_ip, rem_host;
core::stringb buffer_in, buffer_out;
private:
int SocketIndex;







};



}
}

#endif
