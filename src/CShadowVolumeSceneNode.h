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

#ifndef __C_SHADOW_VOLUME_SCENE_NODE_H_INCLUDED__
#define __C_SHADOW_VOLUME_SCENE_NODE_H_INCLUDED__

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glext.h>

#include "IShadowVolumeSceneNode.h"
#include "SLight.h"

#include "ShadInfo.h"
#include "CMemory.h"

namespace engine
{
namespace scene
{

//! Scene node for rendering a shadow volume into a stencil buffer.
class CShadowVolumeSceneNode : public IShadowVolumeSceneNode
{
public:

//! constructor
CShadowVolumeSceneNode(ISceneNode* parent, ISceneManager* mgr,
                       s32 id,  bool zfailmethod=true, f32 infinity=100.0f, bool removeh = true, bool onlyextrude = false, scene::IMesh *pre = 0);

//! destructor
virtual ~CShadowVolumeSceneNode();

//! sets the mesh from which the shadow volume should be generated.
virtual void setMeshToRenderFrom(IMesh* mesh);

virtual void setCustomModelMatrix(core::matrix4& mat);

//! pre render method
virtual void OnPreRender();

//! renders the node.
virtual void render();

//! returns the axis aligned bounding box of this node
virtual const core::aabbox3d<f32>& getBoundingBox() const;

//! returns the material based on the zero based index i.
virtual video::SMaterial& getMaterial(s32 i);

//! returns amount of materials used by this scene node.
virtual s32 getMaterialCount();

//! Returns type of the scene node
virtual ESCENE_NODE_TYPE getType() {
	return ESNT_SHADOW_VOLUME;
}

private:

struct SShadowVolume
{
	ShadowVertex* vertices;
	Triangle *indices;


	s32 count;
	s32 size;

	u32 vtxcount;
	u32 triscount;
	core::vector3df lpos;

	GLuint vbo[2];
	bool mapped;
};

void createShadowVolume(const core::vector3df& pos);
void addEdge(s32& numEdges, u16 v0, u16 v1);
void renderFromPrebuilt();
void constructPrebuiltMesh(core::vector3df light);


bool UseZFailMethod;
core::aabbox3d<f32> Box;

u16* Indices;
core::vector3df* Vertices;
u16* Adjacency;
bool* FaceData;                 // used for zfail method, if face is front facing
core::matrix4 customMatrix;

s32 IndexCountAllocated;
s32 VertexCountAllocated;
s32 IndexCount;
s32 VertexCount;

scene::IMesh* prebuiltShadow;

core::array<SShadowVolume> ShadowVolumes;                 // a shadow volume for every light
s32 ShadowVolumesUsed;

bool extrudeOnly;

core::CMemory *mem;
bool removeHardVertices;
u16* Edges;
s32 EdgeCount;

f32 Infinity;
};

} // end namespace scene
} // end namespace engine

#endif

