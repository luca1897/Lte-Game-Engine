/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __engine_I_NETWORK_DRIVER_H_INCLUDED__
#define __engine_I_NETWORK_DRIVER_H_INCLUDED__

#include "engineTypes.h"
#include "IUnknown.h"
#include "engineString.h"

namespace engine
{
namespace net
{
	
  //! Socket error codes
	enum E_SOCK_ERROR_CODES
  {
  	
  	//! No error
  	NO_ERROR = 0,
  	
  	//! WiFi was not initialized
  	WIFI_NOT_CONNECTED = 0xFFFFFFF0,
  	
  	//! This error occours when the method connectTo is called
  	//! on an already connected socket
  	INVALID_SOCKET_STATE = 0xFFFFFFF1,
  	
  	//! Cannot resolve hostname
  	CANNOT_RESOLVE_HOSTNAME = 0xFFFFFFF2,
  	
  	//! Connection rejected
  	CONNECTION_REJECTED = 0xFFFFFFF3,
  	
  	//! Unknown error
  	UNKNOWN_ERROR = 0xFFFFFFF4
  	
  };
	
	//! Interface of the socket class
	class ISocket : public IUnknown
	{
		
		public:
		
		//! Connect the socket
	  /** \param HostName: hostname or ip address to connect to, if this param is ommitted the socket will not be connected
	      \param Port: the port number
	      If the socket succesfully connects to the hostname specified the event onConnect is raised, otherwise
	      an event onError will be raised.
	  */
		virtual void connectTo(c8* HostName, int Port) = 0;
		
		//! Returns the current error code
		virtual E_SOCK_ERROR_CODES getError() = 0;
		
		//! Close the current connection
		virtual void closeSocket() = 0;
		
		//! Send data
		//! \param data: a null-terminated string
		virtual void write(c8* data) = 0;

		//! Write line
		//! \param data: a string
		virtual void writeLine(c8* data) = 0;

		//! Write binary data
		//! \param data: a pointer to the buffer that has to be sent
		//! \param size: the size of the buffer in bytes
		virtual void writeBytes(c8* data, int size) = 0;
				
		//! Write binary string
		//! \param data: a binary string
		virtual void writeBytes(core::stringb& data) = 0;
			
		//! Write binary string
		//! \param data: pointer to a binary string
		virtual void writeBytes(core::stringb* data) = 0;			
		
		//! Gets received data
		//! \param buffer: The buffer where put the received data
		virtual bool getData(core::stringb* buffer) = 0;
			
	  //! Read line
	  //! \param buffer: The buffer where put the read line
	  virtual bool readLine(core::stringc* buffer) = 0;

    //! Returns true if the socket is connected
    virtual bool isConnected() = 0;
  
  	//! Gets socket id
  	virtual int getSocketId() = 0;
  
		//! Gets the socket index in the cache
		virtual int getIndex() = 0;
		
		//! Returns the remote host ip
		virtual core::stringc& getRemoteIP() = 0;
			
		//! Returns the remote host name
		virtual core::stringc& getRemoteHost() = 0;
			
  };
	
	// Interface of an object wich can receive network callbacks
	class INetworkCallback 
	{
		
		public:
			
		//! This event is called when a socket is closed
		virtual void onClose(ISocket* Socket) = 0;
		
		//! This event is called when data is received
		virtual void onDataReceive(ISocket* Socket) = 0;
		
		//! This event is called when a socket has succesfully connected
		virtual void onConnect(ISocket *Socket) = 0;
		
		//! This event is called when an error occours
		virtual void onError(ISocket *Socket) = 0;
		
		//! This event is called when the wifi connection is estabilished
		//! \param Connected: If true the wifi has estabilished a connection
		virtual void onWiFiStatusChange(bool Connected) = 0;
			
		
	};
	
	
	//! Interface of the network driver
  class INetworkDriver : public IUnknown
	{
	
	  public:
	  
	  //! Initialize the wifi connections
	  /**
	     \param ApctlIndex: apctl config index
	     \param NetworkCallback: an object that extends the INetworkCallback interface
	     \return Returns true if wifi was succesfully initialized */
	  virtual void initialize( INetworkCallback *NetworkCallback, int ApctlIndex = 1) = 0;
	  
	  //! Close the wifi connection
	  virtual void disconnectApctl() = 0;
	  
	  //! Create a socket
	  /** \param HostName: hostname or ip address to connect to, if this param is ommitted the socket will not be connected
	     \param Port: the port number
	     \return Returns the created socket
	  */
	  virtual ISocket* createSocket(c8 *HostName = 0, int Port = 0) = 0;
	  
	  //! Get the socket with the index specified
	  //! \param Index: the socket index you want to get
	  //! \return Returns the socket class
	  virtual ISocket* getSocketByIndex(int Index) = 0;
	  
	  //! Gets the local ip
	  //! \return Return the local ip address
	  virtual core::stringc& getLocalAddress() = 0;
	
  };
	
}
}

#endif
