/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __engine_MATH_H_INCLUDED__
#define __engine_MATH_H_INCLUDED__

#include "engineTypes.h"

namespace engine
{
namespace core
{
	//! Rounding error constant often used when comparing f32 values.
	const f32 ROUNDING_ERROR	= 0.0001f;

	//! Constant for PI.
	const f32 PI				= 3.1415926535897932384626433832795f;

	//! Constant for converting bettween degrees and radiants.
	const f64 GRAD_PI		= 180.0f / 3.1415926535897932384626433832795f;

	//! Constant for converting bettween degrees and radiants.
	const f64 GRAD_PI2		= 3.1415926535897932384626433832795f / 180.0;

	//! returns minimum of two values. Own implementation to get rid of the STL (VS6 problems)
	template<class T>
	inline const T min_(const T a, const T b)
	{
		return a < b ? a : b;
	}
	
	//! returns minimum of two values. Own implementation to get rid of the STL (VS6 problems)
	template<class T>
	inline T max_(const T a, const T b)
	{
		return a < b ? b : a;
	}

	//! returns abs of two values. Own implementation to get rid of STL (VS6 problems)
	template<class T>
	inline T abs_(const T a)
	{
		return a < 0 ? -a : a;
	}

	//! returns if a float equals the other one, taking floating 
	//! point rounding errors into account
	inline bool equals(f32 a, f32 b)
	{
		return (a + ROUNDING_ERROR > b) && (a - ROUNDING_ERROR < b);
	}

} // end namespace core
}// end namespace engine

#endif


