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
 
#include "CengineDeviceLinux.h"

#ifdef LINUX

#include "IEventReceiver.h"
#include "engineList.h"
#include "os.h"
#include "CTimer.h"
#include "engineString.h"
#include "Keycodes.h"
#include <stdio.h>
#include "COSOperator.h"
#include "CColorConverter.h"
#include "engine.h"
#include <sys/utsname.h>

namespace engine
{

const char* wmDeleteWindow = "WM_DELETE_WINDOW";

//! constructor
CengineDeviceLinux::CengineDeviceLinux(video::E_DRIVER_TYPE driverType,
                                       const core::dimension2d<s32>& windowSize,
                                       u32 bits, bool fullscreen,
                                       bool sbuffer, IEventReceiver* receiver,
                                       const char* version)
	: CengineDeviceStub(version, receiver), close(false), DriverType(driverType),
	stencilbuffer(sbuffer), SoftwareImage(0)
{
	#ifdef _DEBUG
	setDebugName("CengineDeviceLinux");
	#endif

	// print version, distribution etc.
	core::stringc linuxversion;
	linuxversion = "Linux ";

	// thx to LynxLuna for pointing me to the uname function
	utsname LinuxInfo;
	uname(&LinuxInfo);

	linuxversion += LinuxInfo.sysname;
	linuxversion += " ";
	linuxversion += LinuxInfo.release;
	linuxversion += " ";
	linuxversion += LinuxInfo.version;

	Operator = new COSOperator(linuxversion.c_str());
	os::Printer::log(linuxversion.c_str(), ELL_INFORMATION);

	// create keymap
	createKeyMap();

	// create window

	if (driverType != video::EDT_NULL)
	{
		// create the window, only if we do not use the null device
		if (!createWindow(windowSize, bits, fullscreen, stencilbuffer))
			return;
	}

	// create cursor control

	CursorControl = new CCursorControl(this, driverType == video::EDT_NULL);

	// create driver

	createDriver(driverType, windowSize, bits, fullscreen, stencilbuffer);

	if (!VideoDriver)
		return;

	createGUIAndScene();
}



//! destructor
CengineDeviceLinux::~CengineDeviceLinux()
{
	if (display)
	{
		//os::Printer::log("Deleting window...", ELL_INFORMATION);

		#ifdef _engine_COMPILE_WITH_OPENGL_
		if (context)
		{
			if (!glXMakeCurrent(display, None, NULL))
				os::Printer::log("Could not release glx context.", ELL_WARNING);
			glXDestroyContext(display, context);
			context = 0;
		}
		#endif // #ifdef _engine_COMPILE_WITH_OPENGL_

		if (fullscreen)
		{
			XF86VidModeSwitchToMode(display, screennr, &oldVideoMode);
			XF86VidModeSetViewPort(display, screennr, 0, 0);
		}

		XCloseDisplay(display);
	}
}



bool CengineDeviceLinux::createWindow(const core::dimension2d<s32>& windowSize,
                                      u32 bits, bool fullscr, bool stencilBuffer)
{
	width = windowSize.Width;
	height = windowSize.Height;
	fullscreen = fullscr;

	os::Printer::log("Creating X window...", ELL_INFORMATION);

	display = XOpenDisplay(0);
	if (!display)
	{
		os::Printer::log("Error: Need running XServer to start engine Engine.", ELL_ERROR);
		return false;
	}

	screennr = DefaultScreen(display);

	// query extension

	s32 minor, major;
	if (fullscreen && !XF86VidModeQueryVersion(display, &major, &minor))
	{
		os::Printer::log("VidMode extension must be intalled to let engine "
		                 " be able to switch to fullscreen mode. Running in window mode.", ELL_WARNING);
		fullscreen = false;
	}

	// enumerate video modes

	s32 modeCount;
	XF86VidModeModeInfo** modes;

	XF86VidModeGetAllModeLines(display, screennr, &modeCount, &modes);

	// save current video mode

	oldVideoMode = *modes[0];

	// find fitting mode

	bool videoListEmpty = VideoModeList.getVideoModeCount() == 0;

	s32 bestMode = -1;
	s32 i = 0;
	for (; i<modeCount; ++i)
		if (modes[i]->hdisplay == width &&
		    modes[i]->vdisplay == height)
		{
			bestMode = i;
			if (videoListEmpty)
				VideoModeList.addMode(core::dimension2d<s32>(
				                              modes[i]->hdisplay, modes[i]->vdisplay), 0);
		}

	// get visual
	XVisualInfo* visual = 0;



	XVisualInfo visTempl; //Template to hold requested values
	int visNumber; // Return value of available visuals

	visTempl.screen = screennr;
	visTempl.depth = 16;
	visual = XGetVisualInfo(display, VisualScreenMask|VisualDepthMask,
	                        &visTempl, &visNumber);


	if (!visual)
	{
		os::Printer::log("Fatal error, could not get visual.", ELL_ERROR);
		XCloseDisplay(display);
		display=NULL;
		return false;
	}


	// create color map

	Colormap colormap;
	colormap = XCreateColormap(display,
	                           RootWindow(display, visual->screen),
	                           visual->visual, AllocNone);

	attributes.colormap = colormap;
	attributes.border_pixel = 0;
	attributes.event_mask = KeyPressMask | ButtonPressMask |
	                        StructureNotifyMask | PointerMotionMask |
	                        ButtonReleaseMask | KeyReleaseMask;

	// switch to fullscreen if extension is installed.

	if (fullscreen && bestMode != -1)
	{
		os::Printer::log("Starting fullscreen mode...", ELL_INFORMATION);

		XF86VidModeSwitchToMode(display, screennr, modes[bestMode]);
		XF86VidModeSetViewPort(display, screennr, 0, 0);

		s32 displayW = modes[bestMode]->hdisplay;
		s32 displayH = modes[bestMode]->vdisplay;

		XFree(modes);

		attributes.override_redirect = True;

		window = XCreateWindow(display,
		                       RootWindow(display, visual->screen),
		                       0, 0, displayW, displayH, 0, visual->depth,
		                       InputOutput, visual->visual,
		                       CWBorderPixel | CWColormap | CWEventMask |
		                       CWOverrideRedirect, &attributes);

		XWarpPointer(display, None, window, 0, 0, 0, 0, 0, 0);
		XMapRaised(display, window);
		XGrabKeyboard(display, window, True, GrabModeAsync,
		              GrabModeAsync, CurrentTime);
		XGrabPointer(display, window, True, ButtonPressMask,
		             GrabModeAsync, GrabModeAsync, window, None, CurrentTime);
	}
	else
	{
		// we want windowed mode
		if (fullscreen)
		{
			os::Printer::log("Could not find specified video mode, running windowed.", ELL_WARNING);
			fullscreen = false;
		}

		//os::Printer::log("Starting windowed mode...", ELL_INFORMATION);

		attributes.event_mask |= ExposureMask;

		window = XCreateWindow(display,
		                       RootWindow(display, visual->screen),
		                       0, 0, width, height, 0, visual->depth,
		                       InputOutput, visual->visual,
		                       CWBorderPixel | CWColormap | CWEventMask,
		                       &attributes);

		Atom wmDelete;
		wmDelete = XInternAtom(display, wmDeleteWindow, True);
		XSetWMProtocols(display, window, &wmDelete, 1);
		XMapRaised(display, window);
	}

	Window tmp;
	u32 tmp2;

	XGetGeometry(display, window, &tmp, &x, &y, &width, &height, &tmp2, &depth);

	//os::Printer::log("Window created.", ELL_INFORMATION);


	// create an XImage for the software renderer
	//(thx to Nadav for some clues on how to do that!)

	if (DriverType == video::EDT_SOFTWARE || DriverType == video::EDT_SOFTWARE2)
	{
		SoftwareImage = XCreateImage(display,
		                             visual->visual, visual->depth,
		                             ZPixmap, 0, 0, width, height,
		                             BitmapPad(display), 0);

		SoftwareImage->data = new char[SoftwareImage->bytes_per_line * SoftwareImage->height];
	}

	XFree(visual);
	return true;
}


//! create the driver
void CengineDeviceLinux::createDriver(video::E_DRIVER_TYPE driverType,
                                      const core::dimension2d<s32>& windowSize,
                                      u32 bits,
                                      bool fullscreen,
                                      bool stencilbuffer)
{
	switch(driverType)
	{

	case video::EDT_SOFTWARE:
		VideoDriver = video::createSoftwareDriver(windowSize, fullscreen, FileSystem, this);
		break;

	case video::EDT_SOFTWARE2:
		VideoDriver = video::createSoftwareDriver2(windowSize, fullscreen, FileSystem, this);
		break;

	case video::EDT_OPENGL:
	#ifdef _engine_COMPILE_WITH_OPENGL_
		VideoDriver = video::createOpenGLDriver(windowSize, fullscreen, doublebuffer, stencilbuffer, window, display, FileSystem);
	#endif
		break;

	case video::EDT_NULL:
		VideoDriver = video::createNullDriver(FileSystem, windowSize);
		break;

	default:
		os::Printer::log("This driver is not available in Linux. Try OpenGL or Software renderer.",
		                 ELL_WARNING);
	}
}



//! runs the device. Returns false if device wants to be deleted
bool CengineDeviceLinux::run()
{
	os::Timer::tick();

	if (DriverType != video::EDT_NULL)
	{
		engine::SEvent engineevent;

		while (XPending(display) > 0 && !close)
		{
			XNextEvent(display, &event);

			switch (event.type)
			{
			case ConfigureNotify:
				// check for changed window size
				if ((event.xconfigure.width != width) ||
				    (event.xconfigure.height != height))
				{
					width = event.xconfigure.width;
					height = event.xconfigure.height;
					//os::Printer::log("TODO: resize should be disabled.", ELL_INFORMATION);
					if (VideoDriver)
						VideoDriver->OnResize(core::dimension2d<s32>(width, height));
				}
				break;

			case MotionNotify:
				engineevent.EventType = engine::EET_MOUSE_INPUT_EVENT;
				engineevent.MouseInput.Event = engine::EMIE_MOUSE_MOVED;
				engineevent.MouseInput.X = event.xbutton.x;
				engineevent.MouseInput.Y = event.xbutton.y;

				postEventFromUser(engineevent);
				break;

			case ButtonPress:
			case ButtonRelease:

				engineevent.EventType = engine::EET_MOUSE_INPUT_EVENT;
				engineevent.MouseInput.X = event.xbutton.x;
				engineevent.MouseInput.Y = event.xbutton.y;

				engineevent.MouseInput.Event = engine::EMIE_MOUSE_MOVED;

				switch(event.xbutton.button)
				{
				case  Button1:
					engineevent.MouseInput.Event =
					        (event.type == ButtonPress) ? engine::EMIE_LMOUSE_PRESSED_DOWN : engine::EMIE_LMOUSE_LEFT_UP;
					break;

				case  Button3:
					engineevent.MouseInput.Event =
					        (event.type == ButtonPress) ? engine::EMIE_RMOUSE_PRESSED_DOWN : engine::EMIE_RMOUSE_LEFT_UP;
					break;

				case  Button2:
					engineevent.MouseInput.Event =
					        (event.type == ButtonPress) ? engine::EMIE_MMOUSE_PRESSED_DOWN : engine::EMIE_MMOUSE_LEFT_UP;
					break;

				case  Button4:
					engineevent.MouseInput.Event = EMIE_MOUSE_WHEEL;
					engineevent.MouseInput.Wheel = 1.0f;
					break;

				case  Button5:
					engineevent.MouseInput.Event = EMIE_MOUSE_WHEEL;
					engineevent.MouseInput.Wheel = -1.0f;
					break;
				}

				if (engineevent.MouseInput.Event != engine::EMIE_MOUSE_MOVED)
					postEventFromUser(engineevent);
				break;

			case MappingNotify:
				XRefreshKeyboardMapping (&event.xmapping) ;
				break;

			case KeyRelease:
			case KeyPress:
			{
				SKeyMap mp;
				//mp.X11Key = XLookupKeysym(&event.xkey, 0);
				KeySym k;
				char buf [4];
				int len = XLookupString (&event.xkey, buf, sizeof buf, &k, NULL) ;
				mp.X11Key = k;

				s32 idx = KeyMap.binary_search(mp);

				if (idx != -1)
				{
					engineevent.EventType = engine::EET_KEY_INPUT_EVENT;
					engineevent.KeyInput.Key = (EKEY_CODE)KeyMap[idx].Win32Key;
					engineevent.KeyInput.PressedDown = (event.type == KeyPress);

					//engineevent.KeyInput.Char = mp.X11Key;

					engineevent.KeyInput.Char = (len>0 ? buf[0] : 0);
					engineevent.KeyInput.Control = (event.xkey.state & ControlMask) != 0;
					if (engineevent.KeyInput.Control)
						engineevent.KeyInput.Char = (engineevent.KeyInput.Char & 0x1f) + '@';

					postEventFromUser(engineevent);
				}
				else
					os::Printer::log("Could not find win32 key for x11 key.", ELL_WARNING);
			}
				break;

			case ClientMessage:
				if (*XGetAtomName(display, event.xclient.message_type) ==
				    *wmDeleteWindow)
				{
					os::Printer::log("Quit message received.", ELL_INFORMATION);
					close = true;
				}
				break;

			default:
				break;
			} // end switch

		} // end while

		XSync(display, false);
	}

	return !close;
}



//! sets the caption of the window
void CengineDeviceLinux::setWindowCaption(const wchar_t* text)
{
	if (DriverType == video::EDT_NULL)
		return;

	core::stringc textc = text;
	XSetStandardProperties(display, window, textc.c_str(), textc.c_str(),
	                       None, NULL, 0, NULL);
}




//! presents a surface in the client area
void CengineDeviceLinux::present(video::IImage* image, s32 windowId, core::rect<s32>* src )
{
	// thx to Nadav, who send me some clues of how to display the image
	// to the X Server.

	if (image->getColorFormat() != video::ECF_A1R5G5B5 &&
	    image->getColorFormat() != video::ECF_A8R8G8B8)
	{
		os::Printer::log("Internal error, can only present A1R5G5B5 and A8R8G8B8 pictures.");
		return;
	}

	int destwidth = SoftwareImage->width;
	int destheight = SoftwareImage->height;
	int srcwidth = image->getDimension().Width;
	int srcheight = image->getDimension().Height;

	if ( image->getColorFormat() == video::ECF_A8R8G8B8 )
	{
		// display 32 bit image

		s32* srcdata = (s32*)image->lock();

		if (depth == 32)
		{
			int destPitch = SoftwareImage->bytes_per_line / 4;
			s32* destData = reinterpret_cast<s32*>(SoftwareImage->data);

			for (int y=0; y<srcheight; ++y)
				for (int x=0; x<srcwidth; ++x)
					if (x < destwidth && y < destheight)
						destData[y*destPitch + x] = srcdata[y*srcwidth+x];
		}
		else
		if (depth == 16)
		{
			// convert from A1R5G5B5 to R5G6B6

			int destPitch = SoftwareImage->bytes_per_line / 2;
			s16* destData = reinterpret_cast<s16*>(SoftwareImage->data);

			for (int x=0; x<srcwidth; ++x)
				for (int y=0; y<srcheight; ++y)
					if (x < destwidth && y < destheight)
					{
						s16 c = video::X8R8G8B8toA1R5G5B5(srcdata[y*srcwidth+x]);
						destData[y*destPitch + x] =
						        ((c >> 10)&0x1F)<<11 |
						        ((c >> 5)&0x1F) << 6 |
						        ((c)&0x1F);
					}
		}
		else
		if (depth == 24)
		{
			// convert from A1R5G5B5 to X8R8G8B8

			int destPitch = SoftwareImage->bytes_per_line / 4;
			s32* destData = reinterpret_cast<s32*>(SoftwareImage->data);

			for (int y=0; y<srcheight; ++y)
				for (int x=0; x<srcwidth; ++x)
					if (x < destwidth && y < destheight)
						destData[y*destPitch + x] = srcdata[y*srcwidth+x];
		}
		else
		{
			os::Printer::log("Unsupported screen depth.");
		}

		image->unlock();
	}
	else
	{

		// display 16 bit image:

		s16* srcdata = (s16*)image->lock();

		if (depth == 16)
		{
			// convert from A1R5G5B5 to R5G6B6

			int destPitch = SoftwareImage->bytes_per_line / 2;
			s16* destData = reinterpret_cast<s16*>(SoftwareImage->data);

			for (int x=0; x<srcwidth; ++x)
				for (int y=0; y<srcheight; ++y)
					if (x < destwidth && y < destheight)
					{
						s16 c = srcdata[y*srcwidth+x];
						destData[y*destPitch + x] =
						        ((c >> 10)&0x1F)<<11 |
						        ((c >> 5)&0x1F) << 6 |
						        ((c)&0x1F);
					}
		}
		else
		if (depth == 32)
		{
			// convert from A1R5G5B5 to X8R8G8B8

			int destPitch = SoftwareImage->bytes_per_line / 4;
			s32* destData = reinterpret_cast<s32*>(SoftwareImage->data);

			for (int y=0; y<srcheight; ++y)
				for (int x=0; x<srcwidth; ++x)
					if (x < destwidth && y < destheight)
						destData[y*destPitch + x] =
						        video::A1R5G5B5toA8R8G8B8(srcdata[y*srcwidth+x]);
		}
		else
		if (depth == 24)
		{
			// convert from A1R5G5B5 to X8R8G8B8

			int destPitch = SoftwareImage->bytes_per_line / 4;
			s32* destData = reinterpret_cast<s32*>(SoftwareImage->data);

			for (int y=0; y<srcheight; ++y)
				for (int x=0; x<srcwidth; ++x)
					if (x < destwidth && y < destheight)
						destData[y*destPitch + x] =
						        video::A1R5G5B5toA8R8G8B8(srcdata[y*srcwidth+x]);
		}
		else
		{
			os::Printer::log("Unsupported screen depth.");
		}

		image->unlock();
	}

	GC gc = DefaultGC(display, DefaultScreen(display));
	XPutImage(display, window, gc, SoftwareImage, 0, 0, 0, 0, destwidth, destheight);
}



//! notifies the device that it should close itself
void CengineDeviceLinux::closeDevice()
{
	close = true;
}



//! returns if window is active. if not, nothing need to be drawn
bool CengineDeviceLinux::isWindowActive()
{
	return true; // TODO
}


void CengineDeviceLinux::createKeyMap()
{
	// I don't know if this is the best method  to create
	// the lookuptable, but I'll leave it like that until
	// I find a better version.
/*
   	KeyMap.push_back(SKeyMap(XK_BackSpace, KEY_BACK));
   	KeyMap.push_back(SKeyMap(XK_Tab, KEY_TAB));
   	KeyMap.push_back(SKeyMap(XK_Linefeed, 0)); // ???
   	KeyMap.push_back(SKeyMap(XK_Clear, KEY_CLEAR));
   	KeyMap.push_back(SKeyMap(XK_Return, KEY_RETURN));
   	KeyMap.push_back(SKeyMap(XK_Pause, KEY_PAUSE));
   	KeyMap.push_back(SKeyMap(XK_Scroll_Lock, 0)); // ???
   	KeyMap.push_back(SKeyMap(XK_Sys_Req, 0)); // ???
   	KeyMap.push_back(SKeyMap(XK_Escape, KEY_ESCAPE));
   	KeyMap.push_back(SKeyMap(XK_Delete, KEY_DELETE));
   	KeyMap.push_back(SKeyMap(XK_Home, KEY_HOME));
   	KeyMap.push_back(SKeyMap(XK_Left, KEY_LEFT));
   	KeyMap.push_back(SKeyMap(XK_Up, KEY_UP));
   	KeyMap.push_back(SKeyMap(XK_Right, KEY_RIGHT));
   	KeyMap.push_back(SKeyMap(XK_Down, KEY_DOWN));
   	KeyMap.push_back(SKeyMap(XK_Prior, KEY_PRIOR));
   	KeyMap.push_back(SKeyMap(XK_Page_Up, KEY_PRIOR));
   	KeyMap.push_back(SKeyMap(XK_Next, KEY_NEXT));
   	KeyMap.push_back(SKeyMap(XK_Page_Down, KEY_NEXT));
   	KeyMap.push_back(SKeyMap(XK_End, KEY_END));
   	KeyMap.push_back(SKeyMap(XK_Begin, KEY_HOME));
   	KeyMap.push_back(SKeyMap(XK_KP_Space, KEY_SPACE));
   	KeyMap.push_back(SKeyMap(XK_KP_Tab, KEY_TAB));
   	KeyMap.push_back(SKeyMap(XK_KP_Enter, KEY_RETURN));
   	KeyMap.push_back(SKeyMap(XK_KP_F1, KEY_F1));
   	KeyMap.push_back(SKeyMap(XK_KP_F2, KEY_F2));
   	KeyMap.push_back(SKeyMap(XK_KP_F3, KEY_F3));
   	KeyMap.push_back(SKeyMap(XK_KP_F4, KEY_F4));
   	KeyMap.push_back(SKeyMap(XK_KP_Left, KEY_LEFT));
   	KeyMap.push_back(SKeyMap(XK_KP_Up, KEY_UP));
   	KeyMap.push_back(SKeyMap(XK_KP_Right, KEY_RIGHT));
   	KeyMap.push_back(SKeyMap(XK_KP_Down, KEY_DOWN));
   	KeyMap.push_back(SKeyMap(XK_KP_Prior, KEY_PRIOR));
   	KeyMap.push_back(SKeyMap(XK_KP_Page_Up, KEY_PRIOR));
   	KeyMap.push_back(SKeyMap(XK_KP_Next, KEY_NEXT));
   	KeyMap.push_back(SKeyMap(XK_KP_Page_Down, KEY_NEXT));
   	KeyMap.push_back(SKeyMap(XK_KP_End, KEY_END));
   	KeyMap.push_back(SKeyMap(XK_KP_Begin, KEY_HOME));
   	KeyMap.push_back(SKeyMap(XK_KP_Insert, KEY_INSERT));
   	KeyMap.push_back(SKeyMap(XK_KP_Delete, KEY_DELETE));
   	KeyMap.push_back(SKeyMap(XK_KP_Equal, 0)); // ???
   	KeyMap.push_back(SKeyMap(XK_KP_Multiply, KEY_MULTIPLY));
   	KeyMap.push_back(SKeyMap(XK_KP_Add, KEY_ADD));
   	KeyMap.push_back(SKeyMap(XK_KP_Separator, KEY_SEPARATOR));
   	KeyMap.push_back(SKeyMap(XK_KP_Subtract, KEY_SUBTRACT));
   	KeyMap.push_back(SKeyMap(XK_KP_Decimal, KEY_DECIMAL));
   	KeyMap.push_back(SKeyMap(XK_KP_Divide, KEY_DIVIDE));
   	KeyMap.push_back(SKeyMap(XK_KP_0, KEY_KEY_0));
   	KeyMap.push_back(SKeyMap(XK_KP_1, KEY_KEY_1));
   	KeyMap.push_back(SKeyMap(XK_KP_2, KEY_KEY_2));
   	KeyMap.push_back(SKeyMap(XK_KP_3, KEY_KEY_3));
   	KeyMap.push_back(SKeyMap(XK_KP_4, KEY_KEY_4));
   	KeyMap.push_back(SKeyMap(XK_KP_5, KEY_KEY_5));
   	KeyMap.push_back(SKeyMap(XK_KP_6, KEY_KEY_6));
   	KeyMap.push_back(SKeyMap(XK_KP_7, KEY_KEY_7));
   	KeyMap.push_back(SKeyMap(XK_KP_8, KEY_KEY_8));
   	KeyMap.push_back(SKeyMap(XK_KP_9, KEY_KEY_9));
   	KeyMap.push_back(SKeyMap(XK_F1, KEY_F1));
   	KeyMap.push_back(SKeyMap(XK_F2, KEY_F2));
   	KeyMap.push_back(SKeyMap(XK_F3, KEY_F3));
   	KeyMap.push_back(SKeyMap(XK_F4, KEY_F4));
   	KeyMap.push_back(SKeyMap(XK_F5, KEY_F5));
   	KeyMap.push_back(SKeyMap(XK_F6, KEY_F6));
   	KeyMap.push_back(SKeyMap(XK_F7, KEY_F7));
   	KeyMap.push_back(SKeyMap(XK_F8, KEY_F8));
   	KeyMap.push_back(SKeyMap(XK_F9, KEY_F9));
   	KeyMap.push_back(SKeyMap(XK_F10, KEY_F10));
   	KeyMap.push_back(SKeyMap(XK_F11, KEY_F11));
   	KeyMap.push_back(SKeyMap(XK_F12, KEY_F12));
   	KeyMap.push_back(SKeyMap(XK_Shift_L, KEY_LSHIFT));
   	KeyMap.push_back(SKeyMap(XK_Shift_R, KEY_RSHIFT));
   	KeyMap.push_back(SKeyMap(XK_Control_L, KEY_LCONTROL));
   	KeyMap.push_back(SKeyMap(XK_Control_R, KEY_RCONTROL));
   	KeyMap.push_back(SKeyMap(XK_space, KEY_SPACE));
   	KeyMap.push_back(SKeyMap(XK_exclam, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_quotedbl, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_numbersign, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_dollar, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_percent, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_ampersand, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_apostrophe, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_parenleft, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_asterisk, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_plus, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_comma, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_minus, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_period, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_slash, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_0, KEY_KEY_0));
   	KeyMap.push_back(SKeyMap(XK_1, KEY_KEY_1));
   	KeyMap.push_back(SKeyMap(XK_2, KEY_KEY_2));
   	KeyMap.push_back(SKeyMap(XK_3, KEY_KEY_3));
   	KeyMap.push_back(SKeyMap(XK_4, KEY_KEY_4));
   	KeyMap.push_back(SKeyMap(XK_5, KEY_KEY_5));
   	KeyMap.push_back(SKeyMap(XK_6, KEY_KEY_6));
   	KeyMap.push_back(SKeyMap(XK_7, KEY_KEY_7));
   	KeyMap.push_back(SKeyMap(XK_8, KEY_KEY_8));
   	KeyMap.push_back(SKeyMap(XK_9, KEY_KEY_9));
   	KeyMap.push_back(SKeyMap(XK_colon, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_semicolon, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_less, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_equal, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_greater, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_question, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_at, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_A, KEY_KEY_A));
   	KeyMap.push_back(SKeyMap(XK_B, KEY_KEY_B));
   	KeyMap.push_back(SKeyMap(XK_C, KEY_KEY_C));
   	KeyMap.push_back(SKeyMap(XK_D, KEY_KEY_D));
   	KeyMap.push_back(SKeyMap(XK_E, KEY_KEY_E));
   	KeyMap.push_back(SKeyMap(XK_F, KEY_KEY_F));
   	KeyMap.push_back(SKeyMap(XK_G, KEY_KEY_G));
   	KeyMap.push_back(SKeyMap(XK_H, KEY_KEY_H));
   	KeyMap.push_back(SKeyMap(XK_I, KEY_KEY_I));
   	KeyMap.push_back(SKeyMap(XK_J, KEY_KEY_J));
   	KeyMap.push_back(SKeyMap(XK_K, KEY_KEY_K));
   	KeyMap.push_back(SKeyMap(XK_L, KEY_KEY_L));
   	KeyMap.push_back(SKeyMap(XK_M, KEY_KEY_M));
   	KeyMap.push_back(SKeyMap(XK_N, KEY_KEY_N));
   	KeyMap.push_back(SKeyMap(XK_O, KEY_KEY_O));
   	KeyMap.push_back(SKeyMap(XK_P, KEY_KEY_P));
   	KeyMap.push_back(SKeyMap(XK_Q, KEY_KEY_Q));
   	KeyMap.push_back(SKeyMap(XK_R, KEY_KEY_R));
   	KeyMap.push_back(SKeyMap(XK_S, KEY_KEY_S));
   	KeyMap.push_back(SKeyMap(XK_T, KEY_KEY_T));
   	KeyMap.push_back(SKeyMap(XK_U, KEY_KEY_U));
   	KeyMap.push_back(SKeyMap(XK_V, KEY_KEY_V));
   	KeyMap.push_back(SKeyMap(XK_W, KEY_KEY_W));
   	KeyMap.push_back(SKeyMap(XK_X, KEY_KEY_X));
   	KeyMap.push_back(SKeyMap(XK_Y, KEY_KEY_Y));
   	KeyMap.push_back(SKeyMap(XK_Z, KEY_KEY_Z));
   	KeyMap.push_back(SKeyMap(XK_bracketleft, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_backslash, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_bracketright, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_asciicircum, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_underscore, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_grave, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_quoteleft, 0)); //?
   	KeyMap.push_back(SKeyMap(XK_a, KEY_KEY_A));
   	KeyMap.push_back(SKeyMap(XK_b, KEY_KEY_B));
   	KeyMap.push_back(SKeyMap(XK_c, KEY_KEY_C));
   	KeyMap.push_back(SKeyMap(XK_d, KEY_KEY_D));
   	KeyMap.push_back(SKeyMap(XK_e, KEY_KEY_E));
   	KeyMap.push_back(SKeyMap(XK_f, KEY_KEY_F));
   	KeyMap.push_back(SKeyMap(XK_g, KEY_KEY_G));
   	KeyMap.push_back(SKeyMap(XK_h, KEY_KEY_H));
   	KeyMap.push_back(SKeyMap(XK_i, KEY_KEY_I));
   	KeyMap.push_back(SKeyMap(XK_j, KEY_KEY_J));
   	KeyMap.push_back(SKeyMap(XK_k, KEY_KEY_K));
   	KeyMap.push_back(SKeyMap(XK_l, KEY_KEY_L));
   	KeyMap.push_back(SKeyMap(XK_m, KEY_KEY_M));
   	KeyMap.push_back(SKeyMap(XK_n, KEY_KEY_N));
   	KeyMap.push_back(SKeyMap(XK_o, KEY_KEY_O));
   	KeyMap.push_back(SKeyMap(XK_p, KEY_KEY_P));
   	KeyMap.push_back(SKeyMap(XK_q, KEY_KEY_Q));
   	KeyMap.push_back(SKeyMap(XK_r, KEY_KEY_R));
   	KeyMap.push_back(SKeyMap(XK_s, KEY_KEY_S));
   	KeyMap.push_back(SKeyMap(XK_t, KEY_KEY_T));
   	KeyMap.push_back(SKeyMap(XK_u, KEY_KEY_U));
   	KeyMap.push_back(SKeyMap(XK_v, KEY_KEY_V));
   	KeyMap.push_back(SKeyMap(XK_w, KEY_KEY_W));
   	KeyMap.push_back(SKeyMap(XK_x, KEY_KEY_X));
   	KeyMap.push_back(SKeyMap(XK_y, KEY_KEY_Y));
   	KeyMap.push_back(SKeyMap(XK_z, KEY_KEY_Z));
 */
	KeyMap.sort();
}


#define

#if defined (_STDCALL_SUPPORTED)
#define  __stdcall  // Declare the calling convention.
#else
#define
#endif // STDCALL_SUPPORTED

engineDevice*  createDeviceEx(const SengineCreationParameters& param)
{
	CengineDeviceLinux* dev = new CengineDeviceLinux(
	        param.DriverType,
	        param.WindowSize,
	        param.Bits,
	        param.Fullscreen,
	        param.Stencilbuffer,
	        param.EventReceiver,
	        param.SDK_version_do_not_use);

	if (dev && !dev->getVideoDriver() && param.DriverType != video::EDT_NULL)
	{
		dev->drop();
		dev = 0;
	}

	return dev;
}


} // end namespace

#endif // LINUX

