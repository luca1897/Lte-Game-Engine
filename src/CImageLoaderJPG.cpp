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
 
#include "CImageLoaderJPG.h"
#include <string.h>
#include "CColorConverter.h"
#include "CImage.h"
#include "os.h"

namespace engine
{
namespace video
{

//! constructor
CImageLoaderJPG::CImageLoaderJPG()
{
	#ifdef _DEBUG
	setDebugName("CImageLoaderJPG");
	#endif
}



//! destructor
CImageLoaderJPG::~CImageLoaderJPG()
{
}



//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CImageLoaderJPG::isALoadableFileExtension(const c8* fileName)
{
	return strstr(fileName, ".jpg") != 0;
}


#ifdef _engine_COMPILE_WITH_LIBJPEG_
void CImageLoaderJPG::init_source (j_decompress_ptr cinfo)
{
	// DO NOTHING
}



boolean CImageLoaderJPG::fill_input_buffer (j_decompress_ptr cinfo)
{
	// DO NOTHING
	return 1;
}



void CImageLoaderJPG::skip_input_data (j_decompress_ptr cinfo, long count)
{
	jpeg_source_mgr * src = cinfo->src;
	if(count > 0)
	{
		src->bytes_in_buffer -= count;
		src->next_input_byte += count;
	}
}



void CImageLoaderJPG::resync_to_restart (j_decompress_ptr cinfo, long desired)
{
	// DO NOTHING
}



void CImageLoaderJPG::term_source (j_decompress_ptr cinfo)
{
	// DO NOTHING
}
#endif


//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderJPG::isALoadableFileFormat(engine::io::IReadFile* file)
{
    #ifndef _engine_COMPILE_WITH_LIBJPEG_

	return false;

    #else

	if (!file)
		return false;

	s32 jfif = 0;
	file->seek(6);
	file->read(&jfif, sizeof(s32));
	return (jfif == 0x4a464946 || jfif == 0x4649464a);

	#endif
}



//! creates a surface from the file
IImage* CImageLoaderJPG::loadImage(engine::io::IReadFile* file)
{
    #ifndef _engine_COMPILE_WITH_LIBJPEG_
	return 0;
    #else

	file->seek(0);
	u8* input = new u8[file->getSize()];
	file->read(input, file->getSize());

	struct jpeg_decompress_struct cinfo;

	// allocate and initialize JPEG decompression object
	struct jpeg_error_mgr jerr;

	//We have to set up the error handler first, in case the initialization
	//step fails.  (Unlikely, but it could happen if you are out of memory.)
	//This routine fills in the contents of struct jerr, and returns jerr's
	//address which we place into the link field in cinfo.

	cinfo.err = jpeg_std_error(&jerr);
	// Now we can initialize the JPEG decompression object.
	jpeg_create_decompress(&cinfo);

	// specify data source
	jpeg_source_mgr jsrc;

	// Set up data pointer
	jsrc.bytes_in_buffer = file->getSize();
	jsrc.next_input_byte = (JOCTET*)input;
	cinfo.src = &jsrc;

	jsrc.init_source = init_source;
	jsrc.fill_input_buffer = fill_input_buffer;
	jsrc.skip_input_data = skip_input_data;
	jsrc.resync_to_restart = jpeg_resync_to_restart;
	jsrc.term_source = term_source;

	// Decodes JPG input from whatever source
	// Does everything AFTER jpeg_create_decompress
	// and BEFORE jpeg_destroy_decompress
	// Caller is responsible for arranging these + setting up cinfo

	// read file parameters with jpeg_read_header()
	(void) jpeg_read_header(&cinfo, TRUE);

	// Start decompressor
	(void) jpeg_start_decompress(&cinfo);

	// Get image data
	u16 rowspan = cinfo.image_width * cinfo.num_components;
	unsigned width = cinfo.image_width;
	unsigned height = cinfo.image_height;

	bool has_alpha= false;  //(JPEG never has alpha)
	bool greyscale;

	if (cinfo.jpeg_color_space == JCS_GRAYSCALE)
		greyscale = true;
	else
		greyscale = false;

	if ( greyscale )
	{
		delete [] input;
		jpeg_destroy_decompress(&cinfo);
		os::Printer::log("Greyscale .jpg textures are not supported by engine, please just convert that file.", ELL_ERROR);
		return 0;
	}

	// Allocate memory for buffer
	u8 *output = new u8[rowspan * height];

	// Here we use the library's state variable cinfo.output_scanline as the
	// loop counter, so that we don't have to keep track ourselves.
	// Create array of row pointers for lib
	u8 **rowPtr = new u8 * [height];

	for( unsigned i = 0; i < height; i++ )
		rowPtr[i] = &output[ i * rowspan ];

	unsigned rowsRead = 0;

	while( cinfo.output_scanline < cinfo.output_height )
		rowsRead += jpeg_read_scanlines( &cinfo, &rowPtr[rowsRead], cinfo.output_height - rowsRead );

	delete [] rowPtr;
	// Finish decompression

	(void) jpeg_finish_decompress(&cinfo);

	// Release JPEG decompression object

	// This is an important step since it will release a good deal of memory.
	jpeg_destroy_decompress(&cinfo);


	// convert image
	IImage* image = new CImage(ECF_R8G8B8,
	                           core::dimension2d<s32>(width, height), output);

	delete [] input;

	return image;

	#endif
}



//! creates a loader which is able to load jpeg images
IImageLoader* createImageLoaderJPG()
{
	return new CImageLoaderJPG();
}

} // end namespace video
} // end namespace engine

