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

#include "CGUIContextMenu.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIFont.h"
#include "GUIIcons.h"

namespace engine
{
namespace gui
{



//! constructor
CGUIContextMenu::CGUIContextMenu(IGUIEnvironment* environment,
                                 IGUIElement* parent, s32 id,
                                 core::rect<s32> rectangle,
                                 bool getFocus)
	: IGUIContextMenu(environment, parent, id, rectangle), HighLighted(-1)
{
	#ifdef _DEBUG
	setDebugName("CGUIContextMenu");
	#endif

	Pos = rectangle.UpperLeftCorner;
	recalculateSize();

	if (getFocus)
		Environment->setFocus(this);
}


//! destructor
CGUIContextMenu::~CGUIContextMenu()
{
	for (s32 i=0; i<(s32)Items.size(); ++i)
		if (Items[i].SubMenu)
			Items[i].SubMenu->drop();
}


//! Returns amount of menu items
s32 CGUIContextMenu::getItemCount() const
{
	return Items.size();
}


//! Adds a menu item.
s32 CGUIContextMenu::addItem(const wchar_t* text, s32 id, bool enabled, bool hasSubMenu)
{
	SItem s;
	s.Enabled = enabled;
	s.Text = text;
	s.IsSeparator = (text == 0);
	s.SubMenu = 0;
	s.CommandId = id;

	if (hasSubMenu)
	{
		s.SubMenu = new CGUIContextMenu(Environment, this, -1,
		                                core::rect<s32>(0,0,100,100), false);
		s.SubMenu->setVisible(false);
	}

	Items.push_back(s);

	recalculateSize();
	return Items.size() - 1;
}


//! Adds a separator item to the menu
void CGUIContextMenu::addSeparator()
{
	addItem(0, true);
}


//! Returns text of the menu item.
const wchar_t* CGUIContextMenu::getItemText(s32 idx)
{
	if (idx < 0 || idx >= (s32)Items.size())
		return 0;

	return Items[idx].Text.c_str();
}


//! Sets text of the menu item.
void CGUIContextMenu::setItemText(s32 idx, const wchar_t* text)
{
	if (idx < 0 || idx >= (s32)Items.size())
		return;

	Items[idx].Text = text;
	recalculateSize();
}


//! Returns if a menu item is enabled
bool CGUIContextMenu::isItemEnabled(s32 idx)
{
	if (idx < 0 || idx >= (s32)Items.size())
	{
		_engine_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return false;
	}

	_engine_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return Items[idx].Enabled;
}


//! Sets if the menu item should be enabled.
void CGUIContextMenu::setItemEnabled(s32 idx, bool enabled)
{
	if (idx < 0 || idx >= (s32)Items.size())
		return;

	Items[idx].Enabled = enabled;
}


//! Removes a menu item
void CGUIContextMenu::removeItem(s32 idx)
{
	if (idx < 0 || idx >= (s32)Items.size())
		return;

	if (Items[idx].SubMenu)
	{
		Items[idx].SubMenu->drop();
		Items[idx].SubMenu = 0;
	}

	Items.erase(idx);
	recalculateSize();
}


//! Removes all menu items
void CGUIContextMenu::removeAllItems()
{
	for (s32 i=0; i<(s32)Items.size(); ++i)
		if (Items[i].SubMenu)
			Items[i].SubMenu->drop();

	Items.clear();
	recalculateSize();
}


//! called if an event happened.
bool CGUIContextMenu::OnEvent(SEvent event)
{
	if (!IsEnabled)
		return Parent ? Parent->OnEvent(event) : false;

	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		switch(event.GUIEvent.EventType)
		{
		case gui::EGET_ELEMENT_FOCUS_LOST:
			remove();
			return true;
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		switch(event.MouseInput.Event)
		{
		case EMIE_LMOUSE_LEFT_UP:
		{
			s32 t = sendClick(core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y));
			if ((t==0 || t==1) && Environment->hasFocus(this))
				Environment->removeFocus(this);
		}
			return true;
		case EMIE_LMOUSE_PRESSED_DOWN:
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


//! Sets the visible state of this element.
void CGUIContextMenu::setVisible(bool visible)
{
	HighLighted = -1;
	for (s32 j=0; j<(s32)Items.size(); ++j)
		if (Items[j].SubMenu)
			Items[j].SubMenu->setVisible(false);

	IGUIElement::setVisible(visible);
}


//! sends a click Returns:
//! 0 if click went outside of the element,
//! 1 if a valid button was clicked,
//! 2 if a nonclickable element was clicked
s32 CGUIContextMenu::sendClick(core::position2d<s32> p)
{
	s32 t = 0;

	// get number of open submenu
	s32 openmenu = -1;
	s32 j;
	for (j=0; j<(s32)Items.size(); ++j)
		if (Items[j].SubMenu && Items[j].SubMenu->isVisible())
		{
			openmenu = j;
			break;
		}

	// delegate click operation to submenu
	if (openmenu != -1)
	{
		t = Items[j].SubMenu->sendClick(p);
		if (t != 0)
			return t; // clicked something
	}

	// check click on myself
	if (AbsoluteRect.isPointInside(p) &&
	    HighLighted >= 0 && HighLighted <(s32)Items.size())
	{
		if (!Items[HighLighted].Enabled ||
		    Items[HighLighted].IsSeparator ||
		    Items[HighLighted].SubMenu)
			return 2;

		SEvent event;
		event.EventType = EET_GUI_EVENT;
		event.GUIEvent.Caller = this;
		event.GUIEvent.EventType = EGET_MENU_ITEM_SELECTED;
		Parent->OnEvent(event);

		return 1;
	}

	return 0;
}


//! returns true, if an element was highligted
bool CGUIContextMenu::highlight(core::position2d<s32> p)
{
	// get number of open submenu
	s32 openmenu = -1;
	s32 j;
	for (j=0; j<(s32)Items.size(); ++j)
		if (Items[j].SubMenu && Items[j].SubMenu->isVisible())
		{
			openmenu = j;
			break;
		}

	// delegate highlight operation to submenu
	if (openmenu != -1)
	{
		if (Items[j].SubMenu->highlight(p))
		{
			HighLighted = openmenu;
			return true;
		}
	}

	// highlight myself
	for (s32 i=0; i<(s32)Items.size(); ++i)
		if (getHRect(Items[i], AbsoluteRect).isPointInside(p))
		{
			HighLighted = i;

			// make submenus visible/invisible
			for (s32 j=0; j<(s32)Items.size(); ++j)
				if (Items[j].SubMenu)
					Items[j].SubMenu->setVisible(j == i);
			return true;
		}

	HighLighted = openmenu;
	return false;
}


//! returns the item highlight-area
core::rect<s32> CGUIContextMenu::getHRect(SItem& i, core::rect<s32>& absolute)
{
	core::rect<s32> r = absolute;
	r.UpperLeftCorner.Y += i.PosY;
	r.LowerRightCorner.Y = r.UpperLeftCorner.Y + i.Dim.Height;
	return r;
}


//! Gets drawing rect of Item
core::rect<s32> CGUIContextMenu::getRect(SItem& i, core::rect<s32>& absolute)
{
	core::rect<s32> r = absolute;
	r.UpperLeftCorner.Y += i.PosY;
	r.LowerRightCorner.Y = r.UpperLeftCorner.Y + i.Dim.Height;
	r.UpperLeftCorner.X += 20;
	return r;
}


//! draws the element and its children
void CGUIContextMenu::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	IGUIFont* font = skin->getFont();
	IGUIFont* defaultFont = Environment->getBuiltInFont();
	video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> rect = AbsoluteRect;
	//core::rect<s32>* clip = &AbsoluteClippingRect;
	core::rect<s32>* clip = 0;

	// draw frame
	skin->draw3DMenuPane(this, AbsoluteRect, clip);

	// loop through all menu items

	rect = AbsoluteRect;
	s32 y = AbsoluteRect.UpperLeftCorner.Y;

	for (s32 i=0; i<(s32)Items.size(); ++i)
	{
		if (Items[i].IsSeparator)
		{
			// draw separator
			rect = AbsoluteRect;
			rect.UpperLeftCorner.Y += Items[i].PosY + 3;
			rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + 1;
			rect.UpperLeftCorner.X += 5;
			rect.LowerRightCorner.X -= 5;
			driver->draw2DRectangle(skin->getColor(EGDC_3D_SHADOW), rect, clip);

			rect.LowerRightCorner.Y += 1;
			rect.UpperLeftCorner.Y += 1;
			driver->draw2DRectangle(skin->getColor(EGDC_3D_HIGH_LIGHT), rect, clip);

			y += 10;
		}
		else
		{
			rect = getRect(Items[i], AbsoluteRect);

			// draw highlighted

			if (i == HighLighted && Items[i].Enabled)
			{
				core::rect<s32> r = AbsoluteRect;
				r.LowerRightCorner.Y = rect.LowerRightCorner.Y;
				r.UpperLeftCorner.Y = rect.UpperLeftCorner.Y;
				r.LowerRightCorner.X -= 5;
				r.UpperLeftCorner.X += 5;
				driver->draw2DRectangle(skin->getColor(EGDC_HIGH_LIGHT), r, clip);
			}

			// draw text

			EGUI_DEFAULT_COLOR c = EGDC_BUTTON_TEXT;

			if (i == HighLighted)
				c = EGDC_HIGH_LIGHT_TEXT;

			if (!Items[i].Enabled)
				c = EGDC_GRAY_TEXT;

			font->draw(Items[i].Text.c_str(), rect,
			           skin->getColor(c), false, true, clip);

			// draw submenu symbol
			if (Items[i].SubMenu && defaultFont)
			{
				core::rect<s32> r = rect;
				r.UpperLeftCorner.X = r.LowerRightCorner.X - 15;

				defaultFont->draw(GUI_ICON_CURSOR_RIGHT, r,
				                  skin->getColor(c), true, true, clip);
			}

		}
	}

	IGUIElement::draw();
}


void CGUIContextMenu::recalculateSize()
{
	IGUISkin* skin = Environment->getSkin();
	IGUIFont* font = skin->getFont();

	if (!font)
		return;

	core::rect<s32> rect;
	rect.UpperLeftCorner = RelativeRect.UpperLeftCorner;
	s32 width = 100;
	s32 height = 3;
	s32 i;

	for (i=0; i<(s32)Items.size(); ++i)
	{
		if (Items[i].IsSeparator)
		{
			Items[i].Dim.Width = 100;
			Items[i].Dim.Height = 10;
		}
		else
		{
			Items[i].Dim = font->getDimension(Items[i].Text.c_str());
			Items[i].Dim.Width += 40;

			if (Items[i].Dim.Width > width)
				width = Items[i].Dim.Width;
		}

		Items[i].PosY = height;
		height += Items[i].Dim.Height;
	}

	height += 5;

	if (height < 10)
		height = 10;

	rect.LowerRightCorner.X = RelativeRect.UpperLeftCorner.X + width;
	rect.LowerRightCorner.Y = RelativeRect.UpperLeftCorner.Y + height;
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
			        core::rect<s32>(width-5, Items[i].PosY,
			                        width+w-5, Items[i].PosY+h));
		}
}


//! Returns the selected item in the menu
s32 CGUIContextMenu::getSelectedItem()
{
	return HighLighted;
}


//! \return Returns a pointer to the submenu of an item.
IGUIContextMenu* CGUIContextMenu::getSubMenu(s32 idx)
{
	if (idx < 0 || idx >= (s32)Items.size())
		return 0;

	return Items[idx].SubMenu;
}


//! Returns command id of a menu item
s32 CGUIContextMenu::getItemCommandId(s32 idx)
{
	if (idx < 0 || idx >= (s32)Items.size())
		return -1;

	return Items[idx].CommandId;
}

//! Sets the command id of a menu item
void CGUIContextMenu::setItemCommandId(s32 idx, s32 id)
{
	if (idx < 0 || idx >= (s32)Items.size())
		return;

	Items[idx].CommandId = id;
}



} // end namespace
} // end namespace

