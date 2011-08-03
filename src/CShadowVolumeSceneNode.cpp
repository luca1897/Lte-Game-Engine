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

#include "CShadowVolumeSceneNode.h"
#include "ISceneManager.h"
#include "IVideoDriver.h"

#include <stdio.h>

#include <pspdebug.h>
#include <pspkernel.h>

#include <matrix4.h>
#include "ICameraSceneNode.h"
#include "Frustum.h"
#include <stdlib.h>
#include <string.h>

#include "ShadInfo.h"

#include <map>
#include <vector>

#include <malloc.h>

#include "CSoftwareDriver2.h"


extern ViewFrustum myVFrustum;
#define printf pspDebugScreenPrintf

void __init_shadow_draw()
{
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT |
	             GL_POLYGON_BIT   | GL_STENCIL_BUFFER_BIT   );
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_STENCIL_TEST);
	glColorMask(GL_FALSE, GL_FALSE,   GL_FALSE, GL_FALSE ); // no color buffer drawing
	//glColor4ub(255,0,0,255);
	glStencilFunc(GL_ALWAYS, 1,   0xFFFFFFFFL   );
	glEnable(GL_CULL_FACE);
	// store current OpenGL   state

}

bool buildShadowVolume(
        const Vertex* vertices, unsigned int numVertices, const Triangle* tris, unsigned int numTris,
        ShadowVertex* outVertices, unsigned int& numOutVertices, Triangle* outTris, unsigned int& numOutTris,
        engine::core::vector3df lpos, float infinity, engine::core::CMemory *mem
        )
{

  #define push_index(x) indexRemap[indexSize++] = x
  #define push_normal(x) normals[normalsSize++] = x


	mem->CFlushMemory();
	u16* indexRemap = (u16*)mem->CMemoryAlloc(65536*2);
	Vertex *normals = (Vertex*)mem->CMemoryAlloc(256*1024 - 65536);

	s32 indexSize = 0;
	s32 normalsSize = 0;

	typedef std::map<u32,Edge> EdgeMap;
	EdgeMap edges;

	unsigned int currentTriangle = 0;
	unsigned int savedEdges = 0;

	for (unsigned int i = 0; i < numTris; ++i)
	{
		const Triangle& tri = tris[i];
		Vertex e1 = vertices[tris[i].i[1]] - vertices[tris[i].i[0]];
		Vertex e2 = vertices[tris[i].i[2]] - vertices[tris[i].i[0]];
		Vertex faceNormal = e1.cross(e2);
		faceNormal = faceNormal.normalize();

		bool quads[3];
		bool found[3];
		u32 position[3];
		Edge data[3];

		for (unsigned int edge = 0; edge < 3; ++edge)
		{
			unsigned short i1 = tri.i[edge];
			unsigned short i2 = tri.i[(edge+1)%3];
			u32 hash = (u32(i2) << 16)|u32(i1);


			position[edge] = (u32(i1) << 16)|u32(i2);

			EdgeMap::iterator j = edges.find(hash);
			if (j != edges.end())
			{
				data[edge] = (*j).second;
				found[edge] = true;

				quads[edge] = faceNormal.dot(data[edge].normal) < 1.0f;


				if (!quads[edge])
					++savedEdges;

				edges.erase(j);
			}
			else
				found[edge] = false;
		}

		unsigned short indices[3];
		for (unsigned int index = 0; index < 3; ++index)
		{
			unsigned short i1 = index;
			unsigned short i2 = (index + 2) % 3;

			if (found[i1] && !quads[i1])
				indices[index] = data[i1].i[1];
			else if (found[i2] && !quads[i2])
				indices[index] = data[i2].i[0];
			else
			{
				indices[index] = indexSize;

				push_index(tri.i[index]);
				push_normal(faceNormal);
				//indexRemap.push_back(tri.i[index]);
				//normals.push_back(faceNormal);
			}
		}

		for (unsigned int j = 0; j < 3; ++j)
		{
			outTris[currentTriangle].i[j] = indices[j];
		}
		++currentTriangle;

		for (unsigned int edge = 0; edge < 3; ++edge)
		{
			unsigned short i0 = indices[edge];
			unsigned short i1 = indices[(edge+1)%3];

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
				outTris[currentTriangle].i[0] = i1;
				outTris[currentTriangle].i[1] = i0;
				outTris[currentTriangle].i[2] = data[edge].i[0];
				++currentTriangle;

				outTris[currentTriangle].i[0] = data[edge].i[1];
				outTris[currentTriangle].i[1] = data[edge].i[0];
				outTris[currentTriangle].i[2] = i0;
				++currentTriangle;
			}
			else if (data[edge].i[1] != i0)
			{
				outTris[currentTriangle].i[0] = i1;
				outTris[currentTriangle].i[1] = i0;
				outTris[currentTriangle].i[2] = data[edge].i[1];
				++currentTriangle;
			}
			else if (data[edge].i[0] != i1)
			{
				outTris[currentTriangle].i[0] = i1;
				outTris[currentTriangle].i[1] = i0;
				outTris[currentTriangle].i[2] = data[edge].i[0];
				++currentTriangle;
			}
		}
	}

	Vertex vlight(lpos);
	for (unsigned int i = 0, n = indexSize; i < n; ++i)
	{
		outVertices[i].pos = vertices[indexRemap[i]];

		Vertex ofs = (vertices[indexRemap[i]]-vlight).normalize();
		float angle = ofs.dot(normals[i]);

		if (angle < 0)
			outVertices[i].pos = outVertices[i].pos  + ofs * infinity;

	}


	numOutVertices = indexSize;
	numOutTris = currentTriangle;

	return true;
}




namespace engine
{
namespace scene
{



unsigned char polynominals[256] =
{
	255, 254, 252, 251, 250, 248, 240, 245, 246, 238, 237, 244, 7, 189,
	214, 236, 235, 20, 33, 8, 227, 14, 233, 178, 172, 60, 229, 133, 152,
	19, 210, 203, 221, 208, 76, 18, 13, 199, 113, 62, 40, 190, 213, 194,
	43, 181, 21, 15, 201, 162, 90, 186, 71, 117, 107, 70, 191, 5, 173, 44,
	39, 12, 174, 183, 99, 11, 176, 163, 161, 72, 86, 105, 2, 83, 42, 52,
	179, 135, 103, 110, 151, 58, 108, 96, 166, 25, 115, 66, 142, 10, 141,
	48, 104, 34, 159, 120, 22, 140, 64, 82, 78, 68, 207, 125, 123, 150,
	144, 138, 128, 139, 136, 114, 119, 53, 148, 185, 41, 124, 216, 143,
	49, 92, 98, 51, 112, 73, 50, 63, 16, 46, 158, 126, 206, 122, 94, 132,
	88, 184, 28, 84, 127, 156, 167, 223, 118, 89, 116, 17, 111, 121, 109,
	77, 146, 61, 224, 101, 81, 218, 97, 188, 243, 155, 57, 102, 54, 129,
	93, 192, 153, 106, 36, 145, 79, 31, 137, 26, 67, 85, 175, 80, 168, 65,
	91, 1, 147, 149, 6, 29, 37, 69, 182, 165, 4, 74, 55, 47, 171, 169, 75,
	134, 193, 195, 198, 131, 38, 180, 56, 196, 23, 154, 177, 200, 205, 27,
	209, 95, 204, 160, 3, 30, 157, 32, 9, 212, 211, 45, 202, 170, 0, 219,
	187, 87, 35, 100, 217, 232, 164, 228, 220, 197, 231, 215, 226, 130,
	225, 234, 241, 239, 59, 230, 247, 24, 249, 242, 222, 253
};

unsigned int computeHash( unsigned int seed, const unsigned char* buffer, unsigned int length )
{
	unsigned int checksum = seed;

	//assert( buffer );

	for( unsigned int i = 0; i < length; i++ )
		checksum = ((checksum<<8)|((buffer[i]+i)&255)) ^ ((int)polynominals[ checksum >> 24]);

	return checksum;
}

//! constructor
CShadowVolumeSceneNode::CShadowVolumeSceneNode(ISceneNode* parent,
                                               ISceneManager* mgr,
                                               s32 id,
                                               bool zfailmethod,
                                               f32 infinity, bool removeh, bool onlyextrude, scene::IMesh *pre)
	: IShadowVolumeSceneNode(parent, mgr, id), Edges(0), EdgeCount(0),
	ShadowVolumesUsed(0), Indices(0), Vertices(0), IndexCount(0),
	VertexCount(0), IndexCountAllocated(0),
	VertexCountAllocated(0), UseZFailMethod(zfailmethod),
	Adjacency(0), FaceData(0), Infinity(infinity), removeHardVertices(removeh), extrudeOnly(onlyextrude), prebuiltShadow(pre)
{
	#ifdef _DEBUG
	setDebugName("CShadowVolumeSceneNode");
	#endif

	mem = ((video::CSoftwareDriver2*)SceneManager->getVideoDriver())->getMemoryBuffer();
	customMatrix.makeIdentity();

	AutomaticCullingEnabled = false;

}

//! destructor
CShadowVolumeSceneNode::~CShadowVolumeSceneNode()
{
	for (u32 i=0; i<ShadowVolumes.size(); ++i)
	{

		free(ShadowVolumes[i].vertices);
		free(ShadowVolumes[i].indices);


	}
	delete [] Vertices;
	delete [] Indices;
}


void CShadowVolumeSceneNode::createShadowVolume(const core::vector3df& light)
{
	SShadowVolume* svp = 0;

	// builds the shadow volume and adds it to the shadow volume list.

	if (ShadowVolumes.size() > (u32)ShadowVolumesUsed)
	{
		// get the next unused buffer
		svp = &ShadowVolumes[ShadowVolumesUsed];
		if (svp->size >= IndexCount*6 && svp->count >= IndexCount * 3)
		{

		}
		else
		{
			svp->size = IndexCount*6;
			svp->count = IndexCount*3;

			free(svp->vertices);
			free(svp->indices);

			svp->vertices = (ShadowVertex*)memalign(16, svp->size * sizeof(ShadowVertex));
			svp->indices = (Triangle*)memalign(16, svp->count * sizeof(Triangle));

			svp->mapped = true;
		}

		++ShadowVolumesUsed;
	}
	else
	{
		// add a buffer
		SShadowVolume tmp;
		ShadowVolumes.push_back(tmp);
		svp = &ShadowVolumes[ShadowVolumes.size()-1];
		++ShadowVolumesUsed;

		// wir machen mal einen ziemlich großen shadowbuffer
		svp->size = IndexCount*6;
		svp->count = IndexCount*3;


		svp->vertices = (ShadowVertex*)memalign(16, svp->size * sizeof(ShadowVertex));
		svp->indices = (Triangle*)memalign(16, svp->count * sizeof(Triangle));

		svp->mapped = true;
	}



	if (removeHardVertices == true)
	{



		u16 *s = new u16[VertexCount];

#if 1

		typedef std::map<u32,u16> ConData;
		ConData tdatas;

		for (int x = 0; x < VertexCount; x++)
		{

			s[x] = 65535;

			unsigned int current_hash = computeHash(0, (unsigned char*)&Vertices[x], sizeof(core::vector3df));
			ConData::iterator j = tdatas.find(current_hash);

			if (j == tdatas.end())
			{

				tdatas[current_hash] = x;

			} else {

				s[x] = tdatas[current_hash];

			}
		}
#else
		core::array<core::vector3df> tdatas;
		core::array<u16> idatas;
		for (int x = 0; x < VertexCount; x++)
		{

			s[x] = 65535;
			int found;

			found = -1;
			for (int y = 0; y < tdatas.size(); y++)
			{
				#define EPS 0.01f
				#define comp_f(a,b) (a - EPS <= b && a + EPS >= b) || (b - EPS <= a && b + EPS >= a))
				if (comp_f(tdatas[y].X, Vertices[x].X) &&
				    comp_f(tdatas[y].Y, Vertices[x].Y) &&
				    comp_f(tdatas[y].Z, Vertices[x].Z))
				{ found = y; break; }
			}

			if (found != -1)
				s[x] = idatas[found];
			else
			{
				idatas.push_back ( x );
				tdatas.push_back ( Vertices[x] ) ;
			}


		}


#endif




		for (int x = 0; x < IndexCount; x++)
			if (s[Indices[x]] != 65535)
				Indices[x] = s[Indices[x]];

		delete s;
	}

	buildShadowVolume((Vertex*)Vertices, VertexCount, (Triangle*)Indices, IndexCount/3, svp->vertices, svp->vtxcount, svp->indices, svp->triscount, light, Infinity, mem);
}


void CShadowVolumeSceneNode::setCustomModelMatrix(core::matrix4& mat) {
	customMatrix = mat;
}

void CShadowVolumeSceneNode::renderFromPrebuilt(){


	// create as much shadow volumes as there are lights but
	// do not ignore the max light settings.

	IndexCount = prebuiltShadow->getMeshBuffer(0)->getIndexCount();
	VertexCount = prebuiltShadow->getMeshBuffer(0)->getVertexCount();

	s32 lights = SceneManager->getVideoDriver()->getDynamicLightCount();
	s32 i;
	core::matrix4 mat = Parent->getAbsoluteTransformation();

	core::matrix4 identity;
	core::vector3df parentpos = mat.getTranslation();

	core::vector3df lpos;
	mat.makeInverse();

	for (i=0; i<lights; ++i)
	{
		const video::SLight& dl = SceneManager->getVideoDriver()->getDynamicLight(i);
		lpos = dl.Position;

		if (dl.CastShadows &&
		    fabs((lpos - parentpos).getLengthSQ()) <= (dl.Radius*dl.Radius*4.0f))
		{
			mat.transformVect(lpos);
			constructPrebuiltMesh(lpos);
		}
	}


}

void CShadowVolumeSceneNode::constructPrebuiltMesh(core::vector3df light)
{
	SShadowVolume* svp = 0;


	// builds the shadow volume and adds it to the shadow volume list.

	if (ShadowVolumes.size() > (u32)ShadowVolumesUsed)
	{
		// get the next unused buffer
		svp = &ShadowVolumes[ShadowVolumesUsed];
		if (svp->size >= VertexCount && svp->count >= IndexCount)
		{

		}
		else
		{
			svp->size = VertexCount;
			svp->count = IndexCount;

			free( svp->vertices );
			free( svp->indices );

			svp->vertices = (ShadowVertex*)memalign(16, svp->size * sizeof(ShadowVertex));
			svp->indices = (Triangle*)memalign(16, svp->count * 2 );

		}

		++ShadowVolumesUsed;
	}
	else
	{
		// add a buffer
		SShadowVolume tmp;
		ShadowVolumes.push_back(tmp);
		svp = &ShadowVolumes[ShadowVolumes.size()-1];
		++ShadowVolumesUsed;

		// wir machen mal einen ziemlich großen shadowbuffer
		svp->size = VertexCount;
		svp->count = IndexCount;

		svp->vertices = (ShadowVertex*)memalign(16, svp->size * sizeof(ShadowVertex));
		svp->indices = (Triangle*)memalign(16, svp->count * 2 );


	}
	memcpy(svp->indices,        prebuiltShadow->getMeshBuffer(0)->getIndices(), 2 * svp->count);

	video::S3DVertex *vt = (video::S3DVertex*)prebuiltShadow->getMeshBuffer(0)->getVertices();
	core::vector3df *vtx = (core::vector3df*)svp->vertices;

	for (int x = 0; x < VertexCount; x++)
	{

		vtx[x] = vt[x].Pos;
		core::vector3df ofs = (vtx[x]-light).normalize();
		float angle = ofs.dotProduct(vt[x].Normal);

		if (angle < 0)
			vtx[x] = vtx[x]  + ofs * Infinity;


	}

	svp->triscount = svp->count / 3;


}

//! sets the mesh from which the shadow volume should be generated.
void CShadowVolumeSceneNode::setMeshToRenderFrom(IMesh* mesh)
{
	ShadowVolumesUsed = 0;
	if (extrudeOnly == true) {  renderFromPrebuilt(); return; }



	s32 oldIndexCount = IndexCount;
	s32 oldVertexCount = VertexCount;

	VertexCount = 0;
	IndexCount = 0;

	if (!mesh)
		return;

	// calculate total amount of vertices and indices

	s32 i;
	s32 totalVertices = 0;
	s32 totalIndices = 0;
	s32 bufcnt = mesh->getMeshBufferCount();
	IMeshBuffer* b;

	for (i=0; i<bufcnt; ++i)
	{
		b = mesh->getMeshBuffer(i);
		totalIndices += b->getIndexCount();
		totalVertices += b->getVertexCount();
	}

	// allocate memory if nececcary

	if (totalVertices > VertexCountAllocated)
	{
		delete [] Vertices;
		Vertices = new core::vector3df[totalVertices];
		VertexCountAllocated = totalVertices;
	}

	if (totalIndices > IndexCountAllocated)
	{
		delete [] Indices;
		Indices = new u16[totalIndices];
		IndexCountAllocated = totalIndices;
	}

	// copy mesh

	for (i=0; i<bufcnt; ++i)
	{
		b = mesh->getMeshBuffer(i);

		s32 idxcnt = b->getIndexCount();
		s32 vtxnow = VertexCount;

		const u16* idxp = b->getIndices();
		const u16* idxpend = idxp + idxcnt;

		for (; idxp!=idxpend; ++idxp)
			Indices[IndexCount++] = *idxp + vtxnow;

		s32 vtxcnt = b->getVertexCount();

		switch(b->getVertexType())
		{
		case video::EVT_STANDARD:
		{
			const video::S3DVertex* vp = (video::S3DVertex*)b->getVertices();
			const video::S3DVertex* vpend = vp + vtxcnt;

			for (; vp!=vpend; ++vp)
				Vertices[VertexCount++] = (*vp).Pos;
		}
			break;
		case video::EVT_2TCOORDS:
		{
			const video::S3DVertex2TCoords* vp = (video::S3DVertex2TCoords*)b->getVertices();
			const video::S3DVertex2TCoords* vpend = vp + vtxcnt;

			for (; vp!=vpend; ++vp)
				Vertices[VertexCount++] = (*vp).Pos;
		}
			break;
		}
	}


	// create as much shadow volumes as there are lights but
	// do not ignore the max light settings.

	s32 lights = SceneManager->getVideoDriver()->getDynamicLightCount();

	core::matrix4 mat = Parent->getAbsoluteTransformation();
	core::matrix4 identity;

	if (identity != customMatrix)
		for (int x = 0; x < VertexCount; x++)
		{
			customMatrix.transformVect ( Vertices[x] );
		}
	core::vector3df parentpos = mat.getTranslation();

	core::vector3df lpos;
	mat.makeInverse();

	for (i=0; i<lights; ++i)
	{
		const video::SLight& dl = SceneManager->getVideoDriver()->getDynamicLight(i);
		lpos = dl.Position;

		if (dl.CastShadows &&
		    fabs((lpos - parentpos).getLengthSQ()) <= (dl.Radius*dl.Radius*4.0f))
		{
			mat.transformVect(lpos);
			createShadowVolume(lpos);
		}
	}
}



//! pre render method
void CShadowVolumeSceneNode::OnPreRender()
{

	if (IsVisible && Parent->isVisible() == true)
		SceneManager->registerNodeForRendering(this, scene::ESNRP_SHADOW);
}




//! renders the node.
void CShadowVolumeSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	if (!ShadowVolumesUsed || !driver)
		return;

	core::matrix4 mat = Parent->getAbsoluteTransformation() /* * customMatrix*/;


	driver->setTransform(video::ETS_WORLD, mat  );

	driver->drawStencilShadowVolume(0, 0, true);
	__init_shadow_draw();

	for (s32 i=0; i<ShadowVolumesUsed; ++i)
	{
		((video::CSoftwareDriver2*)driver)->drawClippedShadow(ShadowVolumes[i].triscount, (core::vector3df*)ShadowVolumes[i].vertices, (u16*)ShadowVolumes[i].indices);
	}
	glPopAttrib();
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CShadowVolumeSceneNode::getBoundingBox() const
{
	return Box;
}



//! returns the material based on the zero based index i.
video::SMaterial& CShadowVolumeSceneNode::getMaterial(s32 i)
{
	// this should never be called, because a shadow volume has got no
	// material
	return *((video::SMaterial*)(0));

}



//! returns amount of materials used by this scene node.
s32 CShadowVolumeSceneNode::getMaterialCount()
{
	return 0;
}

} // end namespace scene
} // end namespace engine
