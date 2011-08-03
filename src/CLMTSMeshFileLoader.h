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
 
//
// I (Nikolaus Gebhardt) did some few changes to Jonas Petersen's original loader:
// - removed setTexturePath() and replaced with the ISceneManager::getStringParameter()-stuff.
// - added EAMT_LMTS enumeration value
// Thanks a lot to Jonas Petersen for his work
// on this and that he gave me his permission to add it into engine.
/*

   CLMTSMeshFileLoader.h

   LMTSMeshFileLoader
   Written by Jonas Petersen (a.k.a. jox)

   Version 1.5 - 15 March 2005

 */

// set this to 1 if you want to integrate the loader directly into the engine
#define LMTS_INTEGRATED_IN_engine 1



#if !defined (__C_LMTS_MESH_FILE_LOADER_H_INCLUDED__)
#define __C_LMTS_MESH_FILE_LOADER_H_INCLUDED__

#if LMTS_INTEGRATED_IN_engine
#define LMTS_LOG os::Printer::log
#else
#define LMTS_LOG Logger->log
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <IMeshLoader.h>
#include <SMesh.h>
#include <IFileSystem.h>
#include <IVideoDriver.h>

#if !LMTS_INTEGRATED_IN_engine
#include <ILogger.h>
#endif

namespace engine
{
namespace scene
{

class CLMTSMeshFileLoader : public IMeshLoader
{
public:

#if LMTS_INTEGRATED_IN_engine
CLMTSMeshFileLoader(io::IFileSystem* fs,
                    video::IVideoDriver* driver, IStringParameters* parameters);
#else
CLMTSMeshFileLoader(engineDevice* device);
#endif

virtual ~CLMTSMeshFileLoader();

void cleanup();

virtual bool isALoadableFileExtension(const c8* fileName);

virtual IAnimatedMesh* createMesh(engine::io::IReadFile* file);

private:
void constructMesh();
void loadTextures();


// byte-align structures
#ifdef _MSC_VER
#       pragma pack( push, packing )
#       pragma pack( 1 )
#       define PACK_STRUCT
#elif defined ( __GNUC__ )
#       define PACK_STRUCT      __attribute__((packed))
#else
#       error compiler not supported
#endif


struct SLMTSMagigID {
	u32 ID;
} PACK_STRUCT;

struct SLMTSHeader
{
	u32 MagicID;
	u32 Version;
	u32 HeaderSize;
	u16 TextureCount;
	u16 SubsetCount;
	u32 TriangleCount;
	u16 SubsetSize;
	u16 VertexSize;
} PACK_STRUCT;

struct SLMTSTextureInfoEntry
{
	c8 Filename[256];
	u16 Flags;
} PACK_STRUCT;

struct SLMTSSubsetInfoEntry
{
	u32 Offset;
	u32 Count;
	u16 TextID1;
	u16 TextID2;
} PACK_STRUCT;

struct SLMTSTriangleDataEntry
{
	f32 X;
	f32 Y;
	f32 Z;
	f32 U1;
	f32 V1;
	f32 U2;
	f32 V2;
} PACK_STRUCT;

// Default alignment
#ifdef _MSC_VER
#       pragma pack( pop, packing )
#endif

#undef PACK_STRUCT

SLMTSHeader Header;
SLMTSTextureInfoEntry* Textures;
u16* TextureIDs;
SLMTSSubsetInfoEntry* Subsets;
SLMTSTriangleDataEntry* Triangles;

scene::SMesh* Mesh;
s32 NumTextures;
s32 NumLightMaps;

IStringParameters* Parameters;
video::IVideoDriver* Driver;
io::IFileSystem* FileSystem;

#if !LMTS_INTEGRATED_IN_engine
ILogger* Logger;
#endif
};

} // end namespace scene
} // end namespace engine

#endif // !defined(__C_LMTS_MESH_FILE_LOADER_H_INCLUDED__)
