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
 
#include "CTestSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "S3DVertex.h"
#include "os.h"

namespace engine
{
namespace scene
{

//! constructor
CTestSceneNode::CTestSceneNode(f32 size, ISceneNode* parent, ISceneManager* mgr, s32 id,
                               const core::vector3df& position, const core::vector3df& rotation,       const core::vector3df& scale)
	: ISceneNode(parent, mgr, id, position, rotation, scale)
{
	#ifdef _DEBUG
	setDebugName("CTestSceneNode");
	#endif

	u16 u[36] = {   0,2,1,   0,3,2,   1,5,4,   1,2,5,   4,6,7,   4,5,6,
		        7,3,0,   7,6,3,   9,5,2,   9,8,5,   0,11,10,   0,10,7};

	s32 i; // new ISO for scoping problem with different compilers
	for (i=0; i<36; ++i)
		Indices[i] = u[i];

	Material.Wireframe = false;
	Material.Lighting = false;

	// nicer texture mapping sent in by Dr Andros C Bragianos
	// .. and then improved by jox.
	Vertices[0]  = video::S3DVertex(0,0,0, -1,-1,-1, video::SColor(255,255,255,255), 0, 1);
	Vertices[1]  = video::S3DVertex(1,0,0,  1,-1,-1, video::SColor(255,255,255,255), 1, 1);
	Vertices[2]  = video::S3DVertex(1,1,0,  1, 1,-1, video::SColor(255,255,255,255), 1, 0);
	Vertices[3]  = video::S3DVertex(0,1,0, -1, 1,-1, video::SColor(255,255,255,255), 0, 0);
	Vertices[4]  = video::S3DVertex(1,0,1,  1,-1, 1, video::SColor(255,255,255,255), 0, 1);
	Vertices[5]  = video::S3DVertex(1,1,1,  1, 1, 1, video::SColor(255,255,255,255), 0, 0);
	Vertices[6]  = video::S3DVertex(0,1,1, -1, 1, 1, video::SColor(255,255,255,255), 1, 0);
	Vertices[7]  = video::S3DVertex(0,0,1, -1,-1, 1, video::SColor(255,255,255,255), 1, 1);
	Vertices[8]  = video::S3DVertex(0,1,1, -1, 1, 1, video::SColor(255,255,255,255), 0, 1);
	Vertices[9]  = video::S3DVertex(0,1,0, -1, 1,-1, video::SColor(255,255,255,255), 1, 1);
	Vertices[10] = video::S3DVertex(1,0,1,  1,-1, 1, video::SColor(255,255,255,255), 1, 0);
	Vertices[11] = video::S3DVertex(1,0,0,  1,-1,-1, video::SColor(255,255,255,255), 0, 0);

	Box.reset(0,0,0);

	for (i=0; i<12; ++i)
	{
		Vertices[i].Pos -= core::vector3df(0.5f, 0.5f, 0.5f);
		Vertices[i].Pos *= size;
		Box.addInternalPoint(Vertices[i].Pos);
	}
}



//! destructor
CTestSceneNode::~CTestSceneNode()
{
}


//! renders the node.
void CTestSceneNode::render()
{
	/*
	   011         111
	   /6--------/5        y
	   /  |      / |        ^  z
	   /   |     /  |        | /
	   010 3---------2  |        |/
	 |   7- - -| -4 101     *---->x
	 |  /      |  /
	 |/        | /
	   0---------1/
	   000       100
	 */

	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	driver->setMaterial(Material);

	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

	driver->drawIndexedTriangleList(&Vertices[0], 12, &Indices[0], 12);
}



//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CTestSceneNode::getBoundingBox() const
{
	return Box;
}


void CTestSceneNode::OnPreRender()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(this);

	ISceneNode::OnPreRender();
}


//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hirachy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
video::SMaterial& CTestSceneNode::getMaterial(s32 i)
{
	return Material;
}


//! returns amount of materials used by this scene node.
s32 CTestSceneNode::getMaterialCount()
{
	return 1;
}




} // end namespace scene
} // end namespace engine

