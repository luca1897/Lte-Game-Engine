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
 
#ifndef __C_GUI_SCROLL_BAR_H_INCLUDED__
#define __C_GUI_SCROLL_BAR_H_INCLUDED__


#include "IGUIScrollBar.h"
#include "IGUIButton.h"

namespace engine
{
namespace gui
{

class CGUIScrollBar : public IGUIScrollBar
{
public:

//! constructor
CGUIScrollBar(bool horizontal, IGUIEnvironment* environment,
              IGUIElement* parent, s32 id, core::rect<s32> rectangle,
              bool noclip=false);

//! destructor
~CGUIScrollBar();

//! called if an event happened.
virtual bool OnEvent(SEvent event);

//! draws the element and its children
virtual void draw();

//! sets the position of the scrollbar
virtual void setPos(s32 pos);

//! sets the maximum value of the scrollbar. must be > 0
virtual void setMax(s32 max);

//! gets the current position of the scrollbar
virtual s32 getPos();

private:

void setPosFromMousePos(s32 x, s32 y);

IGUIButton* UpButton;
IGUIButton* DownButton;

bool Dragging;
bool Horizontal;
bool NoClip;
s32 Pos;
s32 DrawPos;
s32 DrawHeight;
s32 Max;
s32 SmallStep;


};

} // end namespace gui
} // end namespace engine

#endif

