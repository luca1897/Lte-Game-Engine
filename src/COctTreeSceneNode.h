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
 
#ifndef __C_OCT_TREE_SCENE_NODE_H_INCLUDED__
#define __C_OCT_TREE_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"
#include "IMesh.h"
#include "OctTree.h"

namespace engine
{
namespace scene
{
//! implementation of the IBspTreeSceneNode
class COctTreeSceneNode : public ISceneNode
{
public:

//! constructor
COctTreeSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
                  s32 minimalPolysPerNode=128);

//! destructor
virtual ~COctTreeSceneNode();

virtual void OnPreRender();

//! renders the node.
virtual void render();

//! returns the axis aligned bounding box of this node
virtual const core::aabbox3d<f32>& getBoundingBox() const;

//! creates the tree
bool createTree(IMesh* mesh);

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
	return ESNT_OCT_TREE;
}

private:

core::aabbox3d<f32> Box;

OctTree<video::S3DVertex>* StdOctTree;
core::array< OctTree<video::S3DVertex>::SMeshChunk > StdMeshes;

OctTree<video::S3DVertex2TCoords>* LightMapOctTree;
core::array< OctTree<video::S3DVertex2TCoords>::SMeshChunk > LightMapMeshes;

video::E_VERTEX_TYPE vertexType;
core::array< video::SMaterial > Materials;

s32 MinimalPolysPerNode;
s32 PassCount;
};

} // end namespace scene
} // end namespace engine

#endif

