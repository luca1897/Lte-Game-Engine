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
 
#ifndef __C_DEFAULT_MESH_FORMAT_LOADER_H_INCLUDED__
#define __C_DEFAULT_MESH_FORMAT_LOADER_H_INCLUDED__

#include "IMeshLoader.h"
#include "IFileSystem.h"
#include "IVideoDriver.h"

namespace engine
{
namespace scene
{

//! Meshloader capable of loading all engine default build in formats.
/** Which are: Quake 3 Bsp level, Quake 2 MD2 model, Maya .obj mesh,
   Milkshape .ms3d model. */
class CDefaultMeshFormatLoader : public IMeshLoader
{
public:

//! Constructor
CDefaultMeshFormatLoader(io::IFileSystem* fs, video::IVideoDriver* driver);

//! destructor
virtual ~CDefaultMeshFormatLoader();

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
virtual bool isALoadableFileExtension(const c8* fileName);

//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IUnknown::drop() for more information.
virtual IAnimatedMesh* createMesh(engine::io::IReadFile* file);

private:

io::IFileSystem* FileSystem;
video::IVideoDriver* Driver;
};

} // end namespace scene
} // end namespace engine

#endif

