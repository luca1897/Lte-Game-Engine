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
 
#include "CZBuffer.h"

namespace engine
{
namespace video
{



//! constructor
CZBuffer::CZBuffer(const core::dimension2d<s32>& size)
	: Buffer(0), Size(0,0), TotalSize(0), BufferEnd(0)
{

}



//! destructor
CZBuffer::~CZBuffer()
{

}



//! clears the zbuffer
void CZBuffer::clear()
{

}



//! sets the new size of the zbuffer
void CZBuffer::setSize(const core::dimension2d<s32>& size)
{

}



//! returns the size of the zbuffer
const core::dimension2d<s32>& CZBuffer::getSize()
{
	return Size;
}



//! locks the zbuffer
TZBufferType* CZBuffer::lock()
{
	return Buffer;
}



//! unlocks the zbuffer
void CZBuffer::unlock()
{
}



//! creates a ZBuffer
IZBuffer* createZBuffer(const core::dimension2d<s32>& size)
{

}


} // end namespace video
} // end namespace engine


