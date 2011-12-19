/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __I_GUI_COMBO_BOX_H_INCLUDED__
#define __I_GUI_COMBO_BOX_H_INCLUDED__

#include "IGUIElement.h"

namespace engine
{
namespace gui
{
	//! Single line edit box for editing simple text.
	class IGUIComboBox : public IGUIElement
	{
	public:

		//! constructor
		IGUIComboBox(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_COMBO_BOX, environment, parent, id, rectangle) {}

		//! destructor
		~IGUIComboBox() {};

		//! Returns amount of items in box
		virtual s32 getItemCount() = 0;

		//! returns string of an item. the idx may be a value from 0 to itemCount-1
		virtual const wchar_t* getItem(s32 idx) = 0;

		//! adds an item and returns the index of it
		virtual s32 addItem(const wchar_t* text) = 0;

		//! deletes all items in the combo box
		virtual void clear() = 0;

		//! returns id of selected item. returns -1 if no item is selected.
		virtual s32 getSelected() = 0;

		//! sets the selected item. Set this to -1 if no item should be selected
		virtual void setSelected(s32 id) = 0;

	};


} // end namespace gui
} // end namespace engine

#endif


