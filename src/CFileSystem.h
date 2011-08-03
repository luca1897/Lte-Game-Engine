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
 
#ifndef __C_FILE_SYSTEM_H_INCLUDED__
#define __C_FILE_SYSTEM_H_INCLUDED__

#include "IFileSystem.h"
#include "engineArray.h"

namespace engine
{
namespace io
{

class CZipReader;
const s32 FILE_SYSTEM_MAX_PATH = 1024;

/*!
   	FileSystem which uses normal files and one zipfile
 */
class CFileSystem : public IFileSystem
{
public:

//! constructor
CFileSystem();

//! destructor
virtual ~CFileSystem();

//! opens a file for read access
virtual IReadFile* createAndOpenFile(const c8* filename);

//! Opens a file for write access.
virtual IWriteFile* createAndWriteFile(const c8* filename, bool append=false);

//! adds an zip archive to the filesystem
virtual bool addZipFileArchive(const c8* filename, bool ignoreCase = true, bool ignorePaths = true);

//! Returns the string of the current working directory
virtual const c8* getWorkingDirectory();

//! Changes the current Working Directory to the string given.
//! The string is operating system dependent. Under Windows it will look
//! like this: "drive:\directory\sudirectory\"
virtual bool changeWorkingDirectoryTo(const c8* newDirectory);

//! Creates a list of files and directories in the current working directory
//! and returns it.
virtual IFileList* createFileList();

//! determinates if a file exists and would be able to be opened.
virtual bool existFile(const c8* filename);

virtual bool makeDirectory(const c8* newDirectory);

//! Creates a XML Reader from a file.
virtual IXMLReader* createXMLReader(const c8* filename);

//! Creates a XML Reader from a file.
virtual IXMLReader* createXMLReader(IReadFile* file);

//! Creates a XML Reader from a file.
virtual IXMLReaderUTF8* createXMLReaderUTF8(const c8* filename);

//! Creates a XML Reader from a file.
virtual IXMLReaderUTF8* createXMLReaderUTF8(IReadFile* file);

//! Creates a XML Writer from a file.
virtual IXMLWriter* createXMLWriter(const c8* filename);

//! Creates a XML Writer from a file.
virtual IXMLWriter* createXMLWriter(IWriteFile* file);

private:

core::array<CZipReader*> ZipFileSystems;
c8 WorkingDirectory[FILE_SYSTEM_MAX_PATH];
};



} // end namespace engine
} // end namespace io

#endif

