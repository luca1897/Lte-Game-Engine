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
 
#include "CGUIFont.h"
#include "os.h"

namespace engine
{
namespace gui
{

//! constructor
CGUIFont::CGUIFont(video::IVideoDriver* driver)
	: Driver(driver), Positions(382), Texture(0), WrongCharacter(0)
{
	#ifdef _DEBUG
	setDebugName("CGUIFont");
	#endif

	if (Driver)
		Driver->grab();
}



//! destructor
CGUIFont::~CGUIFont()
{
	if (Driver)
		Driver->drop();

	if (Texture)
		Texture->drop();
}



//! loads a font file
bool CGUIFont::load(io::IReadFile* file)
{
	if (!Driver)
		return false;

	return loadTexture(Driver->getTexture(file));
}


//! loads a font file
bool CGUIFont::load(const c8* filename)
{
	if (!Driver)
		return false;

	return loadTexture(Driver->getTexture(filename));
}



//! load & prepare font from ITexture
bool CGUIFont::loadTexture(video::ITexture* texture)
{
	if (!texture)
		return false;

	Texture = texture;
	Texture->grab();

	s32 lowerRightPostions = 0;

	switch(texture->getColorFormat())
	{
	case video::ECF_A1R5G5B5:
		readPositions16bit(texture, lowerRightPostions);
		break;
	case video::ECF_A8R8G8B8:
		readPositions32bit(texture, lowerRightPostions);
		break;
	default:
		os::Printer::log("Unsupported font texture color format.", ELL_ERROR);
		return false;
	}

	if (Positions.size() > 127)
		WrongCharacter = 127;

	return (!Positions.empty() && lowerRightPostions);
}



void CGUIFont::readPositions32bit(video::ITexture* texture, s32& lowerRightPostions)
{
	s32 pitch = texture->getPitch();
	core::dimension2d<s32> size = texture->getOriginalSize();

	s32* p = (s32*)texture->lock();
	if (!p)
	{
		os::Printer::log("Could not lock texture while preparing texture for a font.", ELL_ERROR);
		return;
	}

	s32 colorTopLeft = *p;
	s32 colorLowerRight = *(p+1);
	s32 colorBackGround = *(p+2);
#ifdef PPC
	s32 colorBackGroundWithAlphaTrue = (0xFF) | (~(0xFF) & colorBackGround);
	s32 colorBackGroundWithAlphaFalse = (0x00) | (~(0xFF) & colorBackGround);
	s32 colorFont = (0xFF) | (~(0xFF) & 0xFFFFFF00);
#else
	s32 colorBackGroundWithAlphaTrue = (0xFF<<24) | (~(0xFF<<24) & colorBackGround);
	s32 colorBackGroundWithAlphaFalse = (0x00<<24) | (~(0xFF<<24) & colorBackGround);
	s32 colorFont = (0xFF<<24) | (~(0xFF<<24) & 0x00FFFFFF);
#endif

	*(p+1) = colorBackGround;
	*(p+2) = colorBackGround;

	// start parsing

	core::position2d<s32> pos(0,0);

	c8* row = (c8*)((void*)p);

	for (pos.Y=0; pos.Y<size.Height; ++pos.Y)
	{
		p = (s32*)((void*)row);

		for (pos.X=0; pos.X<size.Width; ++pos.X)
		{
			if (*p == colorTopLeft)
			{
				*p = colorBackGroundWithAlphaFalse;
				Positions.push_back(core::rect<s32>(pos, pos));
			}
			else
			if (*p == colorLowerRight)
			{
				if (Positions.size()<=(u32)lowerRightPostions)
				{
					texture->unlock();
					lowerRightPostions = 0;
					return;
				}

				*p = colorBackGroundWithAlphaFalse;
				Positions[lowerRightPostions].LowerRightCorner = pos;
				++lowerRightPostions;
			}
			else
			if (*p == colorBackGround)
				*p = colorBackGroundWithAlphaFalse;
			else
				*p = colorFont;


			++p;
		}

		row += pitch;
	}


	// Positions parsed.

	texture->unlock();

	// output warnings
	if (!lowerRightPostions || !Positions.size())
		os::Printer::log("The amount of upper corner pixels or lower corner pixels is == 0, font file may be corrupted.", ELL_ERROR);
	else
	if (lowerRightPostions != (s32)Positions.size())
		os::Printer::log("The amount of upper corner pixels and the lower corner pixels is not equal, font file may be corrupted.", ELL_ERROR);
}


void CGUIFont::readPositions16bit(video::ITexture* texture, s32& lowerRightPostions)
{
	s32 pitch = texture->getPitch();
	core::dimension2d<s32> size = texture->getOriginalSize();

	s16* p = (s16*)texture->lock();
	if (!p)
	{
		os::Printer::log("Could not lock texture while preparing texture for a font.", ELL_ERROR);
		return;
	}

	s16 colorTopLeft = *p;
	s16 colorLowerRight = *(p+1);
	s16 colorBackGround = *(p+2);
	s16 colorBackGroundWithAlphaTrue = (0x1<<15) | (~(0x1<<15) & colorBackGround);
	s16 colorBackGroundWithAlphaFalse = (0x0<<15) | (~(0x1<<15) & colorBackGround);
	s16 colorFont = (0x1<<15) | (~(0x1<<15) & video::RGB16(255,255,255));

	*(p+1) = colorBackGround;
	*(p+2) = colorBackGround;

	// start parsing

	core::position2d<s32> pos(0,0);

	c8* row = (c8*)((void*)p);

	for (pos.Y=0; pos.Y<size.Height; ++pos.Y)
	{
		p = (s16*)((void*)row);

		for (pos.X=0; pos.X<size.Width; ++pos.X)
		{
			if (*p == colorTopLeft)
			{
				*p = colorBackGroundWithAlphaFalse;
				Positions.push_back(core::rect<s32>(pos, pos));
			}
			else
			if (*p == colorLowerRight)
			{
				if (Positions.size()<=(u32)lowerRightPostions)
				{
					texture->unlock();
					lowerRightPostions = 0;
					return;
				}

				*p = colorBackGroundWithAlphaFalse;
				Positions[lowerRightPostions].LowerRightCorner = pos;
				++lowerRightPostions;
			}
			else
			if (*p == colorBackGround)
				*p = colorBackGroundWithAlphaFalse;
			else
				*p = colorFont;


			++p;
		}

		row += pitch;
	}


	// Positions parsed.

	texture->unlock();

	// output warnings
	if (!lowerRightPostions || !Positions.size())
		os::Printer::log("The amount of upper corner pixels or lower corner pixels is == 0, font file may be corrupted.", ELL_ERROR);
	else
	if (lowerRightPostions != (s32)Positions.size())
		os::Printer::log("The amount of upper corner pixels and the lower corner pixels is not equal, font file may be corrupted.", ELL_ERROR);
}


//! returns the dimension of a text
core::dimension2d<s32> CGUIFont::getDimension(const wchar_t* text)
{
	core::dimension2d<s32> dim(0, Positions[0].getHeight());

	u32 n;

	for(const wchar_t* p = text; *p; ++p)
	{
		n = (*p) - 32;
		if ( n > Positions.size())
			n = WrongCharacter;

		dim.Width += Positions[n].getWidth();
	}

	return dim;
}


inline s32 CGUIFont::getWidthFromCharacter(wchar_t c)
{
	u32 n = c - 32;
	if (n > Positions.size())
		n = WrongCharacter;

	return Positions[n].getWidth();
}

#include <pspdebug.h>
#define printf pspDebugScreenPrintf
//! draws an text and clips it to the specified rectangle if wanted
void CGUIFont::draw(const wchar_t* text, const core::rect<s32>& position, video::SColor color, bool hcenter, bool vcenter, const core::rect<s32>* clip)
{
	if (!Driver)
		return;
//	printf("0\n");
	core::dimension2d<s32> textDimension;
	core::position2d<s32> offset = position.UpperLeftCorner;
//printf("1\n");
	if (hcenter || vcenter)
	{
		textDimension = getDimension(text);

		if (hcenter)
			offset.X = ((position.getWidth() - textDimension.Width)>>1) + offset.X;

		if (vcenter)
			offset.Y = ((position.getHeight() - textDimension.Height)>>1) + offset.Y;
	}

	u32 n;
	//printf("2\n");
	while(*text)
	{
		n = (*text) - 32;
		if ( n > Positions.size())
			n = WrongCharacter;
		Driver->draw2DImage(Texture, offset, Positions[n], clip, color, true);
		offset.X += Positions[n].getWidth();

		++text;
	}
}


//! Calculates the index of the character in the text which is on a specific position.
s32 CGUIFont::getCharacterFromPos(const wchar_t* text, s32 pixel_x)
{
	s32 x = 0;
	s32 idx = 0;

	while (text[idx])
	{
		x += getWidthFromCharacter(text[idx]);

		if (x >= pixel_x)
			return idx;

		++idx;
	}

	return -1;
}


} // end namespace gui
} // end namespace engine
