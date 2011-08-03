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
 
#ifndef __S_K_2D_VERTEX_H_INCLUDED__
#define __S_K_2D_VERTEX_H_INCLUDED__

#include "vector2d.h"

typedef signed short TZBufferType;

namespace engine
{
namespace video
{

struct S2DVertex
{
	core::vector2d<s32> Pos;                        // position
	core::vector2d<s32> TCoords;            // texture coordinates
	TZBufferType ZValue;                            // zvalue
	s16 Color;
};


} // end namespace video
} // end namespace engine

#endif

