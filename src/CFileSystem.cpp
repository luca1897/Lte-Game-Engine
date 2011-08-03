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
 
#include "CFileSystem.h"
#include "IReadFile.h"
#include "IWriteFile.h"
#include "CZipReader.h"
#include "CFileList.h"
#include "CXMLReader.h"
#include "CXMLWriter.h"
#include "stdio.h"
#include "os.h"
#include "engineCompileConfig.h"
#include <pspdebug.h>
#include <sys/stat.h>
#define printf pspDebugScreenPrintf

#include <unistd.h>

namespace engine
{
namespace io
{


//! constructor
CFileSystem::CFileSystem()
{
	#ifdef _DEBUG
	setDebugName("CFileSystem");
	#endif
}



//! destructor
CFileSystem::~CFileSystem()
{
	for (u32 i=0; i<ZipFileSystems.size(); ++i)
		ZipFileSystems[i]->drop();
}



//! opens a file for read access
IReadFile* CFileSystem::createAndOpenFile(const c8* filename)
{
	IReadFile* file = 0;

	for (u32 i=0; i<ZipFileSystems.size(); ++i)
	{
		file = ZipFileSystems[i]->openFile(filename);
		if (file)
		{
			return file;
		}
	}

	file = createReadFile(filename);
	return file;
}


//! Opens a file for write access.
IWriteFile* CFileSystem::createAndWriteFile(const c8* filename, bool append)
{
	return createWriteFile(filename, append);
}



//! adds an zip archive to the filesystem
bool CFileSystem::addZipFileArchive(const c8* filename, bool ignoreCase, bool ignorePaths)
{
	IReadFile* file = createReadFile(filename);
	if (file)
	{
		CZipReader* zr = new CZipReader(file, ignoreCase, ignorePaths);
		if (zr)
			ZipFileSystems.push_back(zr);

		file->drop();

		bool ret = (zr != 0);
		_engine_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return ret;
	}

	#ifdef _DEBUG
	os::Printer::log("Could not open file. Zipfile not added", filename, ELL_ERROR);
	#endif

	_engine_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return false;
}



//! Returns the string of the current working directory
const c8* CFileSystem::getWorkingDirectory()
{

	getcwd(WorkingDirectory, (size_t)FILE_SYSTEM_MAX_PATH);
	return WorkingDirectory;

}

bool CFileSystem::makeDirectory(const c8* newDirectory)
{
	return (mkdir(newDirectory, 0777) == 0 ? true : false);
}

//! Changes the current Working Directory to the string given.
//! The string is operating system dependent. Under Windows it will look
//! like this: "drive:\directory\sudirectory\"
//! \return
//! Returns true if successful, otherwise false.
bool CFileSystem::changeWorkingDirectoryTo(const c8* newDirectory)
{
	return !(chdir(newDirectory) == 0);
	return false;
}


//! Creates a list of files and directories in the current working directory
IFileList* CFileSystem::createFileList()
{
	return new CFileList();
}


//! determinates if a file exists and would be able to be opened.
bool CFileSystem::existFile(const c8* filename)
{
	for (u32 i=0; i<ZipFileSystems.size(); ++i)
		if (ZipFileSystems[i]->findFile(filename)!=-1)
			return true;

	FILE* f = fopen(filename, "rb");
	if (f)
	{
		fclose(f);
		return true;
	}

	_engine_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return false;
}


//! Creates a XML Reader from a file.
IXMLReader* CFileSystem::createXMLReader(const c8* filename)
{
	IReadFile* file = createAndOpenFile(filename);
	if (!file)
		return 0;

	IXMLReader* reader = createXMLReader(file);
	file->drop();
	return reader;
}


//! Creates a XML Reader from a file.
IXMLReader* CFileSystem::createXMLReader(IReadFile* file)
{
	if (!file)
		return 0;

	return createIXMLReader(file);
}

//! Creates a XML Reader from a file.
IXMLReaderUTF8* CFileSystem::createXMLReaderUTF8(const c8* filename)
{
	IReadFile* file = createAndOpenFile(filename);
	if (!file)
		return 0;

	IXMLReaderUTF8* reader = createIXMLReaderUTF8(file);
	file->drop();
	return reader;
}

//! Creates a XML Reader from a file.
IXMLReaderUTF8* CFileSystem::createXMLReaderUTF8(IReadFile* file)
{
	if (!file)
		return 0;

	return createIXMLReaderUTF8(file);
}


//! Creates a XML Writer from a file.
IXMLWriter* CFileSystem::createXMLWriter(const c8* filename)
{
	IWriteFile* file = createAndWriteFile(filename);
	IXMLWriter* writer = createXMLWriter(file);
	file->drop();
	return writer;
}


//! Creates a XML Writer from a file.
IXMLWriter* CFileSystem::createXMLWriter(IWriteFile* file)
{
	return new CXMLWriter(file);
}


//! creates a filesystem which is able to open files from the ordinary file system,
//! and out of zipfiles, which are able to be added to the filesystem.
IFileSystem* createFileSystem()
{
	return new CFileSystem();
}


} // end namespace engine
} // end namespace io

