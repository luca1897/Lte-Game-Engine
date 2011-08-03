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
 
#ifndef __C_GUI_FONT_H_INCLUDED__
#define __C_GUI_FONT_H_INCLUDED__

#include "IGUIFont.h"
#include "engineString.h"
#include "IVideoDriver.h"
#include "engineArray.h"

namespace engine
{
namespace gui
{

class CGUIFont : public IGUIFont
{
public:

//! constructor
CGUIFont(video::IVideoDriver* Driver);

//! destructor
virtual ~CGUIFont();

//! loads a font file
bool load(const c8* filename);

//! loads a font file
bool load(io::IReadFile* file);

//! draws an text and clips it to the specified rectangle if wanted
virtual void draw(const wchar_t* text, const core::rect<s32>& position, video::SColor color, bool hcenter=false, bool vcenter=false, const core::rect<s32>* clip=0);

//! returns the dimension of a text
virtual core::dimension2d<s32> getDimension(const wchar_t* text);

//! Calculates the index of the character in the text which is on a specific position.
virtual s32 getCharacterFromPos(const wchar_t* text, s32 pixel_x);

private:

//! load & prepare font from ITexture
bool loadTexture(video::ITexture* texture);

void readPositions16bit(video::ITexture* texture, s32& lowerRightPositions);
void readPositions32bit(video::ITexture* texture, s32& lowerRightPositions);

inline s32 getWidthFromCharacter(wchar_t c);

video::IVideoDriver* Driver;
core::array< core::rect<s32> > Positions;
video::ITexture* Texture;
s32 WrongCharacter;
};

} // end namespace gui
} // end namespace engine

#endif

