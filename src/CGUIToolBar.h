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
 
#ifndef __C_GUI_TOOL_BAR_H_INCLUDED__
#define __C_GUI_TOOL_BAR_H_INCLUDED__

#include "IGUIToolbar.h"

namespace engine
{
namespace gui
{

//! Stays at the top of its parent like the menu bar and contains tool buttons
class CGUIToolBar : public IGUIToolBar
{
public:

//! constructor
CGUIToolBar(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

//! destructor
~CGUIToolBar();

//! draws the element and its children
virtual void draw();

//! Updates the absolute position.
virtual void updateAbsolutePosition();

//! Adds a button to the tool bar
virtual IGUIButton*     addButton(s32 id=-1, const wchar_t* text=0,
                                  video::ITexture* img=0, video::ITexture* pressed=0,
                                  bool isPushButton=false, bool useAlphaChannel=false);

private:

s32 ButtonX;
};


} // end namespace gui
} // end namespace engine

#endif

