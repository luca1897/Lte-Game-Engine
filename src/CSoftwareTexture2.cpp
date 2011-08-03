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

#include "SoftwareDriver2_compile_config.h"
#include "SoftwareDriver2_helper.h"
#include "CSoftwareTexture2.h"
#include "os.h"

namespace engine
{
namespace video
{

//! constructor
CSoftwareTexture2::CSoftwareTexture2(IImage* image)
	: Texture(0)
{
	#ifdef _DEBUG
	setDebugName("CSoftwareTexture2");
	#endif

	if (image)
	{
		core::dimension2d<s32> optSize;
		core::dimension2d<s32> origSize = image->getDimension();
		OrigSize = origSize;

		optSize.Width = getTextureSizeFromSurfaceSize(origSize.Width);
		optSize.Height = getTextureSizeFromSurfaceSize(origSize.Height);


		Image = new CImage(ECF_SOFTWARE2, image);

		if (optSize == origSize)
		{
			Texture = Image;
			Texture->grab();
		}
		else
		{
			Texture = new CImage(ECF_SOFTWARE2, optSize);
			Image->copyToScaling(Texture);
		}
	}
}



//! destructor
CSoftwareTexture2::~CSoftwareTexture2()
{
	if (Image)
		Image->drop();

	if (Texture)
		Texture->drop();
}



//! lock function
void* CSoftwareTexture2::lock()
{
	return Image->lock();
}



//! unlock function
void CSoftwareTexture2::unlock()
{
	if (Image != Texture)
	{
		os::Printer::log("Performance warning, slow unlock of non power of 2 texture.", ELL_WARNING);
		Image->copyToScaling(Texture);
	}

	Image->unlock();
}


//! Returns original size of the texture.
const core::dimension2d<s32>& CSoftwareTexture2::getOriginalSize()
{
	return OrigSize;
}


//! Returns (=size) of the texture.
const core::dimension2d<s32>& CSoftwareTexture2::getSize()
{
	return Image->getDimension();
}


//! returns unoptimized surface
CImage* CSoftwareTexture2::getImage()
{
	return Image;
}



//! returns texture surface
CImage* CSoftwareTexture2::getTexture()
{
	return Texture;
}



//! returns the size of a texture which would be the optimize size for rendering it
inline s32 CSoftwareTexture2::getTextureSizeFromSurfaceSize(s32 size)
{
	s32 ts = 0x01;
	while(ts < size)
		ts <<= 1;

	return ts;
}



//! returns driver type of texture (=the driver, who created the texture)
E_DRIVER_TYPE CSoftwareTexture2::getDriverType()
{
	return EDT_SOFTWARE2;
}



//! returns color format of texture
ECOLOR_FORMAT CSoftwareTexture2::getColorFormat()
{
	return ECF_SOFTWARE2;
}



//! returns pitch of texture (in bytes)
s32 CSoftwareTexture2::getPitch()
{
	return Image->getPitch();
}


//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
void CSoftwareTexture2::regenerateMipMapLevels()
{
	// our software textures don't have mip maps
}



} // end namespace video
} // end namespace engine
