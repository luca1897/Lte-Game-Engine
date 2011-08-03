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
 
#include "CSoftwareTexture.h"
#include "os.h"

namespace engine
{
namespace video
{

//! constructor
CSoftwareTexture::CSoftwareTexture(IImage* image)
	: Texture(0)
{
	#ifdef _DEBUG
	setDebugName("CSoftwareTexture");
	#endif

	if (image)
	{
		core::dimension2d<s32> optSize;
		core::dimension2d<s32> origSize = image->getDimension();
		OrigSize = origSize;

		optSize.Width = getTextureSizeFromSurfaceSize(origSize.Width);
		optSize.Height = getTextureSizeFromSurfaceSize(origSize.Height);

		Image = new CImage(ECF_A1R5G5B5, image);

		if (optSize == origSize)
		{
			Texture = Image;
			Texture->grab();
		}
		else
		{
			Texture = new CImage(ECF_A1R5G5B5, optSize);
			Image->copyToScaling(Texture);
		}
	}
}



//! destructor
CSoftwareTexture::~CSoftwareTexture()
{
	if (Image)
		Image->drop();

	if (Texture)
		Texture->drop();
}



//! lock function
void* CSoftwareTexture::lock()
{
	return Image->lock();
}



//! unlock function
void CSoftwareTexture::unlock()
{
	if (Image != Texture)
	{
		os::Printer::log("Performance warning, slow unlock of non power of 2 texture.", ELL_WARNING);
		Image->copyToScaling(Texture);
	}

	Image->unlock();
}


//! Returns original size of the texture.
const core::dimension2d<s32>& CSoftwareTexture::getOriginalSize()
{
	return OrigSize;
}


//! Returns (=size) of the texture.
const core::dimension2d<s32>& CSoftwareTexture::getSize()
{
	return Image->getDimension();
}


//! returns unoptimized surface
CImage* CSoftwareTexture::getImage()
{
	return Image;
}



//! returns texture surface
CImage* CSoftwareTexture::getTexture()
{
	return Texture;
}



//! returns the size of a texture which would be the optimize size for rendering it
inline s32 CSoftwareTexture::getTextureSizeFromSurfaceSize(s32 size)
{
	s32 ts = 0x01;
	while(ts < size)
		ts <<= 1;

	return ts;
}



//! returns driver type of texture (=the driver, who created the texture)
E_DRIVER_TYPE CSoftwareTexture::getDriverType()
{
	return EDT_SOFTWARE;
}



//! returns color format of texture
ECOLOR_FORMAT CSoftwareTexture::getColorFormat()
{
	return ECF_A1R5G5B5;
}



//! returns pitch of texture (in bytes)
s32 CSoftwareTexture::getPitch()
{
	return Image->getDimension().Width * 2;
}


//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
void CSoftwareTexture::regenerateMipMapLevels()
{
	// our software textures don't have mip maps
}



} // end namespace video
} // end namespace engine
