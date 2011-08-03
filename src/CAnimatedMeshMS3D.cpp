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
 
#include "CAnimatedMeshMS3D.h"
#include "os.h"
#include <string.h>
#include "IVideoDriver.h"

namespace engine
{
namespace scene
{


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

// File header
struct MS3DHeader
{
	c8 ID[10];
	s32 Version;
} PACK_STRUCT;

// Vertex information
struct MS3DVertex
{
	u8 Flags;
	f32 Vertex[3];
	s8 BoneID;
	u8 RefCount;
} PACK_STRUCT;

// Triangle information
struct MS3DTriangle
{
	u16 Flags;
	u16 VertexIndices[3];
	f32 VertexNormals[3][3];
	f32 S[3], T[3];
	u8 SmoothingGroup;
	u8 GroupIndex;
} PACK_STRUCT;

// Material information
struct MS3DMaterial
{
	s8 Name[32];
	f32 Ambient[4];
	f32 Diffuse[4];
	f32 Specular[4];
	f32 Emissive[4];
	f32 Shininess;  // 0.0f - 128.0f
	f32 Transparency; // 0.0f - 1.0f
	u8 Mode; // 0, 1, 2 is unused now
	s8 Texture[128];
	s8 Alphamap[128];
} PACK_STRUCT;

//	Joint information
struct MS3DJoint
{
	u8 Flags;
	s8 Name[32];
	s8 ParentName[32];
	f32 Rotation[3];
	f32 Translation[3];
	u16 NumRotationKeyframes;
	u16 NumTranslationKeyframes;
} PACK_STRUCT;

// Keyframe data
struct MS3DKeyframe
{
	f32 Time;
	f32 Parameter[3];
} PACK_STRUCT;

// Default alignment
#ifdef _MSC_VER
#       pragma pack( pop, packing )
#endif

#undef PACK_STRUCT


//! constructor
CAnimatedMeshMS3D::CAnimatedMeshMS3D(video::IVideoDriver* driver)
	: Driver(driver)
{
	if (Driver)
		Driver->grab();

	lastCalculatedFrame = -1;
}



//! destructor
CAnimatedMeshMS3D::~CAnimatedMeshMS3D()
{
	if (Driver)
		Driver->drop();
}



//! loads an md2 file
bool CAnimatedMeshMS3D::loadFile(io::IReadFile* file)
{
	s32 i=0;

	if (!file)
		return false;

	HasAnimation = false;

	// find file size

	size_t fileSize = file->getSize();

	// read whole file

	u8* buffer = new u8[fileSize];
	size_t read = (s32)file->read(buffer, fileSize);
	if (read != fileSize)
	{
		delete [] buffer;
		os::Printer::log("Could not read full file. Loading failed", file->getFileName(), ELL_ERROR);
		return false;
	}

	// read header

	const u8 *pPtr = (u8*)((void*)buffer);
	MS3DHeader *pHeader = (MS3DHeader*)pPtr;
	pPtr += sizeof(MS3DHeader);

	if ( strncmp( pHeader->ID, "MS3D000000", 10 ) != 0 )
	{
		delete [] buffer;
		os::Printer::log("Not a valid Milkshape3D Model File. Loading failed", file->getFileName(), ELL_ERROR);
		return false;
	}

	if ( pHeader->Version < 3 || pHeader->Version > 4 )
	{
		delete [] buffer;
		os::Printer::log("Only Milkshape3D version 1.3 and 1.4 is supported. Loading failed", file->getFileName(), ELL_ERROR);
		return false;
	}

	// get pointers to data

	// vertices
	u16 numVertices = *(u16*)pPtr;
	pPtr += sizeof(u16);
	MS3DVertex *vertices = (MS3DVertex*)pPtr;
	pPtr += sizeof(MS3DVertex) * numVertices;

	// triangles
	u16 numTriangles = *(u16*)pPtr;
	pPtr += sizeof(u16);
	MS3DTriangle *triangles = (MS3DTriangle*)pPtr;
	pPtr += sizeof(MS3DTriangle) * numTriangles;

	// groups
	u16 numGroups = *(u16*)pPtr;
	pPtr += sizeof(u16);

	//skip groups
	for (i=0; i<numGroups; ++i)
	{
		Groups.push_back(SGroup());
		SGroup& grp = Groups.getLast();

		grp.Name = (const c8*) pPtr;

		pPtr += 33; // name and 1 byte flags
		u16 triangleCount = *(u16*)pPtr;
		pPtr += sizeof(u16);

		//pPtr += sizeof(u16) * triangleCount; // triangle indices
		s32 j;
		for (j=0; j<triangleCount; ++j)
		{
			grp.VertexIds.push_back (*(u16*)pPtr);
			pPtr += sizeof (u16);
		}

		grp.MaterialIdx = *(u8*)pPtr;

		pPtr += sizeof(c8); // material index
	}

	// skip materials
	u16 numMaterials = *(u16*)pPtr;
	pPtr += sizeof(u16);

	// MS3DMaterial *materials = (MS3DMaterial*)pPtr;
	// pPtr += sizeof(MS3DMaterial) * numMaterials;

	if(numMaterials <= 0)
	{
		// if there are no materials, add at least one buffer

		Buffers.push_back (SMS3DMeshBuffer ());
		SMS3DMeshBuffer& buffer = Buffers.getLast();
		buffer.BoundingBox = &BoundingBox;
		buffer.Vertices = &AnimatedVertices;
	}

	for (i=0; i<numMaterials; ++i)
	{
		MS3DMaterial *material = (MS3DMaterial*)pPtr;
		pPtr += sizeof(MS3DMaterial);

		Buffers.push_back (SMS3DMeshBuffer ());
		SMS3DMeshBuffer& buffer = Buffers.getLast();

		buffer.Material.MaterialType = video::EMT_SOLID;

		buffer.Material.AmbientColor = video::SColorf(material->Ambient[0], material->Ambient[1], material->Ambient[2], material->Ambient[3]).toSColor ();
		buffer.Material.DiffuseColor = video::SColorf(material->Diffuse[0], material->Diffuse[1], material->Diffuse[2], material->Diffuse[3]).toSColor ();
		buffer.Material.EmissiveColor = video::SColorf(material->Emissive[0], material->Emissive[1], material->Emissive[2], material->Emissive[3]).toSColor ();
		buffer.Material.SpecularColor = video::SColorf(material->Specular[0], material->Specular[1], material->Specular[2], material->Specular[3]).toSColor ();
		buffer.Material.Shininess = material->Shininess;
		buffer.Material.Texture1 = Driver->getTexture((const c8*)material->Texture);
		buffer.BoundingBox = &BoundingBox;
		buffer.Vertices = &AnimatedVertices;
	}

	// animation time
	f32 framesPerSecond = *(f32*)pPtr;
	pPtr += sizeof(f32) * 2; // fps and current time

	s32 frameCount = *(s32*)pPtr;
	pPtr += sizeof(s32);

	totalTime = (frameCount / framesPerSecond) * 1000.0f;

	s32 jointCount = *(u16*)pPtr;
	pPtr += sizeof(u16);

	// load joints
	SJoint t;

	for (i=0; i<jointCount; ++i)
	{
		MS3DJoint *pJoint = (MS3DJoint*)pPtr;
		pPtr += sizeof(MS3DJoint);
		Joints.push_back(t);
		SJoint& jnt = Joints[Joints.size()-1];

		jnt.Name = pJoint->Name;
		jnt.Index = i;
		jnt.Rotation.X = pJoint->Rotation[0];
		jnt.Rotation.Y = pJoint->Rotation[1];
		jnt.Rotation.Z = pJoint->Rotation[2];
		jnt.Translation.X = pJoint->Translation[0];
		jnt.Translation.Y = pJoint->Translation[1];
		jnt.Translation.Z = pJoint->Translation[2];
		jnt.ParentName = pJoint->ParentName;
		jnt.Parent = -1;

		if (pJoint->NumRotationKeyframes ||
		    pJoint->NumTranslationKeyframes)
			HasAnimation = true;

		// get rotation keyframes
		s32 j;
		for     (j=0; j<pJoint->NumRotationKeyframes; ++j)
		{
			MS3DKeyframe* kf = (MS3DKeyframe*)pPtr;
			pPtr += sizeof(MS3DKeyframe);

			SKeyframe k;
			k.timeindex = kf->Time * 1000.0f;
			k.data.X = kf->Parameter[0];
			k.data.Y = kf->Parameter[1];
			k.data.Z = kf->Parameter[2];
			jnt.RotationKeys.push_back(k);
		}

		// get translation keyframes
		for     (j=0; j<pJoint->NumTranslationKeyframes; ++j)
		{
			MS3DKeyframe* kf = (MS3DKeyframe*)pPtr;
			pPtr += sizeof(MS3DKeyframe);

			SKeyframe k;
			k.timeindex = kf->Time * 1000.0f;
			k.data.X = kf->Parameter[0];
			k.data.Y = kf->Parameter[1];
			k.data.Z = kf->Parameter[2];
			jnt.TranslationKeys.push_back(k);
		}
	}

	//find parent of every joint
	s32 j;
	for (i=0; i<(s32)Joints.size(); ++i)
		if (Joints[i].ParentName.size() != 0)
		{
			for (j=0; j<(s32)Joints.size(); ++j)
				if (i != j && Joints[i].ParentName == Joints[j].Name)
				{
					Joints[i].Parent = j;
					break;
				}

			if (Joints[i].Parent == -1)
				os::Printer::log("Found joint in model without parent.", ELL_WARNING);
		}

	// sets up all joints with initial rotation and translation
	for (i=0; i<(s32)Joints.size(); ++i)
	{
		SJoint& jnt = Joints[i];

		jnt.RelativeTransformation.setRotationRadians(jnt.Rotation);
		jnt.RelativeTransformation.setTranslation(jnt.Translation);

		if (jnt.Parent == -1)
			jnt.AbsoluteTransformation = jnt.RelativeTransformation;
		else
		{
			jnt.AbsoluteTransformation = Joints[jnt.Parent].AbsoluteTransformation;
			jnt.AbsoluteTransformation *= jnt.RelativeTransformation;
		}
	}

	// create vertices and indices, attach them to the joints.
	video::S3DVertex v;

	for (i=0; i<numTriangles; ++i)
	{
		s32 j;
		for (j = 0; j<3; ++j)
		{
			v.TCoords.X = triangles[i].S[j];
			v.TCoords.Y = triangles[i].T[j];
			v.Normal.X = triangles[i].VertexNormals[j][0];
			v.Normal.Y = triangles[i].VertexNormals[j][1];
			v.Normal.Z = triangles[i].VertexNormals[j][2];
			v.Color.set(255,255,255,255);
			v.Pos.X = vertices[triangles[i].VertexIndices[j]].Vertex[0];
			v.Pos.Y = vertices[triangles[i].VertexIndices[j]].Vertex[1];
			v.Pos.Z = vertices[triangles[i].VertexIndices[j]].Vertex[2];

			s32 boneid = vertices[triangles[i].VertexIndices[j]].BoneID;
			if (boneid>=0 && boneid<(s32)Joints.size())
				Joints[boneid].VertexIds.push_back(Vertices.size());
			//else
			//{
			//	os::Debuginfo::print("no bone for vertex, bone", boneid);
			//}

			Vertices.push_back(v);
		}

		Indices.push_back(Vertices.size()-3);
		Indices.push_back(Vertices.size()-2);
		Indices.push_back(Vertices.size()-1);
	}

	for (i=0; i<(int)Groups.size(); ++i)
	{
		SGroup& grp = Groups[i];
		core::array<u16>& vertexIds = grp.VertexIds;

		if (grp.MaterialIdx >= Buffers.size())
			grp.MaterialIdx = 0;

		SMS3DMeshBuffer &buffer = Buffers[grp.MaterialIdx];
		core::array<u16>& indices = buffer.Indices;

		for (s32 j=0; j<(int)grp.VertexIds.size(); ++j)
		{
			u16 vertexId = vertexIds[j];
			indices.push_back (vertexId * 3);
			indices.push_back (vertexId * 3 + 1);
			indices.push_back (vertexId * 3 + 2);
		}
	}

	// calculate bounding box
	if (!Vertices.empty())
		BoundingBox.reset(Vertices[0].Pos);

	for (i=0; i<(s32)Vertices.size(); ++i)
		BoundingBox.addInternalPoint(Vertices[i].Pos);

	// inverse translate and rotate all vertices for making animation easier
	if (HasAnimation)
		for (i=0; i<(s32)Joints.size(); ++i)
			for (s32 j=0; j<(s32)Joints[i].VertexIds.size(); ++j)
			{
				Joints[i].AbsoluteTransformation.inverseTranslateVect(
				        Vertices[Joints[i].VertexIds[j]].Pos);

				Joints[i].AbsoluteTransformation.inverseRotateVect(
				        Vertices[Joints[i].VertexIds[j]].Pos);

				Joints[i].AbsoluteTransformation.inverseRotateVect(
				        Vertices[Joints[i].VertexIds[j]].Normal);
			}

	AnimatedVertices = Vertices;

	delete [] buffer;
	return true;
}



//! returns the amount of frames in milliseconds. If the amount is 1, it is a static (=non animated) mesh.
s32 CAnimatedMeshMS3D::getFrameCount()
{
	return (s32)totalTime;
}



//! returns the animated mesh based on a detail level. 0 is the lowest, 255 the highest detail. Note, that some Meshes will ignore the detail level.
IMesh* CAnimatedMeshMS3D::getMesh(s32 frame, s32 detailLevel, s32 startFrameLoop, s32 endFrameLoop)
{
	animate(frame);
	return this;
}



void CAnimatedMeshMS3D::getKeyframeData(core::array<SKeyframe>& keys, f32 time, core::vector3df& outdata)
{
	for (s32 i=0; i<(s32)keys.size()-1; ++i)
	{
		if (keys[i].timeindex <= time && keys[i+1].timeindex >= time)
		{
			f32 interpolate = (time - keys[i].timeindex)/(keys[i+1].timeindex - keys[i].timeindex);
			outdata = keys[i].data + ((keys[i+1].data - keys[i].data) * interpolate);
			return;
		}
	}
}


//! Returns a pointer to a transformation matrix of a part of the
//! mesh based on a frame time.
core::matrix4* CAnimatedMeshMS3D::getMatrixOfJoint(s32 jointNumber, s32 frame)
{
	if (!HasAnimation || jointNumber < 0 || jointNumber >= (s32)Joints.size())
		return 0;

	animate(frame);

	return &Joints[jointNumber].AbsoluteTransformationAnimated;
}


//! Gets joint count.
s32 CAnimatedMeshMS3D::getJointCount() const
{
	return Joints.size();
}


//! Gets the name of a joint.
const c8* CAnimatedMeshMS3D::getJointName(s32 number) const
{
	if (number < 0 || number >= (s32)Joints.size())
		return 0;
	return Joints[number].Name.c_str();
}


//! Gets a joint number from its name
s32 CAnimatedMeshMS3D::getJointNumber(const c8* name) const
{
	for (s32 i=0; i<(s32)Joints.size(); ++i)
		if (Joints[i].Name == name)
			return i;

	return -1;
}


void CAnimatedMeshMS3D::animate(s32 frame)
{
	if (!HasAnimation || lastCalculatedFrame == frame)
		return;

	f32 time = (f32)frame;
	core::matrix4 transform;
	lastCalculatedFrame = frame;

	for (s32 i=0; i<(s32)Joints.size(); ++i)
	{
		core::vector3df translation = Joints[i].Translation;
		core::vector3df rotation = Joints[i].Rotation;

		// find keyframe translation and roation
		getKeyframeData(Joints[i].TranslationKeys, time, translation);
		getKeyframeData(Joints[i].RotationKeys, time, rotation);

		transform.makeIdentity();
		transform.setRotationRadians(rotation);
		transform.setTranslation(translation);

		Joints[i].AbsoluteTransformationAnimated = Joints[i].RelativeTransformation * transform;

		if (Joints[i].Parent != -1)
			Joints[i].AbsoluteTransformationAnimated = Joints[Joints[i].Parent].AbsoluteTransformationAnimated * Joints[i].AbsoluteTransformationAnimated;

		if (i==0)
			BoundingBox.reset(Joints[0].AbsoluteTransformationAnimated.getTranslation());
		else
			BoundingBox.addInternalPoint(Joints[i].AbsoluteTransformationAnimated.getTranslation());


		// transform all vertices
		for (s32 j=0; j<(s32)Joints[i].VertexIds.size(); ++j)
		{
			Joints[i].AbsoluteTransformationAnimated.transformVect(
			        Vertices[Joints[i].VertexIds[j]].Pos,
			        AnimatedVertices[Joints[i].VertexIds[j]].Pos);

			// TODO: this could be done much more faster by
			// first getting 8 extreme points and putting them in.
			BoundingBox.addInternalPoint(AnimatedVertices[Joints[i].VertexIds[j]].Pos);
		}
	}

}



//! returns amount of mesh buffers.
s32 CAnimatedMeshMS3D::getMeshBufferCount()
{
	return Buffers.size();
}



//! returns pointer to a mesh buffer
IMeshBuffer* CAnimatedMeshMS3D::getMeshBuffer(s32 nr)
{
	return &Buffers[nr];
}


//! returns an axis aligned bounding box
const core::aabbox3d<f32>& CAnimatedMeshMS3D::getBoundingBox() const
{
	return BoundingBox;
}


//! returns an axis aligned bounding box
core::aabbox3d<f32>& CAnimatedMeshMS3D::getBoundingBox()
{
	return BoundingBox;
}


//! sets a flag of all contained materials to a new value
void CAnimatedMeshMS3D::setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
{
	for (s32 i=0; i<(int)Buffers.size(); ++i)
		Buffers[i].Material.Flags[flag] = newvalue;
}


//! Returns the type of the animated mesh.
E_ANIMATED_MESH_TYPE CAnimatedMeshMS3D::getMeshType() const
{
	return EAMT_MS3D;
}

//! constructor
CAnimatedMeshMS3D::SMS3DMeshBuffer::SMS3DMeshBuffer()
{
	#ifdef _DEBUG
	setDebugName("SMS3DMeshBuffer");
	#endif
}

//! destructor
CAnimatedMeshMS3D::SMS3DMeshBuffer::~SMS3DMeshBuffer()
{
}

//! returns the material of this meshbuffer
const video::SMaterial& CAnimatedMeshMS3D::SMS3DMeshBuffer::getMaterial() const
{
	return Material;
}

//! returns the material of this meshbuffer
video::SMaterial& CAnimatedMeshMS3D::SMS3DMeshBuffer::getMaterial()
{
	return Material;
}

//! returns pointer to vertices
const void* CAnimatedMeshMS3D::SMS3DMeshBuffer::getVertices() const
{
	return Vertices->const_pointer();
}

//! returns pointer to vertices
void* CAnimatedMeshMS3D::SMS3DMeshBuffer::getVertices()
{
	return Vertices->pointer();
}

//! returns amount of vertices
s32 CAnimatedMeshMS3D::SMS3DMeshBuffer::getVertexCount() const
{
	return Vertices->size();
}

//! returns pointer to Indices
const u16* CAnimatedMeshMS3D::SMS3DMeshBuffer::getIndices() const
{
	return Indices.const_pointer();
}

//! returns pointer to Indices
u16* CAnimatedMeshMS3D::SMS3DMeshBuffer::getIndices()
{
	return Indices.pointer();
}

//! returns amount of indices
s32 CAnimatedMeshMS3D::SMS3DMeshBuffer::getIndexCount() const
{
	return Indices.size();
}

//! returns an axis aligned bounding box
const core::aabbox3d<f32>& CAnimatedMeshMS3D::SMS3DMeshBuffer::getBoundingBox() const
{
	return *BoundingBox;
}

//! returns an axis aligned bounding box
core::aabbox3d<f32>& CAnimatedMeshMS3D::SMS3DMeshBuffer::getBoundingBox()
{
	return *BoundingBox;
}

//! returns which type of vertex data is stored.
video::E_VERTEX_TYPE CAnimatedMeshMS3D::SMS3DMeshBuffer::getVertexType() const
{
	return video::EVT_STANDARD;
}






} // end namespace scene
} // end namespace engine

