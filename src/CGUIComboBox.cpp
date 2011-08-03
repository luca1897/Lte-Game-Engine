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
 
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "CGUIComboBox.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IGUIFont.h"
#include "GUIIcons.h"
#include "IGUIListBox.h"
#include "CGUIListBox.h"
#include "os.h"

namespace engine
{
namespace gui
{

//! constructor
CGUIComboBox::CGUIComboBox(IGUIEnvironment* environment, IGUIElement* parent,
                           s32 id, core::rect<s32> rectangle)
	: IGUIComboBox(environment, parent, id, rectangle), Selected(-1),
	ListBox(0)
{
	#ifdef _DEBUG
	setDebugName("CGUICheckBox");
	#endif

	IGUISkin* skin = Environment->getSkin();

	s32 width = skin->getSize(EGDS_WINDOW_BUTTON_WIDTH);

	core::rect<s32> r;
	r.UpperLeftCorner.X = rectangle.getWidth() - width - 2;
	r.LowerRightCorner.X = rectangle.getWidth() - 2;

	r.UpperLeftCorner.Y = 2;
	r.LowerRightCorner.Y = rectangle.getHeight() - 2;

	ListButton = Environment->addButton(r, this, -1, GUI_ICON_CURSOR_DOWN);
	ListButton->setOverrideFont(Environment->getBuiltInFont());
}


//! destructor
CGUIComboBox::~CGUIComboBox()
{
}


//! Returns amount of items in box
s32 CGUIComboBox::getItemCount()
{
	return Items.size();
}

//! returns string of an item. the idx may be a value from 0 to itemCount-1
const wchar_t* CGUIComboBox::getItem(s32 idx)
{
	if (idx < 0 || idx >= (s32)Items.size())
		return 0;

	return Items[idx].c_str();
}


//! adds an item and returns the index of it
s32 CGUIComboBox::addItem(const wchar_t* text)
{
	Items.push_back(core::stringw(text));

	if (Selected == -1)
		Selected = 0;

	return Items.size() - 1;
}



//! deletes all items in the combo box
void CGUIComboBox::clear()
{
	Items.clear();
	Selected = -1;
}



//! returns id of selected item. returns -1 if no item is selected.
s32 CGUIComboBox::getSelected()
{
	return Selected;
}



//! sets the selected item. Set this to -1 if no item should be selected
void CGUIComboBox::setSelected(s32 id)
{
	if (id <0 || id>=(s32)Items.size())
		return;

	Selected = id;
}



//! called if an event happened.
bool CGUIComboBox::OnEvent(SEvent event)
{
	switch(event.EventType)
	{
	case EET_GUI_EVENT:

		switch(event.GUIEvent.EventType)
		{
		case EGET_BUTTON_CLICKED:
			if (event.GUIEvent.Caller == ListButton)
			{
				openCloseMenu();
				return true;
			}
			break;
		case EGET_LISTBOX_SELECTED_AGAIN:
		case EGET_LISTBOX_CHANGED:
			if (event.GUIEvent.Caller == ListBox)
			{
				Selected = ListBox->getSelected();
				if (Selected <0 || Selected >= (s32)Items.size())
					Selected = -1;
				openCloseMenu();

				sendSelectionChangedEvent();
			}
			return true;
		}
		break;
	case EET_MOUSE_INPUT_EVENT:

		switch(event.MouseInput.Event)
		{
		case EMIE_LMOUSE_PRESSED_DOWN:
		{
			if (!ListBox)
				Environment->removeFocus(this);

			core::position2d<s32> p(event.MouseInput.X, event.MouseInput.Y);

			// send to list box
			if (ListBox && ListBox->getAbsolutePosition().isPointInside(p) &&
			    ListBox->OnEvent(event))
				return true;

			// check if it is outside
			if (!AbsoluteRect.isPointInside(p))
			{
				Environment->removeFocus(this);
				return false;
			}
			return true;
		}
		case EMIE_LMOUSE_LEFT_UP:
		{
			core::position2d<s32> p(event.MouseInput.X, event.MouseInput.Y);

			// send to list box
			if (ListBox && ListBox->getAbsolutePosition().isPointInside(p) &&
			    ListBox->OnEvent(event))
				return true;
			else
				openCloseMenu();

			if (!AbsoluteRect.isPointInside(p))
			{
				Environment->removeFocus(this);
				return false;
			}
		}
			break;
		}
		break;
	}

	return Parent ? Parent->OnEvent(event) : false;
}



void CGUIComboBox::sendSelectionChangedEvent()
{
	if (Parent)
	{
		SEvent event;

		event.EventType = EET_GUI_EVENT;
		event.GUIEvent.Caller = this;
		event.GUIEvent.EventType = EGET_COMBO_BOX_CHANGED;
		Parent->OnEvent(event);
	}
}



//! draws the element and its children
void CGUIComboBox::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	engine::video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> frameRect(AbsoluteRect);

	// draw the border

	skin->draw3DSunkenPane(this, skin->getColor(EGDC_3D_HIGH_LIGHT),
	                       true, true, frameRect, &AbsoluteClippingRect);

	// Draw text

	if (Selected != -1)
	{
		frameRect = AbsoluteRect;
		frameRect.UpperLeftCorner.X += 2;

		skin->getFont()->draw(Items[Selected].c_str(), frameRect,
		                      skin->getColor(EGDC_BUTTON_TEXT),
		                      false, true, &AbsoluteClippingRect);
	}

	// draw buttons
	IGUIElement::draw();
}


void CGUIComboBox::openCloseMenu()
{
	if (ListBox)
	{
		// close list box
		ListBox->remove();
		ListBox = 0;
	}
	else
	{
		if (Parent)
			Parent->bringToFront(this);

		IGUISkin* skin = Environment->getSkin();
		s32 h = Items.size();

		if (h > 5)
			h = 5;
		if (h == 0)
			h = 1;

		h *= (skin->getFont()->getDimension(L"A").Height + 4);

		// open list box
		core::rect<s32> r(0, AbsoluteRect.getHeight(),
		                  AbsoluteRect.getWidth(), AbsoluteRect.getHeight() + h);

		ListBox = new CGUIListBox(Environment, this, -1, r, false, true, true);
		ListBox->drop();

		for (s32 i=0; i<(s32)Items.size(); ++i)
			ListBox->addItem(Items[i].c_str());

		ListBox->setSelected(-1);

		// set focus
		Environment->setFocus(ListBox);
	}
}

} // end namespace gui
} // end namespace engine

