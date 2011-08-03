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
 
#ifndef __C_SOFTWARE_TEXTURE_H_INCLUDED__
#define __C_SOFTWARE_TEXTURE_H_INCLUDED__

#include "ITexture.h"
#include "CImage.h"

namespace engine
{
namespace video
{

/*!
   	interface for a Video Driver dependent Texture.
 */
class CSoftwareTexture : public ITexture
{
public:

//! constructor
CSoftwareTexture(IImage* surface);

//! destructor
virtual ~CSoftwareTexture();

//! lock function
virtual void* lock();

//! unlock function
virtual void unlock();

//! Returns original size of the texture.
virtual const core::dimension2d<s32>& getOriginalSize();

//! Returns (=size) of the texture.
virtual const core::dimension2d<s32>& getSize();

//! returns unoptimized surface
virtual CImage* getImage();

//! returns texture surface
virtual CImage* getTexture();

//! returns driver type of texture (=the driver, who created the texture)
virtual E_DRIVER_TYPE getDriverType();

//! returns color format of texture
virtual ECOLOR_FORMAT getColorFormat();

//! returns pitch of texture (in bytes)
virtual s32 getPitch();

//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
virtual void regenerateMipMapLevels();

private:

//! returns the size of a texture which would be the optimize size for rendering it
inline s32 getTextureSizeFromSurfaceSize(s32 size);

CImage* Image;
CImage* Texture;
core::dimension2d<s32> OrigSize;

};


} // end namespace video
} // end namespace engine

#endif

