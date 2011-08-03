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

#ifndef __C_engine_DEVICE_WIN32_H_INCLUDED__
#define __C_engine_DEVICE_WIN32_H_INCLUDED__

#include "engineTypes.h"
#include "engineCompileConfig.h"
#include "CengineDeviceStub.h"
#include "engineDevice.h"
#include "IImagePresenter.h"
#include "IGUIEnvironment.h"
#include <pspctrl.h>
#include <pspdebug.h>
#define printf pspDebugScreenPrintf
#define HWND int


namespace engine
{

//! Implementation of the win32 cursor control
class CCursorControl : public gui::ICursorControl
{
public:

CCursorControl()
{
	IsVisible = true;
	speed = 0.03;
	enabled = true;
	xenabled=true;
}



//! Changes the visible state of the mouse cursor.
virtual void setVisible(bool visible)
{
	IsVisible = visible;
}

virtual void setXEnabled(bool state)
{
	xenabled= state;
}
virtual bool isXEnabled()
{
	return xenabled;
}

//! Returns if the cursor is currently visible.
virtual bool isVisible()
{
	return IsVisible;
}

virtual bool isEnabled()
{
	return enabled;
}

virtual void setEnabled(bool state)
{
	enabled = state;

}

virtual void setSpeed(float cspeed)
{
	speed = cspeed / 3333;
}
float getSpeed()
{
	return speed;
}

//! Sets the new position of the cursor.
virtual void setPosition(const core::position2d<f32> &pos)
{
	setPosition(pos.X, pos.Y);
}

//! Sets the new position of the cursor.
virtual void setPosition(f32 x, f32 y)
{
	setPosition((s32)(x*480), (s32)(y*272));
}

//! Sets the new position of the cursor.
virtual void setPosition(const core::position2d<s32> &pos)
{
	setPosition(pos.X, pos.Y);
}

//! Sets the new position of the cursor.
virtual void setPosition(s32 x, s32 y)
{
	CursorPos.X = x;
	CursorPos.Y = y;
}

//! Sets the new position of the cursor.
void setPosition2(const core::position2d<s32> &pos)
{
	setPosition2(pos.X, pos.Y);
}

//! Sets the new position of the cursor.
void setPosition2(s32 x, s32 y)
{
	CursorPos2.X = x;
	CursorPos2.Y = y;
}


//! Returns the current position of the mouse cursor.
virtual core::position2d<s32> getPosition()
{
	return CursorPos;
}


core::position2d<s32> getMousePosition()
{
	return CursorPos2;
}

//! Returns the current position of the mouse cursor.
virtual core::position2d<f32> getRelativePosition()
{
	core::position2d<f32> pos;

	pos.X  = CursorPos.X;
	pos.Y = CursorPos.Y;
	pos.X = pos.X/480;
	pos.Y = pos.Y/272;
	return pos;


}




private:

//! Updates the internal cursor position
void updateInternalCursorPosition()
{



}

core::position2d<s32> CursorPos, CursorPos2;
core::dimension2d<s32> WindowSize;
core::dimension2d<f32> InvWindowSize;
float speed;

bool IsVisible;
bool enabled;
bool xenabled;
HWND HWnd;

s32 BorderX, BorderY;
};


class CengineDeviceWin32 : public CengineDeviceStub, video::IImagePresenter
{
public:

//! constructor
CengineDeviceWin32(video::E_DRIVER_TYPE deviceType,
                   const core::dimension2d<s32> windowSize, u32 bits,
                   bool fullscreen, bool stencilbuffer, bool vsync,
                   bool antiAlias,
                   IEventReceiver* receiver,
                   const char* version, bool showLogo);

//! destructor
virtual ~CengineDeviceWin32();

//! runs the device. Returns false if device wants to be deleted
virtual bool run();



//! presents a surface in the client area
virtual void present(video::IImage* surface, s32 windowId = 0, core::rect<s32>* src=0 );

//! notifies the device that it should close itself
virtual void closeDevice();

//! \return Returns a pointer to a list with all video modes supported
//! by the gfx adapter.
video::IVideoModeList* getVideoModeList();



//! returns the win32 cursor control
CCursorControl* getWin32CursorControl();

private:
void __lte_send_po(EKEY_CODE a);


//! create the driver
void createDriver(video::E_DRIVER_TYPE driverType,
                  const core::dimension2d<s32>& windowSize, u32 bits, bool fullscreen,
                  bool stencilbuffer, bool vsync, bool antiAlias, bool showLogo);

//! switchs to fullscreen
bool switchToFullScreen(s32 width, s32 height, s32 bits);

void getWindowsVersion(core::stringc& version);

void resizeIfNecessary();

HWND HWnd;

bool ChangedToFullScreen;
bool FullScreen;
bool IsNonNTWindows;
bool Resized;
bool ExternalWindow;
CCursorControl* Win32CursorControl;
};


} // end namespace engine

#endif
