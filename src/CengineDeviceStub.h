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
 
#ifndef __C_engine_DEVICE_STUB_H_INCLUDED__
#define __C_engine_DEVICE_STUB_H_INCLUDED__

#include "engineDevice.h"
#include "IImagePresenter.h"
#include "IGUIEnvironment.h"
#include "CVideoModeList.h"
#include "IAudioDriver.h"
#include "CNetworkDriver.h"

namespace engine
{
// lots of prototypes:
class ILogger;
class CLogger;

namespace scene
{
ISceneManager* createSceneManager(video::IVideoDriver* driver,
                                  io::IFileSystem* fs, gui::ICursorControl* cc);
}

namespace io
{
IFileSystem* createFileSystem();
}

namespace gui
{
IGUIEnvironment* createGUIEnvironment(io::IFileSystem* fs,
                                      video::IVideoDriver* Driver, IOSOperator* op);
}

namespace video
{
IVideoDriver* createSoftwareDriver(const core::dimension2d<s32>& windowSize, bool fullscreen, io::IFileSystem* io, video::IImagePresenter* presenter);
IVideoDriver* createSoftwareDriver2(const core::dimension2d<s32>& windowSize, bool fullscreen, io::IFileSystem* io, video::IImagePresenter* presenter,  gui::ICursorControl* curctrl, bool showLogo);
IVideoDriver* createNullDriver(io::IFileSystem* io, const core::dimension2d<s32>& screenSize);
}



//! Stub for an engine Device implementation
class CengineDeviceStub : public engineDevice
{
public:

//! constructor
CengineDeviceStub(const char* version, engine::IEventReceiver* resv);

//! destructor
virtual ~CengineDeviceStub();

//! returns the video driver
virtual video::IVideoDriver* getVideoDriver();

//! return file system
virtual io::IFileSystem* getFileSystem();

//! returns the gui environment
virtual gui::IGUIEnvironment* getGUIEnvironment();

virtual audio::IAudioDriver* getAudioDriver();

virtual net::INetworkDriver*  getNetworkDriver();

//! returns the scene manager
virtual scene::ISceneManager* getSceneManager();

//! \return Returns a pointer to the mouse cursor control interface.
virtual gui::ICursorControl* getCursorControl();

//! \return Returns a pointer to a list with all video modes supported
//! by the gfx adapter.
virtual video::IVideoModeList* getVideoModeList();

//! \return Returns a pointer to the ITimer object. With it the
//! current Time can be received.
virtual ITimer* getTimer();

//! Returns the version of the engine.
virtual const char* getVersion();

//! send the event to the right receiver
virtual void postEventFromUser(SEvent event);

//! Sets a new event receiver to receive events
virtual void setEventReceiver(IEventReceiver* receiver);

//! Returns poinhter to the current event receiver. Returns 0 if there is none.
virtual IEventReceiver* getEventReceiver();

//! \return Returns a pointer to the logger.
virtual ILogger* getLogger();

//! Returns the operation system opertator object.
virtual IOSOperator* getOSOperator();

//! Sets if the window should be resizeable in windowed mode. The default
//! is false.
virtual void setResizeAble(bool resize=false);

protected:

void createGUIAndScene();

//! checks version of sdk and prints warning if there might be a problem
bool checkVersion(const char* version);

io::IFileSystem* FileSystem;
video::IVideoDriver* VideoDriver;
audio::IAudioDriver *AudioDriver;
net::CNetworkDriver *NetworkDriver;
gui::IGUIEnvironment* GUIEnvironment;
scene::ISceneManager* SceneManager;
ITimer* Timer;
gui::ICursorControl* CursorControl;
video::CVideoModeList VideoModeList;
IEventReceiver* UserReceiver;
CLogger* Logger;
IOSOperator* Operator;
};


} // end namespace engine

#endif

