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

#ifndef __C_ANIMATED_MESH_MD2_H_INCLUDED__
#define __C_ANIMATED_MESH_MD2_H_INCLUDED__

#include "IAnimatedMeshMD2.h"
#include "IReadFile.h"
#include "S3DVertex.h"
#include "engineArray.h"
#include "engineString.h"

namespace engine
{
namespace scene
{
#define PACK_STRUCT     __attribute__((packed))
struct SMD2Vertex
{
	u8 vertex[3];
	u8 lightNormalIndex;
} PACK_STRUCT;

struct SMD2Frame
{
	f32 scale[3];
	f32 translate[3];
	c8 name[16];
	SMD2Vertex vertices[1];
} PACK_STRUCT;

struct SMD2Triangle
{
	s16 vertexIndices[3];
	s16 textureIndices[3];
} PACK_STRUCT;

struct SMD2TextureCoordinate
{
	s16 s;
	s16 t;
} PACK_STRUCT;

struct SMD2GLCommand
{
	f32 s, t;
	s32 vertexIndex;
} PACK_STRUCT;
struct SMD2Header
{
	s32 magic;
	s32 version;
	s32 skinWidth;
	s32 skinHeight;
	s32 frameSize;
	s32 numSkins;
	s32 numVertices;
	s32 numTexcoords;
	s32 numTriangles;
	s32 numGlCommands;
	s32 numFrames;
	s32 offsetSkins;
	s32 offsetTexcoords;
	s32 offsetTriangles;
	s32 offsetFrames;
	s32 offsetGlCommands;
	s32 offsetEnd;
} PACK_STRUCT;
#undef PACK_STRUCT

class CAnimatedMeshMD2 : public IAnimatedMeshMD2, IMesh, IMeshBuffer
{
public:

//! constructor
CAnimatedMeshMD2();

//! destructor
virtual ~CAnimatedMeshMD2();

//! loads an md2 file
virtual bool loadFile(io::IReadFile* file);

//! returns the amount of frames in milliseconds. If the amount is 1, it is a static (=non animated) mesh.
virtual s32 getFrameCount();

//! returns the animated mesh based on a detail level. 0 is the lowest, 255 the highest detail. Note, that some Meshes will ignore the detail level.
virtual IMesh* getMesh(s32 frame, s32 detailLevel=255, s32 startFrameLoop=-1, s32 endFrameLoop=-1);

//! returns amount of mesh buffers.
virtual s32 getMeshBufferCount();

//! returns pointer to a mesh buffer
virtual IMeshBuffer* getMeshBuffer(s32 nr);

//! returns the material of this meshbuffer
virtual const video::SMaterial& getMaterial() const;

//! returns the material of this meshbuffer
virtual video::SMaterial& getMaterial();

//! returns pointer to vertices
virtual const void* getVertices() const;

//! returns pointer to vertices
virtual void* getVertices();

//! returns which type of vertex data is stored.
virtual video::E_VERTEX_TYPE getVertexType() const;

//! returns amount of vertices
virtual s32 getVertexCount() const;

//! returns pointer to Indices
virtual const u16* getIndices() const;

//! returns pointer to Indices
virtual u16* getIndices();

//! returns amount of indices
virtual s32 getIndexCount() const;

//! returns an axis aligned bounding box
virtual const core::aabbox3d<f32>& getBoundingBox() const;

//! returns an axis aligned bounding box
virtual core::aabbox3d<f32>& getBoundingBox();

//! sets a flag of all contained materials to a new value
virtual void setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue);

//! Returns the type of the animated mesh.
virtual E_ANIMATED_MESH_TYPE getMeshType() const;

//! Returns frame loop data for a special MD2 animation type.
virtual void getFrameLoop(EMD2_ANIMATION_TYPE,
                          s32& outBegin, s32& outEnd, s32& outFps) const;

//! Returns frame loop data for a special MD2 animation type.
virtual bool getFrameLoop(const c8* name,
                          s32& outBegin, s32& outEnd, s32& outFps) const;

//! Returns amount of md2 animations in this file.
virtual s32 getAnimationCount() const;

//! Returns name of md2 animation.
//! \param nr: Zero based index of animation.
virtual const c8* getAnimationName(s32 nr) const;

//! Create a new Interpolation buffer and returns the buffer ID
virtual MD2Buffer createInterpolationBuffer();

//! Sets an Interpolation buffer created with the previous function
virtual void setInterpolationBuffer(MD2Buffer buffer);

//! Drop an interpolation buffer
virtual void dropInterpolationBuffer(MD2Buffer buffer);

//! Is an Interpolation Buffer?
virtual bool isInterpolationBuffer(MD2Buffer buffer);

//! Drop all interpolation buffers
virtual void dropAllInterpolationBuffers();

//! Check if there are interpolation buffers
virtual bool areThereInterpolationBuffers();

private:
core::array<video::S3DVertex> getFrameList(s32 framenr);

//! returns max element
inline s32 max(s32 a, s32 b);

//! returns max element
inline s32 min(s32 a, s32 b);
//! updates the interpolation buffer
void updateInterpolationBuffer(s32 frame, s32 startFrame, s32 endFrame);

//! calculates the bounding box
virtual void calculateBoundingBox();

//! calculates normals
void calculateNormals();



core::array< core::vector3df >* vertices ;
core::array< core::vector3df >* normals ;
f32 dmaxs;
f32 dmaxt;
SMD2Header header;
core::array<u16> Indices;
core::array<video::S3DVertex> *FrameList;
core::array<core::aabbox3d<f32> > BoxList;
u32 FrameCount;
s32 TriangleCount;
video::SMaterial Material;
SMD2Triangle *triangles;
SMD2TextureCoordinate* textureCoords;
SMD2Frame* frame;
core::array<video::S3DVertex>* InterpolateBuffer;
core::aabbox3d<f32> BoundingBox;
bool grabbed;

struct SFrameData
{
	core::stringc name;
	s32 begin;
	s32 end;
	s32 fps;
};

core::array< SFrameData > FrameData;
typedef core::array<video::S3DVertex>* __interpolation_buffer;
core::array<__interpolation_buffer> interpolationBuffers;
s32 sizeOfInterpolationBuffer;
};

} // end namespace scene
} // end namespace engine

#endif

