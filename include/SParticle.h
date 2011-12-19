/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __S_PARTICLE_H_INCLUDED__
#define __S_PARTICLE_H_INCLUDED__

#include "vector3d.h"
#include "SColor.h"

namespace engine
{
namespace scene
{
	//! Struct for holding particle data
	struct SParticle
	{
		//! Position of the particle
		core::vector3df pos;

		//! Direction and speed of the particle
		core::vector3df vector;

		//! Start life time of the particle
		u32 startTime;

		//! End life time of the particle
		u32 endTime;

		//! Current color of the particle
		video::SColor color;

		//! Original color of the particle. That's the color
		//! of the particle it had when it was emitted.
		video::SColor startColor;

		//! Original direction and speed of the particle, 
		//! the direction and speed the particle had when
		//! it was emitted.
		core::vector3df startVector;
	};


} // end namespace scene
} // end namespace engine

#endif


