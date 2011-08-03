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
 
#ifndef __C_engine_DEVICE_LINUX_H_INCLUDED__
#define __C_engine_DEVICE_LINUX_H_INCLUDED__

#include "engineCompileConfig.h"

#ifdef LINUX

#include "CengineDeviceStub.h"
#include "engineDevice.h"
#include "IImagePresenter.h"
#include "IGUIEnvironment.h"
#include "ICursorControl.h"
/*
 #include <GL/glx.h>
 #include <GL/gl.h>
 #include <X11/Xlib.h>
 #include <X11/extensions/xf86vmode.h>
 #include <X11/keysym.h>
 */

namespace engine
{

class CengineDeviceLinux : public CengineDeviceStub, video::IImagePresenter
{
public:

//! constructor
CengineDeviceLinux(video::E_DRIVER_TYPE deviceType,
                   const core::dimension2d<s32>& windowSize, u32 bits,
                   bool fullscreen, bool stencilbuffer, IEventReceiver* receiver,
                   const char* version);

//! destructor
virtual ~CengineDeviceLinux();

//! runs the device. Returns false if device wants to be deleted
virtual bool run();

//! sets the caption of the window
virtual void setWindowCaption(const wchar_t* text);

//! returns if window is active. if not, nothing need to be drawn
virtual bool isWindowActive();

//! presents a surface in the client area
virtual void present(video::IImage* surface, s32 windowId = 0, core::rect<s32>* src=0 );

//! notifies the device that it should close itself
virtual void closeDevice();

private:

//! create the driver
void createDriver(video::E_DRIVER_TYPE driverType,
                  const core::dimension2d<s32>& windowSize, u32 bits, bool fullscreen,
                  bool stencilbuffer);

bool createWindow(const core::dimension2d<s32>& windowSize,
                  u32 bits, bool fullscreen, bool stencilBuffer);

void createKeyMap();

//! Implementation of the linux cursor control
class CCursorControl : public gui::ICursorControl
{
public:

CCursorControl(CengineDeviceLinux* dev, bool null)
	: Device(dev), IsVisible(true), Null(null)
{
	if (!null)
	{
		GC gc;
		XGCValues values;
		unsigned long valuemask = 0;

		XColor fg, bg;
		int depth = 1;

		// this code, for making the cursor invisible was sent in by
		// Sirshane, thank your very much!

		Colormap screen_colormap;

		invisBitmap = XCreatePixmap( Device->display, Device->window, 32, 32, depth );
		maskBitmap = XCreatePixmap( Device->display, Device->window, 32, 32, depth );

		screen_colormap = DefaultColormap( Device->display, DefaultScreen( Device->display ) );
		XAllocNamedColor( Device->display, screen_colormap, "black", &fg, &fg );
		XAllocNamedColor( Device->display, screen_colormap, "white", &bg, &bg );

		gc = XCreateGC( Device->display, invisBitmap, valuemask, &values );

		XSetForeground( Device->display, gc, BlackPixel( Device->display, DefaultScreen( Device->display ) ) );
		XFillRectangle( Device->display, invisBitmap, gc, 0, 0, 32, 32 );
		XFillRectangle( Device->display, maskBitmap, gc, 0, 0, 32, 32 );

		invisCursor = XCreatePixmapCursor( Device->display, invisBitmap, maskBitmap, &fg, &bg, 1, 1 );
	}
}

//! Changes the visible state of the mouse cursor.
virtual void setVisible(bool visible)
{
	IsVisible = visible;
	if (!Null)
	{
		if ( !IsVisible )
			XDefineCursor( Device->display, Device->window, invisCursor );
		else
			XUndefineCursor( Device->display, Device->window );
	}
}

//! Returns if the cursor is currently visible.
virtual bool isVisible()
{
	return IsVisible;
}

//! Sets the new position of the cursor.
virtual void setPosition(const core::position2d<f32> &pos)
{
	setPosition(pos.X, pos.Y);
}

//! Sets the new position of the cursor.
virtual void setPosition(f32 x, f32 y)
{
	setPosition((s32)(x*Device->width), (s32)(y*Device->height));
}

//! Sets the new position of the cursor.
virtual void setPosition(const core::position2d<s32> &pos)
{
	setPosition(pos.X, pos.Y);
}

//! Sets the new position of the cursor.
virtual void setPosition(s32 x, s32 y)
{
	if (!Null)
	{
		XWarpPointer(Device->display,
		             None,
		             Device->window, 0, 0,
		             Device->width,
		             Device->height, x, y);
		XFlush(Device->display);
	}
}

//! Returns the current position of the mouse cursor.
virtual core::position2d<s32> getPosition()
{
	updateCursorPos();
	return CursorPos;
}

//! Returns the current position of the mouse cursor.
virtual core::position2d<f32> getRelativePosition()
{
	updateCursorPos();
	return core::position2d<f32>(CursorPos.X / (f32)Device->width,
	                             CursorPos.Y / (f32)Device->height);
}

private:

void updateCursorPos()
{
	if (Null)
		return;

	Window tmp;
	int itmp1, itmp2;
	unsigned int maskreturn;
	XQueryPointer(Device->display, Device->window,
	              &tmp, &tmp,
	              &itmp1, &itmp2,
	              &CursorPos.X, &CursorPos.Y, &maskreturn);

	if (CursorPos.X < 0)
		CursorPos.X = 0;
	if (CursorPos.X > Device->width)
		CursorPos.X = Device->width;
	if (CursorPos.Y < 0)
		CursorPos.Y = 0;
	if (CursorPos.Y > Device->height)
		CursorPos.Y = Device->height;
}

core::position2d<s32> CursorPos;
bool IsVisible;
CengineDeviceLinux* Device;
Cursor invisCursor;
Pixmap invisBitmap;
Pixmap maskBitmap;
bool Null;
};

friend class CCursorControl;

Display *display;
GLXContext context;
int screennr;
Window window;
XSetWindowAttributes attributes;
bool fullscreen;
bool doublebuffer;
bool stencilbuffer;
XF86VidModeModeInfo oldVideoMode;
XEvent event;
XImage* SoftwareImage;
video::E_DRIVER_TYPE DriverType;

int x,y;
unsigned int width, height, depth;
bool close;

struct SKeyMap
{
	SKeyMap() {
	}
	SKeyMap(s32 x11, s32 win32)
		: X11Key(x11), Win32Key(win32)
	{
	}

	s32 X11Key;
	s32 Win32Key;

	bool operator<(const SKeyMap& o) const
	{
		return X11Key<o.X11Key;
	}
};

core::array<SKeyMap> KeyMap;
};


} // end namespace engine

#endif // LINUX
#endif // __C_engine_DEVICE_LINUX_H_INCLUDED__

