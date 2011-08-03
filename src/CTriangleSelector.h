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
 
#ifndef __C_TRIANGLE_SELECTOR_H_INCLUDED__
#define __C_TRIANGLE_SELECTOR_H_INCLUDED__

#include "ITriangleSelector.h"
#include "IMesh.h"

namespace engine
{
namespace scene
{

class ISceneNode;

//! Stupid triangle selector without optimization
class CTriangleSelector : public ITriangleSelector
{
public:

//! Constructs a selector based on a mesh
CTriangleSelector(ISceneNode* node);

//! Constructs a selector based on a mesh
CTriangleSelector(IMesh* mesh, ISceneNode* node);

//! Constructs a selector based on a bounding box
CTriangleSelector(core::aabbox3d<f32> box, ISceneNode* node);

~CTriangleSelector();

//! Gets all triangles.
void getTriangles(core::triangle3df* triangles, s32 arraySize, s32& outTriangleCount,
                  const core::matrix4* transform=0);

//! Gets all triangles which lie within a specific bounding box.
void getTriangles(core::triangle3df* triangles, s32 arraySize, s32& outTriangleCount,
                  const core::aabbox3d<f32>& box, const core::matrix4* transform=0);

//! Gets all triangles which have or may have contact with a 3d line.
virtual void getTriangles(core::triangle3df* triangles, s32 arraySize,
                          s32& outTriangleCount, const core::line3d<f32>& line,
                          const core::matrix4* transform=0);

//! Returns amount of all available triangles in this selector
virtual s32 getTriangleCount() const;

protected:

ISceneNode* SceneNode;
core::array<core::triangle3df> Triangles;
};

} // end namespace scene
} // end namespace engine


#endif

