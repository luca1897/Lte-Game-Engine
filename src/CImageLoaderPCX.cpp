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
 
#include "CImageLoaderPCX.h"
#include "SColor.h"
#include <string.h>
#include "CColorConverter.h"
#include "CImage.h"
#include "os.h"


namespace engine
{
namespace video
{


//! constructor
CImageLoaderPCX::CImageLoaderPCX()
	: PCXData(0), PaletteData(0)
{
	#ifdef _DEBUG
	setDebugName("CImageLoaderPCX");
	#endif
}



//! destructor
CImageLoaderPCX::~CImageLoaderPCX()
{
	if (PaletteData)
		delete [] PaletteData;

	if (PCXData)
		delete [] PCXData;
}



//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CImageLoaderPCX::isALoadableFileExtension(const c8* fileName)
{
	return (strstr(fileName, ".PCX") != 0) || (strstr(fileName, ".pcx") != 0);
}



//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderPCX::isALoadableFileFormat(engine::io::IReadFile* file)
{
	u8 headerID;
	file->read(&headerID, sizeof(headerID));
	return headerID == 0x0a;
}


//! creates a image from the file
IImage* CImageLoaderPCX::loadImage(engine::io::IReadFile* file)
{
	SPCXHeader header;

	file->seek(0);
	file->read(&header, sizeof(header));

	s32 pitch = 0;

	//! return if the header is wrong
	if (header.Manufacturer != 0x0a && header.Encoding != 0x01)
		return 0;

	// return if this isn't a supported type
	if( (header.BitsPerPixel < 8) || (header.BitsPerPixel > 24) )
	{
		os::Printer::log("Unsupported bits per pixel in PCX file.",
		                 file->getFileName(), engine::ELL_WARNING);
		return 0;
	}

	// read palette
	if( header.BitsPerPixel >= 8 )
	{
		s32 pos = file->getPos();
		u8 palIndicator;
		file->seek( file->getSize()-256*3-1, false );

		file->read( &palIndicator, 1 );

		if( palIndicator != 12 )
		{
			os::Printer::log("Unsupported pal indicator in PCX file.",
			                 file->getFileName(), engine::ELL_WARNING);
			return 0;
		}

		u8 *tempPalette = new u8[768];
		PaletteData = new s32[256];
		file->read( tempPalette, 768 );

		for( s32 i=0; i<256; i++ )
		{
			PaletteData[i] = (tempPalette[i*3+0] << 16) |
			                 (tempPalette[i*3+1] << 8) |
			                 (tempPalette[i*3+2] );
		}

		delete [] tempPalette;

		file->seek( pos );
	}
	else if( header.BitsPerPixel == 4 )
	{
		PaletteData = new s32[16];
		for( s32 i=0; i<256; i++ )
		{
			PaletteData[i] = (header.Palette[i*3+0] << 16) |
			                 (header.Palette[i*3+1] << 8) |
			                 (header.Palette[i*3+2]);
		}
	}

	// read image data
	s32 width, height, imagebytes;
	width = header.XMax - header.XMin + 1;
	height = header.YMax - header.YMin + 1;
	imagebytes = header.BytesPerLine * height * header.Planes * header.BitsPerPixel / 8;
	PCXData = new c8[imagebytes];

	c8 cnt, value;
	for( s32 offset = 0; offset < imagebytes; )
	{
		file->read( &cnt, 1 );
		if( !((cnt & 0xc0) == 0xc0) )
		{
			value = cnt;
			cnt = 1;
		}
		else
		{
			cnt &= 0x3f;
			file->read( &value, 1 );
		}
		while( cnt )
		{
			PCXData[offset] = value;
			cnt--;
			offset++;
		}
	}

	// create image
	video::IImage* image = 0;
	pitch = header.BytesPerLine - width * header.Planes * header.BitsPerPixel / 8;

	if (pitch < 0)
		pitch = -pitch;

	switch(header.BitsPerPixel)
	{
	case 8:
		image = new CImage(ECF_A1R5G5B5, core::dimension2d<s32>(width, height));
		CColorConverter::convert8BitTo16Bit(PCXData, (s16*)image->lock(), width, height, pitch, PaletteData);
		image->unlock();
		break;
	case 24:
		image = new CImage(ECF_R8G8B8, core::dimension2d<s32>(width, height));
		CColorConverter::convert24BitTo24BitFlipMengineorColorShuffle(PCXData, (c8*)image->lock(), width, height, pitch);
		image->unlock();
		break;
	};

	// clean up

	if( PaletteData )
		delete [] PaletteData;
	PaletteData = 0;

	if( PCXData )
		delete [] PCXData;
	PCXData = 0;

	return image;
}


//! creates a loader which is able to load tgas
IImageLoader* createImageLoaderPCX()
{
	return new CImageLoaderPCX();
}



} // end namespace video
} // end namespace engine
