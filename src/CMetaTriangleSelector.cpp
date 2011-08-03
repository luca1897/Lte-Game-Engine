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
 
#include "CMetaTriangleSelector.h"

namespace engine
{
namespace scene
{

//! constructor
CMetaTriangleSelector::CMetaTriangleSelector()
{
	#ifdef _DEBUG
	setDebugName("CMetaTriangleSelector");
	#endif
}

//! destructor
CMetaTriangleSelector::~CMetaTriangleSelector()
{
	removeAllTriangleSelectors();
}



//! Returns amount of all available triangles in this selector
s32 CMetaTriangleSelector::getTriangleCount() const
{
	s32 count = 0;
	for (s32 i=0; i<(s32)TriangleSelectors.size(); ++i)
		count += TriangleSelectors[i]->getTriangleCount();

	return count;
}



//! Gets all triangles.
void CMetaTriangleSelector::getTriangles(core::triangle3df* triangles, s32 arraySize,
                                         s32& outTriangleCount, const core::matrix4* transform)
{
	s32 outWritten = 0;

	for (s32 i=0; i<(s32)TriangleSelectors.size(); ++i)
	{
		s32 t = 0;
		TriangleSelectors[i]->getTriangles(triangles + outWritten, arraySize - outWritten, t, transform);
		outWritten += t;
	}

	outTriangleCount = outWritten;
}



//! Gets all triangles which lie within a specific bounding box.
void CMetaTriangleSelector::getTriangles(core::triangle3df* triangles, s32 arraySize,
                                         s32& outTriangleCount, const core::aabbox3d<f32>& box,
                                         const core::matrix4* transform)
{
	s32 outWritten = 0;

	for (s32 i=0; i<(s32)TriangleSelectors.size(); ++i)
	{
		s32 t = 0;
		TriangleSelectors[i]->getTriangles(triangles + outWritten, arraySize - outWritten, t,
		                                   box, transform);
		outWritten += t;
	}

	outTriangleCount = outWritten;
}



//! Gets all triangles which have or may have contact with a 3d line.
void CMetaTriangleSelector::getTriangles(core::triangle3df* triangles, s32 arraySize,
                                         s32& outTriangleCount, const core::line3d<f32>& line,
                                         const core::matrix4* transform)
{
	s32 outWritten = 0;

	for (s32 i=0; i<(s32)TriangleSelectors.size(); ++i)
	{
		s32 t = 0;
		TriangleSelectors[i]->getTriangles(triangles + outWritten, arraySize - outWritten, t,
		                                   line, transform);
		outWritten += t;
	}

	outTriangleCount = outWritten;
}



//! Adds a triangle selector to the collection of triangle selectors
//! in this metaTriangleSelector.
void CMetaTriangleSelector::addTriangleSelector(ITriangleSelector* toAdd)
{
	if (!toAdd)
		return;

	TriangleSelectors.push_back(toAdd);
	toAdd->grab();
}



//! Removes a specific triangle selector which was added before	from the collection.
bool CMetaTriangleSelector::removeTriangleSelector(ITriangleSelector* toRemove)
{
	for (s32 i=0; i<(s32)TriangleSelectors.size(); ++i)
		if (toRemove == TriangleSelectors[i])
		{
			TriangleSelectors[i]->drop();
			TriangleSelectors.erase(i);
			return true;
		}

	return false;
}



//! Removes all triangle selectors from the collection.
void CMetaTriangleSelector::removeAllTriangleSelectors()
{
	for (s32 i=0; i<(s32)TriangleSelectors.size(); ++i)
		TriangleSelectors[i]->drop();

	TriangleSelectors.clear();
}



} // end namespace scene
} // end namespace engine

