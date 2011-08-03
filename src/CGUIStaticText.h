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
 
#ifndef __C_GUI_STATIC_TEXT_H_INCLUDED__
#define __C_GUI_STATIC_TEXT_H_INCLUDED__

#include "IGUIStaticText.h"
#include "engineArray.h"

namespace engine
{
namespace gui
{
class CGUIStaticText : public IGUIStaticText
{
public:

//! constructor
CGUIStaticText(const wchar_t* text, bool border, IGUIEnvironment* environment,
               IGUIElement* parent, s32 id, const core::rect<s32>& rectangle,
               bool background = false);

//! destructor
~CGUIStaticText();

//! draws the element and its children
virtual void draw();

//! Sets another skin independent font.
virtual void setOverrideFont(IGUIFont* font=0);

//! Sets another color for the text.
virtual void setOverrideColor(video::SColor color);

//! Sets if the static text should use the overide color or the
//! color in the gui skin.
virtual void enableOverrideColor(bool enable);

//! Enables or disables word wrap for using the static text as
//! multiline text control.
virtual void setWordWrap(bool enable);

//! Sets the new caption of this element.
virtual void setText(const wchar_t* text);

//! Returns the height of the text in pixels when it is drawn.
virtual s32 getTextHeight();

private:

//! Breaks the single text line.
void breakText();

bool Border;
bool OverrideColorEnabled;
bool WordWrap;
bool Background;

video::SColor OverrideColor;
gui::IGUIFont* OverrideFont;
gui::IGUIFont* LastBreakFont;                 // stored because: if skin changes, line break must be recalculated.

core::array< core::stringw > BrokenText;
};

} // end namespace gui
} // end namespace engine

#endif

