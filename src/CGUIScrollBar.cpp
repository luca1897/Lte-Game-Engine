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
 
#include "CGUIScrollBar.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "GUIIcons.h"
#include "CGUIButton.h"

namespace engine
{
namespace gui
{


//! constructor
CGUIScrollBar::CGUIScrollBar(bool horizontal, IGUIEnvironment* environment,
                             IGUIElement* parent, s32 id, core::rect<s32> rectangle,
                             bool noclip)
	: IGUIScrollBar(environment, parent, id, rectangle), UpButton(0), DownButton(0),
	Horizontal(horizontal), Pos(0), Max(100), SmallStep(10), DrawHeight(0),
	DrawPos(0), Dragging(false), NoClip(noclip)
{
	#ifdef _DEBUG
	setDebugName("CGUIScrollBar");
	#endif

	if (horizontal)
	{
		s32 h = RelativeRect.getHeight();

		UpButton = new CGUIButton(Environment, this, -1, core::rect<s32>(0,0, h, h), NoClip);
		UpButton->setText(GUI_ICON_CURSOR_LEFT);
		UpButton->drop();
		DownButton = new CGUIButton(Environment, this, -1, core::rect<s32>(RelativeRect.getWidth()-h, 0, RelativeRect.getWidth(), h), NoClip);
		DownButton->setText(GUI_ICON_CURSOR_RIGHT);
		DownButton->drop();
	}
	else
	{
		s32 w = RelativeRect.getWidth();
		UpButton = new CGUIButton(Environment, this, -1, core::rect<s32>(0,0, w, w), NoClip);
		UpButton->setText(GUI_ICON_CURSOR_UP);
		UpButton->drop();
		DownButton = new CGUIButton(Environment, this, -1, core::rect<s32>(0,RelativeRect.getHeight()-w, w, RelativeRect.getHeight()), NoClip);
		DownButton->setText(GUI_ICON_CURSOR_DOWN);
		DownButton->drop();
	}

	if (UpButton)
	{
		UpButton->setOverrideFont(Environment->getBuiltInFont());
		UpButton->grab();
	}

	if (DownButton)
	{
		DownButton->setOverrideFont(Environment->getBuiltInFont());
		DownButton->grab();
	}

	setPos(0);
}


//! destructor
CGUIScrollBar::~CGUIScrollBar()
{
	if (UpButton)
		UpButton->drop();

	if (DownButton)
		DownButton->drop();
}


//! called if an event happened.
bool CGUIScrollBar::OnEvent(SEvent event)
{

	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
		{
			if (event.GUIEvent.Caller == UpButton)
				setPos(Pos-SmallStep);
			else
			if (event.GUIEvent.Caller == DownButton)
				setPos(Pos+SmallStep);

			SEvent event;
			event.EventType = EET_GUI_EVENT;
			event.GUIEvent.Caller = this;
			event.GUIEvent.EventType = EGET_SCROLL_BAR_CHANGED;
			Parent->OnEvent(event);

			return true;
		}
		else
		if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
		{
			Dragging = false;
			return true;
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		switch(event.MouseInput.Event)
		{
		case EMIE_MOUSE_WHEEL:
		{         // thanks to a bug report by REAPER
			setPos(getPos() + (s32)event.MouseInput.Wheel* -10);
			SEvent event;
			event.EventType = EET_GUI_EVENT;
			event.GUIEvent.Caller = this;
			event.GUIEvent.EventType = EGET_SCROLL_BAR_CHANGED;
			Parent->OnEvent(event);
		}
			return true;
		case EMIE_LMOUSE_PRESSED_DOWN:
			Dragging = true;
			Environment->setFocus(this);
			return true;
		case EMIE_LMOUSE_LEFT_UP:
			Dragging = false;
			Environment->removeFocus(this);
			return true;
		case EMIE_MOUSE_MOVED:
			if (Dragging)
			{
				s32 oldPos = Pos;
				setPosFromMousePos(event.MouseInput.X, event.MouseInput.Y);
				if (Pos != oldPos && Parent)
				{
					SEvent event;
					event.EventType = EET_GUI_EVENT;
					event.GUIEvent.Caller = this;
					event.GUIEvent.EventType = EGET_SCROLL_BAR_CHANGED;
					Parent->OnEvent(event);
				}
				return true;
			}
		}
		break;
	}

	return Parent ? Parent->OnEvent(event) : false;
}

#include <pspdebug.h>
#define printf pspDebugScreenPrintf
//! draws the element and its children
void CGUIScrollBar::draw()
{
//	printf("""1\n");
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;

	engine::video::IVideoDriver* driver = Environment->getVideoDriver();
//printf("""2\n");
	core::rect<s32> rect = AbsoluteRect;
	core::rect<s32>* clip = 0;
//printf("""2\n");
	if (!NoClip)
		clip = &AbsoluteClippingRect;
//printf("""3\n");
	// draws the background
	driver->draw2DRectangle(skin->getColor(EGDC_SCROLLBAR), rect, clip);
//printf("""4\n");
	if (Max!=0)
	{
		// draw thumb
		if (Horizontal)
		{
			rect.UpperLeftCorner.X = AbsoluteRect.UpperLeftCorner.X + DrawPos + RelativeRect.getHeight() - DrawHeight/2;
			rect.LowerRightCorner.X = rect.UpperLeftCorner.X + DrawHeight;
		}
		else
		{
			rect.UpperLeftCorner.Y = AbsoluteRect.UpperLeftCorner.Y + DrawPos + RelativeRect.getWidth() - DrawHeight/2;
			rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + DrawHeight;
		}
//		printf("""Is horizontal: %d\n", Horizontal);

		//	printf("""%d\n", rect.UpperLeftCorner.X);
		//	printf("""%d\n", rect.LowerRightCorner.X);

		skin->draw3DButtonPaneStandard(this, rect, clip);
	}
//printf("""5\n");
	// draw buttons
	IGUIElement::draw();
	//printf("""6\n");
}



void CGUIScrollBar::setPosFromMousePos(s32 x, s32 y)
{
	if (Max == 0)
		return;

	if (Horizontal)
	{
		f32 f = (RelativeRect.getWidth() - ((f32)RelativeRect.getHeight()*3.0f)) / (f32)Max;
		setPos((s32)(((f32)(x - AbsoluteRect.UpperLeftCorner.X - RelativeRect.getHeight())) / f));
	}
	else
	{
		f32 f = (RelativeRect.getHeight() - ((f32)RelativeRect.getWidth()*3.0f)) / (f32)Max;
		setPos((s32)(((f32)y - AbsoluteRect.UpperLeftCorner.Y - RelativeRect.getWidth()) / f));
	}
}



//! sets the position of the scrollbar
void CGUIScrollBar::setPos(s32 pos)
{
	if (Max == 0)
		return;
	Pos = pos;
	if (Pos < 0)
		Pos = 0;
	if (Pos > Max)
		Pos = Max;

	if (Horizontal)
	{
		f32 f = (RelativeRect.getWidth() - ((f32)RelativeRect.getHeight()*3.0f)) / (f32)Max;
		DrawPos = (s32)((Pos * f) + ((f32)RelativeRect.getHeight() * 0.5f));
		DrawHeight = RelativeRect.getHeight();
	}
	else
	{
		f32 f = 0.0f;
		if (Max != 0)
			f = (RelativeRect.getHeight() - ((f32)RelativeRect.getWidth()*3.0f)) / (f32)Max;

		DrawPos = (s32)((Pos * f) + ((f32)RelativeRect.getWidth() * 0.5f));
		DrawHeight = RelativeRect.getWidth();
	}
}



//! sets the maximum value of the scrollbar. must be > 0
void CGUIScrollBar::setMax(s32 max)
{
	if (max > 0)
		Max = max;
	else
		Max = 0;

	bool enable = (Max != 0);
	UpButton->setEnabled(enable);
	DownButton->setEnabled(enable);
	setPos(Pos);
}



//! gets the current position of the scrollbar
s32 CGUIScrollBar::getPos()
{
	return Pos;
}


} // end namespace gui
} // end namespace engine

