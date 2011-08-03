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
 
#include "CLimitReadFile.h"
#include "engineString.h"

namespace engine
{
namespace io
{


CLimitReadFile::CLimitReadFile(IReadFile* alreadyOpenedFile, s32 areaSize, const c8* name)
	: AreaSize(areaSize), File(alreadyOpenedFile), Filename(name)
{
	#ifdef _DEBUG
	setDebugName("CLimitReadFile");
	#endif

	if (File)
		File->grab();

	init();
}


void CLimitReadFile::init()
{
	if (!File)
		return;

	AreaStart = File->getPos();
	AreaEnd = AreaStart + AreaSize;
}



CLimitReadFile::~CLimitReadFile()
{
	if (File)
		File->drop();
}



//! returns how much was read
s32 CLimitReadFile::read(void* buffer, s32 sizeToRead)
{
	s32 pos = File->getPos();

	if (pos >= AreaEnd)
		return 0;

	if (pos + sizeToRead >= AreaEnd)
		sizeToRead = AreaEnd - pos;

	return File->read(buffer, sizeToRead);
}



//! changes position in file, returns true if successful
//! if relativeMovement==true, the pos is changed relative to current pos,
//! otherwise from begin of file
bool CLimitReadFile::seek(s32 finalPos, bool relativeMovement)
{
	s32 pos = File->getPos();

	if (relativeMovement)
	{
		if (pos + finalPos > AreaEnd)
			finalPos = AreaEnd - pos;
	}
	else
	{
		finalPos += AreaStart;
		if ((s32)finalPos > AreaEnd)
			return false;
	}

	return File->seek(finalPos, relativeMovement);
}



//! returns size of file
s32 CLimitReadFile::getSize()
{
	return AreaSize;
}



//! returns where in the file we are.
s32 CLimitReadFile::getPos()
{
	return File->getPos() - AreaStart;
}



//! returns name of file
const c8* CLimitReadFile::getFileName()
{
	return Filename.c_str();
}


IReadFile* createLimitReadFile(const c8* fileName, IReadFile* alreadyOpenedFile, s32 areaSize)
{
	return new CLimitReadFile(alreadyOpenedFile, areaSize, fileName);
}


} // end namespace io
} // end namespace engine
