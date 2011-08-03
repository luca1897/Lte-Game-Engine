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
 
#ifndef __C_GUI_IN_OUT_FADER_H_INCLUDED__
#define __C_GUI_IN_OUT_FADER_H_INCLUDED__

#include "IGUIInOutFader.h"

namespace engine
{
namespace gui
{

class CGUIInOutFader : public IGUIInOutFader
{
public:

//! constructor
CGUIInOutFader(IGUIEnvironment* environment, IGUIElement* parent,
               s32 id, core::rect<s32> rectangle);

//! destructor
~CGUIInOutFader();

//! draws the element and its children
virtual void draw();

//! Gets the color to fade out to or to fade in from.
virtual video::SColor getColor() const;

//! Sets the color to fade out to or to fade in from.
virtual void setColor(video::SColor color);

//! Starts the fade in process.
virtual void fadeIn(u32 time);

//! Starts the fade out process.
virtual void fadeOut(u32 time);

//! Returns if the fade in or out process is done.
virtual bool isReady() const;

private:

enum EFadeAction
{
	EFA_NOTHING = 0,
	EFA_FADE_IN,
	EFA_FADE_OUT
};

u32 StartTime;
u32 EndTime;
EFadeAction Action;

video::SColor Color;
video::SColor FullColor;
video::SColor TransColor;

};

} // end namespace gui
} // end namespace engine

#endif

