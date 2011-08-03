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
 
#include "Frustum.h"
#include <fmathv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CMemory.h"
#include "SColor.h"


#include <pspdebug.h>
#define printf pspDebugScreenPrintf

int tempoMemcpy = 0;
f32 temp_ln[30];

extern int DEBUG_A;

using namespace engine;

//* Calcola la magnitudine di un vettore  e restituisce 1/sqrt utilizzato
//* la vfpu
//////////////////////////////////////////////////////////////////////////
f32 fnormalize(f32 X, f32 Y,  f32 Z)
{
	// if X,Y,Z == 0 length = 0
	if ( !X && !Y && !Z) return 0;


	f32 output;
	asm volatile (

	        "mtv    %1, s501\n"   // carica x y z in registri riga
	        "mtv    %2, s511\n"
	        "mtv    %3, s521\n"
	        "vdot.t s531, r501, r501\n" // esegue il dot product
	        "vrsq.s s531, s531\n" // t = 1/sqrt(t)
	        "mfv    %0, s531\n"   // restituisce
		: "=r" (output)
		: "r" (X), "r" (Y), "r" (Z));

	return output;
}
//* Calcola la frustum view
//////////////////////////////////////////////////////////////////////////
void CalculateViewFrustum( ViewFrustum* pFrustum, f32* pProMatrix, f32* pModMatrix )
{
	f32 fMatrix[16];
	int i;

	//* Moltiplica la matrice proiezione con la matrice modellazione
	fMatrix[0] = pModMatrix[0] * pProMatrix[0] + pModMatrix[1] * pProMatrix[4] + pModMatrix[2] * pProMatrix[8]  + pModMatrix[3] * pProMatrix[12];
	fMatrix[1] = pModMatrix[0] * pProMatrix[1] + pModMatrix[1] * pProMatrix[5] + pModMatrix[2] * pProMatrix[9]  + pModMatrix[3] * pProMatrix[13];
	fMatrix[2] = pModMatrix[0] * pProMatrix[2] + pModMatrix[1] * pProMatrix[6] + pModMatrix[2] * pProMatrix[10] + pModMatrix[3] * pProMatrix[14];
	fMatrix[3] = pModMatrix[0] * pProMatrix[3] + pModMatrix[1] * pProMatrix[7] + pModMatrix[2] * pProMatrix[11] + pModMatrix[3] * pProMatrix[15];

	fMatrix[4] = pModMatrix[4] * pProMatrix[0] + pModMatrix[5] * pProMatrix[4] + pModMatrix[6] * pProMatrix[8]  + pModMatrix[7] * pProMatrix[12];
	fMatrix[5] = pModMatrix[4] * pProMatrix[1] + pModMatrix[5] * pProMatrix[5] + pModMatrix[6] * pProMatrix[9]  + pModMatrix[7] * pProMatrix[13];
	fMatrix[6] = pModMatrix[4] * pProMatrix[2] + pModMatrix[5] * pProMatrix[6] + pModMatrix[6] * pProMatrix[10] + pModMatrix[7] * pProMatrix[14];
	fMatrix[7] = pModMatrix[4] * pProMatrix[3] + pModMatrix[5] * pProMatrix[7] + pModMatrix[6] * pProMatrix[11] + pModMatrix[7] * pProMatrix[15];

	fMatrix[8]  = pModMatrix[8] * pProMatrix[0] + pModMatrix[9] * pProMatrix[4] + pModMatrix[10] * pProMatrix[8]  + pModMatrix[11] * pProMatrix[12];
	fMatrix[9]  = pModMatrix[8] * pProMatrix[1] + pModMatrix[9] * pProMatrix[5] + pModMatrix[10] * pProMatrix[9]  + pModMatrix[11] * pProMatrix[13];
	fMatrix[10] = pModMatrix[8] * pProMatrix[2] + pModMatrix[9] * pProMatrix[6] + pModMatrix[10] * pProMatrix[10] + pModMatrix[11] * pProMatrix[14];
	fMatrix[11] = pModMatrix[8] * pProMatrix[3] + pModMatrix[9] * pProMatrix[7] + pModMatrix[10] * pProMatrix[11] + pModMatrix[11] * pProMatrix[15];

	fMatrix[12] = pModMatrix[12] * pProMatrix[0] + pModMatrix[13] * pProMatrix[4] + pModMatrix[14] * pProMatrix[8]  + pModMatrix[15] * pProMatrix[12];
	fMatrix[13] = pModMatrix[12] * pProMatrix[1] + pModMatrix[13] * pProMatrix[5] + pModMatrix[14] * pProMatrix[9]  + pModMatrix[15] * pProMatrix[13];
	fMatrix[14] = pModMatrix[12] * pProMatrix[2] + pModMatrix[13] * pProMatrix[6] + pModMatrix[14] * pProMatrix[10] + pModMatrix[15] * pProMatrix[14];
	fMatrix[15] = pModMatrix[12] * pProMatrix[3] + pModMatrix[13] * pProMatrix[7] + pModMatrix[14] * pProMatrix[11] + pModMatrix[15] * pProMatrix[15];

	//* Destro
	pFrustum->FrustrumPlane[0].normal.x = fMatrix[3]  - fMatrix[0];
	pFrustum->FrustrumPlane[0].normal.y = fMatrix[7]  - fMatrix[4];
	pFrustum->FrustrumPlane[0].normal.z = fMatrix[11] - fMatrix[8];
	pFrustum->FrustrumPlane[0].dist = fMatrix[15] - fMatrix[12];

	//* Sinistro
	pFrustum->FrustrumPlane[1].normal.x = fMatrix[3]  + fMatrix[0];
	pFrustum->FrustrumPlane[1].normal.y = fMatrix[7]  + fMatrix[4];
	pFrustum->FrustrumPlane[1].normal.z = fMatrix[11] + fMatrix[8];
	pFrustum->FrustrumPlane[1].dist = fMatrix[15] + fMatrix[12];

	//* Sopra
	pFrustum->FrustrumPlane[2].normal.x = fMatrix[3]  + fMatrix[1];
	pFrustum->FrustrumPlane[2].normal.y = fMatrix[7]  + fMatrix[5];
	pFrustum->FrustrumPlane[2].normal.z = fMatrix[11] + fMatrix[9];
	pFrustum->FrustrumPlane[2].dist = fMatrix[15] + fMatrix[13];

	//* Sotto
	pFrustum->FrustrumPlane[3].normal.x = fMatrix[3]  - fMatrix[1];
	pFrustum->FrustrumPlane[3].normal.y = fMatrix[7]  - fMatrix[5];
	pFrustum->FrustrumPlane[3].normal.z = fMatrix[11] - fMatrix[9];
	pFrustum->FrustrumPlane[3].dist = fMatrix[15] - fMatrix[13];

	//* Lontano
	pFrustum->FrustrumPlane[5].normal.x = fMatrix[3]  - fMatrix[2];
	pFrustum->FrustrumPlane[5].normal.y = fMatrix[7]  - fMatrix[6];
	pFrustum->FrustrumPlane[5].normal.z = fMatrix[11] - fMatrix[10];
	pFrustum->FrustrumPlane[5].dist = fMatrix[15] - fMatrix[14];

	//* Vicino
	pFrustum->FrustrumPlane[4].normal.x = fMatrix[3]  + fMatrix[2];
	pFrustum->FrustrumPlane[4].normal.y = fMatrix[7]  + fMatrix[6];
	pFrustum->FrustrumPlane[4].normal.z = fMatrix[11] + fMatrix[10];
	pFrustum->FrustrumPlane[4].dist = fMatrix[15] + fMatrix[14];




	//* Normalizza
	for( i = 0; i < 6; i++ )
		NormalizePlane( &pFrustum->FrustrumPlane[i] );

}

inline void CopyVertex(BspClipVertex *dst, BspClipVertex *src)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->normal[0] = src->normal[0];
	dst->normal[1] = src->normal[1];
	dst->normal[2] = src->normal[2];
	dst->u = src->u;
	dst->v = src->v;
	dst->color = src->color;
}

// Normalizza un piano
//////////////////////////////////////////////////////////////////////////
void NormalizePlane( Quake3BspPlane* pPlane )
{



	f32 output = fnormalize(pPlane->normal.x, pPlane->normal.y, pPlane->normal.z);

	//* Normalize Planae
	pPlane->normal.x *= output;
	pPlane->normal.y *= output;
	pPlane->normal.z *= output;
	pPlane->dist *= output;

}


inline int IsPointVisable( Quake3BspPlane* pPlane, f32 x, f32 y, f32 z )
{
	f32 output =  x*pPlane->normal.x+y*pPlane->normal.y+pPlane->normal.z*z; // vfpudotproduct(x,y,z,pPlane->normal.x,pPlane->normal.y,pPlane->normal.z);
	output += pPlane->dist;

	if( output >= 0 )
		return 1;


	return 0;
}

inline int IsPointVisableInt ( Quake3BspPlane* pPlane, int x, int y, int z )
{
	f32 output = x*pPlane->normal.x+y*pPlane->normal.y+pPlane->normal.z*z; // vfpudotproduct(x,y,z,pPlane->normal.x,pPlane->normal.y,pPlane->normal.z);
	output += pPlane->dist;

	if( output >= 0 )
		return 1;


	return 0;
}

inline f32 IsPointVisableFloat( Quake3BspPlane* pPlane, f32 x, f32 y, f32 z )
{
	f32 output =  x*pPlane->normal.x+y*pPlane->normal.y+pPlane->normal.z*z;
	output += pPlane->dist;
	return output;
}


int IsClusterVisableInFrustum( ViewFrustum* pFrustum, int* pMin, int* pMax )
{
	int i;

	for( i = 0; i < 6; i++ )
	{
		if( IsPointVisable( &pFrustum->FrustrumPlane[i], (f32)pMin[0], (f32)pMin[1], (f32)pMin[2] ) ) continue;
		if( IsPointVisable( &pFrustum->FrustrumPlane[i], (f32)pMax[0], (f32)pMin[1], (f32)pMin[2] ) ) continue;
		if( IsPointVisable( &pFrustum->FrustrumPlane[i], (f32)pMin[0], (f32)pMax[1], (f32)pMin[2] ) ) continue;
		if( IsPointVisable( &pFrustum->FrustrumPlane[i], (f32)pMax[0], (f32)pMax[1], (f32)pMin[2] ) ) continue;

		if( IsPointVisable( &pFrustum->FrustrumPlane[i], (f32)pMin[0], (f32)pMin[1], (f32)pMax[2] ) ) continue;
		if( IsPointVisable( &pFrustum->FrustrumPlane[i], (f32)pMax[0], (f32)pMin[1], (f32)pMax[2] ) ) continue;
		if( IsPointVisable( &pFrustum->FrustrumPlane[i], (f32)pMin[0], (f32)pMax[1], (f32)pMax[2] ) ) continue;
		if( IsPointVisable( &pFrustum->FrustrumPlane[i], (f32)pMax[0], (f32)pMax[1], (f32)pMax[2] ) ) continue;

		return 0;
	}

	return 1;
}

int IsClusterVisableInFrustumFloat( ViewFrustum* pFrustum, f32* pMin, f32* pMax )
{
	int i;

	for( i = 0; i < 6; i++ )
	{
		if( IsPointVisable( &pFrustum->FrustrumPlane[i], pMin[0], pMin[1], pMin[2] ) ) continue;
		if( IsPointVisable( &pFrustum->FrustrumPlane[i], pMax[0], pMin[1], pMin[2] ) ) continue;
		if( IsPointVisable( &pFrustum->FrustrumPlane[i], pMin[0], pMax[1], pMin[2] ) ) continue;
		if( IsPointVisable( &pFrustum->FrustrumPlane[i], pMax[0], pMax[1], pMin[2] ) ) continue;

		if( IsPointVisable( &pFrustum->FrustrumPlane[i], pMin[0], pMin[1], pMax[2] ) ) continue;
		if( IsPointVisable( &pFrustum->FrustrumPlane[i], pMax[0], pMin[1], pMax[2] ) ) continue;
		if( IsPointVisable( &pFrustum->FrustrumPlane[i], pMin[0], pMax[1], pMax[2] ) ) continue;
		if( IsPointVisable( &pFrustum->FrustrumPlane[i], pMax[0], pMax[1], pMax[2] ) ) continue;

		return 0;
	}

	return 1;
}

int IsClusterVisibleInFrustum( ViewFrustum* pFrustum, f32* pMin, f32* pMax )
{
	int i;
	for( i = 0; i < 5; i++ )
	{
		if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pMin[0], pMin[1], pMin[2] ) ) return 0;
		if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pMax[0], pMin[1], pMin[2] ) ) return 0;
		if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pMin[0], pMax[1], pMin[2] ) ) return 0;
		if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pMax[0], pMax[1], pMin[2] ) ) return 0;

		if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pMin[0], pMin[1], pMax[2] ) ) return 0;
		if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pMax[0], pMin[1], pMax[2] ) ) return 0;
		if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pMin[0], pMax[1], pMax[2] ) ) return 0;
		if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pMax[0], pMax[1], pMax[2] ) ) return 0;
	}

	return 1;
}

int IsAABInPlane(ViewFrustum* pFrustum, f32 *pMin, f32* pMax, int i)
{
	if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pMin[0], pMin[1], pMin[2] ) ) return 0;
	if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pMax[0], pMin[1], pMin[2] ) ) return 0;
	if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pMin[0], pMax[1], pMin[2] ) ) return 0;
	if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pMax[0], pMax[1], pMin[2] ) ) return 0;

	if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pMin[0], pMin[1], pMax[2] ) ) return 0;
	if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pMax[0], pMin[1], pMax[2] ) ) return 0;
	if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pMin[0], pMax[1], pMax[2] ) ) return 0;
	if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pMax[0], pMax[1], pMax[2] ) ) return 0;

	return 1;
}

int IsPolygonVisableInFrustum( ViewFrustum* pFrustum, void* pPolygon, unsigned int uStride, int iNumVertexs )
{
	BspClipVertex* pPolygonV1;
	int iCurrVert = 0;
	int i;
	pPolygonV1 = (BspClipVertex *) pPolygon;

	while (iCurrVert != iNumVertexs)
	{
		for (int i = 0; i < 5; i++)
		{
			if( !IsPointVisable( &pFrustum->FrustrumPlane[i], pPolygonV1[iCurrVert].x,
			                     pPolygonV1[iCurrVert].y, pPolygonV1[iCurrVert].z ) )
				return 0;
		}
		++iCurrVert;
	}

	return 1;
}


BspClipVertex* ClipPolygonToFrustum( ViewFrustum* pFrustum, BspClipVertex* pPolygon, int* iNumVertexs )
{
	return NULL;
}

inline BspClipVertex* ClipPolygonToPlane( Quake3BspPlane* pPlane, BspClipVertex* pPolygon, int* iNumVertexs )
{
	return NULL;
}

inline int ClipLineToPlane( Quake3BspPlane* pPlane, BspClipVertex* vStart, BspClipVertex* vEnd )
{
	return 0;
}
