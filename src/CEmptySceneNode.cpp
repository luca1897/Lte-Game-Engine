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
 
#include "CEmptySceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"

namespace engine
{
namespace scene
{

//! constructor
CEmptySceneNode::CEmptySceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
	: ISceneNode(parent, mgr, id)
{
	#ifdef _DEBUG
	setDebugName("CEmptySceneNode");
	#endif

	setAutomaticCulling(false);
}


#include <pspdebug.h>
#define printf pspDebugScreenPrintf
//! pre render event
void CEmptySceneNode::OnPreRender()
{
//	printf("empty scene\n");
	if (IsVisible)
	{
		SceneManager->registerNodeForRendering(this);
		ISceneNode::OnPreRender();
	}
}


//! render
void CEmptySceneNode::render()
{
	// do nothing
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CEmptySceneNode::getBoundingBox() const
{
	return Box;
}

} // end namespace scene
} // end namespace engine
