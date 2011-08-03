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
 
#ifndef __C_IMAGE_H_INCLUDED__
#define __C_IMAGE_H_INCLUDED__
#include <string.h>
#include "IImage.h"


namespace engine
{
namespace video
{

//! IImage implementation with a lot of special image operations for
//! 16 bit A1R5G5B5/32 Bit A8R8G8B8 images, which are used by the SoftwareDevice.
class CImage : public IImage
{
public:

//! constructor
CImage(ECOLOR_FORMAT format, IImage* imageToCopy);

//! constructor
//! \param useForeignMemory: If true, the image will use the data pointer
//! directly and own it from now on, which means it will also try to delete [] the
//! data when the image will be destructed. If false, the memory will by copied.
CImage(ECOLOR_FORMAT format, const core::dimension2d<s32>& size,
       void* data, bool ownForeignMemory=true);

//! constructor
CImage(ECOLOR_FORMAT format, const core::dimension2d<s32>& size);

//! constructor
CImage(IImage* imageToCopy,
       const core::position2d<s32>& pos, const core::dimension2d<s32>& size);

//! destructor
virtual ~CImage();

//! Lock function.
virtual void* lock();

//! Unlock function.
virtual void unlock();

//! Returns width and height of image data.
virtual const core::dimension2d<s32>& getDimension();

//! Returns bits per pixel.
virtual s32 getBitsPerPixel();

//! Returns bytes per pixel
virtual s32 getBytesPerPixel();

//! Returns image data size in bytes
virtual s32 getImageDataSizeInBytes();

//! Returns image data size in pixels
virtual s32 getImageDataSizeInPixels();

//! returns mask for red value of a pixel
virtual u32 getRedMask();

//! returns mask for green value of a pixel
virtual u32 getGreenMask();

//! returns mask for blue value of a pixel
virtual u32 getBlueMask();

//! returns mask for alpha value of a pixel
virtual u32 getAlphaMask();

//! returns a pixel
virtual SColor getPixel(s32 x, s32 y);

//! returns the color format
virtual ECOLOR_FORMAT getColorFormat();

//! draws a rectangle
void drawRectangle(const core::rect<s32>& rect, const SColor &color);

//! copies this surface into another
void copyTo(CImage* target, const core::position2d<s32>& pos);

//! copies this surface into another
void copyTo(CImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect=0);

//! copies this surface into another, using the alpha mask, an cliprect and a color to add with
void copyToWithAlpha(CImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const SColor &color, const core::rect<s32>* clipRect = 0);

//! copies this surface into another, scaling it to fit it.
void copyToScaling(CImage* target);

//! draws a line from to
void drawLine(const core::position2d<s32>& from, const core::position2d<s32>& to, const SColor &color);

//! fills the surface with black or white
void fill(const SColor &color);

//! returns pitch of image
virtual u32 getPitch();


private:

//! assumes format and size has been set and creates the rest
void initData();


s32 getBitsPerPixelFromFormat();

void* Data;
core::dimension2d<s32> Size;
s32 BitsPerPixel;
s32 BytesPerPixel;
u32 Pitch;
ECOLOR_FORMAT Format;

u32 RedMask;
u32 GreenMask;
u32 BlueMask;
u32 AlphaMask;
};

} // end namespace video
} // end namespace engine


#endif

