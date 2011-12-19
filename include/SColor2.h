/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __COLOR_H_INCLUDED__
#define __COLOR_H_INCLUDED__

#include "engineTypes.h"

namespace engine
{
namespace video  
{
	//! Creates a 16 bit A1R5G5B5 color 
	inline s16 RGB16(s32 r, s32 g, s32 b)
	{
		//return (((r>>3) & 0x1F)<<10) | (((g>>3) & 0x1F)<<5) | ((b>>3) & 0x1F);
		return	( r & 0xF8) << 7 |
				( g & 0xF8) << 2 |
				( b & 0xF8) >> 3;
	}

	//! Creates a 16 bit A1R5G5B5 color 
	inline s16 RGBA16(s32 r, s32 g, s32 b, s32 a)
	{
		return  (((a>>7) & 0x1)<<15) |
			    (((r>>3) & 0x1F)<<10) |
				(((g>>3) & 0x1F)<<5) |
				 ((b>>3) & 0x1F);
	}
	
	//! Converts a 32 bit (X8R8G8B8) color to a 16 A1R5G5B5 color
	inline s16 X8R8G8B8toA1R5G5B5(s32 color)
	{
//		return RGB16(color>>16, color>>8, color);
		return	( color & 0x00F80000) >> 9 |
				( color & 0x0000F800) >> 6 |
				( color & 0x000000F8) >> 3;
	}

	//! Converts a 32 bit (A8R8G8B8) color to a 16 A1R5G5B5 color
	inline u16 A8R8G8B8toA1R5G5B5(u32 color)
	{
		return	( color & 0x80000000) >> 16 |
				( color & 0x00F80000) >> 9  |
				( color & 0x0000F800) >> 6  |
				( color & 0x000000F8) >> 3;
	}


	//! Returns A8R8G8B8 Color from A1R5G5B5 color
	inline u32 A1R5G5B5toA8R8G8B8(u16 color)
	{
		return	( color & 0x8000 ) << 16 |
				( color & 0x7C00 ) << 9 |
				( color & 0x03E0 ) << 6 |
				( color & 0x001F ) << 3;

/*
		return	(((color >> 15)&0x1)<<31) |	(((color >> 10)&0x1F)<<19) |
				(((color >> 5)&0x1F)<<11) |	(color&0x1F)<<3;
*/
	}

	//! Returns the red component from A1R5G5B5 color
	inline s32 getAlpha(s16 color)
	{
		return ((color >> 11)&0x1);
	}

	//! Returns the red component from A1R5G5B5 color. 
	//! Shift left by 3 to get 8 bit value.
	inline s32 getRed(s16 color)
	{
		return ((color >> 10)&0x1F);
	}


	//! Returns the green component from A1R5G5B5 color
	//! Shift left by 3 to get 8 bit value.
	inline s32 getGreen(s16 color)
	{
		return ((color >> 5)&0x1F);
	}


	//! Returns the blue component from A1R5G5B5 color
	//! Shift left by 3 to get 8 bit value.
	inline s32 getBlue(s16 color)
	{
		return ((color)&0x1F);
	}

	//! Returns the luminance from a 16 bit A1R5G5B5 color 
	inline s32 getLuminance(s16 color)
	{
		return ((getRed(color)<<3) + (getGreen(color)<<3) + (getBlue(color)<<3)) / 3;
	}



	//! Returns A8R8G8B8 Color from R5G6B5 color
	inline s32 R5G6B5toA8R8G8B8(s16 color)
	{
		return	0xFF000000 & ((((color >> 11)&0x1F)<<19) |
				(((color >> 5)&0x3F)<<11) |	(color&0x1F)<<3);
	}


	//! Class representing a 32 bit ARGB color.
	/** The color values for red, green, blue
	and alpha are stored in a single s32. So all four values may be between 0 and 255.
	This class is used by most parts of the engine Engine
	to specify a color. An other way is using the class Colorf, which
	stores the color values in 4 floats.
	*/
	class SColor
	{
	public:

		//! Constructor of the Color. Does nothing. The color value is not
		//! initialized to save time.
		inline SColor() {}

		//! Constructs the color from 4 values representing the alpha, red, green and 
		//! blue components of the color. Must be values between 0 and 255.
		inline SColor (s32 a1, s32 r1, s32 g1, s32 b1)
			: a(a1), b(b1), r(r1), g(g1)	{}

		//! Constructs the color from a 32 bit value. Could be another color.
		inline SColor(s32 clr)
		 {
            a = (clr&0xFF000000)>>24;
            b = (clr&0xFF);
            g = (clr&0xFF00) >> 8;
            r = (clr&0xFF0000) >> 16;

			}

		//! Returns the alpha component of the color. The alpha component
		//! defines how transparent a color should be.
		//! 0 means not transparent, 255 means fully transparent.
		inline u8 getAlpha() const	{	return a;	}

		//! Returns the red component of the color. 
		//! \return Returns a value between 0 and 255, specifying how red the color is.
		//! 0 means dark, 255 means full red.
		inline u8 getRed() const	{	return r;	}

		//! Returns the green component of the color. 
		//! \return Returns a value between 0 and 255, specifying how green the color is.
		//! 0 means dark, 255 means full green.
		inline u8 getGreen() const	{	return g; }

		//! Returns the blue component of the color. 
		//! \return Returns a value between 0 and 255, specifying how blue the color is.
		//! 0 means dark, 255 means full blue.
		inline u8 getBlue() const	{	return b;	}

		//! Sets the alpha comonent of the Color. The alpha component
		//! defines how transparent a color should be.
		//! \param a: Has to be a value between 0 and 255. 
		//! 0 means not transparent, 255 means fully transparent.
		inline void setAlpha(s32 a) { this->a=a; }

		//! Sets the red comonent of the Color. 
		//! \param r: Has to be a value between 0 and 255. 
		//! 0 means dark red (=black), 255 means full red.
		inline void setRed(s32 r) { this->r=r; }

		//! Sets the green comonent of the Color. 
		//! \param g: Has to be a value between 0 and 255. 
		//! 0 means dark green (=black), 255 means full green.
		inline void setGreen(s32 g) { this->g=g;}

		//! Sets the blue comonent of the Color. 
		//! \param b: Has to be a value between 0 and 255. 
		//! 0 means dark blue (=black), 255 means full blue.
		inline void setBlue(s32 b) { this->b=b; }

		//! Calculates a 16 bit A1R5G5B5 value of this color.
		//! \return Returns the 16 bit A1R5G5B5 value of this color.
		inline s16 toA1R5G5B5() const { return RGBA16(r, g, b , a); };

		//! Converts color to open gl color format.
		//! \return Returns the 32 bit openGL color value.
		inline s32 toOpenGLColor() const 
		{
			return ((a)<<24) |
					(b<<16) |
					(g<<8) |
					(r);
		};

		//! Sets all four components of the color at once.
		//! Constructs the color from 4 values representing the alpha, red, green and 
		//! blue components of the color. Must be values between 0 and 255.
		//! \param a: Alpha component of the color. 
		//! The alpha component defines how transparent a color should be.
		//! Has to be a value between 0 and 255. 
		//! 0 means not transparent, 255 means fully transparent.
		//! \param r: Sets the red comonent of the Color. 
		//! Has to be a value between 0 and 255. 
		//! 0 means dark red (=black), 255 means full red.
		//! \param g: Sets the green comonent of the Color. 
		//! Has to be a value between 0 and 255. 
		//! 0 means dark green (=black), 255 means full green.
		//! \param b: Sets the blue comonent of the Color. 
		//! Has to be a value between 0 and 255. 
		//! 0 means dark blue (=black), 255 means full blue.
		inline void set(s32 a, s32 r, s32 g, s32 b) { this->a=a; this->b=b; this->r=r; this->g=g; }

		//! Compares the color to another color.
		//! \return Returns true if the colors are the same, and false if not.
		inline bool operator==(const SColor& other) const { return (other.a == a)&&(other.g == g)&&(other.r == r)&&(other.b == b); }

		//! Compares the color to another color.
		//! \return Returns true if the colors are different, and false if they are the same.
		inline bool operator!=(const SColor& other) const { return (other.a != a)||(other.r != r)||(other.g != g)||(other.b != b); }

    inline u32 color () const {
			return ((a)<<24) |
					(r<<16) |
					(g<<8) |
					(b);

    }
		//! Interpolates the color with a f32 value to an other color 
		//! \param other: Other color
		//! \param d: value between 0.0f and 1.0f
		//! \return Returns interpolated color.
		inline SColor getInterpolated(SColor other, f32 d) const
		{
			f32 inv = 1.0f - d;
			return SColor((s32)(other.getAlpha()*inv + getAlpha()*d),
				(s32)(other.getRed()*inv + getRed()*d),
				(s32)(other.getGreen()*inv + getGreen()*d),
				(s32)(other.getBlue()*inv + getBlue()*d));
		}
        
		//! color in A8B8G8R8 Format
		u8 a,b,g,r; 
	};


	//! Class representing a color with four floats.
	/**	The color values for red, green, blue
	and alpha are each stored in a 32 bit floating point variable.
	So all four values may be between 0.0f and 1.0f.
	This class is used rarely used by the engine Engine
	to specify a color. An other, faster way is using the class Color, which
	stores the color values in a single 32 bit integer. 
	*/
	class SColorf
	{
	public:

		//! Constructs a color. All values are initialised with 0.0f, resulting
		//! in a black color.
		SColorf() : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {};

		//! Constructs a color from three color values: red, green and blue.
		//! \param r: Red color component. Should be a value between 0.0f meaning
		//! very dark red (=black) and 1.0f, meaning full red.
		//! \param g: Green color component. Should be a value between 0.0f meaning
		//! very dark green (=black) and 1.0f, meaning full green.
		//! \param b: Blue color component. Should be a value between 0.0f meaning
		//! very dark blue (=black) and 1.0f, meaning full blue.
		SColorf(f32 r, f32 g, f32 b) : r(r), g(g), b(b), a(1.0f) {};

		//! Constructs a color from four color values: alpha, red, green and blue.
		//! \param a: Alpha color component of the color.
		//! The alpha component defines how transparent a color should be.
		//! Has to be a value between 0.0f and 1.0f,  
		//! 0.0f means not transparent, 1.0f means fully transparent.
		//! \param r: Red color component. Should be a value between 0.0f meaning
		//! very dark red (=black) and 1.0f, meaning full red.
		//! \param g: Green color component. Should be a value between 0.0f meaning
		//! very dark green (=black) and 1.0f, meaning full green.
		//! \param b: Blue color component. Should be a value between 0.0f meaning
		//! very dark blue (=black) and 1.0f, meaning full blue.
		SColorf(f32 r, f32 g, f32 b, f32 a) : r(r), g(g), b(b), a(a) {};

		//! Constructs a color from 32 bit Color.
		//! \param c: 32 bit color value from which this Colorf class is 
		//! constructed from.
		SColorf(SColor c) { const f32 inv = 1.0f / 255.0f; r = c.getRed() * inv; g = c.getGreen() * inv; b = c.getBlue() * inv;	a = c.getAlpha() * inv;	};

		//! Converts this color to a SColor without floats.
		SColor toSColor() const
		{
			return SColor((s32)(a*255.0f), (s32)(r*255.0f), (s32)(g*255.0f), (s32)(b*255.0f));
		}

		//! red color component
		f32 r;

		//! green color component
		f32 g;

		//! blue component
		f32 b;

		//! alpha color component
		f32 a;

		//! Sets three color components to new values at once.
		//! \param rr: Red color component. Should be a value between 0.0f meaning
		//! very dark red (=black) and 1.0f, meaning full red.
		//! \param gg: Green color component. Should be a value between 0.0f meaning
		//! very dark green (=black) and 1.0f, meaning full green.
		//! \param bb: Blue color component. Should be a value between 0.0f meaning
		//! very dark blue (=black) and 1.0f, meaning full blue.
		void set(f32 rr, f32 gg, f32 bb) {r = rr; g =gg; b = bb; };

		//! Sets all four color components to new values at once.
		//! \param a: Alpha component. 
		//! \param rr: Red color component. Should be a value between 0.0f meaning
		//! very dark red (=black) and 1.0f, meaning full red.
		//! \param gg: Green color component. Should be a value between 0.0f meaning
		//! very dark green (=black) and 1.0f, meaning full green.
		//! \param bb: Blue color component. Should be a value between 0.0f meaning
		//! very dark blue (=black) and 1.0f, meaning full blue.
		void set(f32 aa, f32 rr, f32 gg, f32 bb) {a = aa; r = rr; g =gg; b = bb; };

		//! Interpolates the color with a f32 value to an other color 
		//! \param other: Other color
		//! \param d: value between 0.0f and 1.0f
		//! \return Returns interpolated color.
		inline SColorf getInterpolated(SColorf other, f32 d) const
		{
			f32 inv = 1.0f - d;
			return SColorf(other.r*inv + r*d,
				other.g*inv + g*d, other.b*inv + b*d, other.a*inv + a*d);
		}
	};

} // end namespace video
} // end namespace engine

#endif


