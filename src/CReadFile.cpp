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
 
#include "CReadFile.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/unistd.h>


namespace engine
{
namespace io
{


CReadFile::CReadFile(const c8* fileName)
	: FileSize(0)
{
	#ifdef _DEBUG
	setDebugName("CReadFile");
	#endif

	Filename = fileName;
	openFile();
}



CReadFile::~CReadFile()
{
	if (File)
		sceIoClose(File);
}



//! returns if file is open
inline bool CReadFile::isOpen()
{
	return File > -1;
}



//! returns how much was read
s32 CReadFile::read(void* buffer, s32 sizeToRead)
{
	if (!isOpen())
		return 0;

	return sceIoRead(File, buffer, sizeToRead);
}



//! changes position in file, returns true if successful
//! if relativeMovement==true, the pos is changed relative to current pos,
//! otherwise from begin of file
bool CReadFile::seek(s32 finalPos, bool relativeMovement)
{
	if (!isOpen())
		return false;

	return sceIoLseek(File, finalPos, relativeMovement ? PSP_SEEK_CUR : PSP_SEEK_SET) == 0;
}



//! returns size of file
s32 CReadFile::getSize()
{
	return FileSize;
}



//! returns where in the file we are.
s32 CReadFile::getPos()
{
	if (!isOpen())
		return 0;

	return sceIoLseek(File, 0, PSP_SEEK_CUR);
}



//! opens the file
void CReadFile::openFile()
{
	if (Filename.size() == 0) // bugfix posted by rt
	{
		File = -1;
		return;
	}

	c8 *FilePath;
	c8 *RealPath;
	core::stringc file_name;


	FilePath = (c8*)Filename.c_str();

	if (FilePath[0]== '/')
	{

		file_name = "ms0:";
		file_name += FilePath;
		RealPath = (c8*)file_name.c_str();

	}
	else
	{

		if (strstr(FilePath, ":/") == 0)
		{

			c8 WorkingDirectory[1024];
			getcwd(WorkingDirectory, 1024);

			if (WorkingDirectory[ strlen(WorkingDirectory) - 1 ] != '\\' &&
			    WorkingDirectory[ strlen(WorkingDirectory) - 1 ] != '/')
			{

				WorkingDirectory[ strlen(WorkingDirectory) + 1 ] = 0;
				WorkingDirectory[ strlen(WorkingDirectory) ] = '/';

			}


			file_name = WorkingDirectory;
			file_name += FilePath;
			RealPath = (c8*)file_name.c_str();
		}
		else
		{
			RealPath = FilePath;
		}

	}


	File = sceIoOpen(RealPath, PSP_O_RDONLY, 0777);

	if (File > -1)
	{
		// get FileSize

		FileSize = sceIoLseek(File, 0, PSP_SEEK_END);
		sceIoLseek(File, 0, PSP_SEEK_SET);
	}
}



//! returns name of file
const c8* CReadFile::getFileName()
{
	return Filename.c_str();
}



IReadFile* createReadFile(const c8* fileName)
{
	CReadFile* file = new CReadFile(fileName);
	if (file->isOpen())
		return file;

	file->drop();
	return 0;
}


} // end namespace io
} // end namespace engine

