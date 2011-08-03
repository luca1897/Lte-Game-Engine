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
 
#ifndef __C_PLANAR_SHADOW_
#define __C_PLANAR_SHADOW_

#include <GL/gl.h>

namespace engine
{
namespace scene
{
class CPlanarShadow : public ISceneNode
{

public:
CPlanarShadow(scene::ISceneNode *parent, scene::ISceneManager *mgr, IMesh* mesh, float divisor);
~CPlanarShadow();


void setPlane(core::plane3df plane);
void setAnimated(bool animated);
void setMeshToRenderFrom(IMesh* mesh);
void setTransform(core::matrix4& transform);

void buildMesh(IMesh* mesh);
void setDepth(bool DepthTest);


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
	return ESNT_PLANAR_SHADOW;
}


private:
bool Animated;
core::plane3df Plane;
core::matrix4 Transform;
bool enableDepth;

core::vector3df *Vertices;
u16* Indices;

int IndexCount;
int VertexCount;

scene::ISceneManager*SceneManager;
float Divisor;

core::aabbox3d<f32> Box;
core::matrix4 View;

};
}
}

#endif
