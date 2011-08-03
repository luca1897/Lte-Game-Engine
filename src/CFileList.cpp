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
 
#include "CFileList.h"
#include "engineCompileConfig.h"

namespace engine
{
namespace io
{


#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <pspdebug.h>
#define printf pspDebugScreenPrintf


CFileList::CFileList()
{

	static char __curpath[255];
	struct dirent *namelist;
	struct stat buf;
	DIR *dir;

	FileEntry entry;

	// Add default parent - even when at /, this is available




	Path = getcwd(__curpath,255);

	dir = opendir(".");

	if ( dir)
	{
		while( (namelist = readdir(dir) ))
		{
			if((strcmp(namelist->d_name,    ".")==0) ||
			   (strcmp(namelist->d_name, "..")==0))
			{
				// Only	add	entries	that aren't	.. or .	since they are already handled
				continue;
			}

			entry.Name = namelist->d_name;

			if (stat(namelist->d_name, &buf)==0)
			{
				entry.Size = buf.st_size;
				entry.isDirectory =     S_ISDIR(buf.st_mode);
			}
			else
			{
				entry.Size = 0;
				entry.isDirectory =     FIO_SO_ISDIR(namelist->d_stat.st_mode);
			}

			if (entry.isDirectory)
				Files.push_front(entry);
			else
				Files.push_back(entry);
		}
		closedir(dir);
	}
	entry.Name = "..";
	entry.Size = 0;
	entry.isDirectory = true;
	Files.push_front(entry);


}


CFileList::~CFileList()
{
}


s32 CFileList::getFileCount()
{
	return Files.size();
}


const c8* CFileList::getFileName(s32 index)
{
	if (index < 0 || index > (s32)Files.size())
		return 0;

	return Files[index].Name.c_str();
}


//! Gets the full name of a file in the list, path included, based on an index.
const c8* CFileList::getFullFileName(s32 index)
{
	if (index < 0 || index > (s32)Files.size())
		return 0;

	if (Files[index].FullName.size() < Files[index].Name.size())
	{
		// create full name
		Files[index].FullName = Path;

		if (Path.size() > 3)
			Files[index].FullName.append("/");

		Files[index].FullName.append(Files[index].Name);
	}

	return Files[index].FullName.c_str();
}


bool CFileList::isDirectory(s32 index)
{
	if (index < 0 || index > (s32)Files.size())
		return false;

	_engine_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return Files[index].isDirectory;
}

} // end namespace engine
} // end namespace io

