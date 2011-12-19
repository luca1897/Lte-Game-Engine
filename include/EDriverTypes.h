/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __E_DRIVER_TYPES_H_INCLUDED__
#define __E_DRIVER_TYPES_H_INCLUDED__

namespace engine
{
namespace video  
{
	//! Unused
	enum E_DRIVER_TYPE
	{
		EDT_NULL,     
		EDT_SOFTWARE, 
		EDT_SOFTWARE2,
		EDT_DIRECT3D8, 
		EDT_DIRECT3D9,
		EDT_OPENGL    
	};

} // end namespace video
} // end namespace engine


#endif


