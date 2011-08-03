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
 
#ifndef __C_STATIC_MESH_OBJ_H_INCLUDED__
#define __C_STATIC_MESH_OBJ_H_INCLUDED__

#include "IAnimatedMesh.h"
#include "IReadFile.h"
#include "S3DVertex.h"
#include "SMesh.h"

namespace engine
{
namespace scene
{

class CStaticMeshOBJ : public IAnimatedMesh
{
public:

//! constructor
CStaticMeshOBJ();

//! destructor
virtual ~CStaticMeshOBJ();

//! loads an obj file
virtual bool loadFile(io::IReadFile* file);

//! returns the amount of frames in milliseconds. If the amount is 1, it is a static (=non animated) mesh.
virtual s32 getFrameCount();

//! returns the animated mesh based on a detail level. 0 is the lowest, 255 the highest detail. Note, that some Meshes will ignore the detail level.
virtual IMesh* getMesh(s32 frame, s32 detailLevel=255, s32 startFrameLoop=-1, s32 endFrameLoop=-1);

//! Returns an axis aligned bounding box of the mesh.
//! \return A bounding box of this mesh is returned.
virtual const core::aabbox3d<f32>& getBoundingBox() const;

//! Returns the type of the animated mesh.
virtual E_ANIMATED_MESH_TYPE getMeshType() const;

private:

c8* getFirstWord(c8* buf);
void copyWord(c8* outBuf, s32 bufLength, c8* word);
void copyWordLineEnd(c8* outBuf, s32 bufLength, c8* word);
void copyWordSpaceEnd(c8* outBuf, s32 bufLength, c8* word);
c8* getNextWord(c8* word);

scene::SMesh Mesh;
};

} // end namespace scene
} // end namespace engine

#endif

