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
 
#ifndef __C_SCENE_NODE_ANIMATOR_TEXTURE_H_INCLUDED__
#define __C_SCENE_NODE_ANIMATOR_TEXTURE_H_INCLUDED__

#include "engineArray.h"
#include "ISceneNode.h"

namespace engine
{
namespace scene
{
class CSceneNodeAnimatorTexture : public ISceneNodeAnimator
{
public:

//! constructor
CSceneNodeAnimatorTexture(const core::array<video::ITexture*>& textures,
                          s32 timePerFrame, bool loop, u32 now);

//! destructor
virtual ~CSceneNodeAnimatorTexture();

//! animates a scene node
virtual void animateNode(ISceneNode* node, u32 timeMs);

private:

core::array<video::ITexture*> Textures;
u32 TimePerFrame;
u32 StartTime;
u32 EndTime;
bool Loop;
};


} // end namespace scene
} // end namespace engine

#endif

