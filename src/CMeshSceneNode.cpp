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
 
#include "CMeshSceneNode.h"
#include "IVideoDriver.h"
#include "os.h"
#include "ISceneManager.h"
#include "S3DVertex.h"
#include "SViewFrustrum.h"
#include "ICameraSceneNode.h"


namespace engine
{
namespace scene
{



//! constructor
CMeshSceneNode::CMeshSceneNode(IMesh* mesh, ISceneNode* parent, ISceneManager* mgr, s32 id,
                               const core::vector3df& position, const core::vector3df& rotation,
                               const core::vector3df& scale)
	: ISceneNode(parent, mgr, id, position, rotation, scale), Mesh(mesh), PassCount(0)
{
	#ifdef _DEBUG
	setDebugName("CMeshSceneNode");
	#endif

	if (Mesh)
	{
		// get materials.
		video::SMaterial mat;
		for (s32 i=0; i<Mesh->getMeshBufferCount(); ++i)
		{
			IMeshBuffer* mb = Mesh->getMeshBuffer(i);
			if (mb)
				mat = mb->getMaterial();

			Materials.push_back(mat);
		}

		// grab the mesh

		Mesh->grab();
	}
}



//! destructor
CMeshSceneNode::~CMeshSceneNode()
{
	if (Mesh)
		Mesh->drop();
}



#include <pspdebug.h>
#define printf pspDebugScreenPrintf
//! frame
void CMeshSceneNode::OnPreRender()
{
	int d= os::Timer::getRealTime();
//	printf("mesh scene\n");
	if (IsVisible)
	{
		// because this node supports rendering of mixed mode meshes consisting of
		// transparent and solid material at the same time, we need to go through all
		// materials, check of what type they are and register this node for the right
		// render pass according to that.

		video::IVideoDriver* driver = SceneManager->getVideoDriver();

		PassCount = 0;
		int transparentCount = 0;
		int solidCount = 0;

		// count transparent and solid materials in this scene node
		for (u32 i=0; i<Materials.size(); ++i)
		{
			video::IMaterialRenderer* rnd =
			        driver->getMaterialRenderer(Materials[i].MaterialType);

			if (rnd && rnd->isTransparent())
				++transparentCount;
			else
				++solidCount;

			if (solidCount && transparentCount)
				break;
		}

		// register according to material types counted

		if (solidCount)
			SceneManager->registerNodeForRendering(this, scene::ESNRP_SOLID);

		if (transparentCount)
			SceneManager->registerNodeForRendering(this, scene::ESNRP_TRANSPARENT);
	}

	ISceneNode::OnPreRender();
//	printf("meshp %d\n", os::Timer::getRealTime() -d);
}



//! renders the node.
void CMeshSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	if (!Mesh || !driver)
		return;

	bool isTransparentPass =
	        SceneManager->getSceneNodeRenderPass() == scene::ESNRP_TRANSPARENT;

	++PassCount;

	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
	Box = Mesh->getBoundingBox();

	// for debug purposes only:
	if (DebugDataVisible && PassCount==1)
	{
		video::SMaterial m;
		m.Lighting = false;
		driver->setMaterial(m);
		driver->draw3DBox(Box, video::SColor(0,255,255,255));

#if 0 // draw normals
		for (s32 g=0; g<Mesh->getMeshBufferCount(); ++g)
		{
			scene::IMeshBuffer* mb = Mesh->getMeshBuffer(g);

			u32 vSize;
			u32 i;
			vSize = 0;
			switch( mb->getVertexType() )
			{
			case video::EVT_STANDARD:
				vSize = sizeof ( video::S3DVertex );
				break;
			case video::EVT_2TCOORDS:
				vSize = sizeof ( video::S3DVertex2TCoords );
				break;
			case video::EVT_TANGENTS:
				vSize = sizeof ( video::S3DVertexTangents );
				break;
			}

			const video::S3DVertex* v = ( const video::S3DVertex*)mb->getVertices();
			video::SColor c ( 255, 128,0, 0 );
			video::SColor c1 ( 255, 255,0, 0 );
			for ( i = 0; i != mb->getVertexCount(); ++i )
			{
				core::vector3df h = v->Normal * 5.f;
				core::vector3df h1 = h.crossProduct ( core::vector3df ( 0.f, 1.f, 0.f ) );

				driver->draw3DLine ( v->Pos, v->Pos + h, c );
				driver->draw3DLine ( v->Pos + h, v->Pos + h + h1, c1 );
				v = (const video::S3DVertex*) ( (u8*) v + vSize );
			}

		}
#endif // Draw normals
	}

	for (s32 i=0; i<Mesh->getMeshBufferCount(); ++i)
	{
		video::IMaterialRenderer* rnd = driver->getMaterialRenderer(Materials[i].MaterialType);
		bool transparent = (rnd && rnd->isTransparent());

		// only render transparent buffer if this is the transparent render pass
		// and solid only in solid pass
		if (transparent == isTransparentPass)
		{
			scene::IMeshBuffer* mb = Mesh->getMeshBuffer(i);
			driver->setMaterial(Materials[i]);
			driver->drawMeshBuffer(mb);
		}
	}
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CMeshSceneNode::getBoundingBox() const
{
	return Mesh ? Mesh->getBoundingBox() : Box;
}


//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hirachy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
video::SMaterial& CMeshSceneNode::getMaterial(s32 i)
{
	if (i < 0 || i >= (s32)Materials.size())
		return ISceneNode::getMaterial(i);

	return Materials[i];
}



//! returns amount of materials used by this scene node.
s32 CMeshSceneNode::getMaterialCount()
{
	return Materials.size();
}



} // end namespace scene
} // end namespace engine

