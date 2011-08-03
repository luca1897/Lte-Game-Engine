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
 
#ifndef __C_WRITE_FILE_H_INCLUDED__
#define __C_WRITE_FILE_H_INCLUDED__

#include <stdio.h>
#include "IWriteFile.h"
#include "engineString.h"
#include <pspiofilemgr.h>

namespace engine
{

namespace io
{

/*!
   	Class for writing a real file to disk.
 */
class CWriteFile : public IWriteFile
{
public:

CWriteFile(const wchar_t* fileName, bool append);
CWriteFile(const c8* fileName, bool append);

virtual ~CWriteFile();

//! Reads an amount of bytes from the file.
virtual s32 write(const void* buffer, s32 sizeToWrite);

//! Changes position in file, returns true if successful.
virtual bool seek(s32 finalPos, bool relativeMovement = false);

//! Returns the current position in the file.
virtual s32 getPos();

//! Returns name of file.
virtual const c8* getFileName();

//! returns if file is open
bool isOpen();

private:

//! opens the file
void openFile(bool append);

core::stringc Filename;
int File;
s32 FileSize;
};

} // end namespace io
} // end namespace engine

#endif

