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
 
#include "SColor.h"
#include "CColorConverter.h"

namespace engine
{
namespace video
{

//! converts a 4 bit palettized image into R5G5B5
void CColorConverter::convert4BitTo16BitFlipMengineor(const c8* in, s16* out, s32 width, s32 height, s32 pitch, const s32* palette)
{
	s32 shift = 0;
	out += width*height;
	s16* oout = out;

	for (s32 y=0; y<height; ++y)
	{
		shift = 4;

		out = oout - (y*width) - width;

		for (s32 x=0; x<width; ++x)
		{
			*out = X8R8G8B8toA1R5G5B5(palette[(u8)((*in >> shift) & 0xf)]);
			++out;

			shift -= 4;
			if (shift<0)
			{
				shift = 4;
				++in;
			}
		}

		if (shift !=4)
			++in;

		in+=pitch;
	}
}

//! converts a 8 bit palettized image into R5G5B5
void CColorConverter::convert8BitTo16Bit(const c8* in, s16* out, s32 width, s32 height, s32 pitch, const s32* palette)
{
	s32 lineWidth = width + pitch;
	const c8* p = in;

	for (s32 y=1; y<=height; ++y)
	{
		for (s32 x=0; x<width; ++x)
		{

			*out = X8R8G8B8toA1R5G5B5(palette[(u8)(*p)]);

			++out;
			++p;
		}

		p= in + y*lineWidth + pitch;
	}
}



//! converts a 8 bit palettized image into R5G5B5
void CColorConverter::convert8BitTo16BitFlipMengineor(const c8* in, s16* out, s32 width, s32 height, s32 pitch, const s32* palette)
{
	out += width * height;
	s32 lineWidth = width + pitch;
	const c8* p = in;

	for (s32 y=1; y<=height; ++y)
	{
		p= in + lineWidth*y - pitch;

		for (s32 x=0; x<width; ++x)
		{
			--out;
			--p;
			*out = X8R8G8B8toA1R5G5B5(palette[(u8)(*p)]);
		}
	}
}


//! converts a monochrome bitmap to A1R5G5B5 data
void CColorConverter::convert1BitTo16BitFlipMengineor(const c8* in, s16* out, s32 width, s32 height, s32 pitch)
{
	s16* p = out + width * height;

	for (s32 y=0; y<height; ++y)
	{
		s32 shift = 7;
		out = p - y * width - width;

		for (s32 x=0; x<width; ++x)
		{
			*out = *in>>shift & 0x01 ? (s16)0xffff : (s16)0x0000;
			++out;

			--shift;
			if (shift<0)
			{
				shift=7;
				++in;
			}
		}

		if (shift != 7)
			++in;

		in += pitch;
	}
}

//! converts R8G8B8 16 bit data to A1R5G5B5 data, and flips and
//! mengineors the image during the process.
void CColorConverter::convert16BitTo16BitFlipMengineor(const s16* in, s16* out, s32 width, s32 height, s32 pitch)
{
	const s16 *p = in;
	const s32 lineWidth = width + pitch;
	out += width * height;

	for (s32 y=0; y<height; ++y)
	{
		for (s32 x=0; x<width; ++x)
		{
			p = in + (width-x-1);
			--out;
			*out = *p;
		}

		in += lineWidth;
	}
}


//! converts R8G8B8 24 bit data to A1R5G5B5 data, and flips and
//! mengineors the image during the process.
void CColorConverter::convert24BitTo16BitFlipMengineor(const c8* in, s16* out, s32 width, s32 height, s32 pitch)
{
	const c8 *p = in;
	const s32 lineWidth = 3 * width + pitch;
	out += width * height;

	for (s32 y=0; y<height; ++y)
	{
		for (s32 x=0; x<width; ++x)
		{
			p = in + (width-x-1)*3;
			--out;
			*out = RGB16(p[2], p[1], *p);
		}

		in += lineWidth;
	}
}



//! copies R8G8B8 24 bit data to 24 data, and flips and
//! mengineors the image during the process.
void CColorConverter::convert24BitTo24BitFlipMengineorColorShuffle(const c8* in, c8* out, s32 width, s32 height, s32 pitch)
{
	const c8 *p = in;
	const s32 lineWidth = 3 * width + pitch;
	out += width * height * 3;

	for (s32 y=0; y<height; ++y)
	{
		for (s32 x=0; x<width; ++x)
		{
			p = in + (width-x-1)*3;
			out -= 3;
			out[0] = p[2];
			out[1] = p[1];
			out[2] = p[0];
		}

		in += lineWidth;
	}
}


//! converts R8G8B8 24 bit data to A1R5G5B5 data (used e.g for JPG to A1R5G5B5)
void CColorConverter::convert24BitTo16BitColorShuffle(const c8* in, s16* out, s32 width, s32 height, s32 pitch)
{
	const c8 *p = in;
	const s32 lineWidth = 3 * width + pitch;

	for (s32 y=0; y<height; ++y)
	{
		for (s32 x=0; x<width; ++x)
		{
			p = in + (width-x-1)*3;
			*out = RGB16(*p, p[1], p[2]);
			++out;
		}

		in += lineWidth;
	}
}


//! converts R8G8B8 24 bit data to A1R5G5B5 data (used e.g for JPG to A1R5G5B5)
//! accepts colors in different order.
void CColorConverter::convert24BitTo16BitFlipColorShuffle(const c8* in, s16* out, s32 width, s32 height, s32 pitch)
{
	const c8 *p = in;
	const s32 lineWidth = 3 * width + pitch;

	for (s32 y=0; y<height; ++y)
	{
		for (s32 x=0; x<width; ++x)
		{
			p = in + (x)*3;
			*out = RGB16(*p, p[1], p[2]);
			++out;
		}

		in += lineWidth;
	}
}


//! converts X8R8G8B8 32 bit data to A1R5G5B5 data, and flips and
//! mengineors the image during the process.
void CColorConverter::convert32BitTo16BitColorShuffle(const c8* in, s16* out, s32 width, s32 height, s32 pitch)
{
	const c8 *p = in;
	const s32 lineWidth = 4 * width + pitch;

	for (s32 y=0; y<height; ++y)
	{
		for (s32 x=0; x<width; ++x)
		{
			p = in + (width-x-1)*4;
			*out = RGB16(p[2], p[1], p[0]);
			++out;
		}

		in += lineWidth;
	}
}


//! converts X8R8G8B8 32 bit data to A1R5G5B5 data, and flips and
//! mengineors the image during the process.
void CColorConverter::convert32BitTo16BitFlipMengineorColorShuffle(const c8* in, s16* out, s32 width, s32 height, s32 pitch)
{
	const c8 *p = in;
	const s32 lineWidth = 4 * width + pitch;
	out += (width+pitch) * height;

	for (s32 y=0; y<height; ++y)
	{
		for (s32 x=0; x<width; ++x)
		{
			p = in + (width-x-1)*4;
			--out;
			*out = RGB16(p[2], p[1], p[0]);
		}

		in += lineWidth;
	}
}


//! Resizes the surface to a new size and converts it at the same time
//! to an A8R8G8B8 format, returning the pointer to the new buffer.
//! The returned pointer has to be deleted.
void CColorConverter::convert16bitToA8R8G8B8andResize(const s16* in, s32* out, s32 newWidth, s32 newHeight, s32 currentWidth, s32 currentHeight)
{
	if (!newWidth || !newHeight)
		return;

	// note: this is very very slow. (i didn't want to write a fast version.
	// but hopefully, nobody wants to convert surfaces every frame.

	s32 nDataSize = newWidth * newHeight;
	s32 nDataSizeInBytes = nDataSize * sizeof(s32);
	//s32* nData = new s32[nDataSize];

	f32 sourceXStep = (f32)currentWidth / (f32)newWidth;
	f32 sourceYStep = (f32)currentHeight / (f32)newHeight;
	f32 sy;
	s32 t;

	for (s32 x=0; x<newWidth; ++x)
	{
		sy = 0.0f;

		for (s32 y=0; y<newHeight; ++y)
		{
			t = in[(s32)(((s32)sy)*currentWidth + x*sourceXStep)];
			t = (((t >> 15)&0x1)<<31) |     (((t >> 10)&0x1F)<<19) |
			    (((t >> 5)&0x1F)<<11) | (t&0x1F)<<3;
			out[(s32)(y*newWidth + x)] = t;

			sy+=sourceYStep;
		}
	}
}


//! copies X8R8G8B8 32 bit data, and flips and
//! mengineors the image during the process.
void CColorConverter::convert32BitTo32BitFlipMengineor(const s32* in, s32* out, s32 width, s32 height, s32 pitch)
{
	const s32 *p = in;
	const s32 lineWidth = width;
	out += height * width;

	for (s32 y=0; y<height; ++y)
	{
		for (s32 x=0; x<width; ++x)
		{
			p = in + (width-x-1);
			--out;

#ifdef PPC
			{
				char    *zi = (char *)p;
				char    *zo = (char *)out;

				zo[0] = zi[2];
				zo[1] = zi[1];
				zo[2] = zi[0];
				zo[3] = zi[3];

			}
#else
			*out = *p;
#endif
		}

		in += lineWidth;
	}
}

} // end namespace video
} // end namespace engine
