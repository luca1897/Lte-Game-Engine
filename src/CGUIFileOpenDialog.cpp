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
 
#include "CGUIFileOpenDialog.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIButton.h"
#include "IGUIStaticText.h"
#include "IGUIFont.h"
#include "GUIIcons.h"
#include "IFileList.h"
#include "os.h"

namespace engine
{
namespace gui
{

const s32 FOD_WIDTH = 350;
const s32 FOD_HEIGHT = 250;


//! constructor
CGUIFileOpenDialog::CGUIFileOpenDialog(io::IFileSystem* fs, const wchar_t* title, IGUIEnvironment* environment, IGUIElement* parent, s32 id)
	: IGUIFileOpenDialog(environment, parent, id,
	                     core::rect<s32>((parent->getAbsolutePosition().getWidth()-FOD_WIDTH)/2,
	                                     (parent->getAbsolutePosition().getHeight()-FOD_HEIGHT)/2,
	                                     (parent->getAbsolutePosition().getWidth()-FOD_WIDTH)/2+FOD_WIDTH,
	                                     (parent->getAbsolutePosition().getHeight()-FOD_HEIGHT)/2+FOD_HEIGHT)),
	Dragging(false), FileNameText(0), FileSystem(fs), FileList(0)
{
	#ifdef _DEBUG
	IGUIElement::setDebugName("CGUIFileOpenDialog");
	#endif

	Text = title;

	s32 buttonw = environment->getSkin()->getSize(EGDS_WINDOW_BUTTON_WIDTH);
	s32 posx = RelativeRect.getWidth() - buttonw - 4;

	CloseButton = Environment->addButton(core::rect<s32>(posx, 3, posx + buttonw, 3 + buttonw), this, -1, GUI_ICON_WINDOW_CLOSE);
	CloseButton->setOverrideFont(Environment->getBuiltInFont());
	CloseButton->grab();

	OKButton = Environment->addButton(core::rect<s32>(RelativeRect.getWidth()-80, 30, RelativeRect.getWidth()-10, 50), this, -1, L"OK");
	OKButton->grab();

	CancelButton = Environment->addButton(core::rect<s32>(RelativeRect.getWidth()-80, 55, RelativeRect.getWidth()-10, 75), this, -1, L"Cancel");
	CancelButton->grab();

	FileBox = Environment->addListBox(core::rect<s32>(10, 55, RelativeRect.getWidth()-90, 230), this, -1, true);
	FileBox->grab();
	//Environment->setFocus(FileBox);

	FileNameText = Environment->addStaticText(0, core::rect<s32>(10, 30, RelativeRect.getWidth()-90, 50), true, false, this);
	FileNameText->grab();

	if (FileSystem)
		FileSystem->grab();

	fillListBox();
}



//! destructor
CGUIFileOpenDialog::~CGUIFileOpenDialog()
{
	if (CloseButton)
		CloseButton->drop();

	if (OKButton)
		OKButton->drop();

	if (CancelButton)
		CancelButton->drop();

	if (FileBox)
		FileBox->drop();

	if (FileNameText)
		FileNameText->drop();

	if (FileSystem)
		FileSystem->drop();

	if (FileList)
		FileList->drop();
}


//! returns the filename of the selected file. Returns NULL, if no file was selected.
const wchar_t* CGUIFileOpenDialog::getFilename()
{
	return FileName.c_str();
}



//! called if an event happened.
bool CGUIFileOpenDialog::OnEvent(SEvent event)
{
	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		switch(event.GUIEvent.EventType)
		{
		case EGET_ELEMENT_FOCUS_LOST:
			Dragging = false;
			break;
		case EGET_BUTTON_CLICKED:
			if (event.GUIEvent.Caller == CloseButton ||
			    event.GUIEvent.Caller == CancelButton)
			{
				sendCancelEvent();
				remove();
				return true;
			}
			else
			if (event.GUIEvent.Caller == OKButton && FileName != L"")
			{
				sendSelectedEvent();
				remove();
				return true;
			}
			break;

		case EGET_LISTBOX_CHANGED:
		{
			s32 selected = FileBox->getSelected();
			if (FileList && FileSystem)
			{
				if (FileList->isDirectory(selected))
					FileName = L"";
				else
					FileName = FileList->getFullFileName(selected);
			}
		}
			break;

		case EGET_LISTBOX_SELECTED_AGAIN:
		{
			s32 selected = FileBox->getSelected();
			if (FileList && FileSystem)
			{
				if (FileList->isDirectory(selected))
				{
					FileSystem->changeWorkingDirectoryTo(FileList->getFileName(selected));
					fillListBox();
					FileName = L"";
				}
				else
				{
					FileName = FileList->getFullFileName(selected);
					return true;
				}
			}
		}
			break;
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		switch(event.MouseInput.Event)
		{
		case EMIE_LMOUSE_PRESSED_DOWN:
			DragStart.X = event.MouseInput.X;
			DragStart.Y = event.MouseInput.Y;
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
				move(core::position2d<s32>(event.MouseInput.X - DragStart.X, event.MouseInput.Y - DragStart.Y));
				DragStart.X = event.MouseInput.X;
				DragStart.Y = event.MouseInput.Y;
				return true;
			}
			break;
		}
	}

	return Parent ? Parent->OnEvent(event) : false;
}


//! draws the element and its children
void CGUIFileOpenDialog::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	engine::video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> rect = AbsoluteRect;

	rect = skin->draw3DWindowBackground(this, true, skin->getColor(EGDC_ACTIVE_BORDER),
	                                    rect, &AbsoluteClippingRect);

	if (Text.size())
	{
		rect.UpperLeftCorner.X += 2;
		rect.LowerRightCorner.X -= skin->getSize(EGDS_WINDOW_BUTTON_WIDTH) + 5;

		IGUIFont* font = skin->getFont();
		if (font)
			font->draw(Text.c_str(), rect, skin->getColor(EGDC_ACTIVE_CAPTION), false, true,
			           &AbsoluteClippingRect);
	}

	IGUIElement::draw();
}


//! fills the listbox with files.
void CGUIFileOpenDialog::fillListBox()
{
	if (!FileSystem || !FileBox)
		return;

	if (FileList)
		FileList->drop();

	FileBox->clear();

	FileList = FileSystem->createFileList();
	core::stringw s;

	for (s32 i=0; i<FileList->getFileCount(); ++i)
	{
		s = FileList->getFileName(i);
		FileBox->addItem(s.c_str(), FileList->isDirectory(i) ? GUI_ICON_DIRECTORY : GUI_ICON_FILE);
	}

	if (FileNameText)
	{
		s = FileSystem->getWorkingDirectory();
		FileNameText->setText(s.c_str());
	}
}


//! sends the event that the file has been selected.
void CGUIFileOpenDialog::sendSelectedEvent()
{
	SEvent event;
	event.EventType = EET_GUI_EVENT;
	event.GUIEvent.Caller = this;
	event.GUIEvent.EventType = EGET_FILE_SELECTED;
	Parent->OnEvent(event);
}

//! sends the event that the file choose process has been canceld
void CGUIFileOpenDialog::sendCancelEvent()
{
	SEvent event;
	event.EventType = EET_GUI_EVENT;
	event.GUIEvent.Caller = this;
	event.GUIEvent.EventType = EGET_FILE_CHOOSE_DIALOG_CANCELLED;
	Parent->OnEvent(event);
}

} // end namespace gui
} // end namespace engine
