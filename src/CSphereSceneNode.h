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

#ifndef __C_SHPERE_SCENE_NODE_H_INCLUDED__
#define __C_SHPERE_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"
#include "S3DVertex.h"

namespace engine
{
namespace scene
{
class CSphereSceneNode : public ISceneNode
{
public:

//! constructor
CSphereSceneNode(f32 size, s32 polyCount, ISceneNode* parent, ISceneManager* mgr, s32 id,
                 const core::vector3df& position = core::vector3df(0,0,0),
                 const core::vector3df& rotation = core::vector3df(0,0,0),
                 const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

//! destructor
virtual ~CSphereSceneNode();

virtual void OnPreRender();

//! renders the node.
virtual void render();

//! returns the axis aligned bounding box of this node
virtual const core::aabbox3d<f32>& getBoundingBox() const;

//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hirachy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
virtual video::SMaterial& getMaterial(s32 i);

//! returns amount of materials used by this scene node.
virtual s32 getMaterialCount();

//! Returns type of the scene node
virtual ESCENE_NODE_TYPE getType() {
	return ESNT_SPHERE;
}

video::S3DVertex* getVertices();
u16* getIndices();


s32 getIndexCount()  ;
s32 getVertexCount();




private:

void setSizeAndPolys();

core::aabbox3d<f32> Box;

u16* Indices;
s32 IndexCount;

video::S3DVertex* Vertices;
s32 VertexCount;

video::SMaterial Material;
f32 Radius;
s32 PolyCount;
};

} // end namespace scene
} // end namespace irr

#endif

