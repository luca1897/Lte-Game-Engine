/*
 This tutorial shows how to use the NetworkDriver
 
*/
#include <stdio.h>
#include <wchar.h>
#include <engine.h>
#include "../common.h"

using namespace engine;
using namespace scene;

using namespace io;
using namespace video;
using namespace net;
using namespace core;
using namespace gui;


scene::ISceneNode* node = 0;
engineDevice* device = 0;

// This is a binary buffer that can contain also null chars. This class provides some good
// functions to manage a binary data buffer.
core::stringb buffer;

gui::IGUIStaticText *lblStatus;
gui::IGUIButton *btnWifi, *btnDownload;

/*
 In this tutorial you will learn how to use the network driver, in this simple example
 will be created an user interface that downloads an image from the web and shows it
 on the screen.
*/

// This is our socket event class. This class implements the net::INetworkCallback interface,
// here we will receive the socket events like connection close and data received.
class CSocketEvents : public net::INetworkCallback
{
	
		public:

		//! This event is called when a socket is closed
		virtual void onClose(ISocket* Socket)
		{
		
		  /*
		    We get the received data and cut off the header part
		  */
		  core::array<core::stringb>& p = buffer.explode("\r\n\r\n", 2);
		  
		  // We create a memory read file using the received data
			IReadFile* rfLogo = io::createMemoryReadFile( p.pointer()[1].ret_buffer(), p.pointer()[1].get_size(), "#TempFile.jpg", false );
			video::ITexture *LTELogo = 0;

			// Load the texture
			LTELogo = device->getVideoDriver()->getTexture(rfLogo);
			rfLogo->drop();
			
			// Clear and free space of the buffer
			buffer.clear ();

		  // We create a gui image and display the downloaded picture
			gui::IGUIEnvironment *env = device->getGUIEnvironment();
			env->addImage(core::rect<s32>(102, 100, 378, 210))->setImage(LTELogo);
			
			lblStatus->setText(L"Download complete!");
			
			env->addStaticText(L"http://www.ltestudios.com/images/logo_ge.jpg", core::rect<s32>(105, 218, 378, 240), false);
			
			// Disconnect wifi
			device->getNetworkDriver()->disconnectApctl();	
	  }
		
		//! This event is called when data is received
	  virtual void onDataReceive(ISocket* Socket)
	  {
	 	 core::stringb data;
	 		
	 	 if (Socket->getData(&data) == true)
	 	 {
	 	 	buffer+=data;
	 	 }
	  }

		
		//! This event is called when a socket has succesfully connected
		virtual void onConnect(ISocket *Socket)
	  {
	  
	  	lblStatus->setText(L"Connected. Downloading LTE Logo ...");
	  	Socket->writeLine("GET /images/logo_ge.jpg HTTP/1.0");
	  	Socket->writeLine("HOST: www.ltestudios.com");
	    Socket->writeLine("Connection-close: Close");
	    Socket->writeLine("\r\n");
    }
		
		//! This event is called when a socket could not connect to the host
		virtual void onError(ISocket *Socket)
	  {
	  	
	  
	  	if (Socket->getError() == UNKNOWN_ERROR)
	  	{
	  		
	  		// Retry connection
	  		Socket->connectTo("85.25.18.122", 80);
	  		
	  	}
	  	else 
	  	{
	  		
	  	core::stringw err = L"Unable to connect. Error Number=";
	  	err += (int)Socket->getError();
	  	
	  	lblStatus->setText(err.c_str());
	  	
	    }
    }
		
		//! This event is called when the wifi connection is estabilished
		//! \param Connected: If true the wifi has estabilished a connection
		virtual void onWiFiStatusChange(bool Connected)
	  {
		
		if (Connected == true)
		{
			lblStatus->setText(L"WiFi Connection estabilished");
		  btnDownload->setEnabled(true);	

			
		}
		else
		{
			
			lblStatus->setText(L"Unable to initialize WiFi");
			btnWifi->setEnabled(true);
		}
		
  }
	
};

CSocketEvents SocketEventReceiver;

/*
The Event Receiver is not only capable of getting keyboard and
mouse input events, but also events of the graphical user interface 
(gui). There are events for almost everything: Button click, 
Listbox selection change, events that say that a element was hovered
and so on. To be able to react to some of these events, we create 
an event receiver. 
We only react to gui events, and if it's such an event, we get the
id of the caller (the gui element which caused the event) and get 
the pointer to the gui environment.
*/
class MyEventReceiver : public IEventReceiver
{
public:
	virtual bool OnEvent(SEvent event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();
			IGUIEnvironment* env = device->getGUIEnvironment();

			switch(event.GUIEvent.EventType)
			{



			/*
			If a button was clicked, it could be one of 'our'
			three buttons. If it is the first, we shut down the engine.
			If it is the second, we create a little window with some 
			text on it. We also add a string to the list box to log
			what happened. And if it is the third button, we create
			a file open dialog, and add also this as string to the list box.
			That's all for the event receiver.
			*/
			case EGET_BUTTON_CLICKED:

				if (id == 102)
				{
	
	
					btnDownload->setEnabled(false);
					lblStatus->setText(L"Please wait while connecting ...");
					
					ISocket *sock = device->getNetworkDriver()->createSocket();
					sock->connectTo("www.ltestudios.com", 80);
						
					
					return true;
					
					
				}
				if (id == 101)
				{

			    btnWifi->setEnabled(false);
			    lblStatus->setText(L"Initializing WiFi connection ..."); 
			    device->getNetworkDriver()->initialize(&SocketEventReceiver, 1);
			    
					return true;
				}
      }
		}

		return false;
	}
};




		

/*
 This is the entry point of the program
*/
int engineMain(unsigned int argc, void *argv)
{
  // setup psp
  setupPSP();

	device = createDevice();
	
	
	
	/* The creation was successful, now we set the event receiver and
		store pointers to the driver and to the gui environment. */

	MyEventReceiver receiver;
	device->setEventReceiver(&receiver);

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	

	/*
	To make the font a little bit nicer, we load an external font
	and set it as new font in the skin. 
	*/
	
	IGUISkin* skin = env->getSkin();
	IGUIFont* font = env->getFont("ms0:/media/fonttahoma.bmp");
	if (font)
		skin->setFont(font);
	
/*
	We add three buttons. The first one closes the engine. The second
	creates a window and the third opens a file open dialog. The third
	parameter is the id of the button, with which we can easily identify
	the button in the event receiver.
	*/	

	btnWifi = env->addButton(rect<s32>(105,10,220,30), 0, 101, L"Init Wifi");
	btnDownload = env->addButton(rect<s32>(225,10,395,30), 0, 102, L"Download LTE Logo");
	
	// Disable the download button
	btnDownload->setEnabled(false);
	
	
	/*
	 Status label
	*/
	lblStatus = env->addStaticText(L"Click Init Wifi to initialize WiFi", rect<s32>(105, 40, 395, 60), true);


	while(device->run())
	{
		driver->beginScene(true, true, SColor(0,200,200,200));

		env->drawAll();

		driver->endScene(); 

	}
	sceKernelExitGame();
	return 0;
}

