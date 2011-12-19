/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __I_GUI_IMAGE_H_INCLUDED__
#define __I_GUI_IMAGE_H_INCLUDED__

#include "IGUIElement.h"
#include "ITexture.h"

namespace engine
{
namespace gui
{

	//! GUI element displaying an image.
	class IGUIImage : public IGUIElement
	{
	public:

		//! constructor
		IGUIImage(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_IMAGE, environment, parent, id, rectangle) {}

		//! destructor
		~IGUIImage() {};

		//! sets an image
		virtual void setImage(video::ITexture* image) = 0;

		//! sets if the image should use its alpha channel to draw itself
		virtual void setUseAlphaChannel(bool use) = 0;
	};


} // end namespace gui
} // end namespace engine

#endif


