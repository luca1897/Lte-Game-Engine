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
 
#include "CTriangleSelector.h"
#include "ISceneNode.h"

namespace engine
{
namespace scene
{

//! constructor
CTriangleSelector::CTriangleSelector(ISceneNode* node)
	: SceneNode(node)
{
	#ifdef _DEBUG
	setDebugName("CTriangleSelector");
	#endif
}


//! constructor
CTriangleSelector::CTriangleSelector(IMesh* mesh, ISceneNode* node)
	: SceneNode(node)
{
	#ifdef _DEBUG
	setDebugName("CTriangleSelector");
	#endif

	s32 cnt = mesh->getMeshBufferCount();
	for (s32 i=0; i<cnt; ++i)
	{
		IMeshBuffer* buf = mesh->getMeshBuffer(i);

		s32 idxCnt = buf->getIndexCount();
		s32 vtxCnt = buf->getVertexCount();
		const u16* indices = buf->getIndices();
		core::triangle3df tri;

		switch (buf->getVertexType())
		{
		case video::EVT_STANDARD:
		{
			video::S3DVertex* vtx = (video::S3DVertex*)buf->getVertices();
			for (s32 i=0; i<idxCnt; i+=3)
			{
				tri.pointA = vtx[indices[i+0]].Pos;
				tri.pointB = vtx[indices[i+1]].Pos;
				tri.pointC = vtx[indices[i+2]].Pos;
				Triangles.push_back(tri);
			}
		}
			break;
		case video::EVT_2TCOORDS:
		{
			video::S3DVertex2TCoords* vtx = (video::S3DVertex2TCoords*)buf->getVertices();
			for (s32 i=0; i<idxCnt; i+=3)
			{
				tri.pointA = vtx[indices[i+0]].Pos;
				tri.pointB = vtx[indices[i+1]].Pos;
				tri.pointC = vtx[indices[i+2]].Pos;
				Triangles.push_back(tri);
			}
		}
			break;
		}
	}
}


//! constructor
CTriangleSelector::CTriangleSelector(core::aabbox3d<f32> box, ISceneNode* node)
	: SceneNode(node)
{
	#ifdef _DEBUG
	setDebugName("CTriangleSelector");
	#endif

	// TODO
}



//! destructor
CTriangleSelector::~CTriangleSelector()
{
}



//! Gets all triangles.
void CTriangleSelector::getTriangles(core::triangle3df* triangles,
                                     s32 arraySize, s32& outTriangleCount,
                                     const core::matrix4* transform)
{
	s32 cnt = Triangles.size();
	if (cnt > arraySize)
		cnt = arraySize;

	core::matrix4 mat;

	if (transform)
		mat = (*transform);

	if (SceneNode)
		mat *= SceneNode->getAbsoluteTransformation();

	for (s32 i=0; i<cnt; ++i)
	{
		triangles[i] = Triangles[i];
		mat.transformVect(triangles[i].pointA);
		mat.transformVect(triangles[i].pointB);
		mat.transformVect(triangles[i].pointC);
	}

	outTriangleCount = cnt;
}



//! Gets all triangles which lie within a specific bounding box.
void CTriangleSelector::getTriangles(core::triangle3df* triangles,
                                     s32 arraySize, s32& outTriangleCount,
                                     const core::aabbox3d<f32>& box,
                                     const core::matrix4* transform)
{
	// return all triangles
	getTriangles(triangles, arraySize, outTriangleCount, transform);
}


//! Gets all triangles which have or may have contact with a 3d line.
void CTriangleSelector::getTriangles(core::triangle3df* triangles, s32 arraySize,
                                     s32& outTriangleCount, const core::line3d<f32>& line,
                                     const core::matrix4* transform)
{
	// return all triangles
	getTriangles(triangles, arraySize, outTriangleCount, transform);
}


//! Returns amount of all available triangles in this selector
s32 CTriangleSelector::getTriangleCount() const
{
	return Triangles.size();
}



} // end namespace scene
} // end namespace engine

