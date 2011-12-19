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

#include "CNetworkDriver.h"

#include "os.h"
#include <string.h>
#include <limits.h>
#include <pspdebug.h>
#include <stdio.h>
#include <pspthreadman.h>
#include <pspkernel.h>
#include <unistd.h>
#include <pspnet.h>
#include <pspnet_inet.h>
#include <pspwlan.h>
#include <pspnet_apctl.h>
#include <pspnet_resolver.h>
#include <pspsdk.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <netdb.h>

#define printf pspDebugScreenPrintf

namespace engine
{
namespace net
{


CSocket *__ntdrv__temporary_socket;

CNetworkDriver::CNetworkDriver(bool InetModulesLoaded)
	: ConnectionInitialized(false), socketIndexStart(0), Connecting(false)
{

	ModulesLoaded = InetModulesLoaded;
	localip = "0.0.0.0";
	pspSdkInetInit();

}

#define printf pspDebugScreenPrintf

void CNetworkDriver::initialize(INetworkCallback *NetworkCallback, int ApctlIndex)
{

	Connecting = false;
	cb = NetworkCallback;

	if (ModulesLoaded == false)
	{
		cb->onWiFiStatusChange(false);
		return;
	}
	if (ConnectionInitialized == true)
	{
		cb->onWiFiStatusChange(true);
		return;
	}

	if (sceWlanGetSwitchState() == 0)
	{
		cb->onWiFiStatusChange(false);
		return;
	}



	int err;
	err = sceNetApctlConnect(ApctlIndex);
	if (err != 0)
	{
		cb->onWiFiStatusChange(false);
		return;
	}
	Connecting = true;
	return;
}

void CNetworkDriver::disconnectApctl()
{
	if (ModulesLoaded == false)
		return;

	sceNetApctlDisconnect();
	ConnectionInitialized = false;

}

ISocketLow* CNetworkDriver::searchFromSocket(int sock)
{



}

void CNetworkDriver::run()
{


	if (ModulesLoaded == false)
		return;

	if (Connecting == true && ConnectionInitialized == false)
	{
		int state;
		int err = sceNetApctlGetState(&state);
		if (err != 0)
		{

			Connecting = false;
			cb->onWiFiStatusChange(false);
			return;

		}

		if (state == 4)
		{
			// connection ok
			union SceNetApctlInfo info;
			if (sceNetApctlGetInfo(8, &info) == 0)
			{

				localip = info.ip;

			}
			else {

				localip = "0.0.0.0";
			}

			ConnectionInitialized = true;
			Connecting = false;
			cb->onWiFiStatusChange(true);
			return;
		}
	}


	if (ConnectionInitialized == false)
		return;

	for (int x = 0; x < cache.size(); x++)
	{
		ISocketLow *isl = cache.pointer()[x];

		if (isl->getCatchEvent() == true)
		{
			isl->unsetCatchEvent();

			if (isl->getConnected() == true)
			{
				isl->unsetPerform();
				cb->onConnect(isl);
			}
			if (isl->getConnectError() == true)
			{
				isl->unsetPerform();
				cb->onError(isl);
			}
		}

		if (isl->isPerformingConn() == true)
		{
			if (os::Timer::getRealTime() - isl->getConnTimeout() > 5000)
			{
				isl->unsetPerform();
				cb->onError(isl);
			}
		}

	}

	struct timeval tm;
	tm.tv_sec = 0;
	tm.tv_usec = 1000;


	/*
	   Controllo i socket che sono pronti a scrivere dati,
	   prima di tutto compilo un set di descrittori da utilizzare
	   in combinazione con la select
	 */
	int max_fd = 0;
	fd_set write_set;
	FD_ZERO(&write_set);

	for (int i = 0; i <  cache.size(); i++)
	{
		ISocketLow *p = cache.pointer()[i];
		if (p->isConnected() == false)
			continue;

		int fd = p->getSocketId();
		if (max_fd < fd)
			max_fd = fd;

		FD_SET(fd, &write_set);
	}
	max_fd++;

  # if 1
	if (max_fd > 1)
	{

		/*
		   Esegue la select per controllare quali socket sono pronti ad inviare socket
		 */

		int total_fds = select(max_fd, NULL, &write_set, NULL, &tm);

		/*
		   Controlla i socket pronti
		 */
		if (total_fds > 0)
		{
			for (int x = 0; x < cache.size(); x++)
			{
				ISocketLow *isl = cache.pointer()[x];
				int id = isl->getSocketId();
				if (isl->isConnected() == true && FD_ISSET(id, &write_set))
				{

					// il socket è pronto a inviare dati
					core::stringb* cl = isl->ret_buffer_out();
					if (cl->get_size() != 0)
					{
						int err = 0;
						if ( (err=write( isl->getSocketId(),   cl->ret_buffer(), cl->get_size())) <= 0)
						{
							isl->close_fd();
							cb->onClose(isl);
						}
						else
							cl->clear();
					}
				}
			}
		}
	}

	/*

	   Adesso controlla quali socket sono pronti a ricevere dati

	 */

	tm.tv_sec = 0;
	tm.tv_usec = 1000;

	fd_set read_set;
	max_fd = 0;
	FD_ZERO(&read_set);

	for (int i = 0; i <  cache.size(); i++)
	{
		ISocketLow *p = cache.pointer()[i];
		if (p->isConnected() == false)
			continue;

		int fd = p->getSocketId();
		if (max_fd < fd)
			max_fd = fd;

		FD_SET(fd, &read_set);
	}
	max_fd++;

	if (max_fd > 1)
	{

		int total_fds = select(max_fd, &read_set, NULL, NULL, &tm);

		/*
		   Controlla i socket pronti
		 */
		if (total_fds > 0)
		{
			for (int x = 0; x < cache.size(); x++)
			{
				ISocketLow *isl = cache.pointer()[x];
				int id = isl->getSocketId();
				if (isl->isConnected() == true && FD_ISSET(id, &write_set))
				{


					char buffer[1024];
					buffer[0] = 0;

					int rdb = read(isl->getSocketId(), buffer, 1024);
					if (rdb <= 0)
					{

						isl->close_fd();
						cb->onClose(isl);
					}
					else
					{

						core::stringb* cl = isl->ret_buffer_in();
						cl->append_data(buffer, rdb);
					}
				}
			}
		}
	}


	for (int x = 0; x < cache.size(); x++)
	{
		ISocketLow *isl = cache.pointer()[x];
		if (isl->isConnected() == true)
		{


			core::stringb *cl = isl->ret_buffer_in();
			if (cl->get_size() != 0)
			{
				cb->onDataReceive(isl);
				return;
			}
		}
	}
  #endif
}


void CNetworkDriver::removeSocket(ISocketLow *socket)
{
	int id = cache.linear_search(socket);
	if (id == -1)
		return;

	cache.erase(id);
}

ISocket* CNetworkDriver::createSocket(c8 *HostName, int Port)
{


	CSocket *nw = new CSocket();


	nw->setInfo(this, socketIndexStart);


	socketIndexStart++;

	cache.push_back(nw);

	if (HostName != 0)
	{

		nw->connectTo(HostName,Port);

	}

	return nw;
}

ISocket* CNetworkDriver::getSocketByIndex(int Index)
{

	for (int i = 0; i < cache.size(); i++)
	{

		if (cache.pointer()[i]->getIndex() == Index)
		{

			return cache.pointer()[i];
		}

	}

	return 0;
}

core::stringc& CNetworkDriver::getLocalAddress()
{
	return localip;
}






// -- sockets --

CSocket::~CSocket()
{

	if (Connected == true)
	{
		close(Socket);
	}

	ntwrk->removeSocket(this);
}

CSocket::CSocket()
	: CatchEvent(false), Connected(false), error(NO_ERROR)
{



}

void CSocket::setInfo(void* drv, int ThisIndex)
{
	ntwrk = (CNetworkDriver*)drv;
	SocketIndex = ThisIndex;
}


core::stringc& CSocket::getRemoteIP()
{
	return rem_ip;
}
core::stringc& CSocket::getRemoteHost()
{
	return rem_host;
}

int nd_ConnectionThread(unsigned int argc, void *argv)
{


	int *p = (int*)argv;
	CSocket *origin = (CSocket*)(*p) ;

	struct hostent *data;
	struct sockaddr_in saddr;

	data = gethostbyname(origin->szHostName);
	if (data == 0)
	{

		origin->error =CANNOT_RESOLVE_HOSTNAME;
		origin->ConnectError = true;
		origin->Connected=false;
		origin->CatchEvent = true;

		goto finishLabel ;
	}

	origin->Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (origin->Socket < 0)
	{
		origin->ConnectError = true;
		origin->Connected=false;
		origin->CatchEvent = true;
		origin->error = UNKNOWN_ERROR;
		goto finishLabel ;
	}
	saddr.sin_family = AF_INET;    // host byte order
	saddr.sin_port = htons(origin->lPort);  // short, network byte order
	saddr.sin_addr = *((struct in_addr *)data->h_addr);

	origin->ret_buffer_in()->clear();
	origin->ret_buffer_out()->clear();

	if (connect(origin->Socket, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {


		origin->error = CONNECTION_REJECTED;

		origin->ConnectError = true;
		origin->Connected=false;
		origin->CatchEvent = true;

		goto finishLabel;
	}



	origin->rem_host = origin->szHostName;
	origin->rem_ip = inet_ntoa( saddr.sin_addr );

	origin->ConnectError = false;
	origin->Connected=true;
	origin->CatchEvent = true;

finishLabel:
	sceKernelExitDeleteThread(0);

	return 0;
}
E_SOCK_ERROR_CODES CSocket::getError() {
	return error;
}
bool CSocket::getConnectError() {
	return ConnectError;
}
bool CSocket::getCatchEvent() {
	return CatchEvent;
}
bool CSocket::getConnected() {
	return Connected;
}
void CSocket::unsetCatchEvent()  {
	CatchEvent = false;
}
void CSocket::connectTo(c8* HostName, int Port)
{
	static int address;

	ConnectionTimeout=os::Timer::getRealTime();

	PerformingConnection = true;

	error = UNKNOWN_ERROR;
	if ( ntwrk->ConnectionInitialized == false )
	{
		return;
	}

	int thid = sceKernelCreateThread("connection_thread", nd_ConnectionThread,  0x18, 0x10000, PSP_THREAD_ATTR_USER, NULL);

	if (Connected == true)
	{
		error = INVALID_SOCKET_STATE;
		ntwrk->cb->onError(this);
		return;
	}
	strncpy(szHostName, HostName, 128);
	lPort = Port;

	if (thid >= 0)
	{

		address = (int)this;



		if (sceKernelStartThread(thid, 4, &address))
		{
			sceKernelDeleteThread(thid);
			error = UNKNOWN_ERROR;
			ntwrk->cb->onError(this);
		}

	}
	else
	{
		error = UNKNOWN_ERROR;
		ntwrk->cb->onError(this);

	}
	return;


}

bool CSocket::isPerformingConn()
{
	return PerformingConnection;
}
void CSocket::unsetPerform()
{
	PerformingConnection=false;
}

int CSocket::getConnTimeout()
{
	return ConnectionTimeout;
}
void CSocket::setConnTimeout(int c)
{
	ConnectionTimeout=c;
}


void CSocket::closeSocket()
{


	close_fd();

}

void CSocket::write(c8* data)
{

	if (Connected == false)
		return;

	buffer_out.append_data(data, strlen(data));


}

void CSocket::close_fd()
{

	if (Connected == true)
	{

		close(Socket);
		Connected = false;
		buffer_in.clear();
		buffer_out.clear();

	}

}

void CSocket::writeLine(c8* data)
{
	if (Connected == false)
		return;

	buffer_out.append_data(data, strlen(data));
	buffer_out.append_data("\r\n", 2);
}

void CSocket::writeBytes(c8* data, int size)
{
	if (Connected == false)
		return;

	buffer_out.append_data( data, size );


}
void CSocket::writeBytes(core::stringb& data)
{
	if (Connected == false)
		return;

	buffer_out.append_data( data.ret_buffer(), data.get_size() );


}

void CSocket::writeBytes(core::stringb* data)
{
	if (Connected == false)
		return;

	buffer_out.append_data( data->ret_buffer(), data->get_size() );

}

bool CSocket::getData(core::stringb* buffer)
{
	if (Connected == false)
		return false;

	buffer->clear();
	buffer->set_pointer( buffer_in.ret_buffer(), buffer_in.get_size() );
	buffer_in.set_pointer(0, 0);

	return true;

}

bool CSocket::readLine(core::stringc* buffer)
{

	if (Connected == false)
		return false;

	int count = 0;

	if ( (count=buffer_in.substr_count("\n")) == 0)
		return false;

	core::array<core::stringb>& arr = buffer_in.explode("\n",2);

	core::stringb readl = arr.pointer()[0];
	c8 *p;
	p = readl.ret_buffer();
	if (p[ readl.get_size() - 1] == '\r')
	{
		readl.set_pointer(p, readl.get_size() - 1);
	}
	readl.append_zero();


	*buffer = readl.to_stringc()->c_str();

	buffer_in.clear();
	if (arr.size() != 1)
	{

		buffer_in = arr.pointer()[1];


	}
	return true;
}

bool CSocket::isConnected()
{
	return Connected;
}

int CSocket::getSocketId()
{
	return Socket;
}

int CSocket::getIndex()
{

	return SocketIndex;

}

core::stringb* CSocket::ret_buffer_in()
{
	return &buffer_in;
}

core::stringb* CSocket::ret_buffer_out()
{
	return &buffer_out;
}






}
}
