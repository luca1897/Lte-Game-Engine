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
 
#include "CGUIModalScreen.h"
#include "IGUIEnvironment.h"
#include "os.h"
#include "IVideoDriver.h"
#include "IGUISkin.h"

namespace engine
{
namespace gui
{

//! constructor
CGUIModalScreen::CGUIModalScreen(IGUIEnvironment* environment, IGUIElement* parent, s32 id)
	: IGUIElement(EGUIET_MODAL_SCREEN, environment, parent, id, parent->getAbsolutePosition()),
	MouseDownTime(0)
{
	#ifdef _DEBUG
	setDebugName("CGUIModalScreen");
	#endif
}


//! destructor
CGUIModalScreen::~CGUIModalScreen()
{
}


//! called if an event happened.
bool CGUIModalScreen::OnEvent(SEvent event)
{
	switch(event.EventType)
	{
	case EET_MOUSE_INPUT_EVENT:
		switch(event.MouseInput.Event)
		{
		case EMIE_LMOUSE_PRESSED_DOWN:
			MouseDownTime = os::Timer::getTime();
		}
	}

	return IGUIElement::OnEvent(event);
}


//! draws the element and its children
void CGUIModalScreen::draw()
{
	u32 now = os::Timer::getTime();
	if (now - MouseDownTime < 300 && (now / 70)%2)
	{
		core::list<IGUIElement*>::Iterator it = Children.begin();
		core::rect<s32> r;
		video::IVideoDriver* driver = Environment->getVideoDriver();
		video::SColor c = Environment->getSkin()->getColor(gui::EGDC_3D_HIGH_LIGHT);

		for (; it != Children.end(); ++it)
		{
			r = (*it)->getAbsolutePosition();
			r.LowerRightCorner.X += 1;
			r.LowerRightCorner.Y += 1;
			r.UpperLeftCorner.X -= 1;
			r.UpperLeftCorner.Y -= 1;

			driver->draw2DRectangle(c, r, &AbsoluteClippingRect);
		}
	}

	IGUIElement::draw();
}



//! Removes a child.
void CGUIModalScreen::removeChild(IGUIElement* child)
{
	IGUIElement::removeChild(child);

	if (Children.empty())
		remove();
}


void CGUIModalScreen::updateAbsolutePosition()
{
	core::rect<s32> parentRect(0,0,0,0);

	if (Parent)
	{
		parentRect = Parent->getAbsolutePosition();
		RelativeRect.UpperLeftCorner.X = 0;
		RelativeRect.UpperLeftCorner.Y = 0;
		RelativeRect.LowerRightCorner.X = parentRect.getWidth();
		RelativeRect.LowerRightCorner.Y = parentRect.getHeight();
	}

	IGUIElement::updateAbsolutePosition();
}




} // end namespace gui
} // end namespace engine

