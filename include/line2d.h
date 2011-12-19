/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __engine_LINE_2D_H_INCLUDED__
#define __engine_LINE_2D_H_INCLUDED__

#include "engineTypes.h"
#include "vector2d.h"

namespace engine
{
namespace core
{

//! 2D line between two points with intersection methods.
template <class T>
class line2d
{
	public:

		line2d(): start(0,0), end(1,1) {};
		line2d(T xa, T ya, T xb, T yb) : start(xa, ya), end(xb, yb) {};
		line2d(const vector2d<T>& start, const vector2d<T>& end) : start(start), end(end) {};
		line2d(const line2d<T>& other) :start(other.start), end(other.end) {};

		// operators

		line2d<T> operator+(const vector2d<T>& point) const { return line2d<T>(start + point, end + point); };
		line2d<T>& operator+=(const vector2d<T>& point) { start += point; end += point; return *this; };

		line2d<T> operator-(const vector2d<T>& point) const { return line2d<T>(start - point, end - point); };
		line2d<T>& operator-=(const vector2d<T>& point) { start -= point; end -= point; return *this; };

		bool operator==(const line2d<T>& other) const { return (start==other.start && end==other.end) || (end==other.start && start==other.end);};
		bool operator!=(const line2d<T>& other) const { return !(start==other.start && end==other.end) || (end==other.start && start==other.end);};

		// functions

		void setLine(const T& xa, const T& ya, const T& xb, const T& yb){start.set(xa, ya); end.set(xb, yb);}
		void setLine(const vector2d<T>& nstart, const vector2d<T>& nend){start.set(nstart); end.set(nend);}
		void setLine(const line2d<T>& line){start.set(line.start); end.set(line.end);}

		//! Returns length of line
		//! \return Returns length of line.
		f32 getLength() const { return start.getDistanceFrom(end); };

		//! Returns the vector of the line.
		//! \return Returns the vector of the line.
		vector2d<T> getVector() const { return vector2d<T>(start.X - end.X, start.Y - end.Y); };

		//! Tests if this line intersects with an other line.
		//! \param l: Other line to test intersection with.
		//! \param out: If there is an intersection, the location of the intersection will
		//! be stored in this vector.
		//! \return Returns true if there is an intersection, false if not.
		bool intersectWith(const line2d<T>& l, vector2d<T>& out)
		{
			
			float m1, m2;
			float v, t;
			v = (end.Y - start.Y);
			t = (end.X - start.X);
			
			if (v == 0)
				m1 = 0.00001;
			else
			if (t == 0)
				m1 = 100000;
			else
			m1 = v / t;
			
			
			v = (l.end.Y - l.start.Y);
			t = (l.end.X - l.start.X);
			
			if (v == 0)
				m2 = 0.00001;
			else
			if (t == 0)
				m2 = 100000;
			else
			m2 = v / t;
			
			
			if ( m1 == m2 )
			{
				return false;
      }
	
			float b1, b2;
			b1 = start.Y - m1 * start.X;
			b2 = l.start.Y - m2 * l.start.X;
		

			float x = (b1 - b2)/(m2 - m1);
			float y = m1 * x + b1;
			
			out.X = x;
			out.Y = y;
			
			f32 f = (f32)(end - start).getLengthSQ();
			f32 r = (f32)(l.end - l.start).getLengthSQ();
			
			return (((f32)out.getDistanceFromSQ(start) <= f && 
				(f32)out.getDistanceFromSQ(end) <= f) &&
				((f32)out.getDistanceFromSQ(l.start) <= r && 
				(f32)out.getDistanceFromSQ(l.end) <= r));
				
		}

		//! Returns unit vector of the line.
		//! \return Returns unit vector of this line.
		vector2d<T> getUnitVector()
		{
			T len = (T)1.0 / (T)getLength();
			return vector2d<T>((end.X - start.X) * len, (end.Y - start.Y) * len);
		}

		f32 getAngleWith(const line2d<T>& l)
		{
			vector2d<T> vect = getVector();
			vector2d<T> vect2 = l.getVector();
			return vectorAngle(vect.X, vect.Y, vect2.X, vect2.Y);
		}
		
		// member variables
		
		vector2d<T> start;
		vector2d<T> end;
};



} // end namespace core
} // end namespace engine

#endif


