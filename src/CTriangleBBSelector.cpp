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
 
#include "CTriangleBBSelector.h"
#include "ISceneNode.h"

namespace engine
{
namespace scene
{

//! constructor
CTriangleBBSelector::CTriangleBBSelector(ISceneNode* node)
	: CTriangleSelector(node)
{
	#ifdef _DEBUG
	setDebugName("CTriangleBBSelector");
	#endif

	Triangles.set_used(12); // a box has 12 triangles.
}



//! Gets all triangles.
void CTriangleBBSelector::getTriangles(core::triangle3df* triangles,
                                       s32 arraySize, s32& outTriangleCount,
                                       const core::matrix4* transform)
{
	if (!SceneNode)
		return;

	// construct triangles
	core::aabbox3d<f32> box = SceneNode->getBoundingBox();
	core::vector3df edges[8];
	box.getEdges(edges);

	Triangles[0].set( edges[3], edges[0], edges[2]);
	Triangles[1].set( edges[3], edges[1], edges[0]);

	Triangles[2].set( edges[3], edges[2], edges[7]);
	Triangles[3].set( edges[7], edges[2], edges[6]);

	Triangles[4].set( edges[7], edges[6], edges[4]);
	Triangles[5].set( edges[5], edges[7], edges[4]);

	Triangles[6].set( edges[5], edges[4], edges[0]);
	Triangles[7].set( edges[5], edges[0], edges[1]);

	Triangles[8].set( edges[1], edges[3], edges[7]);
	Triangles[9].set( edges[1], edges[7], edges[5]);

	Triangles[10].set(edges[0], edges[6], edges[2]);
	Triangles[11].set(edges[0], edges[4], edges[6]);

	// call parent
	CTriangleSelector::getTriangles(triangles, arraySize, outTriangleCount, transform);
}


} // end namespace scene
} // end namespace engine

