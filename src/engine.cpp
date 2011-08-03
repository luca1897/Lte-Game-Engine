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
 
#include "engineCompileConfig.h"

#if defined (_engine_WINDOWS_) || defined (_XBOX)

#ifdef _engine_WINDOWS_
        #include <windows.h>
#endif // _engine_WINDOWS_

#ifdef _XBOX
        #include <xtl.h>
#endif // _XBOX

#ifdef _DEBUG
#include <crtdbg.h>
#endif // _DEBUG

#endif // defined(_engine_WINDOWS_) || defined(_XBOX)


#include "engine.h"

namespace engine
{
//! stub for calling createDeviceEx
engineDevice*  createDevice(IEventReceiver* res, bool showLogo)
{
	engine::SengineCreationParameters p;
	p.WindowSize = core::dimension2d<s32>(480,272);
	p.Bits = 32;
	p.Fullscreen = false;
	p.Stencilbuffer = false;
	p.showLogo = showLogo;
	p.Vsync = false;
	p.EventReceiver = res;
	p.SDK_version_do_not_use = "1.0";

	return createDeviceEx(p);
}

} // end namespace engine

