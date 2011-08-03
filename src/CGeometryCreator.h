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
 
#ifndef __C_GEOMETRY_CREATOR_H_INCLUDED__
#define __C_GEOMETRY_CREATOR_H_INCLUDED__

#include "IAnimatedMesh.h"
#include "IImage.h"



namespace engine
{
namespace video
{
class IVideoDriver;
}

namespace scene
{

//! class for creating geometry on the fly
class CGeometryCreator
{
public:

static IAnimatedMesh* createHillPlaneMesh(
        const core::dimension2d<f32>& tileSize, const core::dimension2d<s32>& tileCount,
        video::SMaterial* material,     f32 hillHeight, const core::dimension2d<f32>& countHills,
        const core::dimension2d<f32>& textureRepeatCount);

static IAnimatedMesh* createTerrainMesh(video::IImage* texture,
                                        video::IImage* heightmap, const core::dimension2d<f32>& stretchSize, f32 maxHeight,
                                        video::IVideoDriver* driver,
                                        const core::dimension2d<s32> defaultVertexBlockSize,
                                        bool debugBorders=false);
};


} // end namespace scene
} // end namespace engine

#endif

