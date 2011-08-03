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
 
//
// This file was originally written by ZDimitor.
// I (Nikolaus Gebhardt) did some few changes to this:
// - replaced logging calls to their os:: counterparts
// - removed some logging calls
// - enabled image dropping of CImage again, because that bug has been fixed now
// - removed setTexture path and replaced it with the directory of the mesh
// - added EAMT_MY3D file type
// - fixed a memory leak when decompressing RLE data.
// - cleaned multi character constant problems with gcc
// - removed octree child scene node generation because engine is now able to draw
//   scene nodes with transparent and sold materials in them at the same time. (see changes.txt)
// Thanks a lot to ZDimitor for his work on this and that he gave me
// his permission to add it into engine.

//--------------------------------------------------------------------------------
// This tool created by ZDimitor everyone can use it as wants
//--------------------------------------------------------------------------------

#ifndef __CMY3D_MESH_FILE_LOADER_H_INCLUDED__
#define __CMY3D_MESH_FILE_LOADER_H_INCLUDED__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <IMeshLoader.h>
#include <SMesh.h>
#include <IFileSystem.h>
#include <IVideoDriver.h>
#include <engineString.h>
#include <ISceneManager.h>

#include "CMY3DStuff.h"

namespace engine
{
namespace scene
{



class CMY3DMeshFileLoader : public IMeshLoader
{
public:
CMY3DMeshFileLoader(
        io::IFileSystem* fs, video::IVideoDriver* driver, ISceneManager *scmgr);
virtual ~CMY3DMeshFileLoader();

virtual bool isALoadableFileExtension(const c8* fileName);

virtual IAnimatedMesh* createMesh(io::IReadFile* file);

//! getting access to the nodes (with transparent material), creating
//! while loading .my3d file
core::array<ISceneNode*>& getChildNodes();

private:

scene::SMesh* Mesh;

video::IVideoDriver* Driver;
io::IFileSystem* FileSystem;
ISceneManager *SceneManager;

video::SColor SceneBackgrColor;
video::SColor SceneAmbientColor;

struct SMyMaterialEntry
{       SMyMaterialEntry ()
		: Texture1FileName("null"), Texture2FileName("null"),
		Texture1(0), Texture2(0), MaterialType(video::EMT_SOLID) {
		;
	}
	SMyMaterialHeader Header;
	core::stringc Texture1FileName;
	core::stringc Texture2FileName;
	video::ITexture *Texture1;
	video::ITexture *Texture2;
	video::E_MATERIAL_TYPE MaterialType;};

struct SMyMeshBufferEntry
{
	SMyMeshBufferEntry() : MaterialIndex(-1), MeshBuffer(0) {
	}
	SMyMeshBufferEntry(s32 mi, SMeshBufferLightMap* mb)
		: MaterialIndex(mi), MeshBuffer(mb) {
		;
	}

	s32 MaterialIndex;
	SMeshBufferLightMap* MeshBuffer;
};

core::array<SMyMaterialEntry>   MaterialEntry;
core::array<SMyMeshBufferEntry> MeshBufferEntry;

SMyMaterialEntry*    getMaterialEntryByIndex     (u32 matInd);
SMeshBufferLightMap* getMeshBufferByMaterialIndex(u32 matInd);

core::array<ISceneNode*> ChildNodes;
};


} // end namespace scene
} // end namespace engine


#endif // __CMY3D_MESH_FILE_LOADER_H_INCLUDED__
