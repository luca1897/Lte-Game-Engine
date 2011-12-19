/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __I_GUI_TOOL_BAR_H_INCLUDED__
#define __I_GUI_TOOL_BAR_H_INCLUDED__

#include "IGUIElement.h"

namespace engine
{
namespace video
{
	class ITexture;
}
namespace gui
{
	class IGUIButton;

	//! Stays at the top of its parent like the menu bar and contains tool buttons
	class IGUIToolBar : public IGUIElement
	{
	public:

		//! constructor
		IGUIToolBar(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_TOOL_BAR, environment, parent, id, rectangle) {}

		//! destructor
		~IGUIToolBar() {};

		//! Adds a button to the tool bar
		virtual IGUIButton* addButton(s32 id=-1, const wchar_t* text=0,
			video::ITexture* img=0, video::ITexture* pressedimg=0,
			bool isPushButton=false, bool useAlphaChannel=false) = 0;
	};


} // end namespace gui
} // end namespace engine

#endif


