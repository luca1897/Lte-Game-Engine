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
 
#include "CGUITabControl.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IGUIFont.h"
#include "IVideoDriver.h"
#include "rect.h"
#include "os.h"

namespace engine
{
namespace gui
{

// ------------------------------------------------------------------
// Tab
// ------------------------------------------------------------------

//! constructor
CGUITab::CGUITab(s32 number, IGUIEnvironment* environment,
                 IGUIElement* parent, const core::rect<s32>& rectangle,
                 s32 id)
	: IGUITab(environment, parent, id, rectangle), Number(number),
	BackColor(0,0,0,0), DrawBackground(false)
{
	#ifdef _DEBUG
	setDebugName("CGUITab");
	#endif
}



//! destructor
CGUITab::~CGUITab()
{
}



//! Returns number of tab in tabcontrol. Can be accessed
//! later IGUITabControl::getTab() by this number.
s32 CGUITab::getNumber()
{
	return Number;
}


//! Sets the number
void CGUITab::setNumber(s32 n)
{
	Number = n;
}


//! draws the element and its children
void CGUITab::draw()
{
	if (!IsVisible)
		return;

	video::IVideoDriver* driver = Environment->getVideoDriver();

	if (DrawBackground)
		driver->draw2DRectangle(BackColor, AbsoluteRect, &AbsoluteClippingRect);

	IGUIElement::draw();
}

//! sets if the tab should draw its background
void CGUITab::setDrawBackground(bool draw)
{
	DrawBackground = draw;
}

//! sets the color of the background, if it should be drawn.
void CGUITab::setBackgroundColor(video::SColor c)
{
	BackColor = c;
}

// ------------------------------------------------------------------
// Tabcontrol
// ------------------------------------------------------------------

//! destructor
CGUITabControl::CGUITabControl(IGUIEnvironment* environment,
                               IGUIElement* parent, const core::rect<s32>& rectangle,
                               bool fillbackground, bool border, s32 id)
	: IGUITabControl(environment, parent, id, rectangle), ActiveTab(-1),
	Border(border), FillBackground(fillbackground)
{
}



//! destructor
CGUITabControl::~CGUITabControl()
{
	for (s32 i=0; i<(s32)Tabs.size(); ++i)
		if (Tabs[i])
			Tabs[i]->drop();
}


//! Adds a tab
IGUITab* CGUITabControl::addTab(wchar_t* caption, s32 id)
{
	IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return 0;

	s32 tabheight = skin->getSize(gui::EGDS_BUTTON_HEIGHT) + 2;
	core::rect<s32> r(1,tabheight,
	                  AbsoluteRect.getWidth()-1,
	                  AbsoluteRect.getHeight()-1);

	CGUITab* tab = new CGUITab(Tabs.size(), Environment, this,
	                           r, id);

	tab->setText(caption);
	tab->setVisible(false);
	Tabs.push_back(tab);

	if (ActiveTab == -1)
	{
		ActiveTab = 0;
		tab->setVisible(true);
	}

	return tab;
}


//! Returns amount of tabs in the tabcontrol
s32 CGUITabControl::getTabcount()
{
	return Tabs.size();
}



//! Returns a tab based on zero based index
IGUITab* CGUITabControl::getTab(s32 idx)
{
	if (idx < 0 || idx >= (s32)Tabs.size())
		return 0;

	return Tabs[idx];
}



//! called if an event happened.
bool CGUITabControl::OnEvent(SEvent event)
{
	if (!IsEnabled)
		return Parent ? Parent->OnEvent(event) : false;

	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		switch(event.GUIEvent.EventType)
		{
		case gui::EGET_ELEMENT_FOCUS_LOST:
			return true;
			break;
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		switch(event.MouseInput.Event)
		{
		case EMIE_LMOUSE_PRESSED_DOWN:
			Environment->setFocus(this);
			return true;
		case EMIE_LMOUSE_LEFT_UP:
			Environment->removeFocus(this);
			selectTab(core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y));
			return true;
		}
		break;
	}

	return Parent ? Parent->OnEvent(event) : false;
}

void CGUITabControl::selectTab(core::position2d<s32> p)
{
	IGUISkin* skin = Environment->getSkin();
	IGUIFont* font = skin->getFont();

	core::rect<s32> frameRect(AbsoluteRect);

	s32 tabheight = skin->getSize(gui::EGDS_BUTTON_HEIGHT);
	frameRect.UpperLeftCorner.Y += 2;
	frameRect.LowerRightCorner.Y = frameRect.UpperLeftCorner.Y + tabheight;
	s32 pos = frameRect.UpperLeftCorner.X + 2;

	for (s32 i=0; i<(s32)Tabs.size(); ++i)
	{
		// get Text
		const wchar_t* text = 0;
		if (Tabs[i])
			text = Tabs[i]->getText();

		// get text length
		s32 len = font->getDimension(text).Width + 20;
		frameRect.UpperLeftCorner.X = pos;
		frameRect.LowerRightCorner.X = frameRect.UpperLeftCorner.X + len;
		pos += len;

		if (frameRect.isPointInside(p))
		{
			setActiveTab(i);
			return;
		}
	}
}


//! draws the element and its children
void CGUITabControl::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;

	IGUIFont* font = skin->getFont();
	video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> frameRect(AbsoluteRect);

	if (Tabs.empty())
		driver->draw2DRectangle(skin->getColor(EGDC_3D_HIGH_LIGHT),
		                        frameRect, &AbsoluteClippingRect);

	if (!font)
		return;

	s32 tabheight = skin->getSize(gui::EGDS_BUTTON_HEIGHT);
	frameRect.UpperLeftCorner.Y += 2;
	frameRect.LowerRightCorner.Y = frameRect.UpperLeftCorner.Y + tabheight;
	core::rect<s32> tr;
	s32 pos = frameRect.UpperLeftCorner.X + 2;

	// left and right pos of the active tab
	s32 left = 0;
	s32 right = 0;
	const wchar_t* activetext = 0;

	for (s32 i=0; i<(s32)Tabs.size(); ++i)
	{
		// get Text
		const wchar_t* text = 0;
		if (Tabs[i])
			text = Tabs[i]->getText();

		// get text length
		s32 len = font->getDimension(text).Width + 20;
		frameRect.UpperLeftCorner.X = pos;
		frameRect.LowerRightCorner.X = frameRect.UpperLeftCorner.X + len;
		pos += len;

		if (i == ActiveTab)
		{
			left = frameRect.UpperLeftCorner.X;
			right = frameRect.LowerRightCorner.X;
			activetext = text;
		}
		else
		{
			skin->draw3DTabButton(this, false, frameRect, &AbsoluteClippingRect);

			// draw text
			font->draw(text, frameRect, skin->getColor(EGDC_BUTTON_TEXT),
			           true, true, &AbsoluteClippingRect);
		}
	}

	// draw active tab
	if (left != 0 && right != 0)
	{
		frameRect.UpperLeftCorner.X = left-2;
		frameRect.LowerRightCorner.X = right+2;
		frameRect.UpperLeftCorner.Y -= 2;

		skin->draw3DTabButton(this, true, frameRect, &AbsoluteClippingRect);

		// draw text
		font->draw(activetext, frameRect, skin->getColor(EGDC_BUTTON_TEXT),
		           true, true, &AbsoluteClippingRect);

		// draw upper highlight frame
		tr.UpperLeftCorner.X = AbsoluteRect.UpperLeftCorner.X;
		tr.LowerRightCorner.X = left - 1;
		tr.UpperLeftCorner.Y = frameRect.LowerRightCorner.Y - 1;
		tr.LowerRightCorner.Y = frameRect.LowerRightCorner.Y;
		driver->draw2DRectangle(skin->getColor(EGDC_3D_HIGH_LIGHT), tr, &AbsoluteClippingRect);

		tr.UpperLeftCorner.X = right;
		tr.LowerRightCorner.X = AbsoluteRect.LowerRightCorner.X;
		driver->draw2DRectangle(skin->getColor(EGDC_3D_HIGH_LIGHT), tr, &AbsoluteClippingRect);
	}

	skin->draw3DTabBody(this, Border, FillBackground, AbsoluteRect, &AbsoluteClippingRect);

	IGUIElement::draw();
}


//! Returns which tab is currently active
s32 CGUITabControl::getActiveTab()
{
	return ActiveTab;
}



//! Brings a tab to front.
bool CGUITabControl::setActiveTab(s32 idx)
{
	if (idx < 0 || idx >= (s32)Tabs.size())
		return false;

	bool changed = (ActiveTab != idx);

	ActiveTab = idx;

	for (s32 i=0; i<(s32)Tabs.size(); ++i)
		if (Tabs[i])
			Tabs[i]->setVisible( i == ActiveTab );

	if (changed)
	{
		SEvent event;
		event.EventType = EET_GUI_EVENT;
		event.GUIEvent.Caller = this;
		event.GUIEvent.EventType = EGET_TAB_CHANGED;
		Parent->OnEvent(event);
	}

	return true;
}


//! Removes a child.
void CGUITabControl::removeChild(IGUIElement* child)
{
	bool isTab = false;
	s32 i=0;

	// check if it is a tab
	for (i=0; i<(s32)Tabs.size(); )
		if (Tabs[i] == child)
		{
			Tabs[i]->drop();
			Tabs.erase(i);
			isTab = true;
		}
		else
			++i;

	// reassign numbers
	if (isTab)
		for (i=0; i<(s32)Tabs.size(); ++i)
			if (Tabs[i])
				Tabs[i]->setNumber(i);

	// remove real element
	IGUIElement::removeChild(child);
}


} // end namespace engine
} // end namespace gui

