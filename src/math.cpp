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
 
#include <engineTypes.h>
#include <pspvfpu.h>
#include <fmathv.h>
#include <pspdebug.h>
#define printf pspDebugScreenPrintf
extern struct pspvfpu_context *ctx;
extern int inormalize;

#define __FAST_MATH__
#include <math.h>


f32 vfpunormalize(f32 X, f32 Y,  f32 Z)
{
	if ( !X && !Y && !Z) return 0;


	// return 1.f/FastSqrt<f32>(vfpudotproduct(X,Y,Z,X,Y,Z));

	/*  pspvfpu_use_matrices(ctx,
	   0, VMAT5);*/
	f32 output;
	asm volatile (

	        "mtv    %1, s501\n"                                   // load x,y,z into the vfpu as a row vector
	        "mtv    %2, s511\n"
	        "mtv    %3, s521\n"
	        "vdot.t s531, r501, r501\n"                           // t = v dot v (= x*x + y*y + z*z)
	        "vrsq.s s531, s531\n"                                 // t = 1/sqrt(t)
	        "mfv    %0, s531\n"                                   // return result
		: "=r" (output)
		: "r" (X), "r" (Y), "r" (Z));

	return output;
}

f32 vfpudotproduct(f32 x,f32 y,f32 z,f32 x1,f32 x2,f32 x3)
{

	return x*x1+y*x2+x3*z;

	                #if 0
	f32 output;


	f32 rs = 0 ;
	f32 res = 0;



	asm volatile (

	        "mtv    %1, s501\n"                                   // load x,y,z into the vfpu as a row vector
	        "mtv    %2, s511\n"
	        "mtv    %3, s521\n"
	        "mtv    %4, s401\n"
	        "mtv    %5, s411\n"
	        "mtv    %6, s421\n"
	        "vdot.t s531, r501, r401\n"                           // t = v dot v (= x*x + y*y + z*z)
	        "mfv    %0, s531\n"                                   // return result
		: "=r" (output)
		: "r" (x), "r" (y), "r" (z), "r" ( x1 ),"r" (x2),"r" (x3));




	return output;
	        #endif
}

f32 vfpugetlengthsq(f32 x, f32 y, f32 z)
{
	#if 0

	//	        pspvfpu_use_matrices(ctx,
	// 0, VMAT5);
	f32 output;
	asm volatile (

	        "mtv    %1, s501\n"                                   // load x,y,z into the vfpu as a row vector
	        "mtv    %2, s511\n"
	        "mtv    %3, s521\n"
	        "vdot.t s531, r501, r501\n"                           // t = v dot v (= x*x + y*y + z*z)
	        "mfv    %0, s531\n"                                   // return result
		: "=r" (output)
		: "r" (x), "r" (y), "r" (z));


	return output;
       #endif
	return x*x+y*y+z*z;
}

f32 vfpugetlength(f32 X,f32 Y,f32 Z)
{
	f32 output;


	if (!X && !Y && !Z) return 0;


	//pspvfpu_use_matrices(ctx,
	// 0, VMAT5);

	asm volatile (

	        "mtv    %1, s501\n"                                   // load x,y,z into the vfpu as a row vector
	        "mtv    %2, s511\n"
	        "mtv    %3, s521\n"
	        "vdot.t s531, r501, r501\n"                           // t = v dot v (= x*x + y*y + z*z)
	        "vrsq.s s531, s531\n"                                 // t = 1/sqrt(t)
	        "mfv    %0, s531\n"                                   // return result
		: "=r" (output)
		: "r" (X), "r" (Y), "r" (Z));




	return 1/output;

}
