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
 
  #include "CGUIProgressBar.h"
namespace engine {
namespace gui {
CGUIProgressBar::CGUIProgressBar(bool horizontal, bool border, bool background, IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
	: IGUIProgressBar(horizontal, border, background, environment, parent, id, rectangle){
	dHorizontal = horizontal;
	dBorder = border;
	dBackground = background;
	setMax();
	setPos();
	setFontText();
	setColorBack();
	setColorFront();
	setColorText();
}
CGUIProgressBar::~CGUIProgressBar(){
}
void CGUIProgressBar::setMax(s32 m){
	pMax = m;
	if(pMax < 1) pMax = 1;
}
void CGUIProgressBar::setPos(s32 p){
	pPos = p;
	if(pPos < 0) pPos = 0;
	if(pPos > pMax) pPos = pMax;
}
void CGUIProgressBar::setColorBack(video::SColor color){
	cBack = color;
}
void CGUIProgressBar::setColorFront(video::SColor color){
	cFront = color;
}
void CGUIProgressBar::setColorText(video::SColor color){
	cText = color;
}
void CGUIProgressBar::setFontText(IGUIFont* font){
	oFont = font;
}
void CGUIProgressBar::draw(){
	if(!IsVisible) return;
	static IGUISkin* skin = Environment->getSkin();
	if(!skin) return;
	static engine::video::IVideoDriver* driver = Environment->getVideoDriver();
	if(!driver) return;
	core::rect<s32> frameRect(AbsoluteRect);
	if(dBackground)
		driver->draw2DRectangle(cBack, frameRect, &AbsoluteClippingRect);
	if(dBorder){
		skin->draw3DSunkenPane(this, 0, true, false, frameRect, &AbsoluteClippingRect);
		frameRect.UpperLeftCorner.X += 1;
		frameRect.UpperLeftCorner.Y += 1;
		frameRect.LowerRightCorner.X -= 1;
		frameRect.LowerRightCorner.Y -= 1;
	}
	core::rect<s32> progRect(frameRect);
	if(dHorizontal)
		progRect.LowerRightCorner.X = progRect.UpperLeftCorner.X + (pPos * (frameRect.LowerRightCorner.X - frameRect.UpperLeftCorner.X) / pMax);
	else
		progRect.UpperLeftCorner.Y = progRect.LowerRightCorner.Y - (pPos * (frameRect.LowerRightCorner.Y - frameRect.UpperLeftCorner.Y) / pMax);
	driver->draw2DRectangle(cFront, progRect, &AbsoluteClippingRect);
	if(Text.size()){
		if(!oFont) oFont = skin->getFont();
		if(oFont){
			frameRect.UpperLeftCorner.X += ((frameRect.getWidth() - oFont->getDimension(Text.c_str()).Width) / 2);
			oFont->draw(Text.c_str(), frameRect, cText, false, true, &AbsoluteClippingRect);
		}
	}
	IGUIElement::draw();
}
}
}
