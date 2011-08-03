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
 
#ifndef __C_PARTICLE_BOX_EMITTER_H_INCLUDED__
#define __C_PARTICLE_BOX_EMITTER_H_INCLUDED__

#include "IParticleEmitter.h"
#include "engineArray.h"
#include "aabbox3d.h"

namespace engine
{
namespace scene
{

//! A default box emitter
class CParticleBoxEmitter : public IParticleEmitter
{
public:

//! constructor
CParticleBoxEmitter(
        core::aabbox3d<f32> box,
        core::vector3df direction = core::vector3df(0.0f,0.03f,0.0f),
        u32 minParticlesPerSecond = 20,
        u32 maxParticlePerSecond = 40,
        video::SColor minStartColor = video::SColor(255,0,0,0),
        video::SColor maxStartColor = video::SColor(255,255,255,255),
        u32 lifeTimeMin=2000,
        u32 lifeTimeMax=4000,
        s32 maxAngleDegrees=0);

//! Prepares an array with new particles to emitt into the system
//! and returns how much new particles there are.
virtual s32 emitt(u32 now, u32 timeSinceLastCall, SParticle*& outArray);

private:

core::array<SParticle> Particles;
core::aabbox3d<f32> Box;
core::vector3df Direction;
u32 MinParticlesPerSecond, MaxParticlesPerSecond;
video::SColor MinStartColor, MaxStartColor;
u32 MinLifeTime, MaxLifeTime;

u32 Time;
u32 Emitted;
s32 MaxAngleDegrees;
};

} // end namespace scene
} // end namespace engine


#endif

