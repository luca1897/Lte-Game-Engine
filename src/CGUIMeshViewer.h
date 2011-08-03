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
 
#ifndef __C_GUI_MESH_VIEWER_H_INCLUDED__
#define __C_GUI_MESH_VIEWER_H_INCLUDED__

#include "IGUIMeshViewer.h"
#include "SMaterial.h"
#include "vector3d.h"

namespace engine
{

namespace gui
{

class CGUIMeshViewer : public IGUIMeshViewer
{
public:

//! constructor
CGUIMeshViewer(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

//! destructor
~CGUIMeshViewer();

//! sets the mesh to be shown
virtual void setMesh(scene::IAnimatedMesh* mesh);

//! sets the material
virtual void setMaterial(const video::SMaterial& material);

//! gets the material
virtual const video::SMaterial& getMaterial();

//! called if an event happened.
virtual bool OnEvent(SEvent event);

//! draws the element and its children
virtual void draw();


private:

video::SMaterial Material;
scene::IAnimatedMesh* Mesh;
};


} // end namespace gui
} // end namespace engine

#endif

