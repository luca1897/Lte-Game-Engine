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
 
#include "CMeshManipulator.h"
#include "IMesh.h"
#include "SMesh.h"
#include "SMeshBuffer.h"
#include "SMeshBufferLightMap.h"
#include "SMeshBufferTangents.h"
#include "IAnimatedMesh.h"
#include "os.h"
#include <map>
#include <vector>
#include <pspdebug.h>
#define printf pspDebugScreenPrintf


namespace engine
{
namespace scene
{

extern unsigned int computeHash( unsigned int seed, const unsigned char* buffer, unsigned int length );


//! Recalculates the normals in vertex array.
//! This template function was a member of the CMeshManipulator class, but
//! visual studio 6.0 didn't like it.
template<class VTXTYPE>
inline void recalculateNormalsT_Flat(VTXTYPE* v, int vtxcnt,
                                     u16* idx, int idxcnt)
{

	for (int i=0; i<idxcnt; i+=3)
	{
		core::plane3d<f32> p(v[idx[i+0]].Pos, v[idx[i+1]].Pos, v[idx[i+2]].Pos);
		p.Normal.normalize();

		v[idx[i+0]].Normal = p.Normal;
		v[idx[i+1]].Normal = p.Normal;
		v[idx[i+2]].Normal = p.Normal;
	}
}

template<class VTXTYPE>
inline void recalculateNormalsT_Smooth(VTXTYPE* v, int vtxcnt,
                                       u16* idx, int idxcnt)
{
	s32 i;

	for ( i = 0; i!= vtxcnt; ++i )
	{
		v[i].Normal.set ( 0.f, 0.f, 0.f );
	}

	for ( i=0; i<idxcnt; i+=3)
	{
		core::plane3d<f32> p(v[idx[i+0]].Pos, v[idx[i+1]].Pos, v[idx[i+2]].Pos);
		//p.Normal.normalize();

		v[idx[i+0]].Normal += p.Normal;
		v[idx[i+1]].Normal += p.Normal;
		v[idx[i+2]].Normal += p.Normal;
	}

	for ( i = 0; i!= vtxcnt; ++i )
	{
		v[i].Normal.normalize ();
	}

}

#include <pspdebug.h>
#define printf pspDebugScreenPrintf


//! Recalculates normals in a vertex array.
//! This template function was a member of the CMeshManipulator class, but
//! visual studio 6.0 didn't like it.
template<class VERTEXTYPE>
inline void makePlanarMappingT(VERTEXTYPE *v,
                               int vtxcnt,
                               u16* idx, int idxcnt, f32 resolution)
{
	for (int i=0; i<idxcnt; i+=3)
	{
		core::plane3d<f32> p(v[idx[i+0]].Pos, v[idx[i+1]].Pos, v[idx[i+2]].Pos);
		p.Normal.normalize();

		p.Normal.X = (f32)(fabs(p.Normal.X));
		p.Normal.Y = (f32)(fabs(p.Normal.Y));
		p.Normal.Z = (f32)(fabs(p.Normal.Z));
		// calculate planar mapping worldspace coordinates

		if (p.Normal.X > p.Normal.Y && p.Normal.X > p.Normal.Z)
		{
			for (s32 o=0; o<3; ++o)
			{
				v[idx[i+o]].TCoords.X = (v[idx[i+o]].Pos.Y * resolution);
				v[idx[i+o]].TCoords.Y = (v[idx[i+o]].Pos.Z * resolution);
			}
		}
		else
		if (p.Normal.Y > p.Normal.X && p.Normal.Y > p.Normal.Z)
		{
			for (s32 o=0; o<3; ++o)
			{
				v[idx[i+o]].TCoords.X = (v[idx[i+o]].Pos.X * resolution);
				v[idx[i+o]].TCoords.Y = (v[idx[i+o]].Pos.Z * resolution);
			}
		}
		else
		{
			for (s32 o=0; o<3; ++o)
			{
				v[idx[i+o]].TCoords.X = (v[idx[i+o]].Pos.X * resolution);
				v[idx[i+o]].TCoords.Y = (v[idx[i+o]].Pos.Y * resolution);
			}
		}
	}
}

//! Constructor
CMeshManipulator::CMeshManipulator()
{
}


//! destructor
CMeshManipulator::~CMeshManipulator()
{
}

//! Flips the direction of surfaces. Changes backfacing triangles to frontfacing
//! triangles and vice versa.
//! \param mesh: Mesh on which the operation is performed.
void CMeshManipulator::flipSurfaces(scene::IMesh* mesh) const
{
	if (!mesh)
		return;

	s32 bcount = mesh->getMeshBufferCount();
	for (s32 b=0; b<bcount; ++b)
	{
		IMeshBuffer* buffer = mesh->getMeshBuffer(b);
		s32 idxcnt = buffer->getIndexCount();
		u16* idx = buffer->getIndices();
		s32 tmp;

		for (s32 i=0; i<idxcnt; i+=3)
		{
			tmp = idx[i+1];
			idx[i+1] = idx[i+2];
			idx[i+2] = tmp;
		}
	}
}



//! Sets the alpha vertex color value of the whole mesh to a new value
//! \param mesh: Mesh on which the operation is performed.
void CMeshManipulator::setVertexColorAlpha(scene::IMesh* mesh, s32 alpha) const
{
	if (!mesh)
		return;

	s32 i = 0;

	s32 bcount = mesh->getMeshBufferCount();
	for (s32 b=0; b<bcount; ++b)
	{
		IMeshBuffer* buffer = mesh->getMeshBuffer(b);
		void* v = buffer->getVertices();
		s32 vtxcnt = buffer->getVertexCount();

		switch(buffer->getVertexType())
		{
		case video::EVT_STANDARD:
			for (; i<vtxcnt; ++i)
				((video::S3DVertex*)v)[i].Color.setAlpha(alpha);
			break;
		case video::EVT_2TCOORDS:
			for (; i<vtxcnt; ++i)
				((video::S3DVertex2TCoords*)v)[i].Color.setAlpha(alpha);
			break;
		case video::EVT_TANGENTS:
			for (; i<vtxcnt; ++i)
				((video::S3DVertexTangents*)v)[i].Color.setAlpha(alpha);
			break;
		}
	}
}

//! Sets the colors of all vertices to one color
void CMeshManipulator::setVertexColors(IMesh* mesh, video::SColor color) const
{
	if (!mesh)
		return;

	s32 i = 0;

	s32 bcount = mesh->getMeshBufferCount();
	for (s32 b=0; b<bcount; ++b)
	{
		IMeshBuffer* buffer = mesh->getMeshBuffer(b);
		void* v = buffer->getVertices();
		s32 vtxcnt = buffer->getVertexCount();

		switch(buffer->getVertexType())
		{
		case video::EVT_STANDARD:
			for (; i<vtxcnt; ++i)
				((video::S3DVertex*)v)[i].Color = color;
			break;
		case video::EVT_2TCOORDS:
			for (; i<vtxcnt; ++i)
				((video::S3DVertex2TCoords*)v)[i].Color = color;
			break;
		case video::EVT_TANGENTS:
			for (; i<vtxcnt; ++i)
				((video::S3DVertexTangents*)v)[i].Color = color;
			break;
		}
	}
}


//! Recalculates all normals of the mesh buffer.
/** \param buffer: Mesh buffer on which the operation is performed. */
void CMeshManipulator::recalculateNormals(IMeshBuffer* buffer, bool smooth) const
{
	if (!buffer)
		return;

	s32 vtxcnt = buffer->getVertexCount();
	s32 idxcnt = buffer->getIndexCount();
	u16* idx = buffer->getIndices();

	switch(buffer->getVertexType())
	{
	case video::EVT_STANDARD:
	{
		video::S3DVertex* v = (video::S3DVertex*)buffer->getVertices();

		if (!smooth)
			recalculateNormalsT_Flat(v, vtxcnt, idx, idxcnt);
		else
			recalculateNormalsT_Smooth(v, vtxcnt, idx, idxcnt);
	}
		break;
	case video::EVT_2TCOORDS:
	{
		video::S3DVertex2TCoords* v = (video::S3DVertex2TCoords*)buffer->getVertices();

		if (!smooth)
			recalculateNormalsT_Flat(v, vtxcnt, idx, idxcnt);
		else
			recalculateNormalsT_Smooth(v, vtxcnt, idx, idxcnt);
	}
		break;
	case video::EVT_TANGENTS:
	{
		// TODO: recalculate tangent and binormal
		video::S3DVertexTangents* v = (video::S3DVertexTangents*)buffer->getVertices();
		if (!smooth)
			recalculateNormalsT_Flat(v, vtxcnt, idx, idxcnt);
		else
			recalculateNormalsT_Smooth(v, vtxcnt, idx, idxcnt);
	}
	}
}



//! Recalculates all normals of the mesh.
//! \param mesh: Mesh on which the operation is performed.
void CMeshManipulator::recalculateNormals(scene::IMesh* mesh, bool smooth) const
{
	if (!mesh)
		return;

	s32 bcount = mesh->getMeshBufferCount();
	for (s32 b=0; b<bcount; ++b)
		recalculateNormals(mesh->getMeshBuffer(b), smooth);
}



//! Scales the whole mesh.
//! \param mesh: Mesh on which the operation is performed.
void CMeshManipulator::scaleMesh(scene::IMesh* mesh, const core::vector3df& scale) const
{
	if (!mesh)
		return;

	core::aabbox3df meshbox;

	s32 bcount = mesh->getMeshBufferCount();
	for (s32 b=0; b<bcount; ++b)
	{
		s32 i = 0;

		IMeshBuffer* buffer = mesh->getMeshBuffer(b);
		void* v = buffer->getVertices();
		s32 vtxcnt = buffer->getVertexCount();
		core::aabbox3df bufferbox;

		switch(buffer->getVertexType())
		{
		case video::EVT_STANDARD:
			if (vtxcnt != 0)
				bufferbox.reset(((video::S3DVertex*)v)[0].Pos * scale);
			for (; i<vtxcnt; ++i)
			{
				((video::S3DVertex*)v)[i].Pos *= scale;
				bufferbox.addInternalPoint(((video::S3DVertex*)v)[i].Pos);
			}
			break;
		case video::EVT_2TCOORDS:
			if (vtxcnt != 0)
				bufferbox.reset(((video::S3DVertex2TCoords*)v)[0].Pos * scale);
			for (; i<vtxcnt; ++i)
			{
				((video::S3DVertex2TCoords*)v)[i].Pos *= scale;
				bufferbox.addInternalPoint(((video::S3DVertex2TCoords*)v)[i].Pos);
			}
			break;
		case video::EVT_TANGENTS:
			if (vtxcnt != 0)
				bufferbox.reset(((video::S3DVertexTangents*)v)[0].Pos * scale);
			for (; i<vtxcnt; ++i)
			{
				((video::S3DVertexTangents*)v)[i].Pos *= scale;
				bufferbox.addInternalPoint(((video::S3DVertexTangents*)v)[i].Pos);
			}
			break;
		}

		buffer->getBoundingBox() = bufferbox;

		if (b == 0)
			meshbox.reset(buffer->getBoundingBox());
		else
			meshbox.addInternalBox(buffer->getBoundingBox());
	}

	mesh->getBoundingBox() = meshbox;
}


//! Recalculates the bounding box for a meshbuffer
void CMeshManipulator::recalculateBoundingBox(scene::IMeshBuffer* buffer) const
{
	void* v = buffer->getVertices();
	s32 vtxcnt = buffer->getVertexCount();
	core::aabbox3df box;

	s32 i = 1;

	switch(buffer->getVertexType())
	{
	case video::EVT_STANDARD:
		if (vtxcnt != 0)
			box.reset(((video::S3DVertex*)v)[0].Pos);
		for (; i<vtxcnt; ++i)
			box.addInternalPoint(((video::S3DVertex*)v)[i].Pos);
		break;
	case video::EVT_2TCOORDS:
		if (vtxcnt != 0)
			box.reset(((video::S3DVertex2TCoords*)v)[0].Pos);
		for (; i<vtxcnt; ++i)
			box.addInternalPoint(((video::S3DVertex2TCoords*)v)[i].Pos);
		break;
	case video::EVT_TANGENTS:
		if (vtxcnt != 0)
			box.reset(((video::S3DVertexTangents*)v)[0].Pos);
		for (; i<vtxcnt; ++i)
			box.addInternalPoint(((video::S3DVertexTangents*)v)[i].Pos);
		break;
	}

	buffer->getBoundingBox() = box;
}

struct Triangle {
	u16 i[3];
};
struct Edge
{
	unsigned int i[2];
	core::vector3df normal;
};


// build shadow volume, this algorithm assumes that there are no hard edges in the model
bool __buildShadowVolume(
        const core::vector3df* vertices, unsigned int numVertices, const Triangle* tris, unsigned int numTris,
        core::array<video::S3DVertex>* outVertices, core::array<Triangle>* outTris
        )
{
	core::array<unsigned int> indexRemap;
	core::array<core::vector3df> normals;

	typedef std::map<u64,Edge> EdgeMap;
	EdgeMap edges;

	//unsigned int currentTriangle = 0;
	unsigned int savedEdges = 0;

	for (unsigned int i = 0; i < numTris; ++i)
	{
		const Triangle& tri = tris[i];
		core::vector3df e1 = vertices[tris[i].i[1]] - vertices[tris[i].i[0]];
		core::vector3df e2 = vertices[tris[i].i[2]] - vertices[tris[i].i[0]];
		core::vector3df faceNormal = e1.crossProduct(e2);
		faceNormal = faceNormal.normalize();

		bool quads[3];
		bool found[3];
		u64 position[3];
		Edge data[3];

		for (unsigned int edge = 0; edge < 3; ++edge)
		{
			unsigned int i1 = tri.i[edge];
			unsigned int i2 = tri.i[(edge+1)%3];
			u64 hash = (u64(i2) << 32)|u64(i1);

			position[edge] = (u64(i1) << 32)|u64(i2);

			EdgeMap::iterator j = edges.find(hash);
			if (j != edges.end())
			{
				data[edge] = (*j).second;
				found[edge] = true;

				quads[edge] = faceNormal.dotProduct(data[edge].normal) < 1.0f;

				if (!quads[edge])
					++savedEdges;

				edges.erase(j);
			}
			else
				found[edge] = false;
		}

		unsigned int indices[3];
		for (unsigned int index = 0; index < 3; ++index)
		{
			unsigned int i1 = index;
			unsigned int i2 = (index + 2) % 3;

			if (found[i1] && !quads[i1])
				indices[index] = data[i1].i[1];
			else if (found[i2] && !quads[i2])
				indices[index] = data[i2].i[0];
			else
			{
				indices[index] = indexRemap.size();
				indexRemap.push_back(tri.i[index]);
				normals.push_back(faceNormal);
			}
		}

		Triangle T;

		for (unsigned int j = 0; j < 3; ++j)
		{

			T.i[j] = indices[j];
		}
		outTris->push_back( T );
		//	++currentTriangle;

		for (unsigned int edge = 0; edge < 3; ++edge)
		{
			unsigned int i0 = indices[edge];
			unsigned int i1 = indices[(edge+1)%3];

			if (!found[edge])
			{
				Edge e;
				e.i[0] = i0;
				e.i[1] = i1;
				e.normal = faceNormal;

				edges[position[edge]] = e;
				continue;
			}

			if ((data[edge].i[1] != i0) && (data[edge].i[0] != i1))
			{
				T.i[0] = i1;
				T.i[1] = i0;
				T.i[2] = data[edge].i[0];
				outTris->push_back( T );

				T.i[0] = data[edge].i[1];
				T.i[1] = data[edge].i[0];
				T.i[2] = i0;
				outTris->push_back( T );
			}
			else if (data[edge].i[1] != i0)
			{
				T.i[0] = i1;
				T.i[1] = i0;
				T.i[2] = data[edge].i[1];
				outTris->push_back( T );
			}
			else if (data[edge].i[0] != i1)
			{
				T.i[0] = i1;
				T.i[1] = i0;
				T.i[2] = data[edge].i[0];
				outTris->push_back( T );
			}
		}
	}

	for (unsigned int i = 0, n = indexRemap.size(); i < n; ++i)
	{
		video::S3DVertex P;

		P.Pos = vertices[indexRemap[i]];
		P.Normal = normals[i];

		outVertices->push_back( P );

	}

	return true;
}



// pregenera uno shadow volume, utilissimo per le static mesh
SMesh* CMeshManipulator::buildShadowVolume(scene::IMesh* mesh)
{

	SMeshBuffer* output = new SMeshBuffer;
	SMesh* output_mesh = new SMesh;

	int i = 0;

	core::array<core::vector3df> Vertices;
	core::array<u16> Indices;


	// store the mesh buffers
	for (int x = 0; x < mesh->getMeshBufferCount(); x++)
	{

		IMeshBuffer *k = mesh->getMeshBuffer(x);

		int vcount = Vertices.size();
		int icount = Indices.size();

		Vertices.reallocate( vcount + k->getVertexCount());
		Indices.reallocate( icount + k->getIndexCount());

		video::S3DVertex *ptr = (video::S3DVertex*)k->getVertices();

		for (int o = 0; o < k->getVertexCount(); o++)
			Vertices.push_back (  ptr[o].Pos );

		for (int o = 0; o < k->getIndexCount(); o++)
			Indices.push_back( k->getIndices()[o] + vcount );

	}




	// remove hard vertices
	typedef std::map<u32,u16> ConData;
	ConData tdatas;

	for (int x = 0; x < Indices.size(); x++)
	{
		unsigned int current_hash = computeHash(0, (unsigned char*)&Vertices.pointer()[Indices[x]], sizeof(core::vector3df));
		ConData::iterator j = tdatas.find(current_hash);

		if (j == tdatas.end())
			tdatas[current_hash] = Indices[x];
		else
			Indices[x] = tdatas[current_hash];
	}
	core::array<Triangle> p;



	__buildShadowVolume(
	        Vertices.pointer(), Vertices.size(), (Triangle*)Indices.pointer(), Indices.size() / 3,
	        &output->Vertices, &p
	        );

	for (int o = 0; o < p.size(); o++)
		for (int v= 0; v < 3; v++)
			output->Indices.push_back( p[o].i[v]);

	output->Indices.reallocate( output->Indices.size() );
	output->Vertices.reallocate( output->Vertices.size() );

	output_mesh->addMeshBuffer( output );
	output->drop();

	return output_mesh;
}

//! Clones a static IMesh into a modifyable SMesh.
SMesh* CMeshManipulator::createMeshCopy(scene::IMesh* mesh) const
{
	if (!mesh)
		return 0;

	SMesh* clone = new SMesh();

	s32 meshBufferCount = mesh->getMeshBufferCount();

	for (s32 b=0; b<meshBufferCount; ++b)
	{
		s32 vtxCnt = mesh->getMeshBuffer(b)->getVertexCount();
		s32 idxCnt = mesh->getMeshBuffer(b)->getIndexCount();
		const u16* idx = mesh->getMeshBuffer(b)->getIndices();

		switch(mesh->getMeshBuffer(b)->getVertexType())
		{
		case video::EVT_STANDARD:
		{
			SMeshBuffer* buffer = new SMeshBuffer();
			buffer->Material = mesh->getMeshBuffer(b)->getMaterial();

			video::S3DVertex* v =
			        (video::S3DVertex*)mesh->getMeshBuffer(b)->getVertices();

			s32 i;

			for (i=0; i<vtxCnt; ++i)
				buffer->Vertices.push_back(v[i]);

			for (i=0; i<idxCnt; ++i)
				buffer->Indices.push_back(idx[i]);

			clone->addMeshBuffer(buffer);
			buffer->drop();
		}
			break;
		case video::EVT_2TCOORDS:
		{
			SMeshBufferLightMap* buffer = new SMeshBufferLightMap();
			buffer->Material = mesh->getMeshBuffer(b)->getMaterial();

			video::S3DVertex2TCoords* v =
			        (video::S3DVertex2TCoords*)mesh->getMeshBuffer(b)->getVertices();

			s32 i;

			for (i=0; i<vtxCnt; ++i)
				buffer->Vertices.push_back(v[i]);

			for (i=0; i<idxCnt; ++i)
				buffer->Indices.push_back(idx[i]);

			clone->addMeshBuffer(buffer);
			buffer->drop();
		}
			break;
		case video::EVT_TANGENTS:
		{
			SMeshBufferTangents* buffer = new SMeshBufferTangents();
			buffer->Material = mesh->getMeshBuffer(b)->getMaterial();

			video::S3DVertexTangents* v =
			        (video::S3DVertexTangents*)mesh->getMeshBuffer(b)->getVertices();

			s32 i;

			for (i=0; i<vtxCnt; ++i)
				buffer->Vertices.push_back(v[i]);

			for (i=0; i<idxCnt; ++i)
				buffer->Indices.push_back(idx[i]);

			clone->addMeshBuffer(buffer);
			buffer->drop();
		}
			break;
		} // end switch

	} // end for all mesh buffers

	clone->BoundingBox = mesh->getBoundingBox();
	return clone;
}

video::SColor __interpolate_color(video::SColor col0, video::SColor col1)
{
	return col0.getInterpolated(col1, 0.5);
}
#include <pspdebug.h>
#define printf pspDebugScreenPrintf

void  __subdivide_triangles(core::array<u16>* idx, core::array<video::S3DVertex> *vtx, s32 maxlen)
{
	u16* indexes = idx->pointer();
	video::S3DVertex* vertexes = vtx->pointer();
	s32 count = idx->size();

	for (int x = 0; x < count; x+= 3)
	{

		float a,b,c,p;
		a = (vertexes[indexes[x]].Pos - vertexes[indexes[x+1]].Pos).getLength();
		b = (vertexes[indexes[x+1]].Pos - vertexes[indexes[x+2]].Pos).getLength();
		c = (vertexes[indexes[x]].Pos - vertexes[indexes[x+2]].Pos).getLength();

		if (a > maxlen || b > maxlen || c > maxlen)
		{
			// il triangolo va suddiviso
			video::S3DVertex out[3];
			s32 curidx = vtx->size();

			out[0].Pos = (vertexes[indexes[x]].Pos + vertexes[indexes[x+1]].Pos) * 0.5;
			out[1].Pos = (vertexes[indexes[x]].Pos + vertexes[indexes[x+2]].Pos) * 0.5;
			out[2].Pos = (vertexes[indexes[x+1]].Pos + vertexes[indexes[x+2]].Pos) * 0.5;
			out[0].Normal = (vertexes[indexes[x]].Normal + vertexes[indexes[x+1]].Normal) * 0.5;
			out[1].Normal = (vertexes[indexes[x]].Normal + vertexes[indexes[x+2]].Normal) * 0.5;
			out[2].Normal = (vertexes[indexes[x+1]].Normal + vertexes[indexes[x+2]].Normal) * 0.5;
			out[0].TCoords = (vertexes[indexes[x]].TCoords + vertexes[indexes[x+1]].TCoords) * 0.5;
			out[1].TCoords = (vertexes[indexes[x]].TCoords + vertexes[indexes[x+2]].TCoords) * 0.5;
			out[2].TCoords = (vertexes[indexes[x+1]].TCoords + vertexes[indexes[x+2]].TCoords) * 0.5;
			out[0].Color = __interpolate_color(vertexes[indexes[x]].Color,vertexes[indexes[x+1]].Color);
			out[1].Color =  __interpolate_color(vertexes[indexes[x]].Color,vertexes[indexes[x+2]].Color);
			out[2].Color =  __interpolate_color(vertexes[indexes[x+1]].Color,vertexes[indexes[x+2]].Color);

			vtx->push_back(out[0]);
			vtx->push_back(out[1]);
			vtx->push_back(out[2]);
			vertexes = vtx->pointer();
			// costruisco i triangoli


			{
				idx->push_back(curidx+1);
				idx->push_back(curidx);
				idx->push_back(curidx+2);
			}



			{
				idx->push_back(curidx+2);
				//refresh ptr
				indexes = idx->pointer();
				idx->push_back(indexes[x + 2]);
				idx->push_back(curidx+1);
			}


			{
				//refresh ptr
				indexes = idx->pointer();
				idx->push_back(curidx);
				idx->push_back(indexes[x+1]);
				idx->push_back(curidx+2);
			}

			{
				//refresh ptr
				indexes = idx->pointer();

				indexes[x+2] = curidx+1;
				indexes[x+1] = curidx;
			}

		}
	}



	if (count != idx->size())
		__subdivide_triangles(idx, vtx, maxlen);
}

void CMeshManipulator::subdivideTriangles(scene::IMesh* mesh, s32 maxlen) const
{
	if (!mesh)
		return;

	s32 bcount = mesh->getMeshBufferCount();
	for (s32 b = 0; b < bcount; b++)
	{
		IMeshBuffer* buffer = mesh->getMeshBuffer(b);
		s32 vtxcnt = buffer->getVertexCount();
		s32 idxcnt = buffer->getIndexCount();
		core::array<u16>* idx;
		core::array<video::S3DVertex>* vtx;

		switch(buffer->getVertexType())
		{
		case video::EVT_STANDARD:
		{
			idx = &((SMeshBuffer*)buffer)->Indices;
			vtx = &((SMeshBuffer*)buffer)->Vertices;

			__subdivide_triangles(idx, vtx, maxlen);

			idx->reallocate( idx->size() );
			vtx->reallocate( vtx->size() );

		}
			break;
		case video::EVT_2TCOORDS:
		{
			idx = &((SMeshBufferLightMap*)buffer)->Indices;
			vtx = (core::array<video::S3DVertex>*)&(((SMeshBufferLightMap*)buffer)->Vertices);



			__subdivide_triangles(idx, vtx, maxlen);

			idx->reallocate( idx->size() );
			vtx->reallocate( vtx->size() );
		}
			break;
		}

	}



}


//! Creates a planar texture mapping on the mesh
//! \param mesh: Mesh on which the operation is performed.
//! \param resolution: resolution of the planar mapping. This is the value
//! specifying which is the releation between world space and
//! texture coordinate space.
void CMeshManipulator::makePlanarTextureMapping(scene::IMesh* mesh, f32 resolution=0.01f) const
{
	if (!mesh)
		return;

	s32 bcount = mesh->getMeshBufferCount();
	for (s32 b=0; b<bcount; ++b)
	{
		IMeshBuffer* buffer = mesh->getMeshBuffer(b);
		s32 vtxcnt = buffer->getVertexCount();
		s32 idxcnt = buffer->getIndexCount();
		u16* idx = buffer->getIndices();

		switch(buffer->getVertexType())
		{
		case video::EVT_STANDARD:
		{
			video::S3DVertex* v = (video::S3DVertex*)buffer->getVertices();
			makePlanarMappingT(v, vtxcnt, idx, idxcnt, resolution);
		}
			break;
		case video::EVT_2TCOORDS:
		{
			video::S3DVertex2TCoords* v = (video::S3DVertex2TCoords*)buffer->getVertices();
			makePlanarMappingT(v, vtxcnt, idx, idxcnt, resolution);
		}
			break;
		case video::EVT_TANGENTS:
		{
			video::S3DVertexTangents* v = (video::S3DVertexTangents*)buffer->getVertices();
			makePlanarMappingT(v, vtxcnt, idx, idxcnt, resolution);
		}
			break;
		}
	}
}


//! Creates a copy of the mesh, which will only consist of unique primitives
IMesh* CMeshManipulator::createMeshUniquePrimitives(IMesh* mesh) const
{
	if (!mesh)
		return 0;

	SMesh* clone = new SMesh();

	s32 meshBufferCount = mesh->getMeshBufferCount();

	for (s32 b=0; b<meshBufferCount; ++b)
	{
		s32 vtxCnt = mesh->getMeshBuffer(b)->getVertexCount();
		s32 idxCnt = mesh->getMeshBuffer(b)->getIndexCount();
		const u16* idx = mesh->getMeshBuffer(b)->getIndices();

		switch(mesh->getMeshBuffer(b)->getVertexType())
		{
		case video::EVT_STANDARD:
		{
			SMeshBuffer* buffer = new SMeshBuffer();
			buffer->Material = mesh->getMeshBuffer(b)->getMaterial();

			video::S3DVertex* v =
			        (video::S3DVertex*)mesh->getMeshBuffer(b)->getVertices();

			s32 i;
			for (i=0; i<idxCnt; i += 3)
			{
				buffer->Vertices.push_back( v[idx[i + 0 ]] );
				buffer->Vertices.push_back( v[idx[i + 1 ]] );
				buffer->Vertices.push_back( v[idx[i + 2 ]] );

				buffer->Indices.push_back( i + 0 );
				buffer->Indices.push_back( i + 1 );
				buffer->Indices.push_back( i + 2 );
			}

			clone->addMeshBuffer(buffer);
			buffer->drop();
		}
			break;
		case video::EVT_2TCOORDS:
		{
			SMeshBufferLightMap* buffer = new SMeshBufferLightMap();
			buffer->Material = mesh->getMeshBuffer(b)->getMaterial();

			video::S3DVertex2TCoords* v =
			        (video::S3DVertex2TCoords*)mesh->getMeshBuffer(b)->getVertices();

			s32 i;
			for (i=0; i<idxCnt; i += 3)
			{
				buffer->Vertices.push_back( v[idx[i + 0 ]] );
				buffer->Vertices.push_back( v[idx[i + 1 ]] );
				buffer->Vertices.push_back( v[idx[i + 2 ]] );

				buffer->Indices.push_back( i + 0 );
				buffer->Indices.push_back( i + 1 );
				buffer->Indices.push_back( i + 2 );
			}
			clone->addMeshBuffer(buffer);
			buffer->drop();
		}
			break;
		case video::EVT_TANGENTS:
		{
			SMeshBufferTangents* buffer = new SMeshBufferTangents();
			buffer->Material = mesh->getMeshBuffer(b)->getMaterial();

			video::S3DVertexTangents* v =
			        (video::S3DVertexTangents*)mesh->getMeshBuffer(b)->getVertices();

			s32 i;
			for (i=0; i<idxCnt; i += 3)
			{
				buffer->Vertices.push_back( v[idx[i + 0 ]] );
				buffer->Vertices.push_back( v[idx[i + 1 ]] );
				buffer->Vertices.push_back( v[idx[i + 2 ]] );

				buffer->Indices.push_back( i + 0 );
				buffer->Indices.push_back( i + 1 );
				buffer->Indices.push_back( i + 2 );
			}

			clone->addMeshBuffer(buffer);
			buffer->drop();
		}
			break;
		} // end switch

	} // end for all mesh buffers

	clone->BoundingBox = mesh->getBoundingBox();
	return clone;

}



//! Creates a copy of the mesh, which will only consist of S3DVertexTangents vertices.
IMesh* CMeshManipulator::createMeshWithTangents(IMesh* mesh) const
{
	if (!mesh)
		return 0;

	// copy mesh and fill data into SMeshBufferTangents

	SMesh* clone = new SMesh();
	s32 meshBufferCount = mesh->getMeshBufferCount();

	s32 b=0;
	for (; b<meshBufferCount; ++b)
	{
		s32 vtxCnt = mesh->getMeshBuffer(b)->getVertexCount();
		s32 idxCnt = mesh->getMeshBuffer(b)->getIndexCount();
		const u16* idx = mesh->getMeshBuffer(b)->getIndices();

		SMeshBufferTangents* buffer = new SMeshBufferTangents();
		buffer->Material = mesh->getMeshBuffer(b)->getMaterial();

		// copy vertices
		s32 i;

		switch(mesh->getMeshBuffer(b)->getVertexType())
		{
		case video::EVT_STANDARD:
		{
			video::S3DVertex* v =
			        (video::S3DVertex*)mesh->getMeshBuffer(b)->getVertices();

			for (i=0; i<idxCnt; ++i)
				buffer->Vertices.push_back(
				        video::S3DVertexTangents(
				                v[idx[i]].Pos, v[idx[i]].TCoords, v[idx[i]].Color));
		}
			break;
		case video::EVT_2TCOORDS:
		{
			video::S3DVertex2TCoords* v =
			        (video::S3DVertex2TCoords*)mesh->getMeshBuffer(b)->getVertices();

			for (i=0; i<idxCnt; ++i)
				buffer->Vertices.push_back(video::S3DVertexTangents(
				                                   v[idx[i]].Pos, v[idx[i]].TCoords, v[idx[i]].Color));
		}
			break;
		case video::EVT_TANGENTS:
		{
			video::S3DVertexTangents* v =
			        (video::S3DVertexTangents*)mesh->getMeshBuffer(b)->getVertices();

			for (i=0; i<idxCnt; ++i)
				buffer->Vertices.push_back(v[idx[i]]);
		}
			break;
		}

		// create new indices

		buffer->Indices.set_used(idxCnt);
		for (i=0; i<idxCnt; ++i)
			buffer->Indices[i] = i;

		// add new buffer
		clone->addMeshBuffer(buffer);
		buffer->drop();
	}

	clone->BoundingBox = mesh->getBoundingBox();

	// now calculate tangents
	b=0;
	for (; b<meshBufferCount; ++b)
	{
		int vtxCnt = clone->getMeshBuffer(b)->getVertexCount();
		int idxCnt = clone->getMeshBuffer(b)->getIndexCount();

		u16* idx = clone->getMeshBuffer(b)->getIndices();
		video::S3DVertexTangents* v =
		        (video::S3DVertexTangents*)clone->getMeshBuffer(b)->getVertices();

		for (int i=0; i<idxCnt; i+=3)
		{
			calculateTangents(
			        v[idx[i+0]].Normal,
			        v[idx[i+0]].Tangent,
			        v[idx[i+0]].Binormal,
			        v[idx[i+0]].Pos,
			        v[idx[i+1]].Pos,
			        v[idx[i+2]].Pos,
			        v[idx[i+0]].TCoords,
			        v[idx[i+1]].TCoords,
			        v[idx[i+2]].TCoords);

			calculateTangents(
			        v[idx[i+1]].Normal,
			        v[idx[i+1]].Tangent,
			        v[idx[i+1]].Binormal,
			        v[idx[i+1]].Pos,
			        v[idx[i+2]].Pos,
			        v[idx[i+0]].Pos,
			        v[idx[i+1]].TCoords,
			        v[idx[i+2]].TCoords,
			        v[idx[i+0]].TCoords);

			calculateTangents(
			        v[idx[i+2]].Normal,
			        v[idx[i+2]].Tangent,
			        v[idx[i+2]].Binormal,
			        v[idx[i+2]].Pos,
			        v[idx[i+0]].Pos,
			        v[idx[i+1]].Pos,
			        v[idx[i+2]].TCoords,
			        v[idx[i+0]].TCoords,
			        v[idx[i+1]].TCoords);
		}
	}

	return clone;
}


void CMeshManipulator::calculateTangents(
        core::vector3df& normal,
        core::vector3df& tangent,
        core::vector3df& binormal,
        core::vector3df& vt1, core::vector3df& vt2, core::vector3df& vt3, // vertices
        core::vector2df& tc1, core::vector2df& tc2, core::vector2df& tc3) // texture coords
{
	// choose one of them:
	//#define USE_NVIDIA_GLH_VERSION // use version used by nvidia in glh headers
	#define USE_engine_VERSION

#ifdef USE_engine_VERSION

	core::vector3df v1 = vt1 - vt2;
	core::vector3df v2 = vt3 - vt1;
	normal = v2.crossProduct(v1);
	normal.normalize();

	// binormal

	f32 deltaX1 = tc1.X - tc2.X;
	f32 deltaX2 = tc3.X - tc1.X;
	binormal = (v1 * deltaX2) - (v2 * deltaX1);
	binormal.normalize();

	// tangent

	f32 deltaY1 = tc1.Y - tc2.Y;
	f32 deltaY2 = tc3.Y - tc1.Y;
	tangent = (v1 * deltaY2) - (v2 * deltaY1);
	tangent.normalize();

	// adjust

	core::vector3df txb = tangent.crossProduct(binormal);
	if (txb.dotProduct(normal) < 0.0f)
	{
		tangent *= -1.0f;
		binormal *= -1.0f;
	}

#endif // USE_engine_VERSION

#ifdef USE_NVIDIA_GLH_VERSION

	tangent.set(0,0,0);
	binormal.set(0,0,0);

	core::vector3df v1(vt2.X - vt1.X, tc2.X - tc1.X, tc2.Y - tc1.Y);
	core::vector3df v2(vt3.X - vt1.X, tc3.X - tc1.X, tc3.Y - tc1.Y);

	core::vector3df txb = v1.crossProduct(v2);
	if (fabs(txb.X) > core::ROUNDING_ERROR)
	{
		tangent.X  = -txb.Y / txb.X;
		binormal.X = -txb.Z / txb.X;
	}

	v1.X = vt2.Y - vt1.Y;
	v2.X = vt3.Y - vt1.Y;
	txb = v1.crossProduct(v2);

	if (fabs(txb.X) > core::ROUNDING_ERROR)
	{
		tangent.Y  = -txb.Y / txb.X;
		binormal.Y = -txb.Z / txb.X;
	}

	v1.X = vt2.Z - vt1.Z;
	v2.X = vt3.Z - vt1.Z;
	txb = v1.crossProduct(v2);

	if (fabs(txb.X) > core::ROUNDING_ERROR)
	{
		tangent.Z  = -txb.Y / txb.X;
		binormal.Z = -txb.Z / txb.X;
	}

	tangent.normalize();
	binormal.normalize();

	normal = tangent.crossProduct(binormal);
	normal.normalize();

	binormal = tangent.crossProduct(normal);
	binormal.normalize();

	core::plane3d<f32> pl(vt1, vt2, vt3);

	if(normal.dotProduct(pl.Normal) < 0.0f )
		normal *= -1.0f;

#endif // USE_NVIDIA_GLH_VERSION
}


//! Returns amount of polygons in mesh.
s32 CMeshManipulator::getPolyCount(scene::IMesh* mesh) const
{
	if (!mesh)
		return 0;

	s32 trianglecount = 0;

	for (int g=0; g<mesh->getMeshBufferCount(); ++g)
		trianglecount += mesh->getMeshBuffer(g)->getIndexCount() / 3;

	return trianglecount;
}


//! Returns amount of polygons in mesh.
s32 CMeshManipulator::getPolyCount(scene::IAnimatedMesh* mesh) const
{
	if (mesh && mesh->getFrameCount() != 0)
		return getPolyCount(mesh->getMesh(0));

	return 0;
}




} // end namespace scene
} // end namespace engine

