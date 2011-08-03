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
 
#ifndef __C_IMAGE_LOADER_PCX_H_INCLUDED__
#define __C_IMAGE_LOADER_PCX_H_INCLUDED__

#include "IImageLoader.h"

namespace engine
{
namespace video
{


// byte-align structures
#ifdef _MSC_VER
#       pragma pack( push, packing )
#       pragma pack( 1 )
#       define PACK_STRUCT
#elif defined ( __GNUC__ )
#       define PACK_STRUCT      __attribute__((packed))
#else
#       error compiler not supported
#endif

struct SPCXHeader
{
	u8 Manufacturer;
	u8 Version;
	u8 Encoding;
	u8 BitsPerPixel;
	u16 XMin;
	u16 YMin;
	u16 XMax;
	u16 YMax;
	u16 HorizDPI;
	u16 VertDPI;
	u8 Palette[48];
	u8 Reserved;
	u8 Planes;
	u16 BytesPerLine;
	u16 PaletteType;
	u16 HScrSize;
	u16 VScrSize;
	u8 Filler[54];
} PACK_STRUCT;


// Default alignment
#ifdef _MSC_VER
#       pragma pack( pop, packing )
#endif

#undef PACK_STRUCT


/*!
   	Image Loader for Windows PCX bitmaps.
   	This loader was written and sent in by Dean P. Macri. I modified
   	only some small bits of it.
 */
class CImageLoaderPCX : public IImageLoader
{
public:

//! constructor
CImageLoaderPCX();

//! destructor
virtual ~CImageLoaderPCX();

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
virtual bool isALoadableFileExtension(const c8* fileName);

//! returns true if the file maybe is able to be loaded by this class
virtual bool isALoadableFileFormat(engine::io::IReadFile* file);

//! creates a surface from the file
virtual IImage* loadImage(engine::io::IReadFile* file);

private:

c8* PCXData;
s32* PaletteData;
};


} // end namespace video
} // end namespace engine


#endif

