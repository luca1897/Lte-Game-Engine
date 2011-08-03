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
 
#include "CGUIImage.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"

namespace engine
{
namespace gui
{



//! constructor
CGUIImage::CGUIImage(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
	: IGUIImage(environment, parent, id, rectangle), Texture(0), UseAlphaChannel(false)
{
	#ifdef _DEBUG
	setDebugName("CGUIImage");
	#endif
}



//! destructor
CGUIImage::~CGUIImage()
{
	if (Texture)
		Texture->drop();
}



//! sets an image
void CGUIImage::setImage(video::ITexture* image)
{
	if (Texture)
		Texture->drop();

	Texture = image;

	if (Texture)
		Texture->grab();
}



//! draws the element and its children
void CGUIImage::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	engine::video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> rect = AbsoluteRect;

	if (Texture)
		driver->draw2DImage(Texture, AbsoluteRect.UpperLeftCorner,
		                    core::rect<s32>(core::position2d<s32>(0,0), Texture->getOriginalSize()),
		                    &AbsoluteClippingRect, video::SColor(255,255,255,255), UseAlphaChannel);
	else
		driver->draw2DRectangle(skin->getColor(EGDC_3D_DARK_SHADOW), AbsoluteRect, &AbsoluteClippingRect);

	IGUIElement::draw();
}


//! sets if the image should use its alpha channel to draw itself
void CGUIImage::setUseAlphaChannel(bool use)
{
	UseAlphaChannel = use;
}



} // end namespace gui
} // end namespace engine

