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
 
#ifndef __C_SOFTWARE2_MATERIAL_RENDERER_H_INCLUDED__
#define __C_SOFTWARE2_MATERIAL_RENDERER_H_INCLUDED__

#include "SoftwareDriver2_compile_config.h"

#include "IMaterialRenderer.h"
#include "CSoftwareDriver2.h"

namespace engine
{
namespace video
{

//! Base class for all internal Software2 material renderers
class CSoftware2MaterialRenderer : public IMaterialRenderer
{
public:

//! Constructor
CSoftware2MaterialRenderer(video::CSoftwareDriver2* driver)
	: Driver(driver)
{
}

protected:

video::CSoftwareDriver2* Driver;
};

//! solid material renderer
class CSoftware2MaterialRenderer_SOLID : public CSoftware2MaterialRenderer
{
public:
CSoftware2MaterialRenderer_SOLID ( video::CSoftwareDriver2* driver )
	: CSoftware2MaterialRenderer ( driver ) {
}

//! Returns if the material is transparent.
virtual bool isTransparent()
{
	return false;
}

};


//! Transparent material renderer
class CSoftware2MaterialRenderer_TRANSPARENT_ADD_COLOR : public CSoftware2MaterialRenderer
{
public:
CSoftware2MaterialRenderer_TRANSPARENT_ADD_COLOR ( video::CSoftwareDriver2* driver )
	: CSoftware2MaterialRenderer ( driver ) {
}


//! Returns if the material is transparent.
virtual bool isTransparent()
{
	return true;
}

};

//! unsupported material renderer
class CSoftware2MaterialRenderer_UNSUPPORTED : public CSoftware2MaterialRenderer
{
public:
CSoftware2MaterialRenderer_UNSUPPORTED ( video::CSoftwareDriver2* driver )
	: CSoftware2MaterialRenderer ( driver ) {
}

virtual s32 getRenderCapability() {
	return 1;
}

};


} // end namespace video
} // end namespace engine

#endif

