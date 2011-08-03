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
 
// Copyright (C) 2002-2006 Nikolaus Gebhardt/Alten Thomas
// This file is part of the "engine Engine".
// For conditions of distribution and use, see copyright notice in engine.h

#ifndef __S_VIDEO_2_SOFTWARE_COMPILE_CONFIG_H_INCLUDED__
#define __S_VIDEO_2_SOFTWARE_COMPILE_CONFIG_H_INCLUDED__

#include "engineCompileConfig.h"


// Generic Render Flags for apfelsoft rasterizer

#define APFELSOFT_RENDERER_BEAUTIFUL

#ifdef APFELSOFT_RENDERER_BEAUTIFUL
        #define SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
        #define SOFTWARE_DRIVER_2_SUBTEXEL
        #define SOFTWARE_DRIVER_2_BILINEAR
        #define SOFTWARE_DRIVER_2_LIGHTING
        #define SOFTWARE_DRIVER_2_32BIT
#endif

//#define SOFTWARE_DRIVER_2_STATISTIC
#endif
