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
 
#ifndef __C_SCENE_NODE_ANIMATOR_FOLLOW_SPLINE_H_INCLUDED__
#define __C_SCENE_NODE_ANIMATOR_FOLLOW_SPLINE_H_INCLUDED__

#include "ISceneNode.h"
#include "engineArray.h"

namespace engine
{
namespace scene
{
//! Scene node animator based free code Matthias Gall wrote and sent in. (Most of
//! this code is written by him, I only modified bits.)
class CSceneNodeAnimatorFollowSpline : public ISceneNodeAnimator
{
public:

//! constructor
CSceneNodeAnimatorFollowSpline(u32 startTime,
                               const core::array< core::vector3df >& points,
                               f32 speed = 1.0f, f32 tightness = 0.5f);

//! destructor
virtual ~CSceneNodeAnimatorFollowSpline();

//! animates a scene node
virtual void animateNode(ISceneNode* node, u32 timeMs);

protected:

//! clamps a the value idx to fit into range 0..size-1
s32 clamp(s32 idx, s32 size);

core::array< core::vector3df > Points;
f32 Speed;
f32 Tightness;
u32 StartTime;
u32 NumPoints;
};


} // end namespace scene
} // end namespace engine

#endif

