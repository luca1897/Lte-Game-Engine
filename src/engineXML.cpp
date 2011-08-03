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
 
// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "engine Engine" and the "engineXML" project.
// For conditions of distribution and use, see copyright notice in engine.h and/or engineXML.h

#include "engineXML.h"
#include "engineString.h"
#include "engineArray.h"
#include "fast_atof.h"
#include "CXMLReaderImpl.h"

namespace engine
{
namespace io
{

//! Implementation of the file read callback for ordinary files
class CFileReadCallBack : public IFileReadCallBack
{
public:

//! construct from filename
CFileReadCallBack(const char* filename)
	: File(0), Size(0), Close(true)
{
	// open file
	File = fopen(filename, "rb");

	if (File)
		getFileSize();
}

//! construct from FILE pointer
CFileReadCallBack(FILE* file)
	: File(file), Size(0), Close(false)
{
	if (File)
		getFileSize();
}

//! destructor
virtual ~CFileReadCallBack()
{
	if (Close && File)
		fclose(File);
}

//! Reads an amount of bytes from the file.
virtual int read(void* buffer, int sizeToRead)
{
	if (!File)
		return 0;

	return (int)fread(buffer, 1, sizeToRead, File);
}

//! Returns size of file in bytes
virtual int getSize()
{
	return Size;
}

private:

//! retrieves the file size of the open file
void getFileSize()
{
	fseek(File, 0, SEEK_END);
	Size = ftell(File);
	fseek(File, 0, SEEK_SET);
}

FILE* File;
int Size;
bool Close;

}; // end class CFileReadCallBack



// FACTORY FUNCTIONS:


//! Creates an instance of an UFT-8 or ASCII character xml parser.
engineXMLReader* createengineXMLReader(const char* filename)
{
	return new CXMLReaderImpl<char, IXMLBase>(new CFileReadCallBack(filename));
}


//! Creates an instance of an UFT-8 or ASCII character xml parser.
engineXMLReader* createengineXMLReader(FILE* file)
{
	return new CXMLReaderImpl<char, IXMLBase>(new CFileReadCallBack(file));
}


//! Creates an instance of an UFT-8 or ASCII character xml parser.
engineXMLReader* createengineXMLReader(IFileReadCallBack* callback)
{
	return new CXMLReaderImpl<char, IXMLBase>(callback, false);
}


//! Creates an instance of an UTF-16 xml parser.
engineXMLReaderUTF16* createengineXMLReaderUTF16(const char* filename)
{
	return new CXMLReaderImpl<char16, IXMLBase>(new CFileReadCallBack(filename));
}


//! Creates an instance of an UTF-16 xml parser.
engineXMLReaderUTF16* createengineXMLReaderUTF16(FILE* file)
{
	return new CXMLReaderImpl<char16, IXMLBase>(new CFileReadCallBack(file));
}


//! Creates an instance of an UTF-16 xml parser.
engineXMLReaderUTF16* createengineXMLReaderUTF16(IFileReadCallBack* callback)
{
	return new CXMLReaderImpl<char16, IXMLBase>(callback, false);
}


//! Creates an instance of an UTF-32 xml parser.
engineXMLReaderUTF32* createengineXMLReaderUTF32(const char* filename)
{
	return new CXMLReaderImpl<char32, IXMLBase>(new CFileReadCallBack(filename));
}


//! Creates an instance of an UTF-32 xml parser.
engineXMLReaderUTF32* createengineXMLReaderUTF32(FILE* file)
{
	return new CXMLReaderImpl<char32, IXMLBase>(new CFileReadCallBack(file));
}


//! Creates an instance of an UTF-32 xml parser.
engineXMLReaderUTF32* createengineXMLReaderUTF32(IFileReadCallBack* callback)
{
	return new CXMLReaderImpl<char32, IXMLBase>(callback, false);
}


} // end namespace io
} // end namespace engine
