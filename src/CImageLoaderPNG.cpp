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
 
#include "engineCompileConfig.h"

#ifdef _engine_COMPILE_WITH_LIBPNG_
        #ifndef _engine_USE_NON_SYSTEM_LIB_PNG_
        #include <png.h> // use system lib png
        #else // _engine_USE_NON_SYSTEM_LIB_PNG_
        #include "libpng/png.h" // use engine included lib png
        #endif // _engine_USE_NON_SYSTEM_LIB_PNG_
#endif // _engine_COMPILE_WITH_LIBPNG_

#include "CImageLoaderPNG.h"
#include <string.h>
#include "CColorConverter.h"
#include "CImage.h"
#include "CReadFile.h"
#include "os.h"

namespace engine
{
namespace video
{

#ifdef _engine_COMPILE_WITH_LIBPNG_
// PNG function for error handling
static void png_cpexcept_error(png_structp png_ptr, png_const_charp msg)
{
	if(png_ptr)
	{
		char temp[256];
		sprintf(temp,"PNG FATAL ERROR: png error - %s",msg);
		os::Printer::log(temp, ELL_ERROR);
	}
}

// PNG function for file reading
void user_read_data_fcn(png_structp png_ptr,png_bytep data, png_size_t length)
{
	png_size_t check;

	// changed by zola {
	engine::io::IReadFile* file=(engine::io::IReadFile*)png_ptr->io_ptr;
	check=(png_size_t) file->read((void*)data,length);
	// }

	if (check != length)
		png_error(png_ptr, "Read Error");
}
#endif // _engine_COMPILE_WITH_LIBPNG_

CImageLoaderPng::CImageLoaderPng()
{
	// do something?
}

CImageLoaderPng::~CImageLoaderPng()
{
	// do something?
}

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CImageLoaderPng::isALoadableFileExtension(const c8* fileName)
{
#ifdef _engine_COMPILE_WITH_LIBPNG_
	// jox: added fix for file extension check by jox
	const c8* ext = strrchr(fileName, '.');
	if (ext == 0) return false;
	return (strstr(ext, ".PNG") != 0) || (strstr(ext, ".png") != 0);
#else
	return false;
#endif // _engine_COMPILE_WITH_LIBPNG_
}


//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderPng::isALoadableFileFormat(engine::io::IReadFile* file)
{
#ifdef _engine_COMPILE_WITH_LIBPNG_

	if (!file)
		return false;

	// Read the first few bytes of the PNG file
	if (file->read(&g_png_load_buffer, 8) != 8)
	{
		//os::Printer::log("Not a PNG file: can't read file\n", file->getFileName(), ELL_ERROR);
		return false;
	}

	// CHeck if it really is a PNG file
	if (!png_check_sig((png_bytep)g_png_load_buffer, 8))
	{
		//os::Printer::log("Not a PNG file: wrong header\n", file->getFileName(),ELL_ERROR);
		return false;
	}

	return true; //if we are here then it must be a png

#else
	return false;
#endif // _engine_COMPILE_WITH_LIBPNG_
}


// load in the image data
IImage* CImageLoaderPng::loadImage(engine::io::IReadFile* file)
{
#ifdef _engine_COMPILE_WITH_LIBPNG_

	if (!file)
		return 0;

	bool alphaSupport = true;

	png_structp png_ptr;
	png_infop info_ptr;

	// Read the first few bytes of the PNG file
	if( file->read(&g_png_load_buffer, 8) != 8 )
	{
		os::Printer::log("LOAD PNG: can't read file\n", file->getFileName(), ELL_ERROR);
		return 0;
	}

	// CHeck if it really is a PNG file
	if( !png_check_sig((png_bytep)g_png_load_buffer,8) )
	{
		os::Printer::log("LOAD PNG: not really a png\n", file->getFileName(), ELL_ERROR);
		return 0;
	}

	// Allocate the png read struct
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,(png_error_ptr)png_cpexcept_error, (png_error_ptr)0 );
	if (!png_ptr)
	{
		os::Printer::log("LOAD PNG: Internal PNG create read struct failure\n", file->getFileName(), ELL_ERROR);
		return 0;
	}

	// Allocate the png info struct
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		os::Printer::log("LOAD PNG: Internal PNG create info struct failure\n", file->getFileName(), ELL_ERROR);
		png_destroy_read_struct(&png_ptr, 0, 0);
		return 0;
	}

	//png_init_io(png_ptr,fp);   // Init png
	// changed by zola so we don't need to have public FILE pointers
	png_set_read_fn(png_ptr, file, user_read_data_fcn);

	png_set_sig_bytes(png_ptr, 8);   // Tell png that we read the signature

	png_read_info(png_ptr, info_ptr);   // Read the info section of the png file

	png_get_IHDR(png_ptr, info_ptr, (png_uint_32*)&width,
	             (png_uint_32*)&height, &bitdepth, &colortype, &interlace,
	             &compression, &filter); // Extract info

	if ( bitdepth != 8)
	{
		os::Printer::log("PNG LOAD: Failure - Only 8 bits per color supported", ELL_ERROR);
		if(png_ptr)
			png_destroy_read_struct(&png_ptr,&info_ptr, 0); // Clean up memory
		return 0;
	}

	if (colortype==PNG_COLOR_TYPE_RGBA)
		alphaSupport = true;
	else
	if (colortype==PNG_COLOR_TYPE_RGB)
		alphaSupport = false;
	else
	{
		os::Printer::log("PNG LOAD: Failure - Format not supported - must be 24 or 32 bits per pixel", ELL_ERROR);
		if(png_ptr)
			png_destroy_read_struct(&png_ptr,&info_ptr, 0); // Clean up memory
		return 0;
	}

	if ( interlace!= PNG_INTERLACE_NONE)
	{
		os::Printer::log("PNG LOAD: Failure - Format not supported - must be 24 or 32 bits per pixel", ELL_ERROR);
		if(png_ptr)
			png_destroy_read_struct(&png_ptr,&info_ptr, 0); // Clean up memory
		return 0;
	}

	// Update the changes
	png_read_update_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr,
	             (png_uint_32*)&width, (png_uint_32*)&height,
	             &bitdepth,&colortype, &interlace, &compression,
	             &filter); // Extract info

	// Check the number of bytes per row
	int bytes_per_row = png_get_rowbytes(png_ptr, info_ptr);

	if( bytes_per_row >= (int)sizeof( g_png_load_buffer ) )
	{
		os::Printer::log("PNG LOAD: Failure - Format not supported - must be 24 or 32 bits per pixel", ELL_ERROR);
		if(png_ptr)
			png_destroy_read_struct(&png_ptr,&info_ptr, 0); // Clean up memory
		return 0;
	}


	//now read the png stucture into a simple bitmap array
	video::IImage* image = 0;
	if (alphaSupport)
		image = new CImage(ECF_A8R8G8B8, core::dimension2d<s32>(width, height));
	else
		image = new CImage(ECF_R8G8B8, core::dimension2d<s32>(width, height));

	const unsigned char* pSrc;
	const unsigned char inc = alphaSupport ? 4 : 3;
	unsigned char* data = (unsigned char*)image->lock();

	for ( unsigned int y = 0; y < this->height; y++ )
	{
		//read in a row of pixels
		pSrc = this->ReadRow(png_ptr);

		for (unsigned int x = 0; x < this->width; x++ )
		{
			//loop through the row of pixels
			if (!alphaSupport)
			{
				data[y*width*inc + x*inc] = *(pSrc); //red
				data[y*width*inc + x*inc + 1] = *(pSrc+1); //green
				data[y*width*inc + x*inc + 2] = *(pSrc+2); //blue
			}
			else
			{
				data[y*width*inc + x*inc] = *(pSrc+2); //red
				data[y*width*inc + x*inc + 1] = *(pSrc+1); //green
				data[y*width*inc + x*inc + 2] = *(pSrc); //blue
				data[y*width*inc + x*inc + 3] = *(pSrc+3); //alpha
			}

			pSrc+=inc; //move to next pixel (24 or 32 bits - depends on alpha)
		}
	}

	if (png_ptr)
		png_destroy_read_struct(&png_ptr,&info_ptr, 0); // Clean up memory

	return image;

#else
	return 0;
#endif // _engine_COMPILE_WITH_LIBPNG_
}


const unsigned char* CImageLoaderPng::ReadRow(void *row_ptr)
{
#ifdef _engine_COMPILE_WITH_LIBPNG_
	png_read_row((png_structp)row_ptr, (png_bytep)g_png_load_buffer, 0);
	return (const unsigned char*)g_png_load_buffer;
#else
	return 0;
#endif // _engine_COMPILE_WITH_LIBPNG_
}

IImageLoader* createImageLoaderPNG()
{
	return new CImageLoaderPng();
}


} // end namespace engine
} //end namespace video

