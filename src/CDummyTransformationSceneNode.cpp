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
 
#include "CDummyTransformationSceneNode.h"

namespace engine
{
namespace scene
{

//! constructor
CDummyTransformationSceneNode::CDummyTransformationSceneNode(
        ISceneNode* parent, ISceneManager* mgr, s32 id)
	: IDummyTransformationSceneNode(parent, mgr, id)
{
	#ifdef _DEBUG
	setDebugName("CDummyTransformationSceneNode");
	#endif

	setAutomaticCulling(false);
}



//! destructor
CDummyTransformationSceneNode::~CDummyTransformationSceneNode()
{
}



//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CDummyTransformationSceneNode::getBoundingBox() const
{
	return Box;
}



//! Returns a reference to the current relative transformation matrix.
//! This is the matrix, this scene node uses instead of scale, translation
//! and rotation.
core::matrix4& CDummyTransformationSceneNode::getRelativeTransformationMatrix()
{
	return RelativeTransformationMatrix;
}


//! Returns the relative transformation of the scene node.
core::matrix4 CDummyTransformationSceneNode::getRelativeTransformation() const
{
	return RelativeTransformationMatrix;
}


} // end namespace scene
} // end namespace engine

