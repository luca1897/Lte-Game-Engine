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
 
#ifndef __C_COLOR_CONVERTER_H_INCLUDED__
#define __C_COLOR_CONVERTER_H_INCLUDED__

namespace engine
{
namespace video
{

class CColorConverter
{
public:

//! converts a 4 bit palettized image into R5G5B5
static void convert4BitTo16BitFlipMengineor(const c8* in, s16* out, s32 width, s32 height, s32 pitch, const s32* palette);

//! converts a 8 bit palettized image into R5G5B5
static void convert8BitTo16BitFlipMengineor(const c8* in, s16* out, s32 width, s32 height, s32 pitch, const s32* palette);

//! converts a 8 bit palettized image into R5G5B5
static void convert8BitTo16Bit(const c8* in, s16* out, s32 width, s32 height, s32 pitch, const s32* palette);

//! converts a monochrome bitmap to A1R5G5B5 data
static void convert1BitTo16BitFlipMengineor(const c8* in, s16* out, s32 width, s32 height, s32 pitch);

//! converts R8G8B8 24 bit data to A1R5G5B5 data, and flips and
//! mengineors the image during the process.
static void convert24BitTo16BitFlipMengineor(const c8* in, s16* out, s32 width, s32 height, s32 pitch);

//! converts R8G8B8 16 bit data to A1R5G5B5 data, and flips and
//! mengineors the image during the process.
static void convert16BitTo16BitFlipMengineor(const s16* in, s16* out, s32 width, s32 height, s32 pitch);

//! copies R8G8B8 24 bit data to 24 data, and flips and
//! mengineors the image during the process.
static void convert24BitTo24BitFlipMengineorColorShuffle(const c8* in, c8* out, s32 width, s32 height, s32 pitch);

//! converts R8G8B8 24 bit data to A1R5G5B5 data (used e.g for JPG to A1R5G5B5)
//! accepts colors in different order.
static void convert24BitTo16BitColorShuffle(const c8* in, s16* out, s32 width, s32 height, s32 pitch);

//! converts R8G8B8 24 bit data to A1R5G5B5 data (used e.g for JPG to A1R5G5B5)
//! accepts colors in different order.
static void convert24BitTo16BitFlipColorShuffle(const c8* in, s16* out, s32 width, s32 height, s32 pitch);

//! converts X8R8G8B8 32 bit data to A1R5G5B5 data, and flips and
//! accepts colors in different order.
static void convert32BitTo16BitColorShuffle(const c8* in, s16* out, s32 width, s32 height, s32 pitch);

//! converts X8R8G8B8 32 bit data to A1R5G5B5 data, and flips and
//! mengineors the image during the process, accepts colors in different order.
static void convert32BitTo16BitFlipMengineorColorShuffle(const c8* in, s16* out, s32 width, s32 height, s32 pitch);

//! copies X8R8G8B8 32 bit data, and flips and
//! mengineors the image during the process.
static void convert32BitTo32BitFlipMengineor(const s32* in, s32* out, s32 width, s32 height, s32 pitch);

//! Resizes the surface to a new size and converts it at the same time
//! to an A8R8G8B8 format, returning the pointer to the new buffer.
//! The returned pointer has to be deleted.
static void convert16bitToA8R8G8B8andResize(const s16* in, s32* out, s32 newWidth, s32 newHeight, s32 currentWidth, s32 currentHeight);
};


} // end namespace video
} // end namespace engine

#endif

