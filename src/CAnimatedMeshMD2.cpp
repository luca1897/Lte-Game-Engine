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
 
#include "CAnimatedMeshMD2.h"
#include "os.h"
#include "SColor.h"
#include "IReadFile.h"

#include <stdlib.h>
namespace engine
{
namespace scene
{

// structs needed to load the md2-format

const s32 MD2_MAGIC_NUMBER = 844121161;
const s32 MD2_VERSION           = 8;
const s32 MD2_MAX_VERTS         = 2048;
const s32 MD2_FRAME_SHIFT       = 3;





// Default alignment
const s32 Q2_VERTEX_NORMAL_TABLE_SIZE = 162;

const f32 Q2_VERTEX_NORMAL_TABLE[Q2_VERTEX_NORMAL_TABLE_SIZE][3] = {
	{-0.525731f, 0.000000f, 0.850651f},
	{-0.442863f, 0.238856f, 0.864188f},
	{-0.295242f, 0.000000f, 0.955423f},
	{-0.309017f, 0.500000f, 0.809017f},
	{-0.162460f, 0.262866f, 0.951056f},
	{0.000000f, 0.000000f, 1.000000f},
	{0.000000f, 0.850651f, 0.525731f},
	{-0.147621f, 0.716567f, 0.681718f},
	{0.147621f, 0.716567f, 0.681718f},
	{0.000000f, 0.525731f, 0.850651f},
	{0.309017f, 0.500000f, 0.809017f},
	{0.525731f, 0.000000f, 0.850651f},
	{0.295242f, 0.000000f, 0.955423f},
	{0.442863f, 0.238856f, 0.864188f},
	{0.162460f, 0.262866f, 0.951056f},
	{-0.681718f, 0.147621f, 0.716567f},
	{-0.809017f, 0.309017f, 0.500000f},
	{-0.587785f, 0.425325f, 0.688191f},
	{-0.850651f, 0.525731f, 0.000000f},
	{-0.864188f, 0.442863f, 0.238856f},
	{-0.716567f, 0.681718f, 0.147621f},
	{-0.688191f, 0.587785f, 0.425325f},
	{-0.500000f, 0.809017f, 0.309017f},
	{-0.238856f, 0.864188f, 0.442863f},
	{-0.425325f, 0.688191f, 0.587785f},
	{-0.716567f, 0.681718f, -0.147621f},
	{-0.500000f, 0.809017f, -0.309017f},
	{-0.525731f, 0.850651f, 0.000000f},
	{0.000000f, 0.850651f, -0.525731f},
	{-0.238856f, 0.864188f, -0.442863f},
	{0.000000f, 0.955423f, -0.295242f},
	{-0.262866f, 0.951056f, -0.162460f},
	{0.000000f, 1.000000f, 0.000000f},
	{0.000000f, 0.955423f, 0.295242f},
	{-0.262866f, 0.951056f, 0.162460f},
	{0.238856f, 0.864188f, 0.442863f},
	{0.262866f, 0.951056f, 0.162460f},
	{0.500000f, 0.809017f, 0.309017f},
	{0.238856f, 0.864188f, -0.442863f},
	{0.262866f, 0.951056f, -0.162460f},
	{0.500000f, 0.809017f, -0.309017f},
	{0.850651f, 0.525731f, 0.000000f},
	{0.716567f, 0.681718f, 0.147621f},
	{0.716567f, 0.681718f, -0.147621f},
	{0.525731f, 0.850651f, 0.000000f},
	{0.425325f, 0.688191f, 0.587785f},
	{0.864188f, 0.442863f, 0.238856f},
	{0.688191f, 0.587785f, 0.425325f},
	{0.809017f, 0.309017f, 0.500000f},
	{0.681718f, 0.147621f, 0.716567f},
	{0.587785f, 0.425325f, 0.688191f},
	{0.955423f, 0.295242f, 0.000000f},
	{1.000000f, 0.000000f, 0.000000f},
	{0.951056f, 0.162460f, 0.262866f},
	{0.850651f, -0.525731f, 0.000000f},
	{0.955423f, -0.295242f, 0.000000f},
	{0.864188f, -0.442863f, 0.238856f},
	{0.951056f, -0.162460f, 0.262866f},
	{0.809017f, -0.309017f, 0.500000f},
	{0.681718f, -0.147621f, 0.716567f},
	{0.850651f, 0.000000f, 0.525731f},
	{0.864188f, 0.442863f, -0.238856f},
	{0.809017f, 0.309017f, -0.500000f},
	{0.951056f, 0.162460f, -0.262866f},
	{0.525731f, 0.000000f, -0.850651f},
	{0.681718f, 0.147621f, -0.716567f},
	{0.681718f, -0.147621f, -0.716567f},
	{0.850651f, 0.000000f, -0.525731f},
	{0.809017f, -0.309017f, -0.500000f},
	{0.864188f, -0.442863f, -0.238856f},
	{0.951056f, -0.162460f, -0.262866f},
	{0.147621f, 0.716567f, -0.681718f},
	{0.309017f, 0.500000f, -0.809017f},
	{0.425325f, 0.688191f, -0.587785f},
	{0.442863f, 0.238856f, -0.864188f},
	{0.587785f, 0.425325f, -0.688191f},
	{0.688191f, 0.587785f, -0.425325f},
	{-0.147621f, 0.716567f, -0.681718f},
	{-0.309017f, 0.500000f, -0.809017f},
	{0.000000f, 0.525731f, -0.850651f},
	{-0.525731f, 0.000000f, -0.850651f},
	{-0.442863f, 0.238856f, -0.864188f},
	{-0.295242f, 0.000000f, -0.955423f},
	{-0.162460f, 0.262866f, -0.951056f},
	{0.000000f, 0.000000f, -1.000000f},
	{0.295242f, 0.000000f, -0.955423f},
	{0.162460f, 0.262866f, -0.951056f},
	{-0.442863f, -0.238856f, -0.864188f},
	{-0.309017f, -0.500000f, -0.809017f},
	{-0.162460f, -0.262866f, -0.951056f},
	{0.000000f, -0.850651f, -0.525731f},
	{-0.147621f, -0.716567f, -0.681718f},
	{0.147621f, -0.716567f, -0.681718f},
	{0.000000f, -0.525731f, -0.850651f},
	{0.309017f, -0.500000f, -0.809017f},
	{0.442863f, -0.238856f, -0.864188f},
	{0.162460f, -0.262866f, -0.951056f},
	{0.238856f, -0.864188f, -0.442863f},
	{0.500000f, -0.809017f, -0.309017f},
	{0.425325f, -0.688191f, -0.587785f},
	{0.716567f, -0.681718f, -0.147621f},
	{0.688191f, -0.587785f, -0.425325f},
	{0.587785f, -0.425325f, -0.688191f},
	{0.000000f, -0.955423f, -0.295242f},
	{0.000000f, -1.000000f, 0.000000f},
	{0.262866f, -0.951056f, -0.162460f},
	{0.000000f, -0.850651f, 0.525731f},
	{0.000000f, -0.955423f, 0.295242f},
	{0.238856f, -0.864188f, 0.442863f},
	{0.262866f, -0.951056f, 0.162460f},
	{0.500000f, -0.809017f, 0.309017f},
	{0.716567f, -0.681718f, 0.147621f},
	{0.525731f, -0.850651f, 0.000000f},
	{-0.238856f, -0.864188f, -0.442863f},
	{-0.500000f, -0.809017f, -0.309017f},
	{-0.262866f, -0.951056f, -0.162460f},
	{-0.850651f, -0.525731f, 0.000000f},
	{-0.716567f, -0.681718f, -0.147621f},
	{-0.716567f, -0.681718f, 0.147621f},
	{-0.525731f, -0.850651f, 0.000000f},
	{-0.500000f, -0.809017f, 0.309017f},
	{-0.238856f, -0.864188f, 0.442863f},
	{-0.262866f, -0.951056f, 0.162460f},
	{-0.864188f, -0.442863f, 0.238856f},
	{-0.809017f, -0.309017f, 0.500000f},
	{-0.688191f, -0.587785f, 0.425325f},
	{-0.681718f, -0.147621f, 0.716567f},
	{-0.442863f, -0.238856f, 0.864188f},
	{-0.587785f, -0.425325f, 0.688191f},
	{-0.309017f, -0.500000f, 0.809017f},
	{-0.147621f, -0.716567f, 0.681718f},
	{-0.425325f, -0.688191f, 0.587785f},
	{-0.162460f, -0.262866f, 0.951056f},
	{0.442863f, -0.238856f, 0.864188f},
	{0.162460f, -0.262866f, 0.951056f},
	{0.309017f, -0.500000f, 0.809017f},
	{0.147621f, -0.716567f, 0.681718f},
	{0.000000f, -0.525731f, 0.850651f},
	{0.425325f, -0.688191f, 0.587785f},
	{0.587785f, -0.425325f, 0.688191f},
	{0.688191f, -0.587785f, 0.425325f},
	{-0.955423f, 0.295242f, 0.000000f},
	{-0.951056f, 0.162460f, 0.262866f},
	{-1.000000f, 0.000000f, 0.000000f},
	{-0.850651f, 0.000000f, 0.525731f},
	{-0.955423f, -0.295242f, 0.000000f},
	{-0.951056f, -0.162460f, 0.262866f},
	{-0.864188f, 0.442863f, -0.238856f},
	{-0.951056f, 0.162460f, -0.262866f},
	{-0.809017f, 0.309017f, -0.500000f},
	{-0.864188f, -0.442863f, -0.238856f},
	{-0.951056f, -0.162460f, -0.262866f},
	{-0.809017f, -0.309017f, -0.500000f},
	{-0.681718f, 0.147621f, -0.716567f},
	{-0.681718f, -0.147621f, -0.716567f},
	{-0.850651f, 0.000000f, -0.525731f},
	{-0.688191f, 0.587785f, -0.425325f},
	{-0.587785f, 0.425325f, -0.688191f},
	{-0.425325f, 0.688191f, -0.587785f},
	{-0.425325f, -0.688191f, -0.587785f},
	{-0.587785f, -0.425325f, -0.688191f},
	{-0.688191f, -0.587785f, -0.425325f},
};

struct SMD2AnimationType
{
	s32 begin;
	s32 end;
	s32 fps;
};

SMD2AnimationType MD2AnimationTypeList[21] =
{
	{   0,  39,  9 }, // STAND
	{  40,  45, 10 }, // RUN
	{  46,  53, 10 }, // ATTACK
	{  54,  57,  7 }, // PAIN_A
	{  58,  61,  7 }, // PAIN_B
	{  62,  65,  7 }, // PAIN_C
	{  66,  71,  7 }, // JUMP
	{  72,  83,  7 }, // FLIP
	{  84,  94,  7 }, // SALUTE
	{  95, 111, 10 }, // FALLBACK
	{ 112, 122,  7 }, // WAVE
	{ 123, 134,  6 }, // POINT
	{ 135, 153, 10 }, // CROUCH_STAND
	{ 154, 159,  7 }, // CROUCH_WALK
	{ 160, 168, 10 }, // CROUCH_ATTACK
	{ 196, 172,  7 }, // CROUCH_PAIN
	{ 173, 177,  5 }, // CROUCH_DEATH
	{ 178, 183,  7 }, // DEATH_FALLBACK
	{ 184, 189,  7 }, // DEATH_FALLFORWARD
	{ 190, 197,  7 }, // DEATH_FALLBACKSLOW
	{ 198, 198,  5 }, // BOOM
};


//! constructor
CAnimatedMeshMD2::CAnimatedMeshMD2()
	: FrameCount(0), FrameList(0), grabbed(false)
{
	#ifdef _DEBUG
	IAnimatedMesh::setDebugName("CAnimatedMeshMD2 IAnimatedMesh");
	IMesh::setDebugName("CAnimatedMeshMD2 IMesh");
	IMeshBuffer::setDebugName("CAnimatedMeshMD2 IMeshBuffer");
	#endif
}



//! destructor
CAnimatedMeshMD2::~CAnimatedMeshMD2()
{
	if (FrameList)
		delete [] FrameList;

	delete [] normals;
	delete [] vertices;
	delete [] triangles;
	delete [] textureCoords;

	if (grabbed == true)
		dropAllInterpolationBuffers();
	delete InterpolateBuffer;

}



//! returns the amount of frames in milliseconds. If the amount is 1, it is a static (=non animated) mesh.
s32 CAnimatedMeshMD2::getFrameCount()
{
	return FrameCount<<MD2_FRAME_SHIFT;
}



//! returns the animated mesh based on a detail level. 0 is the lowest, 255 the highest detail. Note, that some Meshes will ignore the detail level.
IMesh* CAnimatedMeshMD2::getMesh(s32 frame, s32 detailLevel, s32 startFrameLoop, s32 endFrameLoop)
{
	if ((u32)frame > (FrameCount<<MD2_FRAME_SHIFT))
		frame = (frame % (FrameCount<<MD2_FRAME_SHIFT));

	if (startFrameLoop == -1 && endFrameLoop == -1)
	{
		startFrameLoop = 0;
		endFrameLoop = FrameCount<<MD2_FRAME_SHIFT;
	}

	updateInterpolationBuffer(frame, startFrameLoop, endFrameLoop);
	return this;
}


//! returns amount of mesh buffers.
s32 getMeshBufferCount()
{
	return 1;
}


//! returns pointer to a mesh buffer
IMeshBuffer* CAnimatedMeshMD2::getMeshBuffer(s32 nr)
{
	return this;
}



//! returns the material of this meshbuffer
const video::SMaterial& CAnimatedMeshMD2::getMaterial() const
{
	return Material;
}


//! returns the material of this meshbuffer
video::SMaterial& CAnimatedMeshMD2::getMaterial()
{
	return Material;
}



//! returns pointer to vertices
const void* CAnimatedMeshMD2::getVertices() const
{
	return InterpolateBuffer->const_pointer();
}


//! returns pointer to vertices
void* CAnimatedMeshMD2::getVertices()
{
	return InterpolateBuffer->pointer();
}



//! returns which type of vertex data is stored.
video::E_VERTEX_TYPE CAnimatedMeshMD2::getVertexType() const
{
	return video::EVT_STANDARD;
}



//! returns amount of vertices
s32 CAnimatedMeshMD2::getVertexCount() const
{
	return header.numTriangles*3;
}



//! returns pointer to Indices
const u16* CAnimatedMeshMD2::getIndices() const
{
	return Indices.const_pointer();
}



//! returns pointer to Indices
u16* CAnimatedMeshMD2::getIndices()
{
	return Indices.pointer();
}



//! returns amount of indices
s32 CAnimatedMeshMD2::getIndexCount() const
{
	return Indices.size();
}


//! returns amount of mesh buffers.
s32 CAnimatedMeshMD2::getMeshBufferCount()
{
	return 1;
}


// updates the interpolation buffer
void CAnimatedMeshMD2::updateInterpolationBuffer(s32 frame, s32 startFrameLoop, s32 endFrameLoop)
{
	u32 firstFrame, secondFrame;
	f32 div;

	// TA: resolve missing ipol in loop between end-start

	if (endFrameLoop - startFrameLoop == 0)
	{
		firstFrame = frame>>MD2_FRAME_SHIFT;
		secondFrame = frame>>MD2_FRAME_SHIFT;
		div = 1.0f;
	}
	else
	{
		// key frames
#if 1
		u32 s = startFrameLoop >> MD2_FRAME_SHIFT;
		u32 e = endFrameLoop >> MD2_FRAME_SHIFT;

		firstFrame = frame >> MD2_FRAME_SHIFT;
		// are there reverse anims ?
		secondFrame = firstFrame + 1;
		if ( secondFrame > e )
			secondFrame = s;

		firstFrame = min ( FrameCount - 1, firstFrame );
		secondFrame = min ( FrameCount - 1, secondFrame );

		div = (frame % (1<<MD2_FRAME_SHIFT)) / (f32)(1<<MD2_FRAME_SHIFT);
#else
		firstFrame = frame>>MD2_FRAME_SHIFT;

		if (!((endFrameLoop>>MD2_FRAME_SHIFT) - (startFrameLoop>>MD2_FRAME_SHIFT)))
			secondFrame = firstFrame;
		else
			secondFrame = (startFrameLoop>>MD2_FRAME_SHIFT) + (((frame>>MD2_FRAME_SHIFT)+1 - (startFrameLoop>>MD2_FRAME_SHIFT)) %
			                                                   ((endFrameLoop>>MD2_FRAME_SHIFT) - (startFrameLoop>>MD2_FRAME_SHIFT)));

		div = (frame % (1<<MD2_FRAME_SHIFT)) / (f32)(1<<MD2_FRAME_SHIFT);
#endif
	}

	video::S3DVertex* target = InterpolateBuffer->pointer();
	core::vector3df *normalSecond, *normalFirst;

	normalSecond = normals[secondFrame].pointer();
	normalFirst = normals[firstFrame].pointer();


#if 1
	// interpolate both frames
	for (s32 i=0; i<header.numTriangles; ++i)
	{

		for (s32 n = 0; n <  3; n++)
		{

			target->Pos = (vertices[secondFrame][triangles[i].vertexIndices[n]] - vertices[firstFrame][triangles[i].vertexIndices[n]]) * div + vertices[firstFrame][triangles[i].vertexIndices[n]];
			target->Normal = (normalSecond[triangles[i].vertexIndices[n]] - normalFirst[triangles[i].vertexIndices[n]]) * div + normalFirst[triangles[i].vertexIndices[n]];

			++target;

		}
	}

	//update bounding box
	BoundingBox = BoxList[secondFrame].getInterpolated(BoxList[firstFrame], div);
#else


	core::array<video::S3DVertex> p = getFrameList(firstFrame);
	core::array<video::S3DVertex> z = getFrameList(secondFrame);

	video::S3DVertex* first = p.pointer();
	video::S3DVertex* second = z.pointer();

	s32 count = p.size();

	// interpolate both frames
	for (s32 i=0; i<count; ++i)
	{
		target->Pos = (second->Pos - first->Pos) * div + first->Pos;
		target->Normal = (second->Normal - first->Normal) * div + first->Normal;

		++target;
		++first;
		++second;
	}

	//update bounding box
	BoundingBox = BoxList[secondFrame].getInterpolated(BoxList[firstFrame], div);
#endif
}


//! returns max element
inline s32 CAnimatedMeshMD2::max(s32 a, s32 b)
{
	return a>b ? a : b;
}

//! returns min element
inline s32 CAnimatedMeshMD2::min(s32 a, s32 b)
{
	return a<b ? a : b;
}


/*
   La PSP non ha abbastanza memoria per memorizzare tutti i frame in memoria,
   a questo proposito creo questa funzione che legge dai buffer il frame richiesto.

 */

core::array<video::S3DVertex> CAnimatedMeshMD2::getFrameList(s32 framenr)
{
	video::S3DVertex vtx;
	vtx.Color = video::SColor(255,255,255,255);
	core::array<video::S3DVertex> p;
	core::array< core::vector3df >& vert = vertices[framenr];

	for (s32 t=0; t<header.numTriangles; ++t)
	{

		for (s32 n=0; n<3; ++n)
		{
			vtx.Pos = vert[triangles[t].vertexIndices[n]];
			vtx.TCoords.X = (textureCoords[triangles[t].textureIndices[n]].s + 0.5f) * dmaxs;
			vtx.TCoords.Y = (textureCoords[triangles[t].textureIndices[n]].t + 0.5f) * dmaxt;
			vtx.Normal = normals[framenr].pointer()[triangles[t].vertexIndices[n]];

			p.push_back(vtx);
		}

	}
	return p;
}

#include <pspdebug.h>
#include <stdio.h>
#define printf pspDebugScreenPrintf

//! loads an md3 file
bool CAnimatedMeshMD2::loadFile(io::IReadFile* file)
{
	if (!file)
		return false;



	s8* frames = 0;


	file->read(&header, sizeof(SMD2Header));

#ifdef PPC
	printf("" "PPC è attivo\n");
	header.magic = OSReadSwapInt32(&header.magic,0);
	header.version = OSReadSwapInt32(&header.version,0);
	header.skinWidth = OSReadSwapInt32(&header.skinWidth,0);
	header.skinHeight = OSReadSwapInt32(&header.skinHeight,0);
	header.frameSize = OSReadSwapInt32(&header.frameSize,0);
	header.numSkins = OSReadSwapInt32(&header.numSkins,0);
	header.numVertices = OSReadSwapInt32(&header.numVertices,0);
	header.numTexcoords = OSReadSwapInt32(&header.numTexcoords,0);
	header.numTriangles = OSReadSwapInt32(&header.numTriangles,0);
	header.numGlCommands = OSReadSwapInt32(&header.numGlCommands,0);
	header.numFrames = OSReadSwapInt32(&header.numFrames,0);
	header.offsetSkins = OSReadSwapInt32(&header.offsetSkins,0);
	header.offsetTexcoords = OSReadSwapInt32(&header.offsetTexcoords,0);
	header.offsetTriangles = OSReadSwapInt32(&header.offsetTriangles,0);
	header.offsetFrames = OSReadSwapInt32(&header.offsetFrames,0);
	header.offsetGlCommands = OSReadSwapInt32(&header.offsetGlCommands,0);
	header.offsetEnd = OSReadSwapInt32(&header.offsetEnd,0);
#endif


	if (header.magic != MD2_MAGIC_NUMBER || header.version != MD2_VERSION)
	{
		os::Printer::log("MD2 Loader: Wrong file header", file->getFileName(), ELL_WARNING);
		return false;
	}

	// create Memory for indices and frames


	Indices.reallocate(header.numTriangles);
	//FrameList = new core::array<video::S3DVertex>[header.numFrames];
	FrameCount = header.numFrames;

	s32 i = 0;

	// read TextureCoords

	file->seek(header.offsetTexcoords, false);
	textureCoords = new SMD2TextureCoordinate[header.numTexcoords];

	if (!file->read(textureCoords, sizeof(SMD2TextureCoordinate)*header.numTexcoords))
	{
		os::Printer::log("MD2 Loader: Error reading TextureCoords.", file->getFileName(), ELL_ERROR);
		return false;
	}

	// read Triangles

	file->seek(header.offsetTriangles, false);

	triangles = new SMD2Triangle[header.numTriangles];
	if (!file->read(triangles, header.numTriangles *sizeof(SMD2Triangle)))
	{
		os::Printer::log("MD2 Loader: Error reading triangles.", file->getFileName(), ELL_ERROR);
		return false;
	}


	// read Vertices

	s8 buffer[MD2_MAX_VERTS*4+128];
	frame = (SMD2Frame*)buffer;

	vertices = new core::array< core::vector3df >[header.numFrames];
	normals = new core::array< core::vector3df >[header.numFrames];

	file->seek(header.offsetFrames, false);

	for (i = 0; i<header.numFrames; ++i)
	{
		// read vertices

		vertices[i].reallocate(header.numVertices);
		file->read(frame, header.frameSize);

		// store frame data

		SFrameData fdata;
		fdata.begin = i;
		fdata.end = i;
		fdata.fps = 7;
		if (frame->name[0])
		{
			for (s32 s = 0; frame->name[s]!=0 && (frame->name[s] < '0' ||
			                                      frame->name[s] > '9'); ++s)
				fdata.name += frame->name[s];

			if (!FrameData.empty() && FrameData[FrameData.size()-1].name == fdata.name)
				++FrameData[FrameData.size()-1].end;
			else
				FrameData.push_back(fdata);
		}

		// add vertices

		for (s32 j=0; j<header.numVertices; ++j)
		{
			core::vector3df v;
			v.X = (f32)frame->vertices[j].vertex[0] * frame->scale[0] + frame->translate[0];
			v.Z = (f32)frame->vertices[j].vertex[1] * frame->scale[1] + frame->translate[1];
			v.Y = (f32)frame->vertices[j].vertex[2] * frame->scale[2] + frame->translate[2];

			vertices[i].push_back(v);

			s32 normalidx = frame->vertices[j].lightNormalIndex;
			if (normalidx > 0 && normalidx < Q2_VERTEX_NORMAL_TABLE_SIZE)
			{
				v.X = Q2_VERTEX_NORMAL_TABLE[normalidx][0];
				v.Y = Q2_VERTEX_NORMAL_TABLE[normalidx][1];
				v.Z = Q2_VERTEX_NORMAL_TABLE[normalidx][2];
			}

			normals[i].push_back(v);
		}

		// calculate bounding boxes
		if (header.numVertices)
		{
			core::aabbox3d<f32> box;
			box.reset(vertices[i][0]);

			for (s32 j=1; j<header.numVertices; ++j)
				box.addInternalPoint(vertices[i][j]);

			BoxList.push_back(box);
		}

	}


	// put triangles into frame list

	dmaxs = 1.0f/(header.skinWidth);
	dmaxt = 1.0f/(header.skinHeight);


	// create indices

	Indices.reallocate(header.numVertices);
	s32 count = header.numTriangles*3;
	for (s32 n=0; n<count; n+=3)
	{
		Indices.push_back(n);
		Indices.push_back(n+1);
		Indices.push_back(n+2);
	}

	//calculateNormals();

	video::S3DVertex vtx;
	vtx.Color = video::SColor(255,255,255,255);
	// reallocate interpolate buffer
	if (header.numFrames)
	{
		core::array<video::S3DVertex> p = getFrameList(0);

		InterpolateBuffer = new core::array<video::S3DVertex>;
		InterpolateBuffer->set_used(p.size());
		s32 count = p.size();
		for (i=0; i<count; ++i)
		{
			InterpolateBuffer->pointer()[i].TCoords = p.pointer()[i].TCoords;
			InterpolateBuffer->pointer()[i].Color = vtx.Color.toOpenGLColor();
		}
	}


	TriangleCount = Indices.size() / 3;

	// return
	calculateBoundingBox();

	return true;
}

//! Is an Interpolation Buffer?
bool CAnimatedMeshMD2::isInterpolationBuffer(MD2Buffer buffer)
{

	if (grabbed == false) return false;

	for (int i = 0; i < interpolationBuffers.size(); i++)
	{

		if (interpolationBuffers[i] == (__interpolation_buffer)buffer)
			return true;

	}
	return false;

}

//! Create a new Interpolation buffer and returns the buffer ID
MD2Buffer CAnimatedMeshMD2::createInterpolationBuffer()
{

	__interpolation_buffer buffer;
	if (grabbed == false)
	{
		buffer = InterpolateBuffer;
		grabbed = true;

	}
	else
	{

		video::S3DVertex vtx;
		vtx.Color = video::SColor(255,255,255,255);
		// reallocate interpolate buffer
		if (header.numFrames)
		{
			core::array<video::S3DVertex> p = getFrameList(0);

			buffer = new core::array<video::S3DVertex>;
			buffer->set_used(p.size());
			s32 count = p.size();
			for (int i=0; i<count; ++i)
			{
				buffer->pointer()[i].TCoords = p.pointer()[i].TCoords;
				buffer->pointer()[i].Color = vtx.Color.toOpenGLColor();
			}
		}
	}
	interpolationBuffers.push_back ( buffer );
	return buffer;
}
//! Sets an Interpolation buffer created with the previous function
void CAnimatedMeshMD2::setInterpolationBuffer(MD2Buffer buffer)
{
	InterpolateBuffer = (__interpolation_buffer)buffer;
}

//! Drop an interpolation buffer
void CAnimatedMeshMD2::dropInterpolationBuffer(MD2Buffer buffer)
{


	if (grabbed == false)
		return;

	__interpolation_buffer p = (__interpolation_buffer)buffer;
	delete p;

	s32 i = interpolationBuffers.linear_search((__interpolation_buffer)buffer);

	interpolationBuffers.erase ( i );

}

//! Drop all interpolation buffers
void CAnimatedMeshMD2::dropAllInterpolationBuffers()
{
	if (grabbed == false)
		return;

	while (interpolationBuffers.size() != 1)
		dropInterpolationBuffer(interpolationBuffers[0]);

	grabbed = false;
	InterpolateBuffer = interpolationBuffers[0];

	interpolationBuffers.clear();
}

bool CAnimatedMeshMD2::areThereInterpolationBuffers()
{
	return grabbed;
}

//! calculates the bounding box
void CAnimatedMeshMD2::calculateBoundingBox()
{
	BoundingBox.reset(0,0,0);

	if (FrameCount)
	{
		u32 defaultFrame = 1;

		if (defaultFrame>=FrameCount)
			defaultFrame = 0;
		core::array<video::S3DVertex> p = getFrameList(defaultFrame);

		for (u32 j=0; j<p.size(); ++j)
			BoundingBox.addInternalPoint(p.pointer()[j].Pos);
	}
}


//! sets a flag of all contained materials to a new value
void CAnimatedMeshMD2::setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
{
	Material.Flags[flag] = newvalue;
}



//! returns an axis aligned bounding box
const core::aabbox3d<f32>& CAnimatedMeshMD2::getBoundingBox() const
{
	return BoundingBox;
}



//! returns an axis aligned bounding box
core::aabbox3d<f32>& CAnimatedMeshMD2::getBoundingBox()
{
	return BoundingBox;
}



//! calculates normals
void CAnimatedMeshMD2::calculateNormals()
{
	return;
}


//! Returns the type of the animated mesh.
E_ANIMATED_MESH_TYPE CAnimatedMeshMD2::getMeshType() const
{
	return EAMT_MD2;
}


//! Returns frame loop data for a special MD2 animation type.
void CAnimatedMeshMD2::getFrameLoop(EMD2_ANIMATION_TYPE l,
                                    s32& outBegin, s32& outEnd, s32& outFPS) const
{
	if (l < 0 || l >= EMAT_COUNT)
		return;

	outBegin = MD2AnimationTypeList[l].begin << MD2_FRAME_SHIFT;
	outEnd = MD2AnimationTypeList[l].end << MD2_FRAME_SHIFT;

	// TA:correct to anim between last->first frame
	outEnd += MD2_FRAME_SHIFT == 0 ? 1 : ( 1 << MD2_FRAME_SHIFT ) - 1;
	//TA: don't know about * 5...
	outFPS = MD2AnimationTypeList[l].fps << MD2_FRAME_SHIFT; // * 5;
}



//! Returns frame loop data for a special MD2 animation type.
bool CAnimatedMeshMD2::getFrameLoop(const c8* name,
                                    s32& outBegin, s32&outEnd, s32& outFPS) const
{
	for (s32 i=0; i<(s32)FrameData.size(); ++i)
		if (FrameData[i].name == name)
		{
			outBegin = FrameData[i].begin << MD2_FRAME_SHIFT;
			outEnd = FrameData[i].end << MD2_FRAME_SHIFT;
			outEnd += MD2_FRAME_SHIFT == 0 ? 1 : ( 1 << MD2_FRAME_SHIFT ) - 1;
			outFPS = FrameData[i].fps << MD2_FRAME_SHIFT;
			return true;
		}

	return false;
}



//! Returns amount of md2 animations in this file.
s32 CAnimatedMeshMD2::getAnimationCount() const
{
	return FrameData.size();
}


//! Returns name of md2 animation.
const c8* CAnimatedMeshMD2::getAnimationName(s32 nr) const
{
	if (nr < 0 || nr >= (s32)FrameData.size())
		return 0;

	return FrameData[nr].name.c_str();
}


} // end namespace scene
} // end namespace engine

