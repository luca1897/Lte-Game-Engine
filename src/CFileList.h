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
 
#ifndef __C_FILE_LIST_H_INCLUDED__
#define __C_FILE_LIST_H_INCLUDED__

#include "IFileList.h"
#include "engineString.h"
#include "engineArray.h"

namespace engine
{
namespace io
{

/*!
   	FileSystem, which manages where files are, so that modules which
   	use the the io do not need to know where every file is located.
 */
class CFileList : public IFileList
{
public:

//! constructor
CFileList();

//! destructor
virtual ~CFileList();

//! Returns the amount of files in the filelist.
//! \return
//! Returns the amount of files and directories in the file list.
virtual s32 getFileCount();

//! Gets the name of a file in the list, based on an index.
//! \param index is the zero based index of the file which name should
//!   be returned. The index has to be smaller than the amount getFileCount() returns.
//! \return
//! Returns the file name of the file. Returns 0, if an error occured.
virtual const c8* getFileName(s32 index);

//! Gets the full name of a file in the list, path included, based on an index.
virtual const c8* getFullFileName(s32 index);

//! Returns of the file is a directory
//! \param index is the zero based index of the file which name should
//!  be returned. The index has to be smaller than the amount getFileCount() returns.
//! \return
//! Returns true, if the file is a directory, and false, if it is not.
//!  If an error occurs, the result is undefined.
virtual bool isDirectory(s32 index);

private:

struct FileEntry
{
	core::stringc Name;
	core::stringc FullName;
	s32 Size;
	bool isDirectory;
};

core::stringc Path;
core::array< FileEntry > Files;
};

} // end namespace engine
} // end namespace io


#endif

