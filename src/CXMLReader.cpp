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
 
#include "CXMLReaderImpl.h"
#include "CXMLReader.h"
#include "IReadFile.h"

namespace engine
{
namespace io
{
//! engine implementation of the file read callback for the xml parser
class CengineXMLFileReadCallBack : public IFileReadCallBack
{
public:

//! construct from FILE pointer
CengineXMLFileReadCallBack(IReadFile* file)
	: ReadFile(file)
{
	ReadFile->grab();
}

//! destructor
virtual ~CengineXMLFileReadCallBack()
{
	ReadFile->drop();
}

//! Reads an amount of bytes from the file.
virtual int read(void* buffer, int sizeToRead)
{
	return ReadFile->read(buffer, sizeToRead);
}

//! Returns size of file in bytes
virtual int getSize()
{
	return ReadFile->getSize();
}

private:

IReadFile* ReadFile;
};         // end class CMyXMLFileReadCallBack


// now create an implementation for IXMLReader using engineXML.

//! Creates an instance of a wide character xml parser.
IXMLReader* createIXMLReader(IReadFile* file)
{
	if (!file)
		return 0;

	return new CXMLReaderImpl<wchar_t, IUnknown>(new CengineXMLFileReadCallBack(file));
}

//! Creates an instance of an UFT-8 or ASCII character xml parser.
IXMLReaderUTF8* createIXMLReaderUTF8(IReadFile* file)
{
	if (!file)
		return 0;

	return new CXMLReaderImpl<char, IUnknown>(new CengineXMLFileReadCallBack(file));
}

} // end namespace
} // end namespace
