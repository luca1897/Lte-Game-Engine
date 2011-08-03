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
 
#ifndef __C_CEL_SHADING_SCENE_NODE_H_INCLUDED__
#define __C_CEL_SHADING_SCENE_NODE_H_INCLUDED__

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glext.h>

#include "ICelshadingSceneNode.h"
#include "SLight.h"
#include "ITexture.h"



namespace engine
{
namespace scene
{

//! Scene node for rendering a shadow volume into a stencil buffer.
class CCelshadingSceneNode : public ICelshadingSceneNode
{
public:

//! constructor
CCelshadingSceneNode(ISceneNode* parent, ISceneManager* mgr,
                     s32 id, float *shader, int width, bool recalc_normals);

//! destructor
virtual ~CCelshadingSceneNode();

//! sets the mesh from which the shadow volume should be generated.
virtual void setMeshToRenderFrom(IMesh* mesh);

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
	return ESNT_CELSHADING;
}

private:

typedef struct
{
	core::vector2df TCoords;
	// unsigned int Color;
	core::vector3df Pos;
} __verttex;
struct SCelshadingData
{
	core::vector3df* vertices;
	__verttex* vertices_pos;
	s32 count;
	s32 size;
	GLuint vbo[3];
	bool mapped;
};

void createCelshadingData(const core::vector3df& pos, video::S3DVertex* vtx, const u16* idx, s32 idxcount,s32 vtxcount, bool lightdir, bool lighttype);

core::aabbox3d<f32> Box;

u16* Indices;
core::vector3df* Vertices;
video::ITexture *tex;
s32 IndexCountAllocated;
s32 VertexCountAllocated;
s32 IndexCount;
s32 VertexCount;
bool recalcnormal;

float *shader;
int shader_width;

core::array<SCelshadingData> SCelshadingDatas;                 // a shadow volume for every light
s32 CelshadingCount;
};

} // end namespace scene
} // end namespace engine

#endif

