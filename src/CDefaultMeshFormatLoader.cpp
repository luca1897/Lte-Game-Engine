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
 
#include "CDefaultMeshFormatLoader.h"
#include "CAnimatedMeshMD2.h"
#include "CStaticMeshOBJ.h"
#include "CAnimatedMeshMS3D.h"
#include "CQ3LevelMesh.h"
#include "CAnimatedMeshB3d.h"
#include <string.h>

namespace engine
{
namespace scene
{

//! Constructor
CDefaultMeshFormatLoader::CDefaultMeshFormatLoader(io::IFileSystem* fs, video::IVideoDriver* driver)
	: FileSystem(fs), Driver(driver)
{
	if (FileSystem)
		FileSystem->grab();

	if (Driver)
		Driver->grab();
}



//! destructor
CDefaultMeshFormatLoader::~CDefaultMeshFormatLoader()
{
	if (FileSystem)
		FileSystem->drop();

	if (Driver)
		Driver->drop();
}



//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
bool CDefaultMeshFormatLoader::isALoadableFileExtension(const c8* filename)
{
	return (strstr(filename, ".md2") || strstr(filename, ".obj") ||
	        strstr(filename, ".ms3d") || strstr(filename, ".bsp") || strstr(filename, ".b3d"));
}

#include <pspdebug.h>
#define printf pspDebugScreenPrintf


//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IUnknown::drop() for more information.
IAnimatedMesh* CDefaultMeshFormatLoader::createMesh(engine::io::IReadFile* file)
{
	IAnimatedMesh* msh = 0;

	// This method loads a mesh if it cans.
	// Someday I will have to refactor this, and split the DefaultMeshFormatloader
	// into one loader for every format.

	bool success = false;

	// load quake 2 md2 model
	if (strstr(file->getFileName(), ".md2"))
	{

		file->seek(0);


		msh = new CAnimatedMeshMD2();


		success = ((CAnimatedMeshMD2*)msh)->loadFile(file);


		if (success)
			return msh;

		msh->drop();
	}

	// load maya obj
	if (strstr(file->getFileName(), ".obj"))
	{
		file->seek(0);

		msh = new CStaticMeshOBJ();
		success = ((CStaticMeshOBJ*)msh)->loadFile(file);
		if (success)
			return msh;

		msh->drop();
	}

	// load blitz basic
	if (strstr(file->getFileName(), ".b3d"))
	{
		file->seek(0);

		msh = new CAnimatedMeshB3d(Driver);
		success = ((CAnimatedMeshB3d*)msh)->loadFile(file);
		if (success)
			return msh;

		msh->drop();
	}

	// load milkshape
	if (strstr(file->getFileName(), ".ms3d"))
	{
		file->seek(0);

		msh = new CAnimatedMeshMS3D(Driver);
		success = ((CAnimatedMeshMS3D*)msh)->loadFile(file);
		if (success)
			return msh;

		msh->drop();
	}

	// load quake 3 bsp
	if (strstr(file->getFileName(), ".bsp"))
	{
		file->seek(0);

		msh = new CQ3LevelMesh(FileSystem, Driver);
		success = ((CQ3LevelMesh*)msh)->loadFile(file);

		if (success)
			return msh;

		msh->drop();
	}

	return 0;
}

} // end namespace scene
} // end namespace engine

