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
 
#ifndef __C_LIMIT_READ_FILE_H_INCLUDED__
#define __C_LIMIT_READ_FILE_H_INCLUDED__

/*! this is a read file, which is limited to some boundaries,
   	so that it may only start from a certain file position
   	and may only read until a certain file position.
   	This can be useful, for example for reading uncompressed files
   	in an archive (zip).
   !*/

#include "IReadFile.h"
#include "engineString.h"

namespace engine
{

class CUnicodeConverter;

namespace io
{

class CLimitReadFile : public IReadFile
{
public:

CLimitReadFile(IReadFile* alreadyOpenedFile, s32 areaSize, const c8* name);

virtual ~CLimitReadFile();

//! returns how much was read
virtual s32 read(void* buffer, s32 sizeToRead);

//! changes position in file, returns true if successful
//! if relativeMovement==true, the pos is changed relative to current pos,
//! otherwise from begin of file
virtual bool seek(s32 finalPos, bool relativeMovement = false);

//! returns size of file
virtual s32 getSize();

//! returns where in the file we are.
virtual s32 getPos();

//! returns name of file
virtual const c8* getFileName();

private:

void init();

core::stringc Filename;
s32 AreaSize;
s32 AreaStart;
s32 AreaEnd;
IReadFile* File;
};

} // end namespace io
} // end namespace engine



#endif

