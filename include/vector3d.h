/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __engine_POINT_3D_H_INCLUDED__
#define __engine_POINT_3D_H_INCLUDED__

#include <math.h>
#include "fmathv.h"
#include "engineMath.h"
#include "engineTypes.h"

namespace engine
{
namespace core
{
	
	//! 3d vector template class with lots of operators and methods.
	template <class T>
	class vector3d  
	{
	public:

		vector3d(): X(0), Y(0), Z(0) {};
		vector3d(T nx, T ny, T nz) : X(nx), Y(ny), Z(nz) {};
		vector3d(const vector3d<T>& other)	:X(other.X), Y(other.Y), Z(other.Z) {};

		// operators

		vector3d<T> operator-() const { return vector3d<T>(-X, -Y, -Z);   }

		vector3d<T>& operator=(const vector3d<T>& other)	{ X = other.X; Y = other.Y; Z = other.Z; return *this; }

		vector3d<T> operator+(const vector3d<T>& other) const { return vector3d<T>(X + other.X, Y + other.Y, Z + other.Z);	}
		vector3d<T>& operator+=(const vector3d<T>& other)	{ X+=other.X; Y+=other.Y; Z+=other.Z; return *this; }

		vector3d<T> operator-(const vector3d<T>& other) const { return vector3d<T>(X - other.X, Y - other.Y, Z - other.Z);	}
		vector3d<T>& operator-=(const vector3d<T>& other)	{ X-=other.X; Y-=other.Y; Z-=other.Z; return *this; }

		vector3d<T> operator*(const vector3d<T>& other) const { return vector3d<T>(X * other.X, Y * other.Y, Z * other.Z);	}
		vector3d<T>& operator*=(const vector3d<T>& other)	{ X*=other.X; Y*=other.Y; Z*=other.Z; return *this; }
		vector3d<T> operator*(const T v) const { return vector3d<T>(X * v, Y * v, Z * v);	}
		vector3d<T>& operator*=(const T v) { X*=v; Y*=v; Z*=v; return *this; }

		vector3d<T> operator/(const vector3d<T>& other) const { return vector3d<T>(X / other.X, Y / other.Y, Z / other.Z);	}
		vector3d<T>& operator/=(const vector3d<T>& other)	{ X/=other.X; Y/=other.Y; Z/=other.Z; return *this; }
		vector3d<T> operator/(const T v) const { T i=(T)1.0/v; return vector3d<T>(X * i, Y * i, Z * i);	}
		vector3d<T>& operator/=(const T v) { T i=(T)1.0/v; X*=i; Y*=i; Z*=i; return *this; }

		bool operator<=(const vector3d<T>&other) const { return X<=other.X && Y<=other.Y && Z<=other.Z;};
		bool operator>=(const vector3d<T>&other) const { return X>=other.X && Y>=other.Y && Z>=other.Z;};

		bool operator==(const vector3d<T>& other) const { return other.X==X && other.Y==Y && other.Z==Z; }
		bool operator!=(const vector3d<T>& other) const { return other.X!=X || other.Y!=Y || other.Z!=Z; }

		// functions

		//! returns if this vector equals the other one, taking floating point rounding errors into account
		bool equals(const vector3d<T>& other)
		{
			return core::equals(X, other.X) &&
				   core::equals(Y, other.Y) &&
				   core::equals(Z, other.Z);
		}

		void set(const T nx, const T ny, const T nz) {X=nx; Y=ny; Z=nz; }
		void set(const vector3d<T>& p) { X=p.X; Y=p.Y; Z=p.Z;}

		//! Returns length of the vector.
		f32 getLength() const {
			
       if (X == 0 && Y == 0 && Z == 0) 
         return 0;

 			  float output;
		    asm volatile(
     	  
     			  		"mtv    %1, s501\n"             // load x,y,z into the vfpu as a row vector
        	      "mtv    %2, s511\n"
	              "mtv    %3, s521\n"
                "vdot.t s531, r501, r501\n"     // t = v dot v (= x*x + y*y + z*z)
                "vsqrt.s s531, s531\n"           // t = 1/sqrt(t)
                "mfv    %0, s531\n"             // store result
               	:"=r" (output)
         :"r" (X), "r" (Y), "r" (Z));

         return output;
	   }

		//! Returns squared length of the vector.
		/** This is useful because it is much faster then
		getLength(). */
		f32 getLengthSQ() const {
			return X*X+Y*Y+Z*Z;
    }

		//! Returns the dot product with another vector.
		T dotProduct(const vector3d<T>& other) const
		{
			
			return X*other.X+Y*other.Y+Z*other.Z;
		}

		//! Returns distance from an other point.
		/** Here, the vector is interpreted as point in 3 dimensional space. */
		f32 getDistanceFrom(const vector3d<T>& other) const
		{
			f32 vx = X - other.X; f32 vy = Y - other.Y; f32 vz = Z - other.Z;
      if (vx == 0 && vy == 0 && vz == 0) 
      return 0;

 		  float output;
		  asm volatile(
     	  
     			  		"mtv    %1, s501\n"             // load x,y,z into the vfpu as a row vector
        	      "mtv    %2, s511\n"
	              "mtv    %3, s521\n"
                "vdot.t s531, r501, r501\n"     // t = v dot v (= x*x + y*y + z*z)
                "vsqrt.s s531, s531\n"           // t = 1/sqrt(t)
                "mfv    %0, s531\n"             // store result
               	:"=r" (output)
         :"r" (vx), "r" (vy), "r" (vz));

       return output;
	
		}

		//! Returns squared distance from an other point. 
		/** Here, the vector is interpreted as point in 3 dimensional space. */
		f32 getDistanceFromSQ(const vector3d<T>& other) const
		{
			f32 vx = X - other.X; f32 vy = Y - other.Y; f32 vz = Z - other.Z;
			return vx*vx + vy*vy + vz*vz;
		}

		//! Calculates the cross product with another vector
		vector3d<T> crossProduct(const vector3d<T>& p) const
		{
			return vector3d<T>(Y * p.Z - Z * p.Y, Z * p.X - X * p.Z, X * p.Y - Y * p.X);
		}

		//! Returns if this vector interpreted as a point is on a line between two other points.
		/** It is assumed that the point is on the line. */
		bool isBetweenPoints(const vector3d<T>& begin, const vector3d<T>& end) const
		{
			f32 f = (f32)(end - begin).getLengthSQ();
			return (f32)getDistanceFromSQ(begin) < f && 
				(f32)getDistanceFromSQ(end) < f;
		}

		//! Normalizes the vector.
		vector3d<T>& normalize()
		{
			float op = 1;

      if (X != 0 || Y != 0 || Z != 0)
      {
		    asm volatile(
     	  
     			  		"mtv    %1, s501\n"             // load x,y,z into the vfpu as a row vector
        	      "mtv    %2, s511\n"
	              "mtv    %3, s521\n"
                "vdot.t s531, r501, r501\n"     // t = v dot v (= x*x + y*y + z*z)
                "vrsq.s s531, s531\n"           // t = 1/sqrt(t)
                "mfv    %0, s531\n"             // store result
               	:"=r" (op)
         :"r" (X), "r" (Y), "r" (Z));
      }
      
			X *= op;
			Y *= op;
			Z *= op;
			return *this; 
		}

		//! Sets the lenght of the vector to a new value
		void setLength(T newlength)
		{
			normalize();
			*this *= newlength;
		}

		//! Inverts the vector.
		void invert()
		{
			X *= -1.0f;
			Y *= -1.0f;
			Z *= -1.0f;
		}

		void rotateXZBy(f32 degrees, const vector3d<T>& center)
		{
			degrees *=GRAD_PI2;
			T cs = (T) FastCos(degrees);
			T sn = (T) FastSin(degrees);
			X -= center.X;
			Z -= center.Z;
			set(X*cs - Z*sn, Y, X*sn + Z*cs);
			X += center.X;
			Z += center.Z;
		}

		void rotateXYBy(f32 degrees, const vector3d<T>& center)
		{
			degrees *=GRAD_PI2;
			T cs = (T) FastCos(degrees);
			T sn = (T) FastSin(degrees);
			X -= center.X;
			Y -= center.Y;
			set(X*cs - Y*sn, X*sn + Y*cs, Z);
			X += center.X;
			Y += center.Y;
		}

		void rotateYZBy(f32 degrees, const vector3d<T>& center)
		{
			degrees *=GRAD_PI2;
			T cs = (T) FastCos(degrees);
			T sn = (T) FastSin(degrees);
			Z -= center.Z;
			Y -= center.Y;
			set(X, Y*cs - Z*sn, Y*sn + Z*cs);
			Z += center.Z;
			Y += center.Y;
		}

		//! Returns interpolated vector.
		/** \param other: other vector to interpolate between
		\param d: value between 0.0f and 1.0f. */
		vector3d<T> getInterpolated(const vector3d<T>& other, f32 d) const
		{
			f32 inv = 1.0f - d;
			return vector3d<T>(other.X*inv + X*d,
								other.Y*inv + Y*d,
								other.Z*inv + Z*d);
		}

		//! Gets the Y and Z rotations of a vector.
		/** Thanks to Arras on the engine forums to add this method.
		 \return A vector representing the rotation in degrees of
		this vector. The Z component of the vector will always be 0. */
		vector3d<T> getHorizontalAngle()
		{
			vector3d<T> angle;

			angle.Y = (T)FastAtan2(X, Z); 
			angle.Y *= (f32)GRAD_PI;
			    
			if (angle.Y < 0.0f) angle.Y += 360.0f; 
			if (angle.Y >= 360.0f) angle.Y -= 360.0f; 
			    
			f32 z1; 
			z1 = (T)FastSqrt(X*X + Z*Z); 
			    
			angle.X = (T)FastAtan2(z1, Y); 
			angle.X *= (f32)GRAD_PI;
			angle.X -= 90.0f; 
			    
			if (angle.X < 0.0f) angle.X += 360.0f; 
			if (angle.X >= 360) angle.X -= 360.0f; 

			return angle;
		}

		//! Fills an array of 4 values with the vector data (usually floats).
		/** Useful for setting in shader constants for example. The fourth value
		 will always be 0. */
		void getAs4Values(T* array)
		{
			array[0] = X;
			array[1] = Y;
			array[2] = Z;
			array[3] = 0;
		}


		// member variables

		T X, Y, Z;
	};


	//! Typedef for a f32 3d vector.
	typedef vector3d<f32> vector3df;
	//! Typedef for an integer 3d vector.
	typedef vector3d<s32> vector3di;

} // end namespace core
} // end namespac engine

#endif


