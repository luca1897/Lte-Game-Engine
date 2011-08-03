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
 
#include "C3DSMeshFileLoader.h"
#include <string.h>
#include "os.h"
#include "SMeshBuffer.h"
#include "SAnimatedMesh.h"


namespace engine
{
namespace scene
{


// Primary chunk
const u16 C3DS_MAIN3DS = 0x4D4D;

// Main Chunks
const u16 C3DS_EDIT3DS = 0x3D3D;
const u16 C3DS_KEYF3DS = 0xB000;
const u16 C3DS_VERSION = 0x0002;
const u16 C3DS_MESHVERSION = 0x3D3E;

// sub chunks of C3DS_EDIT3DS
const u16 C3DS_EDIT_MATERIAL = 0xAFFF;
const u16 C3DS_EDIT_OBJECT      = 0x4000;

// sub chunks of C3DS_EDIT_MATERIAL
const u16 C3DS_MATNAME       = 0xA000;
const u16 C3DS_MATLUMINANCE  = 0xA010;
const u16 C3DS_MATDIFFUSE    = 0xA020;
const u16 C3DS_MATSPECULAR   = 0xA030;
const u16 C3DS_MATSHININESS  = 0xA040;
const u16 C3DS_MATMAP        = 0xA200;
const u16 C3DS_MATREFLMAP    = 0xA220;
const u16 C3DS_MATOPACMAP    = 0xA210;
const u16 C3DS_MATSPECMAP    = 0xA204;
const u16 C3DS_MATBUMPMAP    = 0xA230;
const u16 C3DS_MATMAPFILE    = 0xA300;

// subs of C3DS_EDIT_OBJECT
const u16 C3DS_OBJTRIMESH    = 0x4100;

// subs of C3DS_OBJTRIMESH
const u16 C3DS_TRIVERT       = 0x4110;
const u16 C3DS_TRIFACE       = 0x4120;
const u16 C3DS_TRIFACEMAT    = 0x4130;
const u16 C3DS_TRIUV         = 0x4140;
const u16 C3DS_TRISMOOTH     = 0x4150;
const u16 C3DS_TRILOCAL      = 0x4160;

// different color chunk types
const u16 C3DS_COL_RGB  = 0x0010;
const u16 C3DS_COL_TRU  = 0x0011;
const u16 C3DS_COL_UNK  = 0x0013;


//! Constructor
C3DSMeshFileLoader::C3DSMeshFileLoader(IMeshManipulator* manip,io::IFileSystem* fs, video::IVideoDriver* driver)
	: FileSystem(fs), Driver(driver), Vertices(0), CountVertices(0), CountIndices(0),
	Indices(0), TCoords(0), CountTCoords(0), Mesh(0),Manipulator ( manip )
{
	if (FileSystem)
		FileSystem->grab();

	if (Driver)
		Driver->grab();
}



//! destructor
C3DSMeshFileLoader::~C3DSMeshFileLoader()
{
	if (FileSystem)
		FileSystem->drop();

	if (Driver)
		Driver->drop();

	if (Vertices)
		delete [] Vertices;

	if (Indices)
		delete [] Indices;

	if (TCoords)
		delete [] TCoords;

	if (Mesh)
		Mesh->drop();
}



//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
bool C3DSMeshFileLoader::isALoadableFileExtension(const c8* filename)
{
	return strstr(filename, ".3ds")!=0;
}



//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IUnknown::drop() for more information.
IAnimatedMesh* C3DSMeshFileLoader::createMesh(io::IReadFile* file)
{
	ChunkData data;

	file->seek(0);

	file->read(&data.header, sizeof(ChunkHeader));

#ifdef PPC
	data.header.id = OSReadSwapInt16(&data.header.id,0);
	data.header.length = OSReadSwapInt32(&data.header.length,0);
#endif

	if (data.header.id != 0x4D4D)
		return false;

	CurrentMaterial.Material = video::SMaterial();
	CurrentMaterial.Name = "";
	CurrentMaterial.Filename = "";
	Materials.clear();
	cleanUp();

	if (Mesh)
		Mesh->drop();

	Mesh = new SMesh();

	if (readChunk(file, &data))
	{
		// success
		SAnimatedMesh* am = new SAnimatedMesh();
		am->Type = EAMT_3DS;

		for (s32 i=0; i<Mesh->getMeshBufferCount(); ++i)
			((SMeshBuffer*)Mesh->getMeshBuffer(i))->recalculateBoundingBox();

		Mesh->recalculateBoundingBox();

		am->addMesh(Mesh);
		am->recalculateBoundingBox();
		Mesh->drop();
		Mesh = 0;
		return am;
	}

	Mesh->drop();
	Mesh = 0;

	return 0;
}


bool C3DSMeshFileLoader::readColorChunk(io::IReadFile* file, ChunkData* chunk,
                                        video::SColor& out)
{
	ChunkData data;
	readChunkData(file, data);

	u8 c[3];
	f32 cf[3];

	if (sizeof(c) == data.header.length - data.read)
	{
		// read 8 bit data
		file->read(c, data.header.length - data.read);
		out.set(255, c[0], c[1], c[2]);
		data.read += data.header.length - data.read;
	}
	else
	if (sizeof(cf) == data.header.length - data.read)
	{
		// read f32 data
		file->read(cf, data.header.length - data.read);
		out.set(255, (s32)(cf[0]*255.0f), (s32)(cf[1]*255.0f), (s32)(cf[2]*255.0f));
		data.read += data.header.length - data.read;
	}
	else
	{
		// unknown color chunk size
		os::Printer::log("Unknown size of color chunk in 3Ds file.", ELL_WARNING);
		file->seek(data.header.length - data.read, true);
		data.read += data.header.length - data.read;
	}

	chunk->read += data.read;

	return true;
}


bool C3DSMeshFileLoader::readMaterialChunk(io::IReadFile* file, ChunkData* parent)
{
	bool inSubChunk = false;

	while(parent->read < parent->header.length)
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case C3DS_MATNAME:
		{
			c8* c = new c8[data.header.length - data.read];
			file->read(c, data.header.length - data.read);

			if (strlen(c))
				CurrentMaterial.Name = c;

			data.read += data.header.length - data.read;
			delete [] c;
		}
			break;
		case C3DS_MATLUMINANCE:
			readColorChunk(file, &data, CurrentMaterial.Material.EmissiveColor);
			break;
		case C3DS_MATDIFFUSE:
			readColorChunk(file, &data, CurrentMaterial.Material.DiffuseColor);
			break;
		case C3DS_MATSPECULAR:
			readColorChunk(file, &data, CurrentMaterial.Material.SpecularColor);
			break;
		case C3DS_MATREFLMAP:
		case C3DS_MATMAP:
		case C3DS_MATOPACMAP:
		case C3DS_MATSPECMAP:
		case C3DS_MATBUMPMAP:
			readMaterialChunk(file, &data);
			break;
		case C3DS_MATMAPFILE:
		{
			// read texture file name
			c8* c = new c8[data.header.length - data.read];
			file->read(c, data.header.length - data.read);
			CurrentMaterial.Filename = c;
			CurrentMaterial.Filename.make_lower();
			data.read += data.header.length - data.read;
			delete [] c;
			inSubChunk = true;
		}
			break;
		case C3DS_MATSHININESS:
			// TODO: read shinniess
		default:
			// ignore chunk
			file->seek(data.header.length - data.read, true);
			data.read += data.header.length - data.read;
		}

		parent->read += data.read;
	}

	if (!inSubChunk)
		Materials.push_back(CurrentMaterial);

	return true;
}



bool C3DSMeshFileLoader::readChunk(io::IReadFile* file, ChunkData* parent)
{
	while(parent->read < parent->header.length)
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case C3DS_VERSION:
		{
			u16 version;
			file->read(&version, sizeof(u16));
#ifdef PPC
			version = OSReadSwapInt16(&version,0);
#endif
			file->seek(data.header.length - data.read - 2, true);
			data.read += data.header.length - data.read;
			if (version != 0x03)
			{
				os::Printer::log("Cannot load 3ds files of version other than 3.", ELL_ERROR);
				return false;
			}
		}
			break;
		case C3DS_EDIT_MATERIAL:
			readMaterialChunk(file, &data);
			break;
		case C3DS_EDIT3DS:
		{
			ChunkData ndata;
			readChunkData(file, ndata);
			if (ndata.header.id != C3DS_MESHVERSION)
			{
				os::Printer::log("Mesh version not where expected in 3ds file.", ELL_ERROR);
				return false;
			}

			u16 version;
			file->read(&version, sizeof(u16));
#ifdef PPC
			version = OSReadSwapInt16(&version,0);
#endif
			file->seek(ndata.header.length - ndata.read - 2, true);
			data.read += ndata.read;

			/*if (version != 0x03)
			   {
			   	os::Printer::log("Cannot read 3ds meshes of versions other than 3.", ELL_ERROR);
			   	return false;
			   }*/

			readChunk(file, &data);
		}
			break;

		case C3DS_EDIT_OBJECT:
		{
			readAndIgnoreString(file, data);
			readObjectChunk(file, &data);
		}
			break;

		default:
			// ignore chunk
			file->seek(data.header.length - data.read, true);
			data.read += data.header.length - data.read;
		}

		parent->read += data.read;
	}

	return true;
}



bool C3DSMeshFileLoader::readObjectChunk(io::IReadFile* file, ChunkData* parent)
{
	while(parent->read < parent->header.length)
	{
		ChunkData data;
		readChunkData(file, data);

		switch(data.header.id)
		{
		case C3DS_OBJTRIMESH:
		{
			readObjectChunk(file, &data);
			composeObject(file);
		} break;

		case C3DS_TRIVERT:
			readVertices(file, data);
			break;

		case C3DS_TRIFACE:
			readIndices(file, data);
			readObjectChunk(file, &data); // read smooth and material groups
			break;

		case C3DS_TRIFACEMAT:
			readMaterialGroup(file, data);
			break;

		case C3DS_TRIUV: // geting texture coordiantes
			readTextureCoords(file, data);
			break;

		case C3DS_TRISMOOTH: // TODO
		case C3DS_TRILOCAL: // TODO
		default:
			// ignore chunk
			file->seek(data.header.length - data.read, true);
			data.read += data.header.length - data.read;
		}

		parent->read += data.read;
	}

	return true;
}


void C3DSMeshFileLoader::composeObject(io::IReadFile* file)
{
	if (Mesh->getMeshBufferCount() != (s32)Materials.size())
		loadMaterials(file);

	if (MaterialGroups.empty())
	{
		// no material group, so add all
		SMaterialGroup group;
		group.faceCount = CountIndices / 3;
		group.faces = new s16[group.faceCount];
		for (int i=0; i<group.faceCount; ++i)
			group.faces[i] = i;
		MaterialGroups.push_back(group);

		// if we've got no material, add one without a texture
		if (Materials.empty())
		{
			SCurrentMaterial m;
			Materials.push_back(m);
			SMeshBuffer* mb = new scene::SMeshBuffer();
			Mesh->addMeshBuffer(mb);
			mb->Material = Materials[0].Material;
			mb->drop();
		}
	}

	for (u32 i=0; i<MaterialGroups.size(); ++i)
	{
		SMeshBuffer* mb = 0;

		// find mesh buffer for this group
		for (u32 j=0; j<Materials.size(); ++j)
			if (MaterialGroups[i].MaterialName == Materials[j].Name)
			{
				mb = (SMeshBuffer*)Mesh->getMeshBuffer(j);
				break;
			}


		if (mb != 0)
		{
			// add geometry to the buffer.

			video::S3DVertex vtx;
			vtx.Color.set(255,255,255,255);
			vtx.Normal.set(0,0,0);

			for (s32 f=0; f<MaterialGroups[i].faceCount; ++f)
			{
				s32 vtxCount = mb->Vertices.size();

				for (s32 v=0; v<3; ++v)
				{
					s32 idx = Indices[MaterialGroups[i].faces[f]*4 +v];

					if (CountVertices > idx)
					{
						vtx.Pos.X = Vertices[idx*3 + 0];
						vtx.Pos.Z = Vertices[idx*3 + 1];
						vtx.Pos.Y = Vertices[idx*3 + 2];
					}

					if (CountTCoords > idx)
					{
						vtx.TCoords.X = TCoords[idx*2 + 0];
						vtx.TCoords.Y = 1.0f -TCoords[idx*2 + 1];
					}

					mb->Vertices.push_back(vtx);
				}

				// compute normal
				core::plane3d<f32> pl(mb->Vertices[vtxCount].Pos, mb->Vertices[vtxCount+2].Pos,
				                      mb->Vertices[vtxCount+1].Pos);

				mb->Vertices[vtxCount].Normal = pl.Normal;
				mb->Vertices[vtxCount+1].Normal = pl.Normal;
				mb->Vertices[vtxCount+2].Normal = pl.Normal;

				// add indices

				mb->Indices.push_back(vtxCount);
				mb->Indices.push_back(vtxCount+2);
				mb->Indices.push_back(vtxCount+1);
			}
		}
		else
			os::Printer::log("Found no matching material for Group in 3ds file.", ELL_WARNING);
	}

	cleanUp();
}


core::stringc C3DSMeshFileLoader::getTextureFileName(core::stringc texture,
                                                     core::stringc model)
{
	s32 idx = -1;
	idx = model.findLast('/');

	if (idx == -1)
		idx = model.findLast('\\');

	if (idx == -1)
		return core::stringc();

	core::stringc p = model.subString(0, idx+1);
	p.append(texture);
	return p;
}


void C3DSMeshFileLoader::loadMaterials(io::IReadFile* file)
{
	// create a mesh buffer for every material

	core::stringc modelFilename = file->getFileName();

	if (Materials.empty())
		os::Printer::log("No materials found in 3ds file.", ELL_INFORMATION);

	for (u32 i=0; i<Materials.size(); ++i)
	{
		SMeshBuffer* m = new scene::SMeshBuffer();
		Mesh->addMeshBuffer(m);

		m->Material = Materials[i].Material;
		if (Materials[i].Filename.size())
		{
			core::stringc fname = getTextureFileName(
			        Materials[i].Filename, modelFilename);

			m->Material.Texture1 = Driver->getTexture(Materials[i].Filename.c_str());

			if (!m->Material.Texture1)
				m->Material.Texture1 = Driver->getTexture(fname.c_str());

			if (!m->Material.Texture1)
				os::Printer::log("Could not find a texture for entry in 3ds file",
				                 Materials[i].Filename.c_str(), ELL_WARNING);
		}

		m->drop();
	}
}


void C3DSMeshFileLoader::cleanUp()
{
	delete [] Vertices;
	CountVertices = 0;
	Vertices = 0;
	delete [] Indices;
	Indices = 0;
	CountIndices = 0;
	delete [] TCoords;
	TCoords = 0;
	CountTCoords = 0;

	MaterialGroups.clear();
}




void C3DSMeshFileLoader::readTextureCoords(io::IReadFile* file, ChunkData& data)
{
	file->read(&CountTCoords, sizeof(CountTCoords));
#ifdef PPC
	CountTCoords = OSReadSwapInt16(&CountTCoords,0);
#endif
	data.read += sizeof(CountTCoords);

	s32 tcoordsBufferByteSize = CountTCoords * sizeof(f32) * 2;

	if (data.header.length - data.read != tcoordsBufferByteSize)
	{
		os::Printer::log("Invalid size of tcoords found in 3ds file.", ELL_WARNING);
		return;
	}

	TCoords = new f32[CountTCoords * 3];
	file->read(TCoords, tcoordsBufferByteSize);
#ifdef PPC
	for (int i=0; i<CountTCoords*2; i++) *((long*)&TCoords[i]) = OSReadSwapInt32(&TCoords[i],0);
#endif
	data.read += tcoordsBufferByteSize;
}


void C3DSMeshFileLoader::readMaterialGroup(io::IReadFile* file, ChunkData& data)
{
	SMaterialGroup group;

	readString(file, data, group.MaterialName);

	file->read(&group.faceCount, sizeof(group.faceCount));
#ifdef PPC
	group.faceCount = OSReadSwapInt16(&group.faceCount,0);
#endif
	data.read += sizeof(group.faceCount);

	// read faces
	group.faces = new s16[group.faceCount];
	file->read(group.faces, sizeof(s16) * group.faceCount);
#ifdef PPC
	for (int i=0; i<group.faceCount; i++) group.faces[i] = OSReadSwapInt16(&group.faces[i],0);
#endif
	data.read += sizeof(s16) * group.faceCount;

	MaterialGroups.push_back(group);
}


void C3DSMeshFileLoader::readIndices(io::IReadFile* file, ChunkData& data)
{
	file->read(&CountIndices, sizeof(CountIndices));
#ifdef PPC
	CountIndices = OSReadSwapInt16(&CountIndices,0);
#endif
	data.read += sizeof(CountIndices);

	s32 indexBufferByteSize = CountIndices * sizeof(u16) * 4;

	// Indices are u16s.
	// After every 3 Indices in the array, there follows an edge flag.
	Indices = new u16[CountIndices * 4];
	file->read(Indices, indexBufferByteSize);
#ifdef PPC
	for (int i=0; i<CountIndices*4; i++) Indices[i] = OSReadSwapInt16(&Indices[i],0);
#endif
	data.read += indexBufferByteSize;
}



void C3DSMeshFileLoader::readVertices(io::IReadFile* file, ChunkData& data)
{
	file->read(&CountVertices, sizeof(CountVertices));
#ifdef PPC
	CountVertices = OSReadSwapInt16(&CountVertices,0);
#endif
	data.read += sizeof(CountVertices);

	s32 vertexBufferByteSize = CountVertices * sizeof(f32) * 3;

	if (data.header.length - data.read != vertexBufferByteSize)
	{
		os::Printer::log("Invalid size of vertices found in 3ds file.", ELL_WARNING);
		return;
	}

	Vertices = new f32[CountVertices * 3];
	file->read(Vertices, vertexBufferByteSize);
#ifdef PPC
	for (int i=0; i<CountVertices*3; i++) *((long*)&Vertices[i]) = OSReadSwapInt32(&Vertices[i],0);
#endif
	data.read += vertexBufferByteSize;
}



void C3DSMeshFileLoader::readChunkData(io::IReadFile* file, ChunkData& data)
{
	file->read(&data.header, sizeof(ChunkHeader));
#ifdef PPC
	data.header.id = OSReadSwapInt16(&data.header.id,0);
	data.header.length = OSReadSwapInt32(&data.header.length,0);
#endif
	data.read += sizeof(ChunkHeader);
}


void C3DSMeshFileLoader::readAndIgnoreString(io::IReadFile* file, ChunkData& data)
{
	c8 c = 1;

	while (c)
	{
		if (!file->read(&c, sizeof(c8)))
			return;

		++data.read;
	}
}


void C3DSMeshFileLoader::readString(io::IReadFile* file, ChunkData& data, core::stringc& out)
{
	c8 c = 1;
	out = "";

	while (c)
	{
		file->read(&c, sizeof(c8));
		if (c)
			out.append(c);

		++data.read;
	}
}


} // end namespace scene
} // end namespace engine

