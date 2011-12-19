/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __I_GUI_SCROLL_BAR_H_INCLUDED__
#define __I_GUI_SCROLL_BAR_H_INCLUDED__

#include "IGUIElement.h"

namespace engine
{
namespace gui
{

	//! Default scroll bar GUI element.
	class IGUIScrollBar : public IGUIElement
	{
	public:

		//! constructor
		IGUIScrollBar(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_SCROLL_BAR, environment, parent, id, rectangle) {}

		//! destructor
		~IGUIScrollBar() {};

		//! sets the maximum value of the scrollbar. must be > 0
		virtual void setMax(s32 max) = 0;

		//! gets the current position of the scrollbar
		virtual s32 getPos() = 0;

		//! sets the current position of the scrollbar
		virtual void setPos(s32 pos) = 0;
	};


} // end namespace gui
} // end namespace engine

#endif


