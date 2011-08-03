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
 
#ifndef __C_WATER_SURFACE_SCENE_NODE_H_INCLUDED__
#define __C_WATER_SURFACE_SCENE_NODE_H_INCLUDED__

#include "CMeshSceneNode.h"

namespace engine
{
namespace scene
{

class CWaterSurfaceSceneNode : public CMeshSceneNode
{
public:

//! constructor
CWaterSurfaceSceneNode(f32 waveHeight, f32 waveSpeed, f32 waveLength,
                       IMesh* mesh, ISceneNode* parent, ISceneManager* mgr,    s32 id,
                       const core::vector3df& position = core::vector3df(0,0,0),
                       const core::vector3df& rotation = core::vector3df(0,0,0),
                       const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

//! destructor
virtual ~CWaterSurfaceSceneNode();

//! frame
virtual void OnPreRender();

//! Returns type of the scene node
virtual ESCENE_NODE_TYPE getType() {
	return ESNT_WATER_SURFACE;
}

private:

void animateWaterSurface();
f32 WaveLength;
f32 WaveSpeed;
f32 WaveHeight;
IMesh* OriginalMesh;
};

} // end namespace scene
} // end namespace engine

#endif

