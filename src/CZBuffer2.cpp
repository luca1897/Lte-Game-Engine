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
 
// Copyright (C) 2002-2006 Nikolaus Gebhardt/Alten Thomas
// This file is part of the "engine Engine".
// For conditions of distribution and use, see copyright notice in engine.h

#include "CZBuffer2.h"

namespace engine
{
namespace video
{



//! constructor
CZBuffer2::CZBuffer2(const core::dimension2d<s32>& size)
	: Buffer(0), Size(0,0), TotalSize(0), BufferEnd(0)
{

}



//! destructor
CZBuffer2::~CZBuffer2()
{

}



//! clears the zbuffer
void CZBuffer2::clear()
{

}


#include <pspdebug.h>
#define printf pspDebugScreenPrintf

//! sets the new size of the zbuffer
void CZBuffer2::setSize(const core::dimension2d<s32>& size)
{

}



//! returns the size of the zbuffer
const core::dimension2d<s32>& CZBuffer2::getSize()
{
	return Size;
}



//! locks the zbuffer
TZBufferType2* CZBuffer2::lock()
{
	return Buffer;
}



//! unlocks the zbuffer
void CZBuffer2::unlock()
{
}



//! creates a ZBuffer
IZBuffer2* createZBuffer2(const core::dimension2d<s32>& size)
{

}


} // end namespace video
} // end namespace engine


