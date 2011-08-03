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
 
#include "CImageLoaderTGA.h"
#include <string.h>
#include "os.h"
#include "CColorConverter.h"
#include "CImage.h"

namespace engine
{
namespace video
{


//! constructor
CImageLoaderTGA::CImageLoaderTGA()
{
}



//! destructor
CImageLoaderTGA::~CImageLoaderTGA()
{
}



//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CImageLoaderTGA::isALoadableFileExtension(const c8* fileName)
{
	return strstr(fileName, ".tga") != 0;
}


//! loads a compressed tga.
c8 *CImageLoaderTGA::loadCompressedImage(engine::io::IReadFile *file, STGAHeader *header)
{
	// This was written and sent in by Jon Pry, thank you very much!
	// I only changed the formatting a little bit.

	s32 bytesPerPixel = header->PixelDepth/8;
	s32 imageSize =  header->ImageHeight * header->ImageWidth * bytesPerPixel;
	c8* data = new c8[imageSize];
	s32 currentByte = 0;

	while(currentByte < imageSize)
	{
		u8 chunkheader = 0;
		file->read(&chunkheader, sizeof(c8)); // Read The Chunk's Header

		if(chunkheader < 128) // If The Chunk Is A 'RAW' Chunk
		{
			chunkheader++; // Add 1 To The Value To Get Total Number Of Raw Pixels

			file->read(&data[currentByte], bytesPerPixel * chunkheader);
			currentByte += bytesPerPixel * chunkheader;
		}
		else
		{
			// thnx to neojzs for some fixes with this code

			// If It's An RLE Header
			chunkheader -= 127; // Subtract 127 To Get Rid Of The ID Bit

			s32 dataOffset = currentByte;
			file->read(&data[dataOffset], bytesPerPixel);

			currentByte += bytesPerPixel;

			for(s32 counter = 1; counter < chunkheader; counter++)
			{
				for(s32 elementCounter=0; elementCounter < bytesPerPixel; elementCounter++)
					data[currentByte + elementCounter] = data[dataOffset + elementCounter];

				currentByte += bytesPerPixel;
			}
		}
	}

	return data;
}



//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderTGA::isALoadableFileFormat(engine::io::IReadFile* file)
{
	if (!file)
		return false;

	u8 type[3];
	file->read(&type, sizeof(u8)*3);
	return (type[2]==2); // we currently only handle tgas of type 2.
}



//! creates a surface from the file
IImage* CImageLoaderTGA::loadImage(engine::io::IReadFile* file)
{
	STGAHeader header;

	file->seek(0);
	file->read(&header, sizeof(STGAHeader));

#ifdef PPC
	header.ColorMapLength = OSReadSwapInt16(&header.ColorMapLength,0);
	header.ImageWidth = OSReadSwapInt16(&header.ImageWidth,0);
	header.ImageHeight = OSReadSwapInt16(&header.ImageHeight,0);
#endif

	// skip image identifikation field
	if (header.IdLength)
		file->seek(header.IdLength, true);

	if (header.ColorMapType != 0)
	{
		// skip color map
		file->seek((header.ColorMapEntrySize/8 * header.ColorMapLength), true);
	}

	// read image

	s32 bytesPerPixel = header.PixelDepth/8;
	s32 imageSize = header.ImageHeight * header.ImageWidth * bytesPerPixel;
	c8* data = 0;

	if (header.ImageType == 2)
	{
		data = new c8[imageSize];
		file->read(data, imageSize);
	}
	else
	if(header.ImageType == 10)
		data = loadCompressedImage(file, &header);
	else
	{
		os::Printer::log("Unsupported TGA file type", file->getFileName(), ELL_ERROR);
		return 0;
	}


	IImage* image = 0;

	switch(bytesPerPixel)
	{
	case 1:
		os::Printer::log("Unsupported TGA format, 8 bit", file->getFileName(), ELL_ERROR);
		break;
	case 2:
	{
		// 16 bit image

		image = new CImage(ECF_A1R5G5B5,
		                   core::dimension2d<s32>(header.ImageWidth, header.ImageHeight));

		CColorConverter::convert16BitTo16BitFlipMengineor((s16*)data,
		                                                  (s16*)image->lock(), header.ImageWidth, header.ImageHeight, 0);

		image->unlock();
	}
		break;
	case 3:
	{
		// 24 bit image

		image = new CImage(ECF_R8G8B8,
		                   core::dimension2d<s32>(header.ImageWidth, header.ImageHeight));

		CColorConverter::convert24BitTo24BitFlipMengineorColorShuffle(
		        (c8*)data, (c8*)image->lock(), header.ImageWidth, header.ImageHeight, 0);

		image->unlock();
	}
		break;
	case 4:
	{
		// 32 bit image

		image = new CImage(ECF_A8R8G8B8,
		                   core::dimension2d<s32>(header.ImageWidth, header.ImageHeight));

		CColorConverter::convert32BitTo32BitFlipMengineor((s32*)data,
		                                                  (s32*)image->lock(), header.ImageWidth, header.ImageHeight, 0);

		image->unlock();
	}
		break;
	}

	delete [] data;

	return image;
}



//! creates a loader which is able to load tgas
IImageLoader* createImageLoaderTGA()
{
	return new CImageLoaderTGA();
}


} // end namespace video
} // end namespace engine

