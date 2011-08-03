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
 
#include "CGUIMenu.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIFont.h"
#include "GUIIcons.h"

#include "os.h"

namespace engine
{
namespace gui
{

//! constructor
CGUIMenu::CGUIMenu(IGUIEnvironment* environment, IGUIElement* parent,
                   s32 id, core::rect<s32> rectangle)
	: CGUIContextMenu(environment, parent, id, rectangle, false)
{
	#ifdef _DEBUG
	setDebugName("CGUIMenu");
	#endif

	recalculateSize();
}


//! destructor
CGUIMenu::~CGUIMenu()
{

}

//! draws the element and its children
void CGUIMenu::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	IGUIFont* font = skin->getFont();
	IGUIFont* defaultFont = Environment->getBuiltInFont();
	video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> rect = AbsoluteRect;
	core::rect<s32>* clip = &AbsoluteClippingRect;

	// draw frame

	skin->draw3DToolBar(this, rect, clip);

	// loop through all menu items

	rect = AbsoluteRect;

	for (s32 i=0; i<(s32)Items.size(); ++i)
	{
		if (!Items[i].IsSeparator)
		{
			rect = getRect(Items[i], AbsoluteRect);

			// draw highlighted

			if (i == HighLighted && Items[i].Enabled)
			{
				skin->draw3DSunkenPane(this, skin->getColor(EGDC_3D_DARK_SHADOW),
				                       true, true, rect, &AbsoluteClippingRect);
			}

			// draw text

			EGUI_DEFAULT_COLOR c = EGDC_BUTTON_TEXT;

			if (i == HighLighted)
				c = EGDC_HIGH_LIGHT_TEXT;

			if (!Items[i].Enabled)
				c = EGDC_GRAY_TEXT;

			font->draw(Items[i].Text.c_str(), rect,
			           skin->getColor(c), true, true, clip);
		}
	}

	IGUIElement::draw();
}


//! called if an event happened.
bool CGUIMenu::OnEvent(SEvent event)
{
	if (!IsEnabled)
		return Parent ? Parent->OnEvent(event) : false;

	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		switch(event.GUIEvent.EventType)
		{
		case gui::EGET_ELEMENT_FOCUS_LOST:
			closeAllSubMenus();
			return true;
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		switch(event.MouseInput.Event)
		{
		case EMIE_LMOUSE_LEFT_UP:
		{
			core::position2d<s32> p(event.MouseInput.X, event.MouseInput.Y);
			if (AbsoluteRect.isPointInside(p))
			{
				if (HighLighted != -1)
					Environment->removeFocus(this);
				else
					highlight(core::position2d<s32>(event.MouseInput.X,     event.MouseInput.Y));
			}
			else
			{
				s32 t = sendClick(p);
				if ((t==0 || t==1) && Environment->hasFocus(this))
					Environment->removeFocus(this);
			}
		}
			return true;
		case EMIE_LMOUSE_PRESSED_DOWN:
			if (!Environment->hasFocus(this))
			{
				Environment->setFocus(this);
				if (Parent)
					Parent->bringToFront(this);
			}
			return true;
		case EMIE_MOUSE_MOVED:
			if (Environment->hasFocus(this))
				highlight(core::position2d<s32>(event.MouseInput.X,     event.MouseInput.Y));
			return true;
		}
		break;
	}

	return Parent ? Parent->OnEvent(event) : false;
}


void CGUIMenu::recalculateSize()
{
	IGUISkin* skin = Environment->getSkin();
	IGUIFont* font = skin->getFont();

	if (!font)
		return;

	core::rect<s32> rect;
	rect.UpperLeftCorner.X = 0;
	rect.UpperLeftCorner.Y = 0;
	s32 height = font->getDimension(L"A").Height + 5;
	s32 width = 0;
	s32 i;

	for (i=0; i<(s32)Items.size(); ++i)
	{
		if (Items[i].IsSeparator)
		{
			Items[i].Dim.Width = 0;
			Items[i].Dim.Height = height;
		}
		else
		{
			Items[i].Dim = font->getDimension(Items[i].Text.c_str());
			Items[i].Dim.Width += 20;
		}

		Items[i].PosY = width;
		width += Items[i].Dim.Width;
	}

	if (Parent)
		width = Parent->getAbsolutePosition().getWidth();

	rect.LowerRightCorner.X = width;
	rect.LowerRightCorner.Y = height;
	RelativeRect = rect;
	updateAbsolutePosition();

	// recalculate submenus
	for (i=0; i<(s32)Items.size(); ++i)
		if (Items[i].SubMenu)
		{
			// move submenu
			s32 w = Items[i].SubMenu->getAbsolutePosition().getWidth();
			s32 h = Items[i].SubMenu->getAbsolutePosition().getHeight();

			Items[i].SubMenu->setRelativePosition(
			        core::rect<s32>(Items[i].PosY, height,
			                        Items[i].PosY+w-5, height+h));
		}
}


//! returns the item highlight-area
core::rect<s32> CGUIMenu::getHRect(SItem& i, core::rect<s32>& absolute)
{
	core::rect<s32> r = absolute;
	r.UpperLeftCorner.X += i.PosY;
	r.LowerRightCorner.X = r.UpperLeftCorner.X + i.Dim.Width;
	return r;
}

//! Gets drawing rect of Item
core::rect<s32> CGUIMenu::getRect(SItem& i, core::rect<s32>& absolute)
{
	return getHRect(i, absolute);
}


void CGUIMenu::closeAllSubMenus()
{
	for (s32 i=0; i<(s32)Items.size(); ++i)
		if (Items[i].SubMenu)
			Items[i].SubMenu->setVisible(false);

	HighLighted = -1;
}


void CGUIMenu::updateAbsolutePosition()
{
	if (Parent)
		RelativeRect.LowerRightCorner.X = Parent->getAbsolutePosition().getWidth();

	IGUIElement::updateAbsolutePosition();
}


} // end namespace
} // end namespace

