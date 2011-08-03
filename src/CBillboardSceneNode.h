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
 
#ifndef __C_BILLBOARD_SCENE_NODE_H_INCLUDED__
#define __C_BILLBOARD_SCENE_NODE_H_INCLUDED__

#include "IBillboardSceneNode.h"
#include "S3DVertex.h"

namespace engine
{
namespace scene
{

//! Scene node which is a billboard. A billboard is like a 3d sprite: A 2d element,
//! which always looks to the camera.
class CBillboardSceneNode : public IBillboardSceneNode
{
public:

//! constructor
CBillboardSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
                    const core::vector3df& position, const core::dimension2d<f32>& size);

virtual ~CBillboardSceneNode();

//! pre render event
virtual void OnPreRender();

//! render
virtual void render();

//! returns the axis aligned bounding box of this node
virtual const core::aabbox3d<f32>& getBoundingBox() const;

//! sets the size of the billboard
virtual void setSize(const core::dimension2d<f32>& size);

//! gets the size of the billboard
virtual const core::dimension2d<f32>& getSize();

virtual video::SMaterial& getMaterial(s32 i);

//! returns amount of materials used by this scene node.
virtual s32 getMaterialCount();

//! Returns type of the scene node
virtual ESCENE_NODE_TYPE getType() {
	return ESNT_BILLBOARD;
}

private:

core::dimension2d<f32> Size;
core::aabbox3d<f32> BBox;
video::SMaterial Material;

video::S3DVertex vertices[4];
u16 indices[6];
};


} // end namespace scene
} // end namespace engine


#endif

