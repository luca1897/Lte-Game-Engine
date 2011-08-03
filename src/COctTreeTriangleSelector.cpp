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
 
#include "COctTreeTriangleSelector.h"
#include "ISceneNode.h"

#include "os.h"
#include <stdio.h>

namespace engine
{
namespace scene
{

//! constructor
COctTreeTriangleSelector::COctTreeTriangleSelector(IMesh* mesh, ISceneNode* node,
                                                   s32 minimalPolysPerNode)
	: CTriangleSelector(mesh, node), Root(0), NodeCount(0),
	MinimalPolysPerNode(minimalPolysPerNode)
{
	#ifdef _DEBUG
	setDebugName("COctTreeTriangleSelector");
	#endif


	if (!Triangles.empty())
	{
		u32 start = os::Timer::getRealTime();

		// create the triangle octtree
		Root = new SOctTreeNode();
		Root->Triangles = Triangles;
		constructOctTree(Root);

		u32 end = os::Timer::getRealTime();
		c8 tmp[255];
		sprintf(tmp, "Needed %dms to create OctTreeTriangleSelector.(%d nodes, %d polys)",
		        end - start, NodeCount, Triangles.size());
		os::Printer::log(tmp, ELL_INFORMATION);
	}
}



//! destructor
COctTreeTriangleSelector::~COctTreeTriangleSelector()
{
	delete Root;
}



void COctTreeTriangleSelector::constructOctTree(SOctTreeNode* node)
{
	++NodeCount;

	node->Box.reset(node->Triangles[0].pointA);

	// get bounding box
	s32 cnt = node->Triangles.size();
	for (s32 i=0; i<cnt; ++i)
	{
		node->Box.addInternalPoint(node->Triangles[i].pointA);
		node->Box.addInternalPoint(node->Triangles[i].pointB);
		node->Box.addInternalPoint(node->Triangles[i].pointC);
	}

	core::vector3df middle = node->Box.getCenter();
	core::vector3df edges[8];
	node->Box.getEdges(edges);

	core::aabbox3d<f32> box;

	// calculate children

	if (!node->Box.isEmpty() && (s32)node->Triangles.size() > MinimalPolysPerNode)
		for (s32 ch=0; ch<8; ++ch)
		{
			box.reset(middle);
			box.addInternalPoint(edges[ch]);
			node->Child[ch] = new SOctTreeNode();

			for (s32 i=0; i<(s32)node->Triangles.size(); ++i)
			{
				if (node->Triangles[i].isTotalInsideBox(box))
				{
					node->Child[ch]->Triangles.push_back(node->Triangles[i]);
					node->Triangles.erase(i);
					--i;
				}
			}

			if (node->Child[ch]->Triangles.empty())
			{
				delete node->Child[ch];
				node->Child[ch] = 0;
			}
			else
				constructOctTree(node->Child[ch]);
		}
}



//! Gets all triangles which lie within a specific bounding box.
void COctTreeTriangleSelector::getTriangles(core::triangle3df* triangles,
                                            s32 arraySize, s32& outTriangleCount,
                                            const core::aabbox3d<f32>& box,
                                            const core::matrix4* transform)
{
	core::matrix4 mat;

	if (SceneNode)
		mat = SceneNode->getAbsoluteTransformation();

	mat.makeInverse();
	core::aabbox3d<f32> invbox = box;
	mat.transformBox(invbox);

	mat.makeIdentity();

	if (transform)
		mat = (*transform);

	if (SceneNode)
		mat *= SceneNode->getAbsoluteTransformation();

	s32 trianglesWritten = 0;

	if (Root)
		getTrianglesFromOctTree(Root, trianglesWritten,
		                        arraySize, invbox, &mat, triangles);

	outTriangleCount = trianglesWritten;
}


void COctTreeTriangleSelector::getTrianglesFromOctTree(
        SOctTreeNode* node, s32& trianglesWritten,
        s32 maximumSize, const core::aabbox3d<f32>& box,
        const core::matrix4* mat, core::triangle3df* triangles)
{


	if (!box.intersectsWithBox(node->Box))
		return;

	s32 i;
	s32 cnt = node->Triangles.size();
	if (cnt + trianglesWritten > maximumSize)
		cnt -= cnt + trianglesWritten - maximumSize;



	for (i=0; i<cnt; ++i)
	{
		triangles[trianglesWritten] = node->Triangles[i];
		mat->transformVect(triangles[trianglesWritten].pointA);
		mat->transformVect(triangles[trianglesWritten].pointB);
		mat->transformVect(triangles[trianglesWritten].pointC);
		++trianglesWritten;
	}

	for (i=0; i<8; ++i)
		if (node->Child[i])
			getTrianglesFromOctTree(node->Child[i], trianglesWritten,
			                        maximumSize, box, mat, triangles);
}


//! Gets all triangles which have or may have contact with a 3d line.
void COctTreeTriangleSelector::getTriangles(core::triangle3df* triangles, s32 arraySize,
                                            s32& outTriangleCount, const core::line3d<f32>& line,
                                            const core::matrix4* transform)
{
	core::aabbox3d<f32> box(line.start);
	box.addInternalPoint(line.end);

	// TODO: Could be optimized for line a little bit more.
	COctTreeTriangleSelector::getTriangles(triangles, arraySize, outTriangleCount,
	                                       box, transform);
}


} // end namespace scene
} // end namespace engine

