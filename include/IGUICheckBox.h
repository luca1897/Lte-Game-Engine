/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __I_GUI_CHECKBOX_H_INCLUDED__
#define __I_GUI_CHECKBOX_H_INCLUDED__

#include "IGUIElement.h"

namespace engine
{
namespace gui
{

	//! GUI Check box interface.
	class IGUICheckBox : public IGUIElement
	{
	public:

		//! constructor
		IGUICheckBox(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_CHECK_BOX, environment, parent, id, rectangle) {}

		//! destructor
		~IGUICheckBox() {};

		//! Set if box is checked.
		virtual void setChecked(bool checked) = 0;

		//! Returns true if box is checked.
		virtual bool isChecked() = 0;
	};

} // end namespace gui
} // end namespace engine

#endif


