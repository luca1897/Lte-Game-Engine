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
 
#ifndef __C_IMAGE_LOADER_BMP_H_INCLUDED__
#define __C_IMAGE_LOADER_BMP_H_INCLUDED__

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

struct SBMPHeader
{
	u16 Id;                                                 //	BM - Windows 3.1x, 95, NT, 98, 2000, ME, XP
	//	BA - OS/2 Bitmap Array
	//	CI - OS/2 Color Icon
	//	CP - OS/2 Color Pointer
	//	IC - OS/2 Icon
	//	PT - OS/2 Pointer
	u32 FileSize;
	u32 Reserved;
	u32 BitmapDataOffset;
	u32 BitmapHeaderSize;                   // should be 28h for windows bitmaps or
	// 0Ch for OS/2 1.x or F0h for OS/2 2.x
	u32 Width;
	u32 Height;
	u16 Planes;
	u16 BPP;                                                // 1: Monochrome bitmap
	// 4: 16 color bitmap
	// 8: 256 color bitmap
	// 16: 16bit (high color) bitmap
	// 24: 24bit (true color) bitmap
	// 32: 32bit (true color) bitmap

	u32 Compression;                                // 0: none (Also identified by BI_RGB)
	// 1: RLE 8-bit / pixel (Also identified by BI_RLE4)
	// 2: RLE 4-bit / pixel (Also identified by BI_RLE8)
	// 3: Bitfields  (Also identified by BI_BITFIELDS)

	u32 BitmapDataSize;                     // Size of the bitmap data in bytes. This number must be rounded to the next 4 byte boundary.
	u32 PixelPerMeterX;
	u32 PixelPerMeterY;
	u32 Colors;
	u32 ImportantColors;
} PACK_STRUCT;


// Default alignment
#ifdef _MSC_VER
#       pragma pack( pop, packing )
#endif

#undef PACK_STRUCT


/*!
   	Surface Loader fow Windows bitmaps
 */
class CImageLoaderBmp : public IImageLoader
{
public:

//! constructor
CImageLoaderBmp();

//! destructor
virtual ~CImageLoaderBmp();

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
virtual bool isALoadableFileExtension(const c8* fileName);

//! returns true if the file maybe is able to be loaded by this class
virtual bool isALoadableFileFormat(engine::io::IReadFile* file);

//! creates a surface from the file
virtual IImage* loadImage(engine::io::IReadFile* file);

private:

void decompress8BitRLE(c8*& BmpData, s32 size, s32 width, s32 height, s32 pitch);

void decompress4BitRLE(c8*& BmpData, s32 size, s32 width, s32 height, s32 pitch);

c8* BmpData;
s32* PaletteData;

};


} // end namespace video
} // end namespace engine


#endif

