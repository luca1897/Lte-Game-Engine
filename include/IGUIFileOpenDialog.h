/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __I_GUI_FILE_OPEN_DIALOG_H_INCLUDED__
#define __I_GUI_FILE_OPEN_DIALOG_H_INCLUDED__

#include "IGUIElement.h"

namespace engine
{
namespace gui
{

	//! Standard file chooser dialog.
	class IGUIFileOpenDialog : public IGUIElement
	{
	public:

		//! constructor
		IGUIFileOpenDialog(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_FILE_OPEN_DIALOG, environment, parent, id, rectangle) {}

		//! destructor
		virtual ~IGUIFileOpenDialog() {};

		//! Returns the filename of the selected file. Returns NULL, if no file was selected.
		virtual const wchar_t* getFilename() = 0;

	};


} // end namespace gui
} // end namespace engine

#endif


