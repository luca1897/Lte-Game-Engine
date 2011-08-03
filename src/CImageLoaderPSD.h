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
 
#ifndef __C_IMAGE_LOADER_PSD_H_INCLUDED__
#define __C_IMAGE_LOADER_PSD_H_INCLUDED__

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

struct PsdHeader
{
	c8 signature [4];               // Always equal to 8BPS.
	u16 version;                    // Always equal to 1
	c8 reserved [6];                // Must be zero
	u16 channels;           // Number of any channels inc. alphas
	u32 height;                     // Rows Height of image in pixel
	u32 width;                      // Colums Width of image in pixel
	u16 depth;                      // Bits/channel
	u16 mode;                       // Color mode of the file (Bitmap/Grayscale..)
} PACK_STRUCT;


// Default alignment
#ifdef _MSC_VER
#       pragma pack( pop, packing )
#endif

#undef PACK_STRUCT

/*!
   	Surface Loader for psd images
 */
class CImageLoaderPSD : public IImageLoader
{
public:

//! constructor
CImageLoaderPSD();

//! destructor
virtual ~CImageLoaderPSD();

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
virtual bool isALoadableFileExtension(const c8* fileName);

//! returns true if the file maybe is able to be loaded by this class
virtual bool isALoadableFileFormat(engine::io::IReadFile* file);

//! creates a surface from the file
virtual IImage* loadImage(engine::io::IReadFile* file);

private:

bool readRawImageData(engine::io::IReadFile* file);
bool readRLEImageData(engine::io::IReadFile* file);
c8 getShiftFromChannel(c8 channelNr);

inline u32 convert2le (u32 value)
{
	value = (value >> 16) | (value << 16);
	value = ((value >> 8) & 0xFF00FF) | ((value << 8) & 0xFF00FF00);
	return value;
}

inline u16 convert2le (u16 value)
{
	value = (value >> 8) | (value << 8);
	return value;
}

// member variables

PsdHeader header;
u32* imageData;
bool error;

};


} // end namespace video
} // end namespace engine


#endif

