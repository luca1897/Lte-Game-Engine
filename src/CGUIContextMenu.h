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
 
#ifndef __C_GUI_CONTEXT_MENU_H_INCLUDED__
#define __C_GUI_CONTEXT_MENU_H_INCLUDED__

#include "IGUIContextMenu.h"
#include "engineString.h"
#include "engineArray.h"

namespace engine
{
namespace gui
{

//! GUI Context menu interface.
class CGUIContextMenu : public IGUIContextMenu
{
public:

//! constructor
CGUIContextMenu(IGUIEnvironment* environment,
                IGUIElement* parent, s32 id, core::rect<s32> rectangle,
                bool getFocus = true);

//! destructor
~CGUIContextMenu();

//! Returns amount of menu items
virtual s32 getItemCount() const;

//! Adds a menu item.
virtual s32 addItem(const wchar_t* text, s32 commandid, bool enabled=true,
                    bool hasSubMenu=false);

//! Adds a separator item to the menu
virtual void addSeparator();

//! Returns text of the menu item.
virtual const wchar_t* getItemText(s32 idx);

//! Sets text of the menu item.
virtual void setItemText(s32 idx, const wchar_t* text);

//! Returns if a menu item is enabled
virtual bool isItemEnabled(s32 idx);

//! Sets if the menu item should be enabled.
virtual void setItemEnabled(s32 idx, bool enabled);

//! Removes a menu item
virtual void removeItem(s32 idx);

//! Removes all menu items
virtual void removeAllItems();

//! called if an event happened.
virtual bool OnEvent(SEvent event);

//! draws the element and its children
virtual void draw();

//! Returns the selected item in the menu
virtual s32 getSelectedItem();

//! \return Returns a pointer to the submenu of an item.
virtual IGUIContextMenu* getSubMenu(s32 idx);

//! Sets the visible state of this element.
virtual void setVisible(bool visible);

//! Returns command id of a menu item
virtual s32 getItemCommandId(s32 idx);

//! Sets the command id of a menu item
virtual void setItemCommandId(s32 idx, s32 id);

protected:

struct SItem
{
	core::stringw Text;
	bool IsSeparator;
	bool Enabled;
	core::dimension2d<s32> Dim;
	s32 PosY;
	CGUIContextMenu* SubMenu;
	s32 CommandId;
};

virtual void recalculateSize();

//! returns true, if an element was highligted
virtual bool highlight(core::position2d<s32> p);

//! sends a click Returns:
//! 0 if click went outside of the element,
//! 1 if a valid button was clicked,
//! 2 if a nonclickable element was clicked
virtual s32 sendClick(core::position2d<s32> p);

//! returns the item highlight-area
virtual core::rect<s32> getHRect(SItem& i, core::rect<s32>& absolute);

//! Gets drawing rect of Item
virtual core::rect<s32> getRect(SItem& i, core::rect<s32>& absolute);



s32 HighLighted;
core::array<SItem> Items;
core::position2d<s32> Pos;
};

} // end namespace gui
} // end namespace engine

#endif

