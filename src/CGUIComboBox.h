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
 
#ifndef __C_GUI_COMBO_BOX_H_INCLUDED__
#define __C_GUI_COMBO_BOX_H_INCLUDED__

#include "IGUIComboBox.h"
#include "engineString.h"
#include "engineArray.h"

namespace engine
{
namespace gui
{
//! Single line edit box for editing simple text.
class CGUIComboBox : public IGUIComboBox
{
public:

//! constructor
CGUIComboBox(IGUIEnvironment* environment, IGUIElement* parent,
             s32 id, core::rect<s32> rectangle);

//! destructor
~CGUIComboBox();

//! Returns amount of items in box
virtual s32 getItemCount();

//! returns string of an item. the idx may be a value from 0 to itemCount-1
virtual const wchar_t* getItem(s32 idx);

//! adds an item and returns the index of it
virtual s32 addItem(const wchar_t* text);

//! deletes all items in the combo box
virtual void clear();

//! returns id of selected item. returns -1 if no item is selected.
virtual s32 getSelected();

//! sets the selected item. Set this to -1 if no item should be selected
virtual void setSelected(s32 id);

//! called if an event happened.
virtual bool OnEvent(SEvent event);

//! draws the element and its children
virtual void draw();

private:

void openCloseMenu();
void sendSelectionChangedEvent();

IGUIButton* ListButton;
IGUIListBox* ListBox;
core::array< core::stringw > Items;
s32 Selected;
};


} // end namespace gui
} // end namespace engine

#endif

