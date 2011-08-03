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
 
#ifndef __C_Z_BUFFER_H_INCLUDED__
#define __C_Z_BUFFER_H_INCLUDED__

#include "IZBuffer.h"

namespace engine
{
namespace video
{

class CZBuffer : public IZBuffer
{
public:

//! constructor
CZBuffer(const core::dimension2d<s32>& size);

//! destructor
virtual ~CZBuffer();

//! clears the zbuffer
virtual void clear();

//! sets the new size of the zbuffer
virtual void setSize(const core::dimension2d<s32>& size);

//! returns the size of the zbuffer
virtual const core::dimension2d<s32>& getSize();

//! locks the zbuffer
virtual TZBufferType* lock();

//! unlocks the zbuffer
virtual void unlock();

private:

TZBufferType* Buffer;
TZBufferType* BufferEnd;
core::dimension2d<s32> Size;
s32 TotalSize;
};

} // end namespace video
} // end namespace engine

#endif

