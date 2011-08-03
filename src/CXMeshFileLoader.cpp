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
 
#include "CXMeshFileLoader.h"
#include <string.h>
#include "os.h"
#include "SMeshBuffer.h"
#include "SAnimatedMesh.h"
#include "CXFileReader.h"
#include "CXAnimationPlayer.h"
#include "IMeshManipulator.h"

namespace engine
{
namespace scene
{

//! Constructor
CXMeshFileLoader::CXMeshFileLoader(IMeshManipulator* manip,
                                   video::IVideoDriver* driver)
	: Manipulator(manip), Driver(driver)
{
	if (Manipulator)
		Manipulator->grab();

	if (Driver)
		Driver->grab();
}



//! destructor
CXMeshFileLoader::~CXMeshFileLoader()
{
	if (Manipulator)
		Manipulator->drop();

	if (Driver)
		Driver->drop();
}



//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
bool CXMeshFileLoader::isALoadableFileExtension(const c8* filename)
{
	return strstr(filename, ".x")!=0;
}



//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IUnknown::drop() for more information.
IAnimatedMesh* CXMeshFileLoader::createMesh(io::IReadFile* file)
{
	if (!file)
		return 0;



	IAnimatedMesh* mesh = 0;
	CXFileReader *xreader = new CXFileReader(file);

	if (xreader->errorHappened())
	{

		xreader->drop();
		return 0;
	}


	mesh = new CXAnimationPlayer(xreader, Driver, Manipulator,
	                             file->getFileName());

	xreader->drop();
	return mesh;
}


} // end namespace scene
} // end namespace engine

