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
 
#include "CGUIMessageBox.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IGUIButton.h"
#include "IGUIFont.h"

namespace engine
{
namespace gui
{

//! constructor
CGUIMessageBox::CGUIMessageBox(IGUIEnvironment* environment, const wchar_t* caption,
                               const wchar_t* text, s32 flags,
                               IGUIElement* parent, s32 id, core::rect<s32> rectangle)
	: CGUIWindow(environment, parent, id, rectangle), StaticText(0),
	OkButton(0), YesButton(0), NoButton(0), CancelButton(0)
{
	#ifdef _DEBUG
	setDebugName("CGUIMessageBox");
	#endif

	// remove focus
	Environment->setFocus(0);

	// remove buttons

	getMaximizeButton()->remove();
	getMinimizeButton()->remove();

	if (caption)
		setText(caption);

	IGUISkin* skin = Environment->getSkin();

	s32 buttonHeight = skin->getSize(EGDS_BUTTON_HEIGHT);
	s32 buttonWidth = skin->getSize(EGDS_BUTTON_WIDTH);
	s32 titleHeight = skin->getSize(EGDS_WINDOW_BUTTON_WIDTH)+2;
	s32 buttonDistance = skin->getSize(EGDS_WINDOW_BUTTON_WIDTH);

	// add static multiline text

	core::dimension2d<s32> dim(AbsoluteClippingRect.getWidth() - buttonWidth,
	                           AbsoluteClippingRect.getHeight() - (buttonHeight * 3));
	core::position2d<s32> pos((AbsoluteClippingRect.getWidth() - dim.Width) / 2,
	                          buttonHeight / 2 + titleHeight);

	StaticText = Environment->addStaticText(text,
	                                        core::rect<s32>(pos, dim), false, false, this);
	StaticText->setWordWrap(true);
	StaticText->grab();

	// adjust static text height

	s32 textHeight = StaticText->getTextHeight();
	core::rect<s32> tmp = StaticText->getRelativePosition();
	tmp.LowerRightCorner.Y = tmp.UpperLeftCorner.Y + textHeight;
	StaticText->setRelativePosition(tmp);
	dim.Height = textHeight;

	// adjust message box height

	tmp = getRelativePosition();
	s32 msgBoxHeight = textHeight + (s32)(2.5f * buttonHeight) + titleHeight;

	// adjust message box position

	tmp.UpperLeftCorner.Y = (parent->getAbsolutePosition().getHeight() - msgBoxHeight) / 2;
	tmp.LowerRightCorner.Y = tmp.UpperLeftCorner.Y + msgBoxHeight;
	setRelativePosition(tmp);


	// add buttons

	s32 countButtons = 0;
	if (flags & EMBF_OK) ++countButtons;
	if (flags & EMBF_CANCEL) ++countButtons;
	if (flags & EMBF_YES) ++countButtons;
	if (flags & EMBF_NO) ++countButtons;

	core::rect<s32> btnRect;
	btnRect.UpperLeftCorner.Y = pos.Y + dim.Height + buttonHeight / 2;
	btnRect.LowerRightCorner.Y = btnRect.UpperLeftCorner.Y + buttonHeight;
	btnRect.UpperLeftCorner.X = (AbsoluteClippingRect.getWidth() -
	                             ((buttonWidth + buttonDistance)*countButtons)) / 2;
	btnRect.LowerRightCorner.X = btnRect.UpperLeftCorner.X + buttonWidth;

	// add ok button
	if (flags & EMBF_OK)
	{
		OkButton = Environment->addButton(btnRect, this);
		OkButton->setText(skin->getDefaultText(EGDT_MSG_BOX_OK));
		OkButton->grab();

		btnRect.LowerRightCorner.X += buttonWidth + buttonDistance;
		btnRect.UpperLeftCorner.X += buttonWidth + buttonDistance;

		Environment->setFocus(OkButton);
	}

	// add yes button
	if (flags & EMBF_YES)
	{
		YesButton = Environment->addButton(btnRect, this);
		YesButton->setText(skin->getDefaultText(EGDT_MSG_BOX_YES));
		YesButton->grab();

		btnRect.LowerRightCorner.X += buttonWidth + buttonDistance;
		btnRect.UpperLeftCorner.X += buttonWidth + buttonDistance;
	}

	// add no button
	if (flags & EMBF_NO)
	{
		NoButton = Environment->addButton(btnRect, this);
		NoButton->setText(skin->getDefaultText(EGDT_MSG_BOX_NO));
		NoButton->grab();

		btnRect.LowerRightCorner.X += buttonWidth + buttonDistance;
		btnRect.UpperLeftCorner.X += buttonWidth + buttonDistance;
	}

	// add cancel button
	if (flags & EMBF_CANCEL)
	{
		CancelButton = Environment->addButton(btnRect, this);
		CancelButton->setText(skin->getDefaultText(EGDT_MSG_BOX_CANCEL));
		CancelButton->grab();

		btnRect.LowerRightCorner.X += buttonWidth + buttonDistance;
		btnRect.UpperLeftCorner.X += buttonWidth + buttonDistance;
	}


}


//! destructor
CGUIMessageBox::~CGUIMessageBox()
{
	if (StaticText)
		StaticText->drop();

	if (OkButton)
		OkButton->drop();

	if (CancelButton)
		CancelButton->drop();

	if (YesButton)
		YesButton->drop();

	if (NoButton)
		NoButton->drop();
}


//! called if an event happened.
bool CGUIMessageBox::OnEvent(SEvent event)
{
	SEvent outevent;
	outevent.EventType = EET_GUI_EVENT;
	outevent.GUIEvent.Caller = this;

	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
		{
			if (event.GUIEvent.Caller == OkButton)
			{
				outevent.GUIEvent.EventType = EGET_MESSAGEBOX_OK;
				Parent->OnEvent(outevent);
				remove();
				return true;
			}
			else
			if (event.GUIEvent.Caller == CancelButton ||
			    event.GUIEvent.Caller == CloseButton)
			{
				outevent.GUIEvent.EventType = EGET_MESSAGEBOX_CANCEL;
				Parent->OnEvent(outevent);
				remove();
				return true;
			}
			else
			if (event.GUIEvent.Caller == YesButton)
			{
				outevent.GUIEvent.EventType = EGET_MESSAGEBOX_YES;
				Parent->OnEvent(outevent);
				remove();
				return true;
			}
			else
			if (event.GUIEvent.Caller == NoButton)
			{
				outevent.GUIEvent.EventType = EGET_MESSAGEBOX_NO;
				Parent->OnEvent(outevent);
				remove();
				return true;
			}
		}
		break;
	}

	return CGUIWindow::OnEvent(event);
}


} // end namespace gui
} // end namespace engine
