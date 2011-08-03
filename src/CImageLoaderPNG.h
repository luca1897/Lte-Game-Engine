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
 
// this file was created by rt (www.tomkorp.com), based on ttk's png-reader
// i wanted to be able to read in PNG images with engine :)
// why?  lossless compression with 8-bit alpha channel!

#ifndef __C_IMAGE_LOADER_PNG_H_INCLUDED__
#define __C_IMAGE_LOADER_PNG_H_INCLUDED__

#include "IImageLoader.h"

namespace engine
{
namespace video
{

//!  Surface Loader fow PNG files
class CImageLoaderPng : public IImageLoader
{
public:

//! constructor
CImageLoaderPng();

//! destructor
virtual ~CImageLoaderPng();

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".png")
virtual bool isALoadableFileExtension(const c8* fileName);

//! returns true if the file maybe is able to be loaded by this class
virtual bool isALoadableFileFormat(engine::io::IReadFile* file);

//! creates a surface from the file
virtual IImage* loadImage(engine::io::IReadFile* file);

private:

//helper function.  reads in a row of pixels from the image
const unsigned char* ReadRow(void *row_ptr);

//some variables
unsigned int width;
unsigned int height;
int bitdepth;
int colortype;
int interlace;
int compression;
int filter;

// semi global buffer for reading in row data
unsigned char g_png_load_buffer[0x1000];           // 32768
};


} // end namespace video
} // end namespace engine

#endif
