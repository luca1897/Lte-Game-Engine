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
 
#include "CGUICheckBox.h"
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
CGUICheckBox::CGUICheckBox(bool checked, IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
	: IGUICheckBox(environment, parent, id, rectangle), Pressed(false), Checked(checked)
{
	#ifdef _DEBUG
	setDebugName("CGUICheckBox");
	#endif
}



//! destructor
CGUICheckBox::~CGUICheckBox()
{
}



//! called if an event happened.
bool CGUICheckBox::OnEvent(SEvent event)
{
	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
		{
			Pressed = false;
			return true;
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
		{
			Pressed = true;
			Environment->setFocus(this);
			return true;
		}
		else
		if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
		{
			bool wasPressed = Pressed;
			Environment->removeFocus(this);
			Pressed = false;

			if (wasPressed && Parent)
			{
				SEvent event;
				event.EventType = EET_GUI_EVENT;
				event.GUIEvent.Caller = this;
				Checked = !Checked;
				event.GUIEvent.EventType = EGET_CHECKBOX_CHANGED;
				Parent->OnEvent(event);
			}

			return true;
		}
		break;
	}

	return Parent ? Parent->OnEvent(event) : false;
}



//! draws the element and its children
void CGUICheckBox::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	engine::video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> rect = AbsoluteRect;

	s32 height = skin->getSize(EGDS_CHECK_BOX_WIDTH);

	core::rect<s32> checkRect(AbsoluteRect.UpperLeftCorner.X,
	                          ((AbsoluteRect.getHeight() - height) / 2) + AbsoluteRect.UpperLeftCorner.Y,
	                          0, 0);

	checkRect.LowerRightCorner.X = checkRect.UpperLeftCorner.X + height;
	checkRect.LowerRightCorner.Y = checkRect.UpperLeftCorner.Y + height;

	skin->draw3DSunkenPane(this, skin->getColor(Pressed ? EGDC_3D_FACE : EGDC_ACTIVE_CAPTION),
	                       false, true, checkRect, &AbsoluteClippingRect);

	if (Checked && Environment->getBuiltInFont())
		Environment->getBuiltInFont()->draw(GUI_ICON_CHECK_BOX_CHECKED,
		                                    checkRect, skin->getColor(EGDC_BUTTON_TEXT), true, true, &AbsoluteClippingRect);

	if (Text.size())
	{
		checkRect = AbsoluteRect;
		checkRect.UpperLeftCorner.X += height + 5;

		IGUIFont* font = skin->getFont();
		if (font)
			font->draw(Text.c_str(), checkRect,
			           skin->getColor(EGDC_BUTTON_TEXT), false, true, &AbsoluteClippingRect);
	}

	IGUIElement::draw();
}


//! set if box is checked
void CGUICheckBox::setChecked(bool checked)
{
	Checked = checked;
}


//! returns if box is checked
bool CGUICheckBox::isChecked()
{
	_engine_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return Checked;
}



} // end namespace gui
} // end namespace engine
