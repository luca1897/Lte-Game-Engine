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
 
#ifndef __C_GUI_FILE_OPEN_DIALOG_H_INCLUDED__
#define __C_GUI_FILE_OPEN_DIALOG_H_INCLUDED__

#include "IGUIFileOpenDialog.h"
#include "IGUIButton.h"
#include "IGUIListBox.h"
#include "IFileSystem.h"

namespace engine
{
namespace gui
{

class CGUIFileOpenDialog : public IGUIFileOpenDialog
{
public:

//! constructor
CGUIFileOpenDialog(io::IFileSystem* fs, const wchar_t* title, IGUIEnvironment* environment, IGUIElement* parent, s32 id);

//! destructor
virtual ~CGUIFileOpenDialog();

//! returns the filename of the selected file. Returns NULL, if no file was selected.
virtual const wchar_t* getFilename();

//! called if an event happened.
virtual bool OnEvent(SEvent event);

//! draws the element and its children
virtual void draw();

private:

//! fills the listbox with files.
void fillListBox();

//! sends the event that the file has been selected.
void sendSelectedEvent();

//! sends the event that the file choose process has been canceld
void sendCancelEvent();

core::position2d<s32> DragStart;
core::stringw FileName;
bool Dragging;
IGUIButton* CloseButton;
IGUIButton* OKButton;
IGUIButton* CancelButton;
IGUIListBox* FileBox;
IGUIElement* FileNameText;
io::IFileSystem* FileSystem;

io::IFileList* FileList;
};


} // end namespace gui
} // end namespace engine

#endif

