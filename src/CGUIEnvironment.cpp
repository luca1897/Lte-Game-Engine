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
 
#include "CGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUISkin.h"

#include "CGUIButton.h"
#include "CGUIWindow.h"
#include "CGUIScrollBar.h"
#include "CGUIFont.h"
#include "CGUIImage.h"
#include "CGUIMeshViewer.h"
#include "CGUICheckBox.h"
#include "CGUIListBox.h"
#include "CGUIFileOpenDialog.h"
#include "CGUIStaticText.h"
#include "CGUIEditBox.h"
#include "CGUIInOutFader.h"
#include "CGUIMessageBox.h"
#include "CGUIProgressBar.h"
#include "CGUIModalScreen.h"
#include "CGUITabControl.h"
#include "CGUIContextMenu.h"
#include "CGUIComboBox.h"
#include "CGUIMenu.h"
#include "CGUIToolBar.h"

#include "CGUISkin.h"

#include "BuildInFont.h"
#include "os.h"

namespace engine
{
namespace gui
{

//! constructor
CGUIEnvironment::CGUIEnvironment(io::IFileSystem* fs, video::IVideoDriver* driver, IOSOperator* op)
	: IGUIElement(EGUIET_ELEMENT, 0, 0, 0, core::rect<s32>(core::position2d<s32>(0,0),
	                                                       driver ? driver->getScreenSize() : core::dimension2d<s32>(0,0))),
	Hovered(0), CurrentSkin(0), Driver(driver),
	Focus(0), FileSystem(fs), UserReceiver(0), Operator(op)
{
	if (Driver)
		Driver->grab();

	if (FileSystem)
		FileSystem->grab();

	if (Operator)
		Operator->grab();

	#ifdef _DEBUG
	IGUIEnvironment::setDebugName("CGUIEnvironment IGUIEnvironment");
	IGUIElement::setDebugName("CGUIEnvironment IGUIElement");
	#endif

	loadBuidInFont();

	IGUISkin* skin = createSkin(EGST_WINDOWS_METALLIC);
	setSkin(skin);
	skin->drop();
}

IGUIProgressBar* CGUIEnvironment::addProgressBar(const core::rect<s32>& rectangle, bool horizontal, bool border, bool background, IGUIElement* parent, s32 id){
	if(!parent) parent = this;
	IGUIProgressBar* bar = new CGUIProgressBar(horizontal, border, background, this, parent, id, rectangle);
	bar->drop();
	return bar;
}


//! destructor
CGUIEnvironment::~CGUIEnvironment()
{
	if (Hovered)
		Hovered->drop();

	if (CurrentSkin)
		CurrentSkin->drop();

	if (Driver)
		Driver->drop();

	if (Focus)
		Focus->drop();

	if (FileSystem)
		FileSystem->drop();

	if (Operator)
		Operator->drop();

	// delete all fonts

	for (u32 i=0; i<Fonts.size(); ++i)
		Fonts[i].Font->drop();
}



void CGUIEnvironment::loadBuidInFont()
{
	const c8* filename = "#DefaultFont";

#ifdef PPC
	for (int i=0; i<sizeof(BuildInFontData)/sizeof(s32); i++) BuildInFontData[i] = OSReadSwapInt32(&BuildInFontData[i],0);
#endif

	io::IReadFile* file = io::createMemoryReadFile(BuildInFontData, BuildInFontDataSize, filename, false);

	CGUIFont* font = new CGUIFont(Driver);
	if (!font->load(file))
	{
		os::Printer::log("Error: Could not load built-in Font.", ELL_ERROR);
		font->drop();
		file->drop();
		return;
	}

	SFont f;
	f.Filename = filename;
	f.Font = font;
	Fonts.push_back(f);

	file->drop();
}



//! draws all gui elements
void CGUIEnvironment::drawAll()
{
	if (Driver)
	{
		core::dimension2d<s32> dim = Driver->getScreenSize();
		if (AbsoluteRect.LowerRightCorner.X != dim.Width ||
		    AbsoluteRect.LowerRightCorner.Y != dim.Height)
		{
			// resize gui environment
			RelativeRect.LowerRightCorner.X = Driver->getScreenSize().Width;
			RelativeRect.LowerRightCorner.Y = Driver->getScreenSize().Height;
			AbsoluteClippingRect = RelativeRect;
			AbsoluteRect = RelativeRect;
			updateAbsolutePosition();
		}
	}

	draw();
}


//! sets the focus to an element
void CGUIEnvironment::setFocus(IGUIElement* element)
{
	if (Focus == element)
		return;

	removeFocus(Focus);

	Focus = element;
	if (Focus)
		Focus->grab();
}


//! removes the focus from an element
void CGUIEnvironment::removeFocus(IGUIElement* element)
{
	if (Focus && Focus==element)
	{
		SEvent e;
		e.EventType = EET_GUI_EVENT;
		e.GUIEvent.Caller = this;
		e.GUIEvent.EventType = EGET_ELEMENT_FOCUS_LOST;
		Focus->OnEvent(e);
		Focus->drop();
		Focus = 0;
	}
}


//! Returns if the element has focus
bool CGUIEnvironment::hasFocus(IGUIElement* element)
{
	bool ret = (element == Focus);
	_engine_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return ret;
}



//! returns the current video driver
video::IVideoDriver* CGUIEnvironment::getVideoDriver()
{
	return Driver;
}



//! called by ui if an event happened.
bool CGUIEnvironment::OnEvent(SEvent event)
{
	if (UserReceiver && event.GUIEvent.Caller != this)
		return UserReceiver->OnEvent(event);

	return false;
}


void CGUIEnvironment::updateHoveredElement(core::position2d<s32> mousePos)
{
	IGUIElement* lastHovered = Hovered;

	Hovered = getElementFromPoint(mousePos);

	if (Hovered)
	{
		Hovered->grab();

		if (Hovered != lastHovered)
		{
			SEvent event;
			event.EventType = EET_GUI_EVENT;

			if (lastHovered)
			{
				event.GUIEvent.Caller = lastHovered;
				event.GUIEvent.EventType = EGET_ELEMENT_LEFT;
				lastHovered->OnEvent(event);
			}

			event.GUIEvent.Caller = Hovered;
			event.GUIEvent.EventType = EGET_ELEMENT_HOVERED;
			Hovered->OnEvent(event);
		}
	}

	if (lastHovered)
		lastHovered->drop();
}


//! This sets a new event receiver for gui events. Usually you do not have to
//! use this method, it is used by the internal engine.
void CGUIEnvironment::setUserEventReceiver(IEventReceiver* evr)
{
	UserReceiver = evr;
}


//! posts an input event to the environment
bool CGUIEnvironment::postEventFromUser(SEvent event)
{
	switch(event.EventType)
	{
	case EET_GUI_EVENT :
		// hey, why is the user sending gui events..?
		break;
	case EET_MOUSE_INPUT_EVENT:

		// sending input to focus, stopping of focus processed input

		if (Focus && Focus->OnEvent(event))
			return true;

		if (!Focus) // focus could have died in last call
		{
			// trying to send input to hovered element
			updateHoveredElement(core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y));

			if (Hovered && Hovered != this)
				return Hovered->OnEvent(event);
		}
		break;
	case EET_KEY_INPUT_EVENT:
		if (Focus && Focus != this)
			return Focus->OnEvent(event);
		break;
	} // end switch

	return false;
}



//! returns the current gui skin
IGUISkin* CGUIEnvironment::getSkin()
{
	return CurrentSkin;
}


//! Sets a new GUI Skin
void CGUIEnvironment::setSkin(IGUISkin* skin)
{
	if (CurrentSkin)
		CurrentSkin->drop();

	CurrentSkin = skin;

	if (CurrentSkin)
		CurrentSkin->grab();
}

//! Creates a new GUI Skin based on a template.
/** \return Returns a pointer to the created skin.
   If you no longer need the image, you should call IGUISkin::drop().
   See IUnknown::drop() for more information. */
IGUISkin* CGUIEnvironment::createSkin(EGUI_SKIN_TYPE type)
{
	IGUISkin* skin = new CGUISkin(type, Driver);
	skin->setFont(getBuiltInFont());
	return skin;
}


//! adds an button. The returned pointer must not be dropped.
IGUIButton* CGUIEnvironment::addButton(const core::rect<s32>& rectangle, IGUIElement* parent, s32 id, const wchar_t* text)
{
	IGUIButton* button = new CGUIButton(this, parent ? parent : this, id, rectangle);
	if (text)
		button->setText(text);
	button->drop();
	return button;
}


//! adds a window. The returned pointer must not be dropped.
IGUIWindow* CGUIEnvironment::addWindow(const core::rect<s32>& rectangle, bool modal,
                                       const wchar_t* text, IGUIElement* parent, s32 id)
{
	parent = parent ? parent : this;

	if (modal)
	{
		parent = new CGUIModalScreen(this, parent, -1);
		parent->drop();
	}

	IGUIWindow* win = new CGUIWindow(this, parent, id, rectangle);
	if (text)
		win->setText(text);
	win->drop();

	return win;
}


//! Adds a message box.
IGUIWindow* CGUIEnvironment::addMessageBox(const wchar_t* caption, const wchar_t* text,
                                           bool modal, s32 flag, IGUIElement* parent, s32 id)
{
	if (!CurrentSkin)
		return 0;

	parent = parent ? parent : this;

	core::rect<s32> rect;
	core::dimension2d<s32> screenDim, msgBoxDim;

	screenDim.Width = parent->getAbsolutePosition().getWidth();
	screenDim.Height = parent->getAbsolutePosition().getHeight();
	msgBoxDim.Width = CurrentSkin->getSize(gui::EGDS_MESSAGE_BOX_WIDTH);
	msgBoxDim.Height = CurrentSkin->getSize(gui::EGDS_MESSAGE_BOX_HEIGHT);

	rect.UpperLeftCorner.X = (screenDim.Width - msgBoxDim.Width) / 2;
	rect.UpperLeftCorner.Y = (screenDim.Height - msgBoxDim.Height) / 2;
	rect.LowerRightCorner.X = rect.UpperLeftCorner.X + msgBoxDim.Width;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + msgBoxDim.Height;

	if (modal)
	{
		parent = new CGUIModalScreen(this, parent, -1);
		parent->drop();
	}

	IGUIWindow* win = new CGUIMessageBox(this, caption, text, flag,
	                                     parent, id, rect);

	win->drop();
	return win;
}


//! adds a scrollbar. The returned pointer must not be dropped.
IGUIScrollBar* CGUIEnvironment::addScrollBar(bool horizontal, const core::rect<s32>& rectangle, IGUIElement* parent, s32 id)
{
	IGUIScrollBar* bar = new CGUIScrollBar(horizontal, this, parent ? parent : this, id, rectangle);
	bar->drop();
	return bar;
}


//! Adds an image element.
IGUIImage* CGUIEnvironment::addImage(video::ITexture* image, core::position2d<s32> pos,
                                     bool useAlphaChannel, IGUIElement* parent, s32 id, const wchar_t* text)
{
	if (!image)
		return 0;

	core::dimension2d<s32> sz = image->getOriginalSize();
	IGUIImage* img = new CGUIImage(this, parent ? parent : this,
	                               id, core::rect<s32>(pos, sz));

	if (text)
		img->setText(text);

	if (useAlphaChannel)
		img->setUseAlphaChannel(true);

	img->setImage(image);

	img->drop();
	return img;
}


//! adds an image. The returned pointer must not be dropped.
IGUIImage* CGUIEnvironment::addImage(const core::rect<s32>& rectangle, IGUIElement* parent, s32 id, const wchar_t* text)
{
	IGUIImage* img = new CGUIImage(this, parent ? parent : this,
	                               id, rectangle);

	if (text)
		img->setText(text);

	img->drop();
	return img;
}


//! adds an mesh viewer. The returned pointer must not be dropped.
IGUIMeshViewer* CGUIEnvironment::addMeshViewer(const core::rect<s32>& rectangle, IGUIElement* parent, s32 id, const wchar_t* text)
{
	IGUIMeshViewer* v = new CGUIMeshViewer(this, parent ? parent : this,
	                                       id, rectangle);

	if (text)
		v->setText(text);

	v->drop();
	return v;
}


//! adds a checkbox
IGUICheckBox* CGUIEnvironment::addCheckBox(bool checked, const core::rect<s32>& rectangle, IGUIElement* parent, s32 id, const wchar_t* text)
{
	IGUICheckBox* b = new CGUICheckBox(checked, this,
	                                   parent ? parent : this, id, rectangle);

	if (text)
		b->setText(text);

	b->drop();
	return b;
}



//! adds a list box
IGUIListBox* CGUIEnvironment::addListBox(const core::rect<s32>& rectangle,
                                         IGUIElement* parent, s32 id,
                                         bool drawBackground)
{
	IGUIListBox* b = new CGUIListBox(this, parent ? parent : this, id, rectangle,
	                                 true, drawBackground, false);
	b->setIconFont(getBuiltInFont());
	b->drop();
	return b;

}



//! adds a file open dialog. The returned pointer must not be dropped.
IGUIFileOpenDialog* CGUIEnvironment::addFileOpenDialog(const wchar_t* title,
                                                       bool modal,
                                                       IGUIElement* parent, s32 id)
{
	parent = parent ? parent : this;

	if (modal)
	{
		parent = new CGUIModalScreen(this, parent, -1);
		parent->drop();
	}

	IGUIFileOpenDialog* d = new CGUIFileOpenDialog(FileSystem, title,
	                                               this, parent, id);

	d->drop();
	return d;
}



//! adds a static text. The returned pointer must not be dropped.
IGUIStaticText* CGUIEnvironment::addStaticText(const wchar_t* text,
                                               const core::rect<s32>& rectangle,
                                               bool border,
                                               bool wordWrap,
                                               IGUIElement* parent, s32 id,
                                               bool background)
{
	IGUIStaticText* d = new CGUIStaticText(text, border, this,
	                                       parent ? parent : this, id, rectangle, background);

	d->setWordWrap(wordWrap);
	d->drop();

	return d;
}



//! Adds an edit box. The returned pointer must not be dropped.
IGUIEditBox* CGUIEnvironment::addEditBox(const wchar_t* text,
                                         const core::rect<s32>& rectangle,
                                         bool border, IGUIElement* parent,
                                         s32 id)
{
	IGUIEditBox* d = new CGUIEditBox(text, border, this,
	                                 parent ? parent : this, id, rectangle, Operator);

	d->drop();
	return d;
}

//! Adds a tab control to the environment.
IGUITabControl* CGUIEnvironment::addTabControl(const core::rect<s32>& rectangle,
                                               IGUIElement* parent, bool fillbackground, bool border, s32 id)
{
	IGUITabControl* t = new CGUITabControl(this, parent ? parent : this,
	                                       rectangle, fillbackground, border, id);
	t->drop();
	return t;
}


//! Adds tab to the environment.
IGUITab* CGUIEnvironment::addTab(const core::rect<s32>& rectangle,
                                 IGUIElement* parent, s32 id)
{
	IGUITab* t = new CGUITab(-1, this, parent ? parent : this,
	                         rectangle, id);
	t->drop();
	return t;
}


//! Adds a context menu to the environment.
IGUIContextMenu* CGUIEnvironment::addContextMenu(const core::rect<s32>& rectangle,
                                                 IGUIElement* parent, s32 id)
{
	IGUIContextMenu* c = new CGUIContextMenu(this,
	                                         parent ? parent : this, id, rectangle);
	c->drop();
	return c;
}


//! Adds a menu to the environment.
IGUIContextMenu* CGUIEnvironment::addMenu(IGUIElement* parent, s32 id)
{
	if (!parent)
		parent = this;

	IGUIContextMenu* c = new CGUIMenu(this,
	                                  parent, id, core::rect<s32>(0,0,
	                                                              parent->getAbsolutePosition().getWidth(),
	                                                              parent->getAbsolutePosition().getHeight()));

	c->drop();
	return c;
}

//! Adds a toolbar to the environment. It is like a menu is always placed on top
//! in its parent, and contains buttons.
IGUIToolBar* CGUIEnvironment::addToolBar(IGUIElement* parent, s32 id)
{
	if (!parent)
		parent = this;

	IGUIToolBar* b = new CGUIToolBar(this, parent, id, core::rect<s32>(0,0,10,10));
	b->drop();
	return b;
}



//! Adds an element for fading in or out.
IGUIInOutFader* CGUIEnvironment::addInOutFader(const core::rect<s32>* rectangle, IGUIElement* parent, s32 id)
{
	core::rect<s32> rect;

	if (rectangle)
		rect = *rectangle;
	else
	if (Driver)
		rect = core::rect<s32>(core::position2d<s32>(0,0), Driver->getScreenSize());

	if (!parent)
		parent = this;

	IGUIInOutFader* fader = new CGUIInOutFader(this, parent, id, rect);
	fader->drop();
	return fader;
}


//! Adds a combo box to the environment.
IGUIComboBox* CGUIEnvironment::addComboBox(const core::rect<s32>& rectangle,
                                           IGUIElement* parent, s32 id)
{
	IGUIComboBox* t = new CGUIComboBox(this, parent ? parent : this,
	                                   id, rectangle);
	t->drop();
	return t;

}



//! returns the font
IGUIFont* CGUIEnvironment::getFont(const c8* filename)
{
	// search existing font

	SFont f;
	if (!filename)
		filename = "";

	f.Filename = filename;
	f.Filename.make_lower();

	s32 index = Fonts.binary_search(f);
	if (index != -1)
		return Fonts[index].Font;

	// not existing yet. try to load font.

	CGUIFont* font = new CGUIFont(Driver);
	if (!font->load(filename))
	{
		font->drop();
		return 0;
	}

	// add to fonts.

	f.Font = font;
	Fonts.push_back(f);

	return font;
}



//! returns default font
IGUIFont* CGUIEnvironment::getBuiltInFont()
{
	if (Fonts.empty())
		return 0;

	return Fonts[0].Font;
}


//! Returns the root gui element.
IGUIElement* CGUIEnvironment::getRootGUIElement()
{
	return this;
}



//! creates an GUI Environment
IGUIEnvironment* createGUIEnvironment(io::IFileSystem* fs, video::IVideoDriver* Driver,
                                      IOSOperator* op)
{
	return new CGUIEnvironment(fs, Driver, op);
}

} // end namespace gui
} // end namespace engine

