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
 
#include "CMemoryReadFile.h"
#include <string.h>

namespace engine
{
namespace io
{


CMemoryReadFile::CMemoryReadFile(void* memory, s32 len, const c8* fileName, bool d)
	: Buffer(memory), Len(len), Pos(0), deleteMemoryWhenDropped(d)
{
	#ifdef _DEBUG
	setDebugName("CReadFile");
	#endif

	Filename = fileName;
}



CMemoryReadFile::~CMemoryReadFile()
{
	if (deleteMemoryWhenDropped)
		delete [] (c8*)Buffer;
}



//! returns how much was read
s32 CMemoryReadFile::read(void* buffer, s32 sizeToRead)
{
	s32 amount = sizeToRead;
	if (Pos + amount > Len)
		amount -= Pos + amount - Len;

	if (amount < 0)
		amount = 0;

	c8* p = (c8*)Buffer;
	memcpy(buffer, p + Pos, amount);

	Pos += static_cast<u32> ( amount );

	return amount;
}



//! changes position in file, returns true if successful
//! if relativeMovement==true, the pos is changed relative to current pos,
//! otherwise from begin of file
bool CMemoryReadFile::seek(s32 finalPos, bool relativeMovement)
{
	if (relativeMovement)
	{
		if (Pos + finalPos > Len)
			return false;

		Pos += finalPos;
	}
	else
	{
		if ( (unsigned) finalPos > Len)
			return false;

		Pos = finalPos;
	}

	return true;
}



//! returns size of file
s32 CMemoryReadFile::getSize()
{
	return Len;
}



//! returns where in the file we are.
s32 CMemoryReadFile::getPos()
{
	return Pos;
}



//! returns name of file
const c8* CMemoryReadFile::getFileName()
{
	return Filename.c_str();
}



IReadFile* createMemoryReadFile(void* memory, s32 size, const c8* fileName, bool deleteMemoryWhenDropped)
{
	CMemoryReadFile* file = new CMemoryReadFile(memory, size, fileName, deleteMemoryWhenDropped);
	return file;
}


} // end namespace io
} // end namespace engine

