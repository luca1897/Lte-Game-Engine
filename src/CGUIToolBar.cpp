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
 
#include "CGUIToolBar.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIButton.h"
#include "IGUIFont.h"
#include "CGUIButton.h"

namespace engine
{
namespace gui
{

//! constructor
CGUIToolBar::CGUIToolBar(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
	: IGUIToolBar(environment, parent, id, rectangle), ButtonX(5)
{
	#ifdef _DEBUG
	setDebugName("CGUIToolBar");
	#endif

	// calculate position and find other menubars
	s32 y = 0;
	s32 parentwidth = 100;

	if (parent)
	{
		parentwidth = Parent->getAbsolutePosition().getWidth();

		const core::list<IGUIElement*>& children = parent->getChildren();
		core::list<IGUIElement*>::Iterator it = children.begin();

		for (; it != children.end(); ++it)
		{
			core::rect<s32> r = (*it)->getAbsolutePosition();
			if (r.UpperLeftCorner.X == 0 && r.UpperLeftCorner.Y <= y &&
			    r.LowerRightCorner.X == parentwidth)
				y = r.LowerRightCorner.Y;
		}
	}

	RelativeRect.UpperLeftCorner.X = 0;
	RelativeRect.UpperLeftCorner.Y = y;
	s32 height = 30;

	/*IGUISkin* skin = Environment->getSkin();
	   IGUIFont* font = skin->getFont();
	   if (font)
	   {
	   	s32 t = font->getDimension(L"A").Height + 5;
	   	if (t > height)
	   		height = t;
	   }*/

	RelativeRect.LowerRightCorner.X = parentwidth;
	RelativeRect.LowerRightCorner.Y = RelativeRect.UpperLeftCorner.Y + height;
	updateAbsolutePosition();
}



//! destructor
CGUIToolBar::~CGUIToolBar()
{
}



//! draws the element and its children
void CGUIToolBar::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;

	core::rect<s32> rect = AbsoluteRect;
	core::rect<s32>* clip = 0;

	// draw frame
	skin->draw3DToolBar(this, rect, clip);

	IGUIElement::draw();
}


//! Updates the absolute position.
void CGUIToolBar::updateAbsolutePosition()
{
	if (Parent)
		RelativeRect.LowerRightCorner.X = Parent->getAbsolutePosition().getWidth();

	IGUIElement::updateAbsolutePosition();
}


//! Adds a button to the tool bar
IGUIButton* CGUIToolBar::addButton(s32 id, const wchar_t* text,
                                   video::ITexture* img, video::ITexture* pressed, bool isPushButton,
                                   bool useAlphaChannel)
{
	ButtonX += 3;

	core::rect<s32> rectangle(ButtonX,2,0,0);
	rectangle.LowerRightCorner.X = rectangle.UpperLeftCorner.X + 23;
	rectangle.LowerRightCorner.Y = rectangle.UpperLeftCorner.Y + 22;

	ButtonX += rectangle.getWidth();

	IGUIButton* button = new CGUIButton(Environment, this, id, rectangle);
	button->drop();

	if (text)
		button->setText(text);

	if (img)
		button->setImage(img);

	if (pressed)
		button->setPressedImage(pressed);

	if (isPushButton)
		button->setIsPushButton(isPushButton);

	if (useAlphaChannel)
		button->setUseAlphaChannel(useAlphaChannel);

	return button;
}

} // end namespace gui
} // end namespace engine
