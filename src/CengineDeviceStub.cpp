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
 
#include "CengineDeviceStub.h"
#include "IEventReceiver.h"
#include "engineList.h"
#include "os.h"
#include "engineCompileConfig.h"
#include "CTimer.h"
#include "CLogger.h"
#include "engine.h"
#include "engineString.h"
#include <string.h>

namespace engine
{

//! constructor
CengineDeviceStub::CengineDeviceStub(const char* version, engine::IEventReceiver* resv)
	: Logger(0), Operator(0), VideoDriver(0)
{
	UserReceiver = resv;

	Logger = new CLogger(UserReceiver);
	os::Printer::Logger = Logger;

	core::stringw s = L"engine Engine version ";
	s.append(getVersion());
	os::Printer::log(s.c_str(), ELL_NONE);

	checkVersion(version);

	// create timer
	Timer = new engine::CTimer();

	// create filesystem
	FileSystem = io::createFileSystem();
}


CengineDeviceStub::~CengineDeviceStub()
{
	FileSystem->drop();

	if (GUIEnvironment)
		GUIEnvironment->drop();

	if (VideoDriver)
		VideoDriver->drop();

	if (SceneManager)
		SceneManager->drop();

	if (CursorControl)
		CursorControl->drop();

	if (Operator)
		Operator->drop();

	if (AudioDriver)
		AudioDriver->drop();

	if (NetworkDriver)
		NetworkDriver->drop();

	CursorControl = 0;

	Timer->drop();

	Logger->drop();
}


void CengineDeviceStub::createGUIAndScene()
{
	// create gui environment
	GUIEnvironment = gui::createGUIEnvironment(FileSystem, VideoDriver, Operator);

	// create Scene manager
	SceneManager = scene::createSceneManager(VideoDriver, FileSystem, CursorControl);

	setEventReceiver(UserReceiver);
}


//! returns the video driver
video::IVideoDriver* CengineDeviceStub::getVideoDriver()
{
	return VideoDriver;
}

audio::IAudioDriver* CengineDeviceStub::getAudioDriver()
{
	return AudioDriver;
}

net::INetworkDriver* CengineDeviceStub::getNetworkDriver()
{
	return NetworkDriver;
}


//! return file system
io::IFileSystem* CengineDeviceStub::getFileSystem()
{
	return FileSystem;
}



//! returns the gui environment
gui::IGUIEnvironment* CengineDeviceStub::getGUIEnvironment()
{
	return GUIEnvironment;
}



//! returns the scene manager
scene::ISceneManager* CengineDeviceStub::getSceneManager()
{
	return SceneManager;
}


//! \return Returns a pointer to the ITimer object. With it the
//! current Time can be received.
ITimer* CengineDeviceStub::getTimer()
{
	return Timer;
}


//! Returns the version of the engine.
const char* CengineDeviceStub::getVersion()
{
	return engine_SDK_VERSION;
}

//! \return Returns a pointer to the mouse cursor control interface.
gui::ICursorControl* CengineDeviceStub::getCursorControl()
{
	return CursorControl;
}


//! \return Returns a pointer to a list with all video modes supported
//! by the gfx adapter.
video::IVideoModeList* CengineDeviceStub::getVideoModeList()
{
	return &VideoModeList;
}


//! checks version of sdk and prints warning if there might be a problem
bool CengineDeviceStub::checkVersion(const char* version)
{
	if (strcmp(getVersion(), version))
	{
		core::stringw w;
		w = L"Warning: The library version of the engine Engine (";
		w += getVersion();
		w += L") does not match the version the application was compiled with (";
		w += version;
		w += L"). This may cause problems.";
		os::Printer::log(w.c_str(), ELL_WARNING);
		return false;
	}

	return true;
}


//! send the event to the right receiver
void CengineDeviceStub::postEventFromUser(SEvent event)
{
	bool absorbed = false;

	if (UserReceiver)
		absorbed = UserReceiver->OnEvent(event);

	if (!absorbed && GUIEnvironment)
		absorbed = GUIEnvironment->postEventFromUser(event);

	if (!absorbed && SceneManager)
		absorbed = SceneManager->postEventFromUser(event);
}


//! Sets a new event receiver to receive events
void CengineDeviceStub::setEventReceiver(IEventReceiver* receiver)
{
	UserReceiver = receiver;
	Logger->setReceiver(receiver);
	if (GUIEnvironment)
		GUIEnvironment->setUserEventReceiver(receiver);
}


//! Returns poinhter to the current event receiver. Returns 0 if there is none.
IEventReceiver* CengineDeviceStub::getEventReceiver()
{
	return UserReceiver;
}


//! \return Returns a pointer to the logger.
ILogger* CengineDeviceStub::getLogger()
{
	return Logger;
}


//! Returns the operation system opertator object.
IOSOperator* CengineDeviceStub::getOSOperator()
{
	return Operator;
}


//! Sets if the window should be resizeable in windowed mode.
void CengineDeviceStub::setResizeAble(bool resize)
{

}


} // end namespace engine
