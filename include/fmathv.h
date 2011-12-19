/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __ENGINE_FAST_MATH__
#define __ENGINE_FAST_MATH__
#include <math.h>
const float PI          = 3.1415926535897932384626433832795f;
const float PI2         = 6.283185307179586476925286766559f;
const float PID2        = 1.5707963267948966192313216916398f;
const float PID4        = 0.7853981633974483096156608458198f;
const float PI_SQR      = 9.8696044010893586188344909998762f;

#define POW3(x) x*x*x
#define POW5(x) POW3(x)*x*x
#define POW7(x) POW5(x)*x*x
#define POW9(x) POW7(x)*x*x
#define POW11(x) POW9(x)*x*x
#define POW13(x) POW11(x)*x*x
#define POW15(x) POW13(x)*x*x
#define POW17(x) POW15(x)*x*x

#define FATT3 6
#define FATT5 120
#define FATT7 5040
#define FATT9 362880
#define FATT11 39916800
#define FATT13 6227020800
#define FATT15 1307674368000
#define FATT17 355687428096000

#define POW2(x) x*x
#define POW4(x) POW2(x)*x*x
#define POW6(x) POW4(x)*x*x
#define POW8(x) POW6(x)*x*x
#define POW10(x) POW8(x)*x*x
#define POW12(x) POW10(x)*x*x
#define POW14(x) POW12(x)*x*x
#define POW16(x) POW14(x)*x*x

#define FATT2 2
#define FATT4 24
#define FATT6 720
#define FATT8 40320
#define FATT10 3628800
#define FATT12 479001600
#define FATT14 87178291200
#define FATT16 20922789888000


inline float FastAbs(float x)
{ 
  return x >= 0.f ? x : -x;
}

//! Fast sine calculation using the vfpu
/** \param x: value in radiants
    \return value between -1 and 1 */
inline float FastSin(float x)
{ 
   float ret;
   __asm__ volatile ( 
      "mtv %1, s531\n" 
      "vcst.s s431, VFPU_2_PI\n" 
      "vmul.s s631, s431, s531\n" 
      "vsin.s s531, s631\n" // or vcos.s 
      "mfv %0, s531\n" 
      : "=r"(ret) : "r"(x)); 

   return ret; 
}

//! Fast cosine calculation using the vfpu
/** \param x: value in radiants
    \return value between -1 and 1 */
inline float FastCos(float x)
{
   float ret;
   __asm__ volatile ( 
      "mtv %1, s531\n" 
      "vcst.s s431, VFPU_2_PI\n" 
      "vmul.s s631, s431, s531\n" 
      "vcos.s s531, s631\n" // or vcos.s 
      "mfv %0, s531\n" 
      : "=r"(ret) : "r"(x)); 

   return ret; 
}

//! Fast arctangent calculation using the vfpu
/** \param x: value in R
    \return arcotangent of x */
inline float FastAtan(float x)
{
 float ret;
 __asm__ volatile (
  "mtv %1, s531\n"			  // t = x
  "vone.s s532\n"
  "vsgn.s s431, s531\n"   // z = sign(x)
  "vmul.s s531, s531, s531\n"  // t = t*t
  "vadd.s s531, s531, s532\n" // t++
  "vsqrt.s s531, s531\n" // t = sqrt(t)
  "vdiv.s s531, s532, s531\n" // t = 1/t
  "vasin.s s531, s531\n" // t = asin(t)
  "vcst.s s521, VFPU_PI_2\n" // r = PI/2
  "vmul.s s531, s531, s521\n"
  "vsub.s s521, s521, s531\n" // r = pi/2 - asin = acos
  "vmul.s s521, s521, s431\n" // r = r*z ( set sign ) 
  "mfv %0, s521" : "=r" ( ret ) : "r" ( x )); // store result
  return ret;
}

//! Fast arcsine calculation using the vfpu
/** \param x: value between -1 and 1
    \return value between -pi/2 and pi/2 */
inline float FastAsin(float x)
{
   if (x < -1 || x > 1)
    return 0;

   float ret;
   __asm__ volatile ( 
      "mtv %1, s531\n" 
      "vasin.s s531, s531\n"
      "vcst.s s431, VFPU_PI_2\n"
      "vmul.s s531, s531, s431\n"
      "mfv %0, s531\n" 
      : "=r"(ret) : "r"(x)); 

   return ret; 
}

//! Fast arccosine calculation using the vfpu
/** \param x: value between -1 and 1
    \return value between 0 and pi */
inline float FastAcos(float x)
{
   if (x < -1 || x > 1)
    return 0;

 float ret;
 __asm__ volatile (
  "mtv %1, s531\n"
  "vasin.s s531, s531\n"
  "vcst.s s431, VFPU_PI_2\n"
  "vmul.s s531, s431, s531\n"
  "vsub.s s531, s431, s531\n"
  "mfv %0, s531\n" : "=r" ( ret ) : "r" ( x ));
  return ret;
}

//! Fast atan2 calculation using the vfpu
/** \param x: x coord
    \param y: y coord
    \return arctangent of y/x using the signs of both arguments to determine the quadrant of the
     return value */
inline float FastAtan2(float y, float x)
{
 if ( x > 0)
   return FastAtan(y/x);

 return atan2(y, x);
}

//! Fast square root calculation using the cpu
/** \param x: value to calc sqrt
    \return square root of x */
inline float FastSqrt(float x) {
  float output;
  asm("sqrt.s %0,%1" : "=f" (output) : "f" (x));
  return output;
}


#endif

