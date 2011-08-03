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
// Because I (Nikolaus Gebhardt) did some changes to Murphy McCauley's loader,
// I'm writing this down here:
// - Replaced all dependencies to STL and stdio with engine:: methods/constructs
// - Disabled logging define
// - Changed some minor things (Don't remember what exactly.)
// Thanks a lot to Murphy McCauley for writing this loader.

//
//  COCTLoader by Murphy McCauley (February 2005)
//  An engine loader for OCT files
//
//  OCT file format information comes from the sourcecode of the Fluid Studios
//  Radiosity Processor by Paul Nettle.  You can get that sourcecode from
//  http://www.fluidstudios.com .
//
//  Parts of this code are from engine's CQ3LevelMesh and C3DSMeshFileLoader,
//  and are Copyright (C) 2002-2004 Nikolaus Gebhardt.
//
//  Use of this code is subject to the following:
//
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any damages
//  arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any purpose,
//  including commercial applications, and to alter it and redistribute it
//  freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software. If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//  2. Altered source versions must be plainly marked as such, and must not be
//     misrepresented as being the original software.
//  3. This notice may not be removed or altered from any source distribution.
//  4. You may not use this software to directly or indirectly cause harm to others.


#ifndef __C_OCT_LOADER_H_INCLUDED__
#define __C_OCT_LOADER_H_INCLUDED__

#include "IMeshLoader.h"
#include "IReadFile.h"
#include "SMesh.h"
#include "IVideoDriver.h"
#include "engineString.h"

#include "ISceneManager.h"

namespace engine
{
namespace scene
{

class COCTLoader : public IMeshLoader
{
public:
void OCTLoadLights(engine::io::IReadFile* file, engine::scene::ISceneManager * scene, engine::scene::ISceneNode * parent = 0, f32 radius = 500.0f, f32 intensityScale = 0.0000001f*2.5, bool rewind = true);

//! constructor
COCTLoader(video::IVideoDriver* driver);

//! destructor
virtual ~COCTLoader();

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".cob")
virtual bool isALoadableFileExtension(const c8* fileName);

//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IUnknown::drop() for more information.
virtual IAnimatedMesh* createMesh(engine::io::IReadFile* file);

private:
void GetFaceNormal(f32 a[3], f32 b[3], f32 c[3], f32 out[3]);

struct octHeader {
	u32 numVerts;
	u32 numFaces;
	u32 numTextures;
	u32 numLightmaps;
	u32 numLights;
};

struct octHeaderEx {
	u32 magic;                 // 'OCTX' - 0x4F435458L
	u32 numLightmaps;
	u32 lightmapWidth;
	u32 lightmapHeight;
	u32 containsVertexNormals;
};

struct octFace {
	u32 firstVert;
	u32 numVerts;
	u32 textureID;
	u32 lightmapID;
	f32 plane[4];
};

struct octVert {
	f32 tc[2];
	f32 lc[2];
	f32 pos[3];
};

struct octTexture {
	u32 id;
	char fileName[64];
};

struct octLightmap {
	u32 id;
	u8 data[128][128][3];
};

struct octLight {
	f32 pos[3];
	f32 color[3];
	u32 intensity;
};

video::IVideoDriver* Driver;
};

} // end namespace scene
} // end namespace engine

#endif

