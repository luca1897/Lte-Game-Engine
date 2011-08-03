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
 
#ifndef __C_GUI_EDIT_BOX_H_INCLUDED__
#define __C_GUI_EDIT_BOX_H_INCLUDED__

#include "IGUIEditBox.h"
#include "engineArray.h"
#include "IOSOperator.h"

namespace engine
{
namespace gui
{
class CGUIEditBox : public IGUIEditBox
{
public:

//! constructor
CGUIEditBox(const wchar_t* text, bool border, IGUIEnvironment* environment,
            IGUIElement* parent, s32 id, const core::rect<s32>& rectangle,
            IOSOperator* op);

//! destructor
~CGUIEditBox();

//! Sets another skin independent font.
virtual void setOverrideFont(IGUIFont* font=0);

//! Sets another color for the text.
virtual void setOverrideColor(video::SColor color);

//! Sets if the text should use the overide color or the
//! color in the gui skin.
virtual void enableOverrideColor(bool enable);

//! called if an event happened.
virtual bool OnEvent(SEvent event);

//! draws the element and its children
virtual void draw();

//! Sets the new caption of this element.
virtual void setText(const wchar_t* text);

//! Sets the maximum amount of characters which may be entered in the box.
//! \param max: Maximum amount of characters. If 0, the character amount is
//! infinity.
virtual void setMax(s32 max);

//! Returns maximum amount of characters, previously set by setMax();
virtual s32 getMax();

protected:

bool processKey(SEvent& event);
bool processMouse(SEvent& event);
s32 getCursorPos(s32 x);

bool MouseMarking;
bool Border;
bool OverrideColorEnabled;
bool WordWrap;
s32 MarkBegin;
s32 MarkEnd;

video::SColor OverrideColor;
gui::IGUIFont* OverrideFont;
IOSOperator* Operator;

u32 BlinkStartTime;
s32 CursorPos;
s32 ScrollPos;                 // scrollpos in characters
s32 Max;
};

} // end namespace gui
} // end namespace engine

#endif

