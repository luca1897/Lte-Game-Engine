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
// File Format Quake BSP
//////////////////////////////////////////////////////////////////////////
// All Structures and ETC for the Quake BSP format


// Define Entry
//////////////////////////////////////////////////////////////////////////
#ifndef _FILEFORMATQUAKEBSP_H_
#define _FILEFORMATQUAKEBSP_H_


#include <psptypes.h>
// Quake Magical Defines
//////////////////////////////////////////////////////////////////////////
#define QUAKE3_MAGIC    ( ('I') + ('B' << 8) + ('S' << 16) + ('P' << 24) )
#define QUAKE3_VERSION  0x2E

/*
   struct {
   	f32 x,y,z;
   } typedef ScePspFVector3; */

// Lump Types
//////////////////////////////////////////////////////////////////////////
enum enumQuake3LumpTypes
{
	eQuake3Lump_Entities = 0,               // Game-related object descriptions.
	eQuake3Lump_Textures,                   // Surface descriptions.
	eQuake3Lump_Planes,
	eQuake3Lump_Nodes,
	eQuake3Lump_Leafs,
	eQuake3Lump_LeafFaces,
	eQuake3Lump_LeafBrushes,
	eQuake3Lump_Models,
	eQuake3Lump_Brushes,
	eQuake3Lump_BrushSides,
	eQuake3Lump_Vertexes,
	eQuake3Lump_MeshVerts,
	eQuake3Lump_Effects,
	eQuake3Lump_Faces,
	eQuake3Lump_LightMaps,
	eQuake3Lump_LightVols,
	eQuake3Lump_VisData,

	eQuake3Lump_Max
};

// Surface Flags for Quake 3 (copied from quake 3 src)
//////////////////////////////////////////////////////////////////////////
#define SURF_NODAMAGE                   0x1             // never give falling damage
#define SURF_SLICK                              0x2             // effects game physics
#define SURF_SKY                                0x4             // lighting from environment map
#define SURF_LADDER                             0x8
#define SURF_NOIMPACT                   0x10    // don't make missile explosions
#define SURF_NOMARKS                    0x20    // don't leave missile marks
#define SURF_FLESH                              0x40    // make flesh sounds and effects
#define SURF_NODRAW                             0x80    // don't generate a drawsurface at all
#define SURF_HINT                               0x100   // make a primary bsp splitter
#define SURF_SKIP                               0x200   // completely ignore, allowing non-closed brushes
#define SURF_NOLIGHTMAP                 0x400   // surface doesn't need a lightmap
#define SURF_POINTLIGHT                 0x800   // generate lighting info at vertexes
#define SURF_METALSTEPS                 0x1000  // clanking footsteps
#define SURF_NOSTEPS                    0x2000  // no footstep sounds
#define SURF_NONSOLID                   0x4000  // don't collide against curves with this set
#define SURF_LIGHTFILTER                0x8000  // act as a light filter during q3map -light
#define SURF_ALPHASHADOW                0x10000 // do per-pixel light shadow casting in q3map
#define SURF_NODLIGHT                   0x20000 // don't dlight even if solid (solid lava, skies)
#define SURF_DUST                               0x40000 // leave a dust trail when walking on this surface

// BSP Lump
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	unsigned int Offset;            // offset (in bytes) of the data from the start of the file
	unsigned int Length;            // length (in bytes) of the data
} QuakeBspLump;

// BSP Header
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	unsigned int magic;                     // magic number ("IBSP")
	unsigned int version;           // version of the BSP format (0x26 = Quake 2)

	QuakeBspLump lump[19];          // Directory of Lumps

} Quake2BspHeader;

typedef struct
{
	unsigned int magic;                     // magic number ("IBSP")
	unsigned int version;           // version of the BSP format (0x2e = Quake 3)

	QuakeBspLump lump[17];          // Directory of Lumps

} Quake3BspHeader;

// Textures
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	char szName[64];                        // Texture Name
	int iSurfaceFlags;                      // Surface Flags
	int iContentFlags;                      // Content Flags
} Quake3BspTexture;

// Lightmap
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	unsigned char uLightMapData[128*128*3]; // 128x128xRGB
} Quake3BspLightMap;

// Vertex
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	f32 Position[3];                        // Vertex XYZ
	f32 TextureUV[2];                       // Texture Cordinates
	f32 LightMapUV[2];              // Lightmap Cordinates
	f32 Normal[3];                  // Vertex Normal XYZ
	unsigned char Color[4];         // Vertex Color RGBA
} Quake3BspVertex;

// Mesh Vertex
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	int Offset;                                     // Vertex index offset, relative to first vertex of corresponding face
} Quake3BspMeshVert;

// Face
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	int Texture;
	int Effect;
	int Type;
	int Vertex;
	int NumVerts;
	int MeshVerts;
	int NumMeshVerts;
	int LightMapIndex;
	int LightMapStart[2];
	int LightMapSize[2];
	f32 LightMapOrigin[3];
	f32 LightMapVect[2][3];
	f32 Normal[3];
	int Size[2];
} Quake3BspFace;

// Leaf
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	int cluster;
	int area;
	int mins[3];
	int maxs[3];
	int leafface;
	int n_leaffaces;
	int leafbrush;
	int n_leafbrushes;
} Quake3BspLeaf;

// Leaf Face
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	int face;
} Quake3BspLeafFace;

// Leaf Brush
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	int brush;
} Quake3BspLeafBrush;

// Brush
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	int brushside;
	int n_brushsides;
	int texture;
} Quake3BspBrush;

// Brush Side
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	int plane;
	int texture;
} Quake3BspBrushSide;

// Plane
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	ScePspFVector3 normal;
	f32 dist;
} Quake3BspPlane;

// Node
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	int plane;
	int children[2];
	int mins[3];
	int maxs[3];
} Quake3BspNode;

// Visability Data
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	int n_vecs;
	int sz_vecs;
	unsigned char *Vecs; // n*sz
} Quake3BspVisDataHeader;

// End of File //
#endif
