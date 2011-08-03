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
 
#include "CStaticMeshOBJ.h"
#include <stdlib.h>
#include <string.h>
#include "engineString.h"
#include "SMeshBuffer.h"
#include "os.h"
#include "fast_atof.h"


namespace engine
{
namespace scene
{


//! constructor
CStaticMeshOBJ::CStaticMeshOBJ()
{
}



//! destructor
CStaticMeshOBJ::~CStaticMeshOBJ()
{
}



//! returns the amount of frames in milliseconds. If the amount is 1, it is a static (=non animated) mesh.
s32 CStaticMeshOBJ::getFrameCount()
{
	return 1;
}



//! returns the animated mesh based on a detail level. 0 is the lowest, 255 the highest detail. Note, that some Meshes will ignore the detail level.
IMesh* CStaticMeshOBJ::getMesh(s32 frame, s32 detailLevel, s32 startFrameLoop, s32 endFrameLoop)
{
	return &Mesh;
}



//! loads an obj file
bool CStaticMeshOBJ::loadFile(io::IReadFile* file)
{
	const s32 WORD_BUFFER_LENGHT = 255;
	c8 wordBuffer[WORD_BUFFER_LENGHT];

	SMeshBuffer *meshbuffer = new SMeshBuffer;

	core::array< core::vector3df > vertexBuffer;
	core::array< core::vector2d<f32> > textureCoordBuffer;
	core::array< core::vector3df > normalsBuffer;

	s32 filesize = (s32)file->getSize();

	if (!filesize)
		return false;

	c8* buf = new c8[filesize];
	file->read((void*)buf, filesize);

	c8* line = buf;

	while(line)
	{
		c8* word = line;
		copyWord(wordBuffer, WORD_BUFFER_LENGHT, word);

		if (wordBuffer[0] == 'v' && wordBuffer[1] == 0x0)
		{
			// vector
			c8* p1 = getNextWord(word);
			c8* p2 = getNextWord(p1);
			c8* p3 = getNextWord(p2);

			vertexBuffer.push_back(core::vector3df( core::fast_atof(p1),
			                                        core::fast_atof(p2),
			                                        core::fast_atof(p3)));
		}
		else
		if (wordBuffer[0] == 'v' && wordBuffer[1] == 't')
		{
			// texture coordinate
			c8* p1 = getNextWord(word);
			c8* p2 = getNextWord(p1);

			textureCoordBuffer.push_back(core::vector2d<f32>(       core::fast_atof(p1),
			                                                        core::fast_atof(p2)));
		}
		else
		if (wordBuffer[0] == 'v' && wordBuffer[1] == 'n')
		{
			// vertex normal
			c8* p1 = getNextWord(word);
			c8* p2 = getNextWord(p1);
			c8* p3 = getNextWord(p2);

			normalsBuffer.push_back(core::vector3df(        core::fast_atof(p1),
			                                                core::fast_atof(p2),
			                                                core::fast_atof(p3)));
		}
		else
		if (wordBuffer[0] == 'f' && wordBuffer[1] == 0x0)
		{
			// face
			c8 faceBuf[20];
			copyWordLineEnd(wordBuffer, WORD_BUFFER_LENGHT, word);
			//copyWord(wordBuffer, WORD_BUFFER_LENGHT, word);

			const s32 MAX_FACE_POINT_COUNT = 40;
			u32 facePointCount = 0;
			s32 facePoints[MAX_FACE_POINT_COUNT][3];

			for (s32 k=0; k<MAX_FACE_POINT_COUNT; ++k)
			{
				facePoints[k][0] = -1;
				facePoints[k][1] = -1;
				facePoints[k][2] = -1;
			}

			c8* face = getNextWord(wordBuffer);

			while(face && face[0])
			{
				copyWordSpaceEnd(faceBuf, WORD_BUFFER_LENGHT, face);

				//im facebuf steht jetzt die face.
				s32 idx = 0;
				s32 len = (s32)strlen(faceBuf);
				s32 z = 0;

				while (idx < len)
				{
					facePoints[facePointCount][z] = atoi(&faceBuf[idx]);
					for (++idx; idx<len && faceBuf[idx] && faceBuf[idx-1]!='/'; ++idx) ;
					z++;
				}

				++facePointCount;

				if (facePointCount >= (u32)MAX_FACE_POINT_COUNT)
				{
					meshbuffer->drop();
					os::Printer::log("Face with more than 40 face points found s32 file. Not loading.", file->getFileName(), ELL_ERROR);
					delete [] buf;
					return false;
				}

				face = getNextWord(face);
			}

			// now we've got a face.
			// and have to create some vertices and indices from them

			s32 currentVertexCount = meshbuffer->Vertices.size();

			for (u32 i=0; i<facePointCount; ++i)
			{
				video::S3DVertex v;
				v.Color = video::SColor(255,255,255,255);

				if (facePoints[i][2]-1 >= 0 && facePoints[i][2]-1 < (s32)normalsBuffer.size())
				{
					v.Normal.X = normalsBuffer[ facePoints[i][2]-1 ].X;
					v.Normal.Y = normalsBuffer[ facePoints[i][2]-1 ].Y;
					v.Normal.Z = normalsBuffer[ facePoints[i][2]-1].Z;
				}
				else
				{
					v.Normal.X = v.Normal.Y = v.Normal.Z = 0;
				}

				if (facePoints[i][0]-1 >= 0 && facePoints[i][0]-1 < (s32)vertexBuffer.size())
				{
					v.Pos.X  = vertexBuffer [ facePoints[i][0]-1 ].X;
					v.Pos.Y  = vertexBuffer [ facePoints[i][0]-1 ].Y;
					v.Pos.Z  = vertexBuffer [ facePoints[i][0]-1 ].Z;
				}
				else
				{
					v.Pos.X = v.Pos.Y = v.Pos.Z = 0;
				}

				if (facePoints[i][1]-1 >= 0 && facePoints[i][1]-1 < (s32)textureCoordBuffer.size())
				{
					v.TCoords.X = textureCoordBuffer[ facePoints[i][1]-1].X;
					v.TCoords.Y = -textureCoordBuffer[ facePoints[i][1]-1].Y;
				}
				else
				{
					v.TCoords.X = v.TCoords.Y = 0;
				}


				meshbuffer->Vertices.push_back(v);
			}

			meshbuffer->Indices.push_back(0 + currentVertexCount);
			meshbuffer->Indices.push_back(1 + currentVertexCount);
			meshbuffer->Indices.push_back((facePointCount-1) + currentVertexCount);


			for (u32 jk=0; jk<facePointCount-3; ++jk)
			{
				meshbuffer->Indices.push_back(1 + currentVertexCount);
				meshbuffer->Indices.push_back((facePointCount-2-jk) + currentVertexCount);
				meshbuffer->Indices.push_back((facePointCount-1-jk) + currentVertexCount);
			}

		}
		else
		if (wordBuffer[0] == '#' || wordBuffer[0] == 'u' ||wordBuffer[0] =='g')
		{
			// comment
			// find end of line
			s32 i=0;
			while(line[i])
			{
				if (line[i]=='\n' || line[i]=='\r')
					break;

				++i;
			}

			line = &line[i];
		}


		line = getNextWord(line);
	}

	delete [] buf;

	meshbuffer->recalculateBoundingBox();
	Mesh.addMeshBuffer(meshbuffer);
	Mesh.recalculateBoundingBox();
	meshbuffer->drop();
	return true;
}


c8* CStaticMeshOBJ::getFirstWord(c8* buf)
{
	s32 i = 0;
	while(buf[i] && (buf[i]==' ' || buf[i]=='\n' || buf[i]=='\r' || buf[i]=='\t'))
		++i;

	return &buf[i];
}



void CStaticMeshOBJ::copyWord(c8* outBuf, s32 bufLength, c8* word)
{
	if (!word)
		return;

	s32 i = 0;
	while(word[i])
	{
		if (word[i]==' ' || word[i]=='\n' || word[i]=='\r' || word[i]=='\t')
			break;
		++i;
	}

	for (s32 j=0; j<i; ++j)
		outBuf[j] = word[j];

	outBuf[i] = 0x0;
}


void CStaticMeshOBJ::copyWordLineEnd(c8* outBuf, s32 bufLength, c8* word)
{
	if (!word)
		return;

	s32 i = 0;
	while(word[i])
	{
		if (word[i]=='\n')
			break;
		++i;
	}

	for (s32 j=0; j<i; ++j)
		outBuf[j] = word[j];

	outBuf[i] = 0x0;
}

void CStaticMeshOBJ::copyWordSpaceEnd(c8* outBuf, s32 bufLength, c8* word)
{
	if (!word)
		return;

	s32 i = 0;
	while(word[i])
	{
		if (word[i]==' ')
			break;
		++i;
	}

	for (s32 j=0; j<i; ++j)
		outBuf[j] = word[j];

	outBuf[i] = 0x0;
}



c8* CStaticMeshOBJ::getNextWord(c8* word)
{
	if (!word)
		return 0;

	s32 i = 0;
	while(word[i])
	{
		if (word[i]==' ' || word[i]=='\n' || word[i]=='\r' || word[i]=='\t')
			break;

		++i;
	}

	c8* nextWord = getFirstWord(&word[i]);
	return nextWord == word ? 0 : nextWord;
}


//! Returns an axis aligned bounding box of the mesh.
//! \return A bounding box of this mesh is returned.
const core::aabbox3d<f32>& CStaticMeshOBJ::getBoundingBox() const
{
	return Mesh.getBoundingBox();
}


//! Returns the type of the animated mesh.
E_ANIMATED_MESH_TYPE CStaticMeshOBJ::getMeshType() const
{
	return EAMT_OBJ;
}


} // end namespace scene
} // end namespace engine

