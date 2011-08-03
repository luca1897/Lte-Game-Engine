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
 
#include "COctTreeSceneNode.h"
#include "OctTree.h"
#include "ISceneManager.h"
#include "IVideoDriver.h"
#include "ICameraSceneNode.h"

#include "os.h"
#include <stdio.h>

namespace engine
{
namespace scene
{


//! constructor
COctTreeSceneNode::COctTreeSceneNode(ISceneNode* parent, ISceneManager* mgr,
                                     s32 id, s32 minimalPolysPerNode)
	: ISceneNode(parent, mgr, id), StdOctTree(0), LightMapOctTree(0),
	MinimalPolysPerNode(minimalPolysPerNode)
{
#ifdef _DEBUG
	setDebugName("COctTreeSceneNode");
#endif

	AutomaticCullingEnabled = false;
	vertexType = (video::E_VERTEX_TYPE)-1;
}



//! destructor
COctTreeSceneNode::~COctTreeSceneNode()
{
	delete StdOctTree;
	delete LightMapOctTree;
}



void COctTreeSceneNode::OnPreRender()
{
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
}



//! renders the node.
void COctTreeSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	if (vertexType == -1 || !driver)
		return;

	ICameraSceneNode* camera = SceneManager->getActiveCamera();
	if (!camera)
		return;

	bool isTransparentPass =
	        SceneManager->getSceneNodeRenderPass() == scene::ESNRP_TRANSPARENT;
	++PassCount;

	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

	SViewFrustrum frust = *camera->getViewFrustrum();

	//transform the frustrum to the current absolute transformation

	core::matrix4 invTrans(AbsoluteTransformation);
	invTrans.makeInverse();

	frust.transform(invTrans);

	core::aabbox3d<f32> box = frust.getBoundingBox();

	switch(vertexType)
	{
	case video::EVT_STANDARD:
	{
		//StdOctTree->calculatePolys(box);
		StdOctTree->calculatePolys(frust);

		OctTree<video::S3DVertex>::SIndexData* d =  StdOctTree->getIndexData();

		for (u32 i=0; i<Materials.size(); ++i)
		{
			video::IMaterialRenderer* rnd = driver->getMaterialRenderer(Materials[i].MaterialType);
			bool transparent = (rnd && rnd->isTransparent());

			// only render transparent buffer if this is the transparent render pass
			// and solid only in solid pass
			if (transparent == isTransparentPass)
			{
				driver->setMaterial(Materials[i]);
				driver->drawIndexedTriangleList(
				        &StdMeshes[i].Vertices[0], StdMeshes[i].Vertices.size(),
				        d[i].Indices, d[i].CurrentSize / 3);
			}
		}

		// for debug purposes only
		if (DebugDataVisible && !Materials.empty() && PassCount==1)
		{
			core::array< core::aabbox3d<f32> > boxes;
			video::SMaterial m;
			m.Lighting = false;
			driver->setMaterial(m);
			StdOctTree->renderBoundingBoxes(box, boxes);
			for (u32 b=0; b<boxes.size(); ++b)
				driver->draw3DBox(boxes[b], video::SColor(0,255,255,255));
		}
		break;

	}
	case video::EVT_2TCOORDS:
	{
		//LightMapOctTree->calculatePolys(box);
		LightMapOctTree->calculatePolys(frust);

		OctTree<video::S3DVertex2TCoords>::SIndexData* d =  LightMapOctTree->getIndexData();

		for (u32 i=0; i<Materials.size(); ++i)
		{
			video::IMaterialRenderer* rnd = driver->getMaterialRenderer(Materials[i].MaterialType);
			bool transparent = (rnd && rnd->isTransparent());

			// only render transparent buffer if this is the transparent render pass
			// and solid only in solid pass
			if (transparent == isTransparentPass)
			{
				driver->setMaterial(Materials[i]);
				driver->drawIndexedTriangleList(
				        &LightMapMeshes[i].Vertices[0], LightMapMeshes[i].Vertices.size(),
				        d[i].Indices, d[i].CurrentSize / 3);
			}
		}

		// for debug purposes only
		if (DebugDataVisible && !Materials.empty() && PassCount==1)
		{
			core::array< core::aabbox3d<f32> > boxes;
			video::SMaterial m;
			m.Lighting = false;
			driver->setMaterial(m);
			LightMapOctTree->renderBoundingBoxes(box, boxes);
			for (u32 b=0; b<boxes.size(); ++b)
				driver->draw3DBox(boxes[b], video::SColor(0,255,255,255));
		}
	}
		break;
	};
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& COctTreeSceneNode::getBoundingBox() const
{
	return Box;
}


//! creates the tree
bool COctTreeSceneNode::createTree(IMesh* mesh)
{
	if (!mesh)
		return false;

	u32 beginTime = os::Timer::getRealTime();

	s32 nodeCount = 0;
	s32 polyCount = 0;

	Box = mesh->getBoundingBox();

	if (mesh->getMeshBufferCount())
	{
		vertexType = mesh->getMeshBuffer(0)->getVertexType();

		switch(vertexType)
		{
		case video::EVT_STANDARD:
		{
			for (s32 i=0; i<mesh->getMeshBufferCount(); ++i)
			{
				IMeshBuffer* b = mesh->getMeshBuffer(i);
				Materials.push_back(b->getMaterial());

				OctTree<video::S3DVertex>::SMeshChunk chunk;
				chunk.MaterialId = i;
				StdMeshes.push_back(chunk);
				OctTree<video::S3DVertex>::SMeshChunk &nchunk = StdMeshes[StdMeshes.size()-1];

				s32 v;

				for (v=0; v<b->getVertexCount(); ++v)
					nchunk.Vertices.push_back(((video::S3DVertex*)b->getVertices())[v]);

				polyCount += b->getIndexCount();

				for (v=0; v<b->getIndexCount(); ++v)
					nchunk.Indices.push_back(b->getIndices()[v]);
			}

			StdOctTree = new OctTree<video::S3DVertex>(StdMeshes, MinimalPolysPerNode);
			nodeCount = StdOctTree->nodeCount;
		}
			break;
		case video::EVT_2TCOORDS:
		{
			for (s32 i=0; i<mesh->getMeshBufferCount(); ++i)
			{
				IMeshBuffer* b = mesh->getMeshBuffer(i);
				Materials.push_back(b->getMaterial());

				OctTree<video::S3DVertex2TCoords>::SMeshChunk chunk;
				chunk.MaterialId = i;
				LightMapMeshes.push_back(chunk);
				OctTree<video::S3DVertex2TCoords>::SMeshChunk& nchunk =
				        LightMapMeshes[LightMapMeshes.size()-1];

				s32 v;

				for (v=0; v<b->getVertexCount(); ++v)
					nchunk.Vertices.push_back(((video::S3DVertex2TCoords*)b->getVertices())[v]);

				polyCount += b->getIndexCount();

				for (v=0; v<b->getIndexCount(); ++v)
					nchunk.Indices.push_back(b->getIndices()[v]);
			}

			LightMapOctTree = new OctTree<video::S3DVertex2TCoords>(LightMapMeshes, MinimalPolysPerNode);
			nodeCount = LightMapOctTree->nodeCount;
		}
			break;
		}
	}

	u32 endTime = os::Timer::getRealTime();
	c8 tmp[255];
	sprintf(tmp, "Needed %dms to create OctTree SceneNode.(%d nodes, %d polys)",
	        endTime - beginTime, nodeCount, polyCount/3);
	os::Printer::log(tmp, ELL_INFORMATION);

	return true;
}


//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hirachy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
video::SMaterial& COctTreeSceneNode::getMaterial(s32 i)
{
	if (i < 0 || i >= (s32)Materials.size())
		return ISceneNode::getMaterial(i);

	return Materials[i];
}

//! returns amount of materials used by this scene node.
s32 COctTreeSceneNode::getMaterialCount()
{
	return Materials.size();
}


} // end namespace scene
} // end namespace engine
