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
 

#include "ISceneManager.h"
#include "IVideoDriver.h"
#include "CCelshadingSceneNode.h"

#include <stdio.h>
#include <pspdebug.h>
#include <pspkernel.h>
#define printf pspDebugScreenPrintf
#include <matrix4.h>
#include "ICameraSceneNode.h"
#include "Frustum.h"
#include <stdlib.h>
#include <string.h>

#include "CImage.h"
#include "ITexture.h"
#include "SColor.h"
#include "CSoftwareDriver2.h"

namespace engine
{
namespace scene
{


float __default_shader[] =
{
	0.2f,
	0.2f,
	0.2f,
	0.2f,
	0.5f,
	0.5,
	0.5,
	0.5,
	0.5,
	0.5,
	0.5,
	0.5,
	0.5,
	0.5,
	1.0,
	1.0,
	1.0,
	1.0,
	1.0,
	1.0,
	1.0,
	1.0,
	1.0,
	1.0,
	1.0,
	1.0,
	1.0,
	1.0,
	1.0,
	1.0,
	1.0,
	1.0

};



//! constructor
CCelshadingSceneNode::CCelshadingSceneNode(ISceneNode* parent,
                                           ISceneManager* mgr,
                                           s32 id, float *shader, int width, bool recalc_normals)
	: ICelshadingSceneNode(parent, mgr, id),
	CelshadingCount(0), recalcnormal(recalc_normals)
{
	#ifdef _DEBUG
	setDebugName("CCelshadingSceneNode");
	#endif


	float *new_shader = shader;
	if (new_shader == 0) new_shader = __default_shader;

	video::IVideoDriver *drv = SceneManager->getVideoDriver();


	//disable mip-mapping
	bool generateMipLevels = drv->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);

	if (generateMipLevels)
		drv->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

	tex = drv->addTexture(core::dimension2d<s32>(width,1), "shadetex", video::ECF_A8R8G8B8);

	if (!tex)
		printf("CCelshadingSceneNode.cpp: Error on creating texture\n");

	if (tex)
		tex->grab();



	video::SColor c;

	s32 *t = (s32*)tex->lock();
	for (int x = 0; x < width; x++)
	{  float o = new_shader[x];
	   c.set(255, (s32)(255.f * o), (s32)(255.f * o), (s32)(255.f * o));
	   t[x] = c.color;}
	tex->unlock();

	//restore mip-mapping
	if (generateMipLevels)
		drv->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

}



//! destructor
CCelshadingSceneNode::~CCelshadingSceneNode()
{
	for (u32 i=0; i<SCelshadingDatas.size(); ++i)
	{
		glDeleteBuffersARB(3, SCelshadingDatas[i].vbo);

	}
	tex->drop();
}


void CCelshadingSceneNode::createCelshadingData(const core::vector3df& pos, video::S3DVertex* vtx, const u16* idx,s32 idxcount,s32 vtxcount, bool lightdir, bool lighttype)
{
	SCelshadingData* svp = 0;

	// builds the shadow volume and adds it to the shadow volume list.

	if (SCelshadingDatas.size() > (u32)CelshadingCount)
	{
		// get the next unused buffer
		svp = &SCelshadingDatas[CelshadingCount];
		if (svp->size >= vtxcount)
			svp->count = 0;
		else
		{
			svp->size = vtxcount;
			svp->count = 0;

			if (svp->mapped == true)
			{
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, svp->vbo[0]);
				glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);

				if (svp->vertices_pos) {
					glBindBufferARB(GL_ARRAY_BUFFER_ARB, svp->vbo[1]);
					glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
				}
			}

			glBindBufferARB(GL_ARRAY_BUFFER_ARB, svp->vbo[0]);
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, svp->size * sizeof(core::vector3df), 0, GL_STATIC_DRAW_ARB);
			svp->vertices = (core::vector3df*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);

			if (lightdir) {
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, svp->vbo[1]);
				glBufferDataARB(GL_ARRAY_BUFFER_ARB, svp->size * sizeof(__verttex), 0, GL_STATIC_DRAW_ARB);
				svp->vertices_pos = (__verttex*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
			}
			else svp->vertices_pos = 0;
			svp->mapped = true;
		}

		++CelshadingCount;
	}
	else
	{
		// add a buffer
		SCelshadingData tmp;
		SCelshadingDatas.push_back(tmp);
		svp = &SCelshadingDatas[SCelshadingDatas.size()-1];
		++CelshadingCount;

		// wir machen mal einen ziemlich großen shadowbuffer
		svp->size = vtxcount;
		svp->count = 0;
		//svp->vertices = new core::vector3df[svp->size];
		glGenBuffersARB(3, svp->vbo);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, svp->vbo[0]);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, svp->size * sizeof(core::vector3df), 0, GL_STATIC_DRAW_ARB);
		svp->vertices = (core::vector3df*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);


		if  (lightdir) {
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, svp->vbo[1]);
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, svp->size * sizeof(__verttex), 0, GL_STATIC_DRAW_ARB);
			svp->vertices_pos = (__verttex*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
		}
		else svp->vertices_pos = 0;
		svp->mapped = true;
	}
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, svp->vbo[2]);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(u16) * idxcount, idx, GL_STATIC_DRAW_ARB);

	video::IVideoDriver *driver = SceneManager->getVideoDriver();
	driver->setTransform(video::ETS_WORLD, Parent->getAbsoluteTransformation());

  #define d_clampf(x) x < 0.f ? 0.1f : x > 1.0f ? 0.9f : x

#if 1
	if (recalcnormal == true)
	{

		s32 i;

		int *vtx_count  = new int[vtxcount];

		for ( i = 0; i < vtxcount; i++)
		{
			vtx[i].Normal = core::vector3df(0,0,0);
			vtx_count[i] = 0;

		}
		for ( i=0; i<idxcount; i+=3)
		{
			core::vector3df a, b;
			a = vtx[idx[i+1]].Pos - vtx[idx[i+0]].Pos;
			b = vtx[idx[i+2]].Pos - vtx[idx[i+0]].Pos;

			core::vector3df c;
			c = a.crossProduct(  b );
			c.normalize();

			vtx[idx[i+0]].Normal += c;
			vtx[idx[i+1]].Normal += c;
			vtx[idx[i+2]].Normal += c;

			vtx_count[idx[i+0]]++;
			vtx_count[idx[i+1]]++;
			vtx_count[idx[i+2]]++;

		}

		for ( i = 0; i < vtxcount; i++)
			if (vtx_count[i])
				vtx[i].Normal /= vtx_count[i];

		delete vtx_count;

	}
#endif

	for (int x = 0; x < vtxcount; x++)
	{

		svp->vertices[x] = vtx[x].Pos + vtx[x].Normal * 0.12f;


		if (lightdir) {
			svp->vertices_pos[x].Pos = vtx[x].Pos;
			svp->vertices_pos[x].TCoords.Y = 0.f;

			// computazione cel-shading
			core::vector3df ldir;
			if (lighttype == false)
				ldir = pos - vtx[x].Pos;
			else
				ldir = pos;

			ldir.normalize();


			core::vector3df cf = vtx[x].Normal;

			float coord = d_clampf( ldir.dotProduct( cf ) );
			svp->vertices_pos[x].TCoords.X = coord;
		}
	}


	svp->count = idxcount;

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

//! sets the mesh from which the shadow volume should be generated.
void CCelshadingSceneNode::setMeshToRenderFrom(IMesh* mesh)
{

	CelshadingCount = 0;

	if (!mesh)
		return;

	// calculate total amount of vertices and indices
	s32 bufcnt = mesh->getMeshBufferCount();
	IMeshBuffer* b;
	int i;
	// copy mesh
	s32 lights = SceneManager->getVideoDriver()->getDynamicLightCount();
	core::matrix4 mat = Parent->getAbsoluteTransformation();
	core::vector3df parentpos = Parent->getAbsolutePosition();

	core::vector3df lpos;
	mat.makeInverse();
	for (i=0; i<bufcnt; ++i)
	{
		b = mesh->getMeshBuffer(i);
		s32 idxcnt = b->getIndexCount();

		const u16* idxp = b->getIndices();
		video::S3DVertex* vp = (video::S3DVertex*)b->getVertices();

		int processed = 0;


		for (i=0; i<lights; ++i)
		{
			const video::SLight& dl = SceneManager->getVideoDriver()->getDynamicLight(i);
			lpos = dl.Position;

			//if (
			//fabs((lpos - parentpos).getLengthSQ()) <= (dl.Radius*dl.Radius*4.0f))
			//{
			processed++;
			createCelshadingData(dl.Position, vp, idxp, idxcnt, b->getVertexCount(), true, dl.Type == video::ELT_DIRECTIONAL ? true : false);
			//}
		}

		if (processed == 0)
		{
			createCelshadingData(core::vector3df(0,0,0), vp, idxp, idxcnt, b->getVertexCount(), false, false);
		}


	}






}



//! pre render method
void CCelshadingSceneNode::OnPreRender()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(this, scene::ESNRP_CELSHADING);
}

//! renders the node.
void CCelshadingSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	if (!CelshadingCount || !driver)
		return;

	driver->setTransform(video::ETS_WORLD, Parent->getAbsoluteTransformation());

	for (s32 i=0; i<CelshadingCount; ++i)
	{

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, SCelshadingDatas[i].vbo[2]);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, SCelshadingDatas[i].vbo[0]);
		if (SCelshadingDatas[i].mapped == true)
			glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);

		((video::CSoftwareDriver2*)driver)->drawCelshadingOutline(0,0,SCelshadingDatas[i].count);

		if (SCelshadingDatas[i].vertices_pos) {
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, SCelshadingDatas[i].vbo[1]);
			if (SCelshadingDatas[i].mapped == true)
			{
				glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
				SCelshadingDatas[i].mapped = false;
			}


			((video::CSoftwareDriver2*)driver)->setTexture(0, tex);

			((video::CSoftwareDriver2*)driver)->drawCelshadingMultitexture(0,0,SCelshadingDatas[i].count);
		}  else {
			SCelshadingDatas[i].mapped = false;
		}
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	}

}



//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CCelshadingSceneNode::getBoundingBox() const
{
	return Parent->getBoundingBox();
}



//! returns the material based on the zero based index i.
video::SMaterial& CCelshadingSceneNode::getMaterial(s32 i)
{
	// this should never be called, because a shadow volume has got no
	// material
	return *((video::SMaterial*)(0));

}



//! returns amount of materials used by this scene node.
s32 CCelshadingSceneNode::getMaterialCount()
{
	return 0;
}

} // end namespace scene
} // end namespace engine
