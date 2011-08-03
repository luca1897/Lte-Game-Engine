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
 
#ifndef __I_Z_BUFFER_H_INCLUDED__
#define __I_Z_BUFFER_H_INCLUDED__

#include "IUnknown.h"
#include "dimension2d.h"
#include "S2DVertex.h"

namespace engine
{
namespace video
{
class IZBuffer : public IUnknown
{
public:

//! destructor
virtual ~IZBuffer() {
};

//! clears the zbuffer
virtual void clear() = 0;

//! sets the new size of the zbuffer
virtual void setSize(const core::dimension2d<s32>& size) = 0;

//! returns the size of the zbuffer
virtual const core::dimension2d<s32>& getSize() = 0;

//! locks the zbuffer
virtual TZBufferType* lock() = 0;

//! unlocks the zbuffer
virtual void unlock() = 0;
};


//! creates a ZBuffer
IZBuffer* createZBuffer(const core::dimension2d<s32>& size);

} // end namespace video
} // end namespace engine

#endif

