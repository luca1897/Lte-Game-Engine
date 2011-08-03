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
 
#ifndef __C_OCT_TREE_H_INCLUDED__
#define __C_OCT_TREE_H_INCLUDED__

#include "SViewFrustrum.h"
#include "S3DVertex.h"
#include "aabbox3d.h"
#include "engineArray.h"
#include <string.h>

namespace engine
{

//! template octtree. T must be a vertex type which has a member
//! called .Pos, which is a core::vertex3df position.
template <class T>
class OctTree
{
public:

s32 nodeCount;

struct SMeshChunk
{
	core::array<T> Vertices;
	core::array<u16> Indices;
	s32 MaterialId;
};

struct SIndexChunk
{
	core::array<u16> Indices;
	s32 MaterialId;
};

struct SIndexData
{
	u16* Indices;
	s32 CurrentSize;
	s32 MaxSize;
};




//! constructor
OctTree(const core::array<SMeshChunk>& meshes, s32 minimalPolysPerNode=128)
{
	nodeCount = 0;

	IndexDataCount = meshes.size();
	IndexData = new SIndexData[IndexDataCount];
	//printf("octtree!\n");
	// construct array of all indices

	core::array<SIndexChunk>* indexChunks = new core::array<SIndexChunk>;
	SIndexChunk ic;
	//printf("octtree a!\n");
	for (u32 i=0; i<meshes.size(); ++i)
	{
		IndexData[i].CurrentSize = 0;
		IndexData[i].MaxSize = meshes[i].Indices.size();
		IndexData[i].Indices = new u16[IndexData[i].MaxSize];

		ic.MaterialId = meshes[i].MaterialId;
		(*indexChunks).push_back(ic);

		SIndexChunk& tic = (*indexChunks)[i];

		for (u32 t=0; t<meshes[i].Indices.size(); ++t)
			tic.Indices.push_back(meshes[i].Indices[t]);
	}
	//printf("octtree1 !\n");
	// create tree

	Root = new OctTreeNode(nodeCount, 0, meshes, indexChunks, minimalPolysPerNode);
}

//! returns all ids of polygons partially or full enclosed
//! by this bounding box.
void calculatePolys(const core::aabbox3d<f32>& box)
{
	for (s32 i=0; i<IndexDataCount; ++i)
		IndexData[i].CurrentSize = 0;

	Root->getPolys(box, IndexData);
}

//! returns all ids of polygons partially or full enclosed
//! by a view frustrum.
void calculatePolys(const scene::SViewFrustrum& frustrum)
{
	for (s32 i=0; i<IndexDataCount; ++i)
		IndexData[i].CurrentSize = 0;

	Root->getPolys(frustrum, IndexData);
}


SIndexData* getIndexData()
{
	return IndexData;
}

s32 getIndexDataCount()
{
	return IndexDataCount;
}

// for debug purposes only, renders the bounding boxes of the tree
void renderBoundingBoxes(const core::aabbox3d<f32>& box,
                         core::array< core::aabbox3d<f32> >&outBoxes)
{
	Root->renderBoundingBoxes(box, outBoxes);
}

//! destructor
~OctTree()
{
	for (s32 i=0; i<IndexDataCount; ++i)
		delete [] IndexData[i].Indices;

	delete [] IndexData;
	delete Root;
}

private:


// private inner class
class OctTreeNode
{
public:

// constructor
OctTreeNode(s32& nodeCount, s32 currentdepth,
            const core::array<SMeshChunk>& allmeshdata,
            core::array<SIndexChunk>* indices,
            s32 minimalPolysPerNode) : IndexData(0)
{
	depth = currentdepth+1;
	++nodeCount;
	//printf("octtreenode dddd\n");
	u32 i;                 // new ISO for scoping problem with different compilers

	for (i=0; i<8; ++i)
		Children[i] = 0;

	if ((*indices).empty())
	{
		delete indices;
		return;
	}

	bool found = false;

	// find first point for bounding box

	for (i=0; i<(*indices).size(); ++i)
		if (!(*indices)[i].Indices.empty())
		{
			Box.reset(allmeshdata[i].Vertices[(*indices)[i].Indices[0]].Pos);
			found = true;
			break;
		}

	if (!found)
	{
		delete indices;
		return;
	}
	//printf("octtreenode cccc\n");
	s32 totalPrimitives = 0;

	// now letz calculate our bounding box
	for (i=0; i<(*indices).size(); ++i)
	{
		totalPrimitives += (*indices)[i].Indices.size();
		for (u32 j=0; j<(*indices)[i].Indices.size(); ++j)
			Box.addInternalPoint(allmeshdata[i].Vertices[(*indices)[i].Indices[j]].Pos);
	}

	core::vector3df middle = Box.getCenter();
	core::vector3df edges[8];
	Box.getEdges(edges);

	// calculate all children
	core::aabbox3d<f32> box;
	//printf("octtreenode aaaa\n");
	if (totalPrimitives > minimalPolysPerNode && !Box.isEmpty())
		for (s32 ch=0; ch<8; ++ch)
		{
			box.reset(middle);
			box.addInternalPoint(edges[ch]);

			// create indices for child
			core::array<SIndexChunk>* cindexChunks = new core::array<SIndexChunk>;

			bool added = false;

			for (u32 i=0; i<allmeshdata.size(); ++i)
			{
				SIndexChunk ic;
				ic.MaterialId = allmeshdata[i].MaterialId;
				(*cindexChunks).push_back(ic);

				SIndexChunk& tic = (*cindexChunks)[i];

				for (u32 t=0; t<(*indices)[i].Indices.size(); t+=3)
				{
					if (box.isPointInside(allmeshdata[i].Vertices[(*indices)[i].Indices[t]].Pos) &&
					    box.isPointInside(allmeshdata[i].Vertices[(*indices)[i].Indices[t+1]].Pos) &&
					    box.isPointInside(allmeshdata[i].Vertices[(*indices)[i].Indices[t+2]].Pos))
					{
						tic.Indices.push_back((*indices)[i].Indices[t]);
						tic.Indices.push_back((*indices)[i].Indices[t+1]);
						tic.Indices.push_back((*indices)[i].Indices[t+2]);
						//printf("impallato\n");
						(*indices)[i].Indices.erase(t, 3);
						//printf("impallone\n");
						t-=3;

						added = true;
					}
				}
			}
//printf("octtreenode eeeee\n");
			if (added)
				Children[ch] = new OctTreeNode(nodeCount, depth,
				                               allmeshdata, cindexChunks, minimalPolysPerNode);
			else
				delete cindexChunks;

		}         // end for all possible children

	IndexData = indices;
}



// destructor
~OctTreeNode()
{
	delete IndexData;

	for (s32 i=0; i<8; ++i)
		delete Children[i];
}



// returns all ids of polygons partially or full enclosed
// by this bounding box.
void getPolys(const core::aabbox3d<f32>& box, SIndexData* idxdata)
{
	if (Box.intersectsWithBox(box))
	{
		s32 cnt = (*IndexData).size();
		s32 i;                 // new ISO for scoping problem in some compilers

		for (i=0; i<cnt; ++i)
		{
			s32 idxcnt = (*IndexData)[i].Indices.size();

			if (idxcnt)
			{
				memcpy(&idxdata[i].Indices[idxdata[i].CurrentSize],
				       &(*IndexData)[i].Indices[0], idxcnt * sizeof(s16));
				idxdata[i].CurrentSize += idxcnt;
			}
		}

		for (i=0; i<8; ++i)
			if (Children[i])
				Children[i]->getPolys(box, idxdata);
	}
}



// returns all ids of polygons partially or full enclosed
// by the view frustrum.
void getPolys(const scene::SViewFrustrum& frustrum, SIndexData* idxdata)
{
	s32 i;                 // new ISO for scoping problem in some compilers

	bool visible = true;

	core::vector3df edges[8];
	Box.getEdges(edges);

	for (i=0; i<scene::SViewFrustrum::VF_PLANE_COUNT; ++i)
	{
		bool boxInFrustrum = false;

		for (int j=0; j<8; ++j)
			if (frustrum.planes[i].classifyPointRelation(edges[j]) != core::ISREL3D_BACK)
			{
				boxInFrustrum = true;
				break;
			}

		if (!boxInFrustrum)
		{
			visible = false;
			break;
		}
	}

	if (visible)
	{
		s32 cnt = (*IndexData).size();

		for (i=0; i<cnt; ++i)
		{
			s32 idxcnt = (*IndexData)[i].Indices.size();

			if (idxcnt)
			{
				memcpy(&idxdata[i].Indices[idxdata[i].CurrentSize],
				       &(*IndexData)[i].Indices[0], idxcnt * sizeof(s16));
				idxdata[i].CurrentSize += idxcnt;
			}
		}
	}

	for (i=0; i<8; ++i)
		if (Children[i])
			Children[i]->getPolys(frustrum, idxdata);
}



void renderBoundingBoxes(const core::aabbox3d<f32>& box,
                         core::array< core::aabbox3d<f32> >&outBoxes)
{
	if (Box.intersectsWithBox(box))
	{
		outBoxes.push_back(Box);

		for (s32 i=0; i<8; ++i)
			if (Children[i])
				Children[i]->renderBoundingBoxes(box, outBoxes);
	}
}

private:

core::aabbox3d<f32> Box;
core::array<SIndexChunk>* IndexData;
OctTreeNode* Children[8];
s32 depth;
};


OctTreeNode* Root;
SIndexData* IndexData;
s32 IndexDataCount;
};

} // end namespace

#endif

