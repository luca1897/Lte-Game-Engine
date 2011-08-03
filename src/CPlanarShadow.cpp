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

#include "CAnimatedMeshSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "S3DVertex.h"
#include "os.h"
#include "CShadowVolumeSceneNode.h"
#include "SViewFrustrum.h"
#include "ICameraSceneNode.h"
#include "IAnimatedMeshMS3D.h"
#include "IAnimatedMeshX.h"
#include "IDummyTransformationSceneNode.h"
#include "CCelshadingSceneNode.h"
#include "IMaterialRenderer.h"
#include "CPlanarShadow.h"

#include "CSoftwareDriver2.h"

#include <pspkernel.h>
#define GL_GLEXT_PROTOTYPES

#include <pspdebug.h>



#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include "pspgl_internal.h"

/* Vertex Declarations Begin */
#define GE_INT_8BIT             1
#define GE_INT_16BIT            2
#define GE_FLOAT_32BIT          3

#define GE_TEXTURE_SHIFT(n)     ((n)<<0)
#define GE_TEXTURE_8BIT         GE_TEXTURE_SHIFT(GE_INT_8BIT)
#define GE_TEXTURE_16BIT        GE_TEXTURE_SHIFT(GE_INT_16BIT)
#define GE_TEXTURE_32BITF       GE_TEXTURE_SHIFT(GE_FLOAT_32BIT)

#define GE_COLOR_SHIFT(n)       ((n)<<2)
#define GE_COLOR_RES1           GE_COLOR_SHIFT(1)
#define GE_COLOR_RES2           GE_COLOR_SHIFT(2)
#define GE_COLOR_RES3           GE_COLOR_SHIFT(3)
#define GE_COLOR_5650           GE_COLOR_SHIFT(4)
#define GE_COLOR_5551           GE_COLOR_SHIFT(5)
#define GE_COLOR_4444           GE_COLOR_SHIFT(6)
#define GE_COLOR_8888           GE_COLOR_SHIFT(7)

#define GE_NORMAL_SHIFT(n)      ((n)<<5)
#define GE_NORMAL_8BIT          GE_NORMAL_SHIFT(GE_INT_8BIT)
#define GE_NORMAL_16BIT         GE_NORMAL_SHIFT(GE_INT_16BIT)
#define GE_NORMAL_32BITF        GE_NORMAL_SHIFT(GE_FLOAT_32BIT)

#define GE_VERTEX_SHIFT(n)      ((n)<<7)
#define GE_VERTEX_8BIT          GE_VERTEX_SHIFT(GE_INT_8BIT)
#define GE_VERTEX_16BIT         GE_VERTEX_SHIFT(GE_INT_16BIT)
#define GE_VERTEX_32BITF        GE_VERTEX_SHIFT(GE_FLOAT_32BIT)

#define GE_WEIGHT_SHIFT(n)      ((n)<<9)
#define GE_WEIGHT_8BIT          GE_WEIGHT_SHIFT(GE_INT_8BIT)
#define GE_WEIGHT_16BIT         GE_WEIGHT_SHIFT(GE_INT_16BIT)
#define GE_WEIGHT_32BITF        GE_WEIGHT_SHIFT(GE_FLOAT_32BIT)

#define GE_VINDEX_SHIFT(n)      ((n)<<11)
#define GE_VINDEX_8BIT          GE_VINDEX_SHIFT(GE_INT_8BIT)
#define GE_VINDEX_16BIT         GE_VINDEX_SHIFT(GE_INT_16BIT)
#define GE_VINDEX_RES3          GE_VINDEX_SHIFT(GE_FLOAT_32BIT)

#define GE_WEIGHTS(n)           ((((n)-1)&7)<<14)
#define GE_VERTICES(n)          ((((n)-1)&7)<<18)

#define GE_TRANSFORM_SHIFT(n)   ((n)<<23)
#define GE_TRANSFORM_3D         GE_TRANSFORM_SHIFT(0)
#define GE_TRANSFORM_2D         GE_TRANSFORM_SHIFT(1)

#define GE_TRIANGLES            (3)

extern "C"
{
	void glDrawBezierArraysPSP(GLenum mode, GLuint u, GLuint v, GLint first);
	void glDrawBezierElementsPSP(GLenum mode, GLuint u, GLuint v,
	                             GLenum idx_type, const GLvoid *indices);
	void glDrawBezierRangeElementsPSP(GLenum mode,
	                                  GLuint start, GLuint end,
	                                  GLuint u, GLuint v,
	                                  GLenum idx_type, const GLvoid *indices);

	void glDrawSplineArraysPSP(GLenum mode, GLuint u, GLuint v,
	                           GLenum uflags, GLenum vflags,
	                           GLint first);
	void glDrawSplineElementsPSP(GLenum mode, GLuint u, GLuint v,
	                             GLenum uflags, GLenum vflags,
	                             GLenum idx_type, const GLvoid *indices);
	void glDrawSplineRangeElementsPSP(GLenum mode,
	                                  GLuint start, GLuint end,
	                                  GLuint u, GLuint v,
	                                  GLenum uflags, GLenum vflags,
	                                  GLenum idx_type, const GLvoid *indices);

	void glPatchSubdivisionPSP(GLuint u, GLuint v);

	void glBindBuffer (GLenum, GLuint);
	void glDeleteBuffers (GLsizei, const GLuint *);
	void glGenBuffers (GLsizei, GLuint *);
	GLboolean glIsBuffer (GLuint);
	void glBufferData (GLenum, GLsizeiptr, const GLvoid *, GLenum);
	void glBufferSubData (GLenum, GLintptr, GLsizeiptr, const GLvoid *);
	void glGetBufferSubData (GLenum, GLintptr, GLsizeiptr, GLvoid *);
	GLvoid*  glMapBuffer (GLenum, GLenum);
	GLboolean glUnmapBuffer (GLenum);
	void glBufferDataARB(GLenum target, GLsizeiptr size,
	                     const GLvoid *init_data, GLenum usage);

	GLboolean glUnmapBufferARB(GLenum target);
	GLvoid*  glMapBufferARB (GLenum, GLenum);
	void __pspgl_context_render_prim(struct pspgl_context *c,
	                                 unsigned prim, unsigned count, unsigned vtxfmt,
	                                 const void *vertex, const void *index);




}

namespace engine
{
namespace scene
{

CPlanarShadow::~CPlanarShadow()
{

	if(VertexCount)
		delete Vertices;

	if(IndexCount)
		delete Indices;

}

CPlanarShadow::CPlanarShadow(scene::ISceneNode*parent, scene::ISceneManager *mgr, IMesh *mesh, float divisor)
	: ISceneNode(parent, mgr, -1), Vertices(0), Indices(0), SceneManager(mgr), Divisor(divisor), enableDepth(false)
{

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



	Vertices = new core::vector3df[totalVertices];
	Indices = new u16[totalIndices];

	int indStart = 0, vtxNow = 0;

	for (i=0; i < bufcnt; ++i)
	{
		b = mesh->getMeshBuffer(i);

		u16* buff = b->getIndices();

		for (int o = 0; o < b->getIndexCount(); o++)
		{
			Indices[indStart++] = buff[o] + vtxNow;
		}

		vtxNow += b->getVertexCount();

	}

	VertexCount = totalVertices;
	IndexCount = totalIndices;



	buildMesh(mesh);
}

void CPlanarShadow::setDepth(bool DepthTest)
{
	enableDepth=DepthTest;
}
void CPlanarShadow::setPlane(core::plane3df plane)
{
	Plane = plane;
}
void CPlanarShadow::setAnimated(bool animated)
{
	Animated = animated;
}
void CPlanarShadow::setMeshToRenderFrom(IMesh* mesh)
{
	if (Animated == false)
		return;

	buildMesh(mesh);

}
void CPlanarShadow::buildMesh(IMesh*mesh)
{

	s32 i;
	s32 totalVertices = 0;
	s32 totalIndices = 0;
	s32 bufcnt = mesh->getMeshBufferCount();
	IMeshBuffer* b;

	int vtxNow = 0;

	for (i=0; i<bufcnt; ++i)
	{
		IMeshBuffer* b;
		b = mesh->getMeshBuffer(i);

		video::S3DVertex* vtxbuff = (video::S3DVertex*)b->getVertices();

		for(int o = 0; o < b->getVertexCount(); o++)
			Vertices[vtxNow++] = vtxbuff[o].Pos;
	}

}
void CPlanarShadow::setTransform(core::matrix4& transform)
{
	Transform = transform;
	View=SceneManager->getVideoDriver()->getTransform(video::ETS_VIEW);
}

void CPlanarShadow::OnPreRender()
{
	//	if (IsVisible && Parent->isVisible() == true)
	// 	 SceneManager->registerNodeForRendering(this, scene::ESNRP_PLANARSHADOW);
}

const core::aabbox3d<f32>& CPlanarShadow::getBoundingBox() const
{
	return Box;
}

video::SMaterial& CPlanarShadow::getMaterial(s32 i)
{
	return *((video::SMaterial*)(0));
}

s32 CPlanarShadow::getMaterialCount()
{
	return 0;
}

void CPlanarShadow::render()
{

	video::IVideoDriver* driver = SceneManager->getVideoDriver();


	int i;
	core::vector3df lpos;


	s32 lights = SceneManager->getVideoDriver()->getDynamicLightCount();
	for (i=0; i<lights; ++i)
	{
		const video::SLight& dl = SceneManager->getVideoDriver()->getDynamicLight(i);
		lpos = dl.Position;

		if (dl.CastShadows &&
		    fabs((lpos - Transform.getTranslation()).getLengthSQ()) <= (dl.Radius*dl.Radius*4.0f))
		{

			core::matrix4 mat;


			mat.buildShadowMatrix(dl.Position, Plane, 0.f );
			mat *= Transform;

			core::matrix4 backup_view = driver->getTransform(video::ETS_VIEW);


			mat = View*mat;
			for (int x = 0; x<16; x++)
				mat.M[x]/=Divisor;
			driver->setTransform(video::ETS_VIEW, core::matrix4());
			driver->setTransform(video::ETS_WORLD, mat);


			glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT |
			             GL_POLYGON_BIT   | GL_STENCIL_BUFFER_BIT   );
			glEnable(GL_STENCIL_TEST);
			glDisable(GL_ALPHA_TEST);
			glDisable(GL_TEXTURE_2D);

			if (enableDepth == false)
				glDisable (GL_DEPTH_TEST );
			else
			{
				glEnable( GL_DEPTH_TEST );
				glDepthFunc( GL_LEQUAL );
			}
			glStencilOp(GL_INCR, GL_INCR, GL_INCR);
			glStencilFunc(GL_EQUAL, 128, 0xFF);

			video::SColor pow = SceneManager->getShadowColor();

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,
			            GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_LIGHTING);

			glColor4f(pow.getRed()/255.f, pow.getGreen()/255.f, pow.getBlue()/255.f, pow.getAlpha()/255.f);
			sceKernelDcacheWritebackAll();

			__pspgl_context_render_prim(pspgl_curctx, GE_TRIANGLES, IndexCount, GE_VERTEX_32BITF|GE_VINDEX_16BIT|GE_TRANSFORM_3D,
			                            Vertices, Indices);
			glFlush();

			if (enableDepth == false)
				glEnable (GL_DEPTH_TEST );
			else
			{
				glEnable( GL_DEPTH_TEST );
				glDepthFunc( GL_LEQUAL );
			}
			glDisable(GL_STENCIL_TEST);

			glPopAttrib();

			driver->setTransform(video::ETS_VIEW, backup_view);

		}
	}

}

}
}
