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
 
//////////////////////////////////////////////////////////////////////////
// View Frustum
//////////////////////////////////////////////////////////////////////////
// View Frustrum for surface removal
// Thanks to "Allen Sherrod" & his Bsp loader for help


// Define Entry
//////////////////////////////////////////////////////////////////////////
#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

#include "engineTypes.h"
#include "FileFormatQuakeBsp.h"

#define EPSILON2 0.00001

// View Frustum Structure
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	Quake3BspPlane FrustrumPlane[6];
} ViewFrustum;

// Bsp Clipped Vertex
//////////////////////////////////////////////////////////////////////////
typedef struct {
	f32 u,v;
	unsigned int color;
	f32 normal[3];
	f32 x,y,z;
} BspClipVertex;

// Functions
//////////////////////////////////////////////////////////////////////////

extern BspClipVertex* Buffer1, *Buffer2;


void CalculateViewFrustum( ViewFrustum* pFrustum, f32* pProMatrix, f32* pModMatrix );

//* Normalise Bsp Plane
void NormalizePlane( Quake3BspPlane* pPlane );

//* Is Cluster Visable in Frustum
int IsClusterVisableInFrustum( ViewFrustum* pFrustum, int* pMin, int* pMax );

//* Is Polygon Visable
int IsPolygonVisableInFrustum( ViewFrustum* pFrustum, void* pPolygon, unsigned int uStride, int iNumVertexs );

//* Clip Polygon
BspClipVertex* ClipPolygonToFrustum( ViewFrustum* pFrustum, BspClipVertex* pPolygon, int* iNumVertexs );


//*
inline BspClipVertex* ClipPolygonToPlane( Quake3BspPlane* pPlane, BspClipVertex* pPolygon, int* iNumVertexs );
int IsClusterVisibleInFrustum( ViewFrustum* pFrustum, f32 * pMin, f32* pMax );
//*
inline int ClipLineToPlane( Quake3BspPlane* pPlane, BspClipVertex* vStart, BspClipVertex* vEnd );
int IsClusterVisableInFrustumFloat( ViewFrustum* pFrustum, f32* pMin, f32* pMax );

int IsAABInPlane(ViewFrustum* pFrustum, f32 *pMin, f32* pMax, int i) ;

// End of File //
#endif
