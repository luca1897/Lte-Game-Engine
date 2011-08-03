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
 
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "CSoftwareDriver2.h"
#include "CSoftwareTexture2.h"
#include "CSoftware2MaterialRenderer.h"
#include "S3DVertex.h"
#include "S4DVertex.h"
#include "line3d.h"
#include "COpenGLTexture.h"
#include "COpenGLMaterialRenderer.h"
#include "CengineDeviceStub.h"
#include <stdlib.h>

#include "IAudioDriver.h"

#include "Frustum.h"
#include "CursorData.h"
#include "keys.h"



#include "files/audioIntro.c"
#include "files/logo.h"

#include <sys/time.h>

#include <pspdisplay.h>
#include <pspkernel.h>

#include "ShadInfo.h"
#include "pspgl_internal.h"

#define line3df line3d<f32>


#define GL_GLEXT_PROTOTYPES


#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glext.h>

#include <pspdebug.h>
#define MAX_LATO 64
#define printf pspDebugScreenPrintf
using namespace engine;
using namespace scene;
ViewFrustum myVFrustum;

video::ITexture *cursor;
video::ITexture *t_keys; // texture della keyboard
video::ITexture *t_keys_c;
video::ITexture *t_nums;
video::ITexture *t_nums_c;

video::ITexture *st_keys; // texture della keyboard selezionate
video::ITexture *st_keys_c;
video::ITexture *st_nums;
video::ITexture *st_nums_c;

video::ITexture *selector; // selettore della keyboard



extern int key_state; // disable
extern int key_type; // pad selector
extern int key_sel; //  key selector
extern int key_pos_x; // pad x position
extern int key_pos_y; // pad y position

#include "aabbox3d.h"
bool __using_bezier_patches = false;
int __bezier_mesh_size = 0;

int __lastvblank = 0;

extern core::matrix4 light_matrix;
extern audio::IAudioDriver *__lte_intdrv;


int __lte_current_size_vbo1 = 0;
int __lte_current_size_vbo2 = 0;

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

namespace video
{

core::matrix4 projection, vista;



COpenGLMultipassRender_REFLECTION_2_LAYER pass_r2l;
COpenGLMultipassRender_TRANSPARENT_REFLECTION_2_LAYER pass_tr2l;
COpenGLMultipassRender_SOLID_2_LAYER pass_s2l;
COpenGLMultipassRender* multipass = 0;

ITexture* __second_texture = 0;


// returns the current size of the screen or rendertarget
core::dimension2d<s32> CSoftwareDriver2::getCurrentRenderTargetSize()
{

	if ( CurrentRendertargetSize.Width == 0 )
		return ScreenSize;
	else
		return CurrentRendertargetSize;
}


void CSoftwareDriver2::setMultipassRender(int num)
{
	switch (num) {

	case 1: multipass = &pass_r2l; break;
	case 2: multipass = &pass_tr2l; break;
	case 3: multipass = &pass_s2l; break;
	case 0: default: multipass = 0; break;
	}
}

BspClipVertex *vertexStack;
unsigned short *vertexStackIndex;

//! constructor
CSoftwareDriver2::CSoftwareDriver2(const core::dimension2d<s32>& windowSize, bool fullscreen, io::IFileSystem* io, video::IImagePresenter* presenter, gui::ICursorControl* curctrl, bool showLogo)
	: CNullDriver(io, windowSize), CurrentTriangleRenderer(0),
	ZBuffer(0), RenderTargetTexture(0), RenderTargetSurface(0)

{
	// _DisableFPUExceptions();

	int argc = 0;
	glutInit(&argc, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE  |  GLUT_STENCIL | GLUT_DEPTH);
	glutInitWindowSize(480, 272);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("dummy");

	const char *filename = "#DefaultCursor";


	ScreenSize.Width = 480;
	ScreenSize.Height = 272;
	createMaterialRenderers();




	// set the renderstates
	ResetRenderStates = true;
	setRenderStates3DMode();
	StencilBuffer = true;
	glDepthRange(0.f, 10000.f);

	glShadeModel(GL_SMOOTH);

	io::IReadFile* file = io::createMemoryReadFile(CursorData, CursorDataSize, filename, false);
	cursor = getTexture(file);
	makeColorKeyTexture(cursor, SColor(255,255,0,255));
	file->drop();

	tshowLogo = showLogo;
	CursorControl = curctrl;
	bool iscursvis = CursorControl->isVisible();
	CursorControl->setVisible(false);
	glDepthFunc(GL_LEQUAL);

	if (showLogo && 0) {


   #ifndef __NYX_LOGO__

		file = io::createMemoryReadFile(AudioIntro, size_AudioIntro, "#AudioIntro", false);
		audio::IAudioSound *snd = __lte_intdrv->addSound(file);

   #endif

		file = io::createMemoryReadFile(LogoData1, size_LogoData1, "#DefaultLogo", false);
		video::ITexture *logo = getTexture(file);

		int o = os::Timer::getRealTime();

		int fade = 30;

		while (fade < 255) {

			beginScene(false,false,SColor(0,0,0,0));
			draw2DImage(logo, core::position2d<s32>(0, 0), core::rect<s32>(core::position2d<s32>(0,0),
			                                                               core::position2d<s32>(480,272)),0,SColor(fade,fade,fade,fade),true);
			endScene();

			fade += 5;

		}
		fade = 0;

		while (fade < 110)
		{
			beginScene(false,false,SColor(0,0,0,0));
			draw2DImage(logo, core::position2d<s32>(0, 0), core::rect<s32>(core::position2d<s32>(0,0),
			                                                               core::position2d<s32>(480,272)),0,SColor(255,255,255,255),true);
			endScene();
			fade++;
		}
		fade=255;
		while (fade > 0 ) {

			beginScene(false,false,SColor(0,0,0,0));
			draw2DImage(logo, core::position2d<s32>(0, 0), core::rect<s32>(core::position2d<s32>(0,0),
			                                                               core::position2d<s32>(480,272)),0,SColor(fade,fade,fade,fade),true);
			endScene();

			fade -= 5;

		}

		removeTexture(logo);

   #ifndef __NYX_LOGO__

		file = io::createMemoryReadFile(LogoData2, size_LogoData2, "#DefaultLogo", false);
		logo = getTexture(file);
		snd->play();
		fade = 30;

		while (fade < 255) {

			beginScene(false,false,SColor(0,0,0,0));
			draw2DImage(logo, core::position2d<s32>(0, 0), core::rect<s32>(core::position2d<s32>(0,0),
			                                                               core::position2d<s32>(480,272)),0,SColor(fade,fade,fade,fade),true);
			endScene();

			fade += 5;

		}
		fade = 0;

		while (fade < 110)
		{
			beginScene(false,false,SColor(0,0,0,0));
			draw2DImage(logo, core::position2d<s32>(0, 0), core::rect<s32>(core::position2d<s32>(0,0),
			                                                               core::position2d<s32>(480,272)),0,SColor(255,255,255,255),true);
			endScene();
			fade++;
		}
		fade=255;


		while (fade > 0 ) {


			beginScene(false,false,SColor(0,0,0,0));
			draw2DImage(logo, core::position2d<s32>(0, 0), core::rect<s32>(core::position2d<s32>(0,0),
			                                                               core::position2d<s32>(480,272)),0,SColor(fade,fade,fade,fade),true);
			endScene();

			fade -= 5;

		}
		removeTexture(logo);
		snd->stop();

		__lte_intdrv->removeSound(snd);

   #endif

	} else {
		struct timeval tv;
		gettimeofday(&tv, 0);
		startTime = tv.tv_sec;
	}


	CursorControl->setVisible(iscursvis);

}


void CSoftwareDriver2::drawCelshadingOutline(const core::vector3df* triangles, const u16* indexList, s32 indexCount)
{
	if (!indexCount)
		return;

	// unset last 3d material
	if (CurrentRenderMode == ERM_3D &&
	    Material.MaterialType >= 0 && Material.MaterialType < (s32)MaterialRenderers.size())
	{
		MaterialRenderers[Material.MaterialType]->OnUnsetMaterial();
		ResetRenderStates = true;
	}



	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glFrontFace(GL_CCW);
	glColor4ub(0,0,0,0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(core::vector3df), triangles);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, indexList);
	glFlush();
	glDisableClientState(GL_VERTEX_ARRAY);
	glFrontFace(GL_CW);
	glPopAttrib();
}


void CSoftwareDriver2::drawCelshadingMultitexture(__verttex* triangles, const u16* indexList, s32 indexCount)
{
	if (!indexCount)
		return;

	// unset last 3d material
	if (CurrentRenderMode == ERM_3D &&
	    Material.MaterialType >= 0 && Material.MaterialType < (s32)MaterialRenderers.size())
	{
		MaterialRenderers[Material.MaterialType]->OnUnsetMaterial();
		ResetRenderStates = true;
	}


	glPushAttrib(GL_ENABLE_BIT);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);


	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_LIGHTING);

	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
	glBlendFuncFixPSP(
	        GU_COLOR(0.2f, 0.2f, 0.2f, 0.f),
	        GU_COLOR(0.8f, 0.8f, 0.8f, 0.f));

	glDepthFunc( GL_EQUAL);
	glColor4ub(255,255,255,255);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY );

	glTexCoordPointer(2, GL_FLOAT, sizeof(__verttex), &triangles[0].TCoords);
	glVertexPointer(3, GL_FLOAT, sizeof(__verttex), &triangles[0].Pos);


	//  glDrawArrays(GL_TRIANGLES, 0, indexCount);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, indexList);
	glFlush();

	glDisableClientState(GL_TEXTURE_COORD_ARRAY );
	glDisableClientState(GL_VERTEX_ARRAY);

	glDisable(GL_BLEND);
	glDepthFunc( GL_LEQUAL);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPopAttrib();


}

void CSoftwareDriver2::drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail)
{

	if (!StencilBuffer || !count)
		return;


	// unset last 3d material
	if (CurrentRenderMode == ERM_3D &&
	    Material.MaterialType >= 0 && Material.MaterialType < (s32)MaterialRenderers.size())
	{
		MaterialRenderers[Material.MaterialType]->OnUnsetMaterial();
		ResetRenderStates = true;
	}





}


void CSoftwareDriver2::drawStencilShadow(bool clearStencilBuffer, video::SColor leftUpEdge,
                                         video::SColor rightUpEdge, video::SColor leftDownEdge, video::SColor rightDownEdge)
{
	if (!StencilBuffer)
		return;

	setTexture(0,0);
	setTexture(1,0);





#if 1
	glPushAttrib( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT );

	glDisable( GL_LIGHTING );

	glDepthFunc( GL_LESS );
	glEnable( GL_STENCIL_TEST );

	glFrontFace( GL_CCW );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glStencilFunc(GL_NOTEQUAL, 0, 0xFFFFFFFFL);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	glDisable(GL_FOG);
	glDepthMask(GL_FALSE);

	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 1, 0, 0, 1);
	glDisable(GL_DEPTH_TEST);

	glColor4ub(leftUpEdge.getRed(), leftUpEdge.getGreen(), leftUpEdge.getBlue(), leftUpEdge.getAlpha());
	glBegin(GL_SPRITES_PSP);
	glVertex2i(0, 0);
	glVertex2i(1, 1);
	glEnd();

	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();



	glPopAttrib();

	if (clearStencilBuffer)
		glClear(GL_STENCIL_BUFFER_BIT);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
#endif


}

ITexture* CSoftwareDriver2::createRenderTargetTexture(core::dimension2d<s32> size)
{
	//disable mip-mapping
	bool generateMipLevels = getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);

	if (generateMipLevels)
		setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);

	video::ITexture* rtt = addTexture(size, "rt", ECF_A1R5G5B5);

	if (rtt)
		rtt->grab();

	//restore mip-mapping
	if (generateMipLevels)
		setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, true);

	return rtt;
}

bool CSoftwareDriver2::setRenderTarget(video::ITexture* texture, bool clearBackBuffer,
                                       bool clearZBuffer, SColor color)
{
	if (texture && texture->getDriverType() != EDT_OPENGL)
	{
		os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
		return false;
	}

	// check if we should set the previous RT back

	bool ret = true;

	if (texture == 0)
	{
		if (RenderTargetTexture!=0)
		{
			glBindTexture(GL_TEXTURE_2D, RenderTargetTexture->getOpenGLTextureName());

			// Copy Our ViewPort To The Texture
			glCopyTexImage2D(GL_TEXTURE_2D, 0, 0, 0, 0,
			                 RenderTargetTexture->getSize().Width, RenderTargetTexture->getSize().Height, 0);

			glViewport(0, 0, ScreenSize.Width, ScreenSize.Height);
			RenderTargetTexture = 0;
			CurrentRendertargetSize = core::dimension2d<s32>(0,0);
		}
	}
	else
	{
		if (RenderTargetTexture!=0)
		{
			glBindTexture(GL_TEXTURE_2D, RenderTargetTexture->getOpenGLTextureName());

			// Copy Our ViewPort To The Texture
			glCopyTexImage2D(GL_TEXTURE_2D, 0, 0, 0, 0,
			                 RenderTargetTexture->getSize().Width, RenderTargetTexture->getSize().Height, 0);

			glViewport(0, 0, ScreenSize.Width, ScreenSize.Height);
		}

		// we want to set a new target. so do this.
		glViewport(0, 0, texture->getSize().Width, texture->getSize().Height);
		RenderTargetTexture = (COpenGLTexture*)texture;
		CurrentRendertargetSize = texture->getSize();
	}

	GLbitfield mask = 0;
	if (clearBackBuffer)
	{
		f32 inv = 1.0f / 255.0f;
		glClearColor(color.getRed() * inv, color.getGreen() * inv,
		             color.getBlue() * inv, color.getAlpha() * inv);

		mask |= GL_COLOR_BUFFER_BIT;
	}
	if (clearZBuffer)
	{
		glDepthMask(GL_TRUE);
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	glClear(mask);

	return ret;
}




void CSoftwareDriver2::changeCursor(video::ITexture *texture)
{
	if (!texture)
		return;

	removeTexture(cursor);
	cursor = texture;
}

void CSoftwareDriver2::setFog(SColor c, bool linearFog, f32 start,
                              f32 end, f32 density, bool pixelFog, bool rangeFog)
{
	CNullDriver::setFog(c, linearFog, start, end, density, pixelFog, rangeFog);

	glFogi(GL_FOG_MODE, linearFog ? GL_LINEAR : GL_EXP);

	if(linearFog)
	{
		glFogf(GL_FOG_START, start);
		glFogf(GL_FOG_END, end);
	}
	else
		glFogf(GL_FOG_DENSITY, density);

	SColorf color(c);
	GLfloat data[4] = {color.r, color.g, color.b, color.a};
	glFogfv(GL_FOG_COLOR, data);
}

void CSoftwareDriver2::createMaterialRenderers()
{
	// create OpenGL material renderers

	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_SOLID( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_SOLID_2_LAYER( this));

	// add the same renderer for all lightmap types
	COpenGLMaterialRenderer_LIGHTMAP* lmr = new COpenGLMaterialRenderer_LIGHTMAP( this);
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_ADD:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M2:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M4:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M2:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M4:
	lmr->drop();

	// add remaining material renderer
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_DETAIL_MAP( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_SPHERE_MAP( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_REFLECTION_2_LAYER( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_ADD_COLOR( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_VERTEX_ALPHA( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_SOLID( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER( this));

	/* per le mappe */
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_SOLID( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_ADD_COLOR( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_VERTEX_ALPHA( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_SOLID( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_ADD_COLOR( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_VERTEX_ALPHA( this));
}
//! destructor
CSoftwareDriver2::~CSoftwareDriver2()
{

}



void CSoftwareDriver2::setBasicRenderStates(const SMaterial& material, const SMaterial& lastmaterial,
                                            bool resetAllRenderstates)
{

	if (resetAllRenderstates ||
	    lastmaterial.AmbientColor != material.AmbientColor ||
	    lastmaterial.DiffuseColor != material.DiffuseColor ||
	    lastmaterial.SpecularColor != material.SpecularColor ||
	    lastmaterial.EmissiveColor != material.EmissiveColor ||
	    lastmaterial.Shininess != material.Shininess)
	{
		GLfloat color[4];

		f32 inv = 1.0f / 255.0f;

		color[0] = Material.AmbientColor.getRed() * inv;
		color[1] = Material.AmbientColor.getGreen() * inv;
		color[2] = Material.AmbientColor.getBlue() * inv;
		color[3] = Material.AmbientColor.getAlpha() * inv;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color);

		color[0] = Material.DiffuseColor.getRed() * inv;
		color[1] = Material.DiffuseColor.getGreen() * inv;
		color[2] = Material.DiffuseColor.getBlue() * inv;
		color[3] = Material.DiffuseColor.getAlpha() * inv;
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);


		color[0] = 0; //Material.SpecularColor.getRed() * inv;
		color[1] = 0; //Material.SpecularColor.getGreen() * inv;
		color[2] = 0; //Material.SpecularColor.getBlue() * inv;
		color[3] = 0; //Material.SpecularColor.getAlpha() * inv;
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);

		color[0] = Material.EmissiveColor.getRed() * inv;
		color[1] = Material.EmissiveColor.getGreen() * inv;
		color[2] = Material.EmissiveColor.getBlue() * inv;
		color[3] = Material.EmissiveColor.getAlpha() * inv;
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, color);

		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, Material.Shininess);
	}

	// bilinear

	if (resetAllRenderstates ||
	    lastmaterial.BilinearFilter != material.BilinearFilter ||
	    lastmaterial.AnisotropicFilter != material.AnisotropicFilter )
	{

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		                 Material.BilinearFilter ? GL_LINEAR : GL_NEAREST);
	}

	// fillmode

	if (resetAllRenderstates || lastmaterial.Wireframe != material.Wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, material.Wireframe ? GL_LINE : GL_FILL);



	// lighting

	if (resetAllRenderstates || lastmaterial.Lighting != material.Lighting)
	{
		if (Material.Lighting)
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);
	}

	// zbuffer

	if (resetAllRenderstates || lastmaterial.ZBuffer != material.ZBuffer)
	{
		if (material.ZBuffer)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	// zwrite
	if (resetAllRenderstates || lastmaterial.ZWriteEnable != material.ZWriteEnable)
		glDepthMask(material.ZWriteEnable ? GL_TRUE : GL_FALSE);

	// back face culling

	if (resetAllRenderstates || lastmaterial.BackfaceCulling != material.BackfaceCulling)
	{
		if (material.BackfaceCulling)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
	}

	// fog
	if (resetAllRenderstates || lastmaterial.FogEnable != material.FogEnable)
	{
		if (material.FogEnable)
			glEnable(GL_FOG);
		else
			glDisable(GL_FOG);
	}

	// normalization
	if (resetAllRenderstates || lastmaterial.NormalizeNormals != material.NormalizeNormals)
	{
		if (material.NormalizeNormals)
			glEnable(GL_NORMALIZE);
		else
			glDisable(GL_NORMALIZE);
	}
}

//! sets the needed renderstates
void CSoftwareDriver2::setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel)
{
	if (CurrentRenderMode != ERM_2D || Transformation3DChanged)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		Transformation3DChanged = false;

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_FOG);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_LIGHTING);

		glDisable(GL_ALPHA_TEST);
		glCullFace(GL_BACK);

		if (CurrentRenderMode == ERM_3D &&
		    Material.MaterialType >= 0 && Material.MaterialType < (s32)MaterialRenderers.size())
			MaterialRenderers[Material.MaterialType]->OnUnsetMaterial();
	}

	if (texture)
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		if (alphaChannel)
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
		}
		else
		{
			if (alpha)
			{
				glDisable(GL_ALPHA_TEST);
				glEnable(GL_BLEND);
			}
			else
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glDisable(GL_ALPHA_TEST);
				glDisable(GL_BLEND);
			}
		}

	}
	else
	{
		if (alpha)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glDisable(GL_ALPHA_TEST);
		}
		else
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glDisable(GL_BLEND);
			glDisable(GL_ALPHA_TEST);
		}
	}

	CurrentRenderMode = ERM_2D;
}




//! void selects the right triangle renderer based on the render states.
void CSoftwareDriver2::selectRightTriangleRenderer()
{

	/* funzione deprecata, la lascio xkè non ho voglia di cancellare il prototipo */

}





//! queries the features of the driver, returns true if feature is available
bool CSoftwareDriver2::queryFeature(E_VIDEO_DRIVER_FEATURE feature)
{

	switch (feature)
	{
	case EVDF_BILINEAR_FILTER:
		return true;
	case EVDF_RENDER_TO_TARGET:
		return true;
	case EVDF_HARDWARE_TL:
		return false;
	case EVDF_MIP_MAP:
		return true;
	case EVDF_STENCIL_BUFFER:
		return StencilBuffer;
	};

	return false;

}



//! sets transformation
void CSoftwareDriver2::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
{
	Transformation3DChanged = true;

	GLfloat glmat[16];
	Matrizes[state] = mat;

	switch(state)
	{
	case ETS_VIEW:
		// OpenGL only has a model matrix, view and world is not existent. so lets fake these two.
		createGLMatrix(glmat, Matrizes[ETS_VIEW] * Matrizes[ETS_WORLD]);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glmat);

		break;
	case ETS_WORLD:
		// OpenGL only has a model matrix, view and world is not existent. so lets fake these two.
		createGLMatrix(glmat, Matrizes[ETS_VIEW] * Matrizes[ETS_WORLD]);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glmat);
		break;
	case ETS_PROJECTION:
	{

		createGLMatrix(glmat, mat);

		// flip z to compensate OpenGLs right-hand coordinate system
		glmat[12]*= -1.0f;
		Matrizes[ETS_PROJECTION](0,3) *= -1;
		projection = Matrizes[ETS_PROJECTION];
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glmat);
	}
		break;
	}
	f32 modelMatrix[16], projectionMatrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
	CalculateViewFrustum( &myVFrustum, projectionMatrix, modelMatrix );


}




//! sets the current Texture
void CSoftwareDriver2::setTexture(u32 stage, video::ITexture* texture)
{
	if (stage > 1)
		return;

	if (texture == 0)
	{
		if (stage == 0)
		{
			glDisable(GL_TEXTURE_2D);
			__second_texture = 0;
		}
	}
	else
	{
		if (stage == 0)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,
			              ((COpenGLTexture*)texture)->getOpenGLTextureName());
		} else {
			__second_texture = texture;
		}
	}
}



//! sets a material
void CSoftwareDriver2::setMaterial(const SMaterial& material)
{
	Material = material;

	setTexture(0, Material.Texture1);
	setTexture(1, Material.Texture2);
}


typedef struct
{

	unsigned int col;
	float x,y,z;

} __vect3dcol;



#if 0
#define MAX_VTX_SCENE 312*4

char __buffer_2d[MAX_VTX_SCENE * sizeof(__vect3dcol)];
int __buffer_2dc = 0;
__vect3dcol* __rect2d = (__vect3dcol*)&__buffer_2d[0];
#endif



//! clears the zbuffer
bool CSoftwareDriver2::beginScene(bool backBuffer, bool zBuffer, SColor color)
{
	//__buffer_2dc = 0;
	f32 inv = 1.0f / 255.0f;
	glClearColor(color.getRed() * inv, color.getGreen() * inv,
	             color.getBlue() * inv, color.getAlpha() * inv);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	__lte_current_size_vbo2 = 0;
	__lte_current_size_vbo1 = 0;

	if (enableClipping == true) {

		vertexStack = new BspClipVertex[30000];
		vertexStackIndex = new u16[65535];

	}

	glEnable( GL_STENCIL_TEST );
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 128, 0xFF);
	glColorMask(GL_FALSE, GL_FALSE,   GL_FALSE, GL_FALSE );
	draw2DRectangle(SColor(255,0,0,255), core::rect<s32>(core::position2d<s32>(0,0),
	                                                     core::position2d<s32>(480,272)),
	                0);

	glFlush();
	glColorMask(GL_TRUE, GL_TRUE,   GL_TRUE, GL_TRUE );
	glDisable(GL_STENCIL_TEST);

	return true;

}

//! presents the rendered scene on the screen, returns false if failed
bool CSoftwareDriver2::endScene( s32 windowId, core::rect<s32>* sourceRect )
{
	const char *filename2 = "#DefaultKeys";
	const char *filename3 = "#DefaultKeysC";
	const char *filename4 = "#DefaultNums";
	const char *filename5 = "#DefaultNumsC";
	const char *filename6 = "#SDefaultKeys";
	const char *filename7 = "#SDefaultKeysC";
	const char *filename8 = "#SDefaultNums";
	const char *filename9 = "#SDefaultNumsC";
	const char *filename10 = "#Selector";

	if (tshowLogo == false) {
		struct timeval tv;
		gettimeofday(&tv, 0);
		if (tv.tv_sec - startTime > 600) return false;
	}

	CNullDriver::endScene();
	// Draw cursor to the screen if is visible

#if 0

	if (__buffer_2dc) {
		setTexture(0,0);
		setRenderStates2DMode(true, false, false);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY );
		glVertexPointer(3, GL_FLOAT, sizeof(__vect3dcol),  &__rect2d[0].x);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(__vect3dcol), &__rect2d[0].col);



		glDrawArrays(GL_QUADS, 0, __buffer_2dc);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

	}

	pspDebugScreenSetXY(10,20);
	printf("Numero vect3d: %5d", __buffer_2dc);
	printf("Numero vect3d: %5d", __buffer_2dc);
#endif


	if (key_state == 0) {

		if (t_keys) {
			delete (t_keys); t_keys = 0;
		}
		if (t_keys_c)
		{ delete ( t_keys_c); t_keys_c = 0; }
		if (t_nums) {
			delete ( t_nums); t_nums =0;
		}
		if (t_nums_c) {
			delete ( t_nums_c); t_nums_c =0;
		}
		if (st_keys) {
			delete (  st_keys); st_keys = 0;
		}
		if (st_keys_c) {
			delete (  st_keys_c); st_keys_c = 0;
		}
		if (st_nums) {
			delete ( st_nums); st_nums = 0;
		}
		if (st_nums_c) {
			delete ( st_nums_c); st_nums_c = 0;
		}
		if (selector) {
			delete ( selector); selector = 0;
		}




		if (CursorControl->isVisible() == true)
		{

			core::position2d<s32> pos = ((CCursorControl*)CursorControl)->getMousePosition();

			pos.Y -= 5;

			draw2DImage(cursor, pos, core::rect<s32>(core::position2d<s32>(0,0),
			                                         core::position2d<s32>(16,32)),0,SColor(255,255,255,255),true);

/*
   	draw2DImage(cursor, pos, core::rect<s32>(core::position2d<s32>(0,0),
   		cursor->getOriginalSize()), video::SColor(255,255,255,255), true); */
		}
	} else {
		io::IReadFile* file;
		// la keyboard è attiva
		if (!t_keys) {
			if (key_type == 0) {
				file = io::createMemoryReadFile(KeyTexture, KeyLen, filename2, false);
				t_keys = loadTextureFromFile(file);
				makeColorKeyTexture(t_keys, SColor(255,255,0,255));
				file->drop();
				file= io::createMemoryReadFile(SKeyTexture, SKeyLen, filename6, false);

				st_keys = loadTextureFromFile(file);
				makeColorKeyTexture(st_keys, SColor(255,255,0,255));
				file->drop();

			}
		} else if (key_type != 0) { delete (t_keys); t_keys = 0; delete ( st_keys); st_keys = 0; }

		if (!t_keys_c) {
			if (key_type == 1) {
				file = io::createMemoryReadFile(KeyCTexture, KeyCLen, filename3, false);
				t_keys_c = loadTextureFromFile(file);
				makeColorKeyTexture(t_keys_c, SColor(255,255,0,255));
				file->drop();
				file = io::createMemoryReadFile(SKeyCTexture, SKeyCLen, filename7, false);
				st_keys_c = loadTextureFromFile(file);
				makeColorKeyTexture(st_keys_c, SColor(255,255,0,255));
				file->drop();
			}
		} else if (key_type != 1) { delete ( t_keys_c); t_keys_c = 0; delete ( st_keys_c); st_keys_c = 0; }

		if (!t_nums) {
			if (key_type == 2) {
				file = io::createMemoryReadFile(NumTexture, NumLen, filename4, false);
				t_nums = loadTextureFromFile(file);
				makeColorKeyTexture(t_nums, SColor(255,255,0,255));
				file->drop();
				file = io::createMemoryReadFile(SNumTexture, SNumLen, filename8, false);
				st_nums = loadTextureFromFile(file);
				makeColorKeyTexture(st_nums, SColor(255,255,0,255));
				file->drop();
			}
		} else if (key_type != 2) { delete ( t_nums); t_nums = 0; delete ( st_nums); st_nums = 0; }

		if (!t_nums_c) {
			if (key_type == 3) {

				file = io::createMemoryReadFile(NumCTexture, NumCLen, filename5, false);
				t_nums_c = loadTextureFromFile(file);
				makeColorKeyTexture(t_nums_c, SColor(255,255,0,255));
				file->drop();
				file = io::createMemoryReadFile(SNumCTexture, SNumCLen, filename9, false);
				st_nums_c = loadTextureFromFile(file);
				makeColorKeyTexture(st_nums_c, SColor(255,255,0,255));
				file->drop();
			}
		} else if (key_type != 3) { delete ( t_nums_c); t_nums_c = 0; delete ( st_nums_c); st_nums_c = 0; }

		if (!selector) {
			file = io::createMemoryReadFile(KeySelector, KeySelectorLen, filename10, false);
			selector = loadTextureFromFile(file);
			makeColorKeyTexture(selector, SColor(255,255,0,255));
			file->drop();
		}

		video::ITexture *tex, *tex_s;
		if (key_type == 0)
		{ tex = t_keys; tex_s = st_keys; }
		if (key_type == 1)
		{ tex = t_keys_c; tex_s = st_keys_c; }
		if (key_type == 2)
		{ tex = t_nums; tex_s = st_nums; }
		if (key_type == 3)
		{ tex = t_nums_c; tex_s = st_nums_c; }
		draw2DImage(tex, core::position2d<s32>(key_pos_x, key_pos_y), core::rect<s32>(core::position2d<s32>(0,0),
		                                                                              core::position2d<s32>(150,150)),0,SColor(255,255,255,255),true);

		int sel_x = key_sel % 3;
		int sel_y = key_sel / 3;

		int now_pos_x = key_pos_x + sel_x*50 - 7;
		int now_pos_y = key_pos_y + sel_y*50 - 7;

		draw2DImage(tex_s, core::position2d<s32>(now_pos_x, now_pos_y), core::rect<s32>(core::position2d<s32>(sel_x*64,sel_y*64),
		                                                                                core::position2d<s32>(sel_x*64+64,sel_y*64+64)),0,SColor(255,255,255,255),true);

		draw2DImage(selector, core::position2d<s32>(now_pos_x, now_pos_y), core::rect<s32>(core::position2d<s32>(0,0),
		                                                                                   core::position2d<s32>(64,64)),0,SColor(255,255,255,255),true);



	}


	if (sceDisplayGetVcount() == __lastvblank) return true;
	__lastvblank = sceDisplayGetVcount();



	glutSwapBuffers();

	if (enableClipping == true) {
		delete vertexStack;
		delete vertexStackIndex;
	}

	return true;
}


//! sets a viewport
void CSoftwareDriver2::setViewPort(const core::rect<s32>& area)
{
	core::rect<s32> vp = area;
	core::rect<s32> rendert(0,0, ScreenSize.Width, ScreenSize.Height);
	vp.clipAgainst(rendert);

	if (vp.getHeight()>0 && vp.getWidth()>0)
		glViewport(vp.UpperLeftCorner.X,
		           ScreenSize.Height - vp.UpperLeftCorner.Y - vp.getHeight(),
		           vp.getWidth(),
		           vp.getHeight());

	ViewPort = vp;
}


//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
void CSoftwareDriver2::setAmbientLight(const SColorf& color)
{
	GLfloat data[4] = {color.r, color.g, color.b, color.a};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, data);
}


//! adds a dynamic light
void CSoftwareDriver2::addDynamicLight(const SLight& light)
{
	++LastSetLight;
	if (!(LastSetLight < GL_MAX_LIGHTS))
		return;

	setTransform(ETS_WORLD, core::matrix4());

	CNullDriver::addDynamicLight(light);

	s32 lidx = GL_LIGHT0 + LastSetLight;
	GLfloat data[4];

	if( light.Type == video::ELT_DIRECTIONAL )
	{
		// set direction
		data[0] = -light.Position.X;
		data[1] = -light.Position.Y;
		data[2] = -light.Position.Z;

		data[3] = 0.0f;
		glLightfv(lidx, GL_POSITION, data);

		data[3] = 1.0f;
		glLightfv(lidx, GL_SPOT_DIRECTION, data);

		glLightf(lidx, GL_SPOT_CUTOFF, 180.0f);
		glLightf(lidx, GL_SPOT_EXPONENT, 0.0f);
	}
	else
	{
		// set position
		data[0] = light.Position.X;
		data[1] = light.Position.Y;
		data[2] = light.Position.Z;
		data[3] = 1.0f;
		glLightfv(lidx, GL_POSITION, data);
	}

	// set diffuse color
	data[0] = light.DiffuseColor.r;
	data[1] = light.DiffuseColor.g;
	data[2] = light.DiffuseColor.b;
	data[3] = light.DiffuseColor.a;
	glLightfv(lidx, GL_DIFFUSE, data);

	// set specular color
	data[0] = 0; //light.SpecularColor.r;
	data[1] = 0; //light.SpecularColor.g;
	data[2] = 0; //light.SpecularColor.b;
	data[3] = 0; //light.SpecularColor.a;
	glLightfv(lidx, GL_SPECULAR, data);

	// set ambient color
	data[0] = light.AmbientColor.r;
	data[1] = light.AmbientColor.g;
	data[2] = light.AmbientColor.b;
	data[3] = light.AmbientColor.a;
	glLightfv(lidx, GL_AMBIENT, data);

	// 1.0f / (constant + linar * d + quadratic*(d*d);

	// set attenuation
	glLightf(lidx, GL_CONSTANT_ATTENUATION, 0.0f);
	glLightf(lidx, GL_LINEAR_ATTENUATION, 1.0f / light.Radius);
	glLightf(lidx, GL_QUADRATIC_ATTENUATION, 0.0f);

	glEnable(lidx);
}

//! deletes all dynamic lights there are
void CSoftwareDriver2::deleteAllDynamicLights()
{
	for (s32 i=0; i<LastSetLight+1; ++i)
		glDisable(GL_LIGHT0 + i);

	LastSetLight = -1;

	CNullDriver::deleteAllDynamicLights();
}

//! returns the maximal amount of dynamic lights the device can handle
s32 CSoftwareDriver2::getMaximalDynamicLightAmount()
{
	return 4;
}


#if 0
/*!
 */
typedef struct
{

	float u,v;
	float x,y,z;

} __vect3dtcoord;

typedef struct
{

	float x,y,z;

} __vect3d;



inline void __draw_quad(core::rect<f32> npos, core::rect<f32> tcoords)
{

	__vect3dtcoord vtx[4];
	vtx[0].z = 0;
	vtx[1].z = 0;
	vtx[2].z = 0;
	vtx[3].z = 0;

/*
   1___3
 |\  |
 | \ |
 |__\|
   0   2

 */
	vtx[0].x = npos.UpperLeftCorner.X;
	vtx[0].y = npos.LowerRightCorner.Y;
	vtx[0].u = tcoords.UpperLeftCorner.X;
	vtx[0].v = tcoords.LowerRightCorner.Y;


	vtx[1].x = npos.UpperLeftCorner.X;
	vtx[1].y = npos.UpperLeftCorner.Y;
	vtx[1].u = tcoords.UpperLeftCorner.X;
	vtx[1].v = tcoords.UpperLeftCorner.Y;

	vtx[2].x = npos.LowerRightCorner.X;
	vtx[2].y = npos.LowerRightCorner.Y;
	vtx[2].u = tcoords.LowerRightCorner.X;
	vtx[2].v = tcoords.LowerRightCorner.Y;

	vtx[3].x = npos.LowerRightCorner.X;
	vtx[3].y = npos.UpperLeftCorner.Y;
	vtx[3].u = tcoords.LowerRightCorner.X;
	vtx[3].v = tcoords.UpperLeftCorner.Y;
	return;


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY );
	glVertexPointer(3, GL_FLOAT, sizeof(__vect3dtcoord),  &vtx[0].x);
	glTexCoordPointer(2, GL_FLOAT, sizeof(__vect3dtcoord), &vtx[0].u);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}





inline void __draw_quad_col(core::rect<f32> npos, video::SColor lu,
                            video::SColor ld,
                            video::SColor ru,
                            video::SColor rd
                            )
{
	if (__buffer_2dc == MAX_VTX_SCENE) return;

	__rect2d[0].z = 0;
	__rect2d[1].z = 0;
	__rect2d[2].z = 0;
	__rect2d[3].z = 0;


/*
   1___3
 |\  |
 | \ |
 |__\|
   0   2

 */

	__rect2d[__buffer_2dc+3].x = npos.UpperLeftCorner.X;
	__rect2d[__buffer_2dc+3].y = npos.LowerRightCorner.Y;
	__rect2d[__buffer_2dc+3].col = ld.toOpenGLColor();

	__rect2d[__buffer_2dc].x = npos.UpperLeftCorner.X;
	__rect2d[__buffer_2dc].y = npos.UpperLeftCorner.Y;
	__rect2d[__buffer_2dc].col = lu.toOpenGLColor();

	__rect2d[__buffer_2dc+2].x = npos.LowerRightCorner.X;
	__rect2d[__buffer_2dc+2].y = npos.LowerRightCorner.Y;
	__rect2d[__buffer_2dc+2].col = rd.toOpenGLColor();

	__rect2d[__buffer_2dc+1].x = npos.LowerRightCorner.X;
	__rect2d[__buffer_2dc+1].y = npos.UpperLeftCorner.Y;
	__rect2d[__buffer_2dc+1].col = ru.toOpenGLColor();

	__buffer_2dc += 4;



}

inline void __draw_quad_empty(core::rect<f32> npos, int col)
{

	if (__buffer_2dc == MAX_VTX_SCENE) return;

	__rect2d[__buffer_2dc].z = 0;
	__rect2d[__buffer_2dc+1].z = 0;
	__rect2d[__buffer_2dc+2].z = 0;
	__rect2d[__buffer_2dc+3].z = 0;
	__rect2d[__buffer_2dc].col = col;
	__rect2d[__buffer_2dc+1].col = col;
	__rect2d[__buffer_2dc+2].col = col;
	__rect2d[__buffer_2dc+3].col = col;

	__rect2d[__buffer_2dc+3].x = npos.UpperLeftCorner.X;
	__rect2d[__buffer_2dc+3].y = npos.LowerRightCorner.Y;


	__rect2d[__buffer_2dc].x = npos.UpperLeftCorner.X;
	__rect2d[__buffer_2dc].y = npos.UpperLeftCorner.Y;


	__rect2d[__buffer_2dc+2].x = npos.LowerRightCorner.X;
	__rect2d[__buffer_2dc+2].y = npos.LowerRightCorner.Y;


	__rect2d[__buffer_2dc+1].x = npos.LowerRightCorner.X;
	__rect2d[__buffer_2dc+1].y = npos.UpperLeftCorner.Y;


	__buffer_2dc+=4;

}
#endif


#include <pspdebug.h>
#define printf pspDebugScreenPrintf
inline void V2T_To_Bsp( BspClipVertex *dst, const S3DVertex2TCoords *src)
{

	/* Memory copy */
    #if 1

	memcpy ( dst, src, sizeof(BspClipVertex) );
	//  dst[0].color = src[0].Color.toOpenGLColor();

    #else
	/* Normal copy */

	dst[0].x = src[0].Pos.X;
	dst[0].y = src[0].Pos.Y;
	dst[0].z = src[0].Pos.Z;
	dst[0].u = src[0].TCoords.X;
	dst[0].v = src[0].TCoords.Y;
	dst[0].normal[0] = src[0].Normal.X;
	dst[0].normal[1] = src[0].Normal.Y;
	dst[0].normal[2] = src[0].Normal.Z;
	dst[0].color = src[0].Color.toOpenGLColor();
    #endif
}

inline f32 dotProduct( Quake3BspPlane *plane,  const BspClipVertex *p)
{
	return p->x*plane->normal.x +
	       p->y*plane->normal.y +
	       p->z*plane->normal.z +
	       plane->dist;
}
inline void interpolate ( BspClipVertex *out, const BspClipVertex *b, const BspClipVertex *a, f32 inv)
{
	f32 uColor1[4],uColor2[4], uColor3[4];

	out->x = b->x + ( inv * ( a->x - b->x ) );
	out->y = b->y + ( inv * ( a->y - b->y ) );
	out->z = b->z + ( inv * ( a->z - b->z ) );
	out->normal[0] = b->normal[0] + ( inv * ( a->normal[0] - b->normal[0] ) );
	out->normal[1] = b->normal[1] + ( inv * ( a->normal[1] - b->normal[1] ) );
	out->normal[2] = b->normal[2] + ( inv * ( a->normal[2] - b->normal[2] ) );
	out->u = b->u + ( inv * ( a->u - b->u ) );
	out->v = b->v + ( inv * ( a->v - b->v ) );


	//* Generate Color
	uColor2[0] = (f32)(b->color & 0xFF)  / 255.0f;
	uColor2[1] = (f32)((b->color >> 8) & 0xFF) / 255.0f;
	uColor2[2] = (f32)((b->color >> 16) & 0xFF) / 255.0f;
	uColor2[3] = (f32)((b->color >> 24) & 0xFF) / 255.0f;

	uColor3[0] = (f32)(a->color & 0xFF) / 255.0f;
	uColor3[1] = (f32)((a->color >> 8) & 0xFF) / 255.0f;
	uColor3[2] = (f32)((a->color >> 16) & 0xFF) / 255.0f;
	uColor3[3] = (f32)((a->color >> 24) & 0xFF) / 255.0f;

	uColor1[0] = uColor2[0] + ( inv * (uColor3[0] - uColor2[0]) );
	uColor1[1] = uColor2[1] + ( inv * (uColor3[1] - uColor2[1]) );
	uColor1[2] = uColor2[2] + ( inv * (uColor3[2] - uColor2[2]) );
	uColor1[3] = uColor2[3] + ( inv * (uColor3[3] - uColor2[3]) );
	out->color =  (unsigned int)(uColor1[0]*255.f) +
	             ((unsigned int)(uColor1[1]*255.f) << 8) +
	             ((unsigned int)(uColor1[2]*255.f) << 16) +
	             ((unsigned int)(uColor1[3]*255.f) << 24);

}

inline f32 fraction(Quake3BspPlane *plane, const BspClipVertex *a, const BspClipVertex *b, f32 out, f32 c)
{
	f32 t;

	if (out == 0) t = c;
	else
		t = c / out;

	if (t > 1.0f)
		return 1.0f;
	else if ( t < 0.0f)
		return 0.0f;
	else return t;
}

inline u32 clipToPlane ( BspClipVertex * dest, const BspClipVertex * source, u32 inCount, Quake3BspPlane* plane )
{
	u32 outCount;
	BspClipVertex * out;

	const BspClipVertex * vEnd;
	const BspClipVertex * vStart;

	f32 endDotPlane;
	f32 startDotPlane;

	out = dest;
	outCount = 0;
	vStart = source; // PolygonV1
	startDotPlane = dotProduct ( plane, vStart );

	for( u32 i = 1; i < inCount + 1; ++i)
	{

		vEnd = &source[i%inCount]; // PolygonV2
		endDotPlane = dotProduct(plane, vEnd);

		// current point inside
		if ( startDotPlane > 0.f )
		{
			// copy current to out
			memcpy(out, vStart, sizeof(BspClipVertex));
			out += 1;
			outCount += 1;

			// last point outside
			if ( endDotPlane < startDotPlane && endDotPlane <= 0.0f)
			{
				// intersect line segment with plane
				interpolate (out, vStart, vEnd, startDotPlane/(startDotPlane-endDotPlane) );
				out += 1;
				outCount += 1;

			}

		}
		else
		{
			// current point outside
			if ( endDotPlane > startDotPlane && endDotPlane > 0.0f)
			{
				// previous was inside
				// intersect line segment with plane
				interpolate (out, vStart, vEnd, startDotPlane/(startDotPlane-endDotPlane) );

				out += 1;
				outCount += 1;
			}
		}
		vStart = vEnd;
		startDotPlane = endDotPlane;
	}

	return outCount;
}

inline u32 clipToFrustrum ( BspClipVertex *v0, BspClipVertex * v1, u32 vIn, ViewFrustum *pFrustum )
{
	u32 vOut;

	vOut = vIn;


	vOut = clipToPlane ( v1, v0, vOut, &pFrustum->FrustrumPlane[0] );               // right
	if (vOut < 3) return 0;

	vOut = clipToPlane ( v0, v1, vOut, &pFrustum->FrustrumPlane[1] );               // left
	if (vOut < 3) return 0;

	vOut = clipToPlane ( v1, v0, vOut, &pFrustum->FrustrumPlane[2] );               // top
	if (vOut < 3) return 0;

	vOut = clipToPlane ( v0, v1, vOut, &pFrustum->FrustrumPlane[3] );               // bottom
	if (vOut < 3) return 0;

	/*vOut = clipToPlane ( v1, v0, vOut, &pFrustum->FrustrumPlane[4] );		// near
	   if (vOut < 3) return 0;

	   vOut = clipToPlane ( v0, v1, vOut, &pFrustum->FrustrumPlane[5] );		// far  */
	return vOut;
}



#define _DRAW_UNCLIPPED_
#define _DRAW_CLIPPED_

inline void BSP_Step2(BspClipVertex *dst, u16 *index, const S3DVertex2TCoords *src, const u16* indexList, int count, int *output, int *vcount)
{
#if 1
	// the bounding box
	core::aabbox3d<f32> box;
	// reset bounding box
	box.reset(src[indexList[0]].Pos);

	// insert points into it
	for (int x = 1; x < count; x++)
		box.addInternalPoint( src[indexList[x]].Pos );

	// if the cluster is not visable discard it
	if (!IsClusterVisableInFrustumFloat(&myVFrustum, (f32*)&box.MinEdge, (f32*)&box.MaxEdge ))
		return;

	int frustVis[4];
	for (int x = 0; x < 4; x++)
		frustVis[x] = IsAABInPlane(&myVFrustum, (f32*)&box.MinEdge, (f32*)&box.MaxEdge, x);



	// check if boundingbox is fully inside the frustum view
	if ((frustVis[0] &  frustVis[1] & frustVis[2] & frustVis[3]))
	{
#endif
       #if 1
	// yes, just copy the whole vertices into the buffer
	for ( int x = 0; x < count; x++)
	{
		V2T_To_Bsp( &dst[x], &src[indexList[x]]);
		index[x] = *vcount + x;

	}
	*output += count;
	*vcount += count;
       #endif
#if 1
}
else
{

	int l = 0, o = 0, s = *vcount;
	// no, clip them


	for (int c = 0; c < count; c+= 3) {
		BspClipVertex tr[10], temp[10];
		V2T_To_Bsp(&tr[0], &src[indexList[c]]);
		V2T_To_Bsp(&tr[1], &src[indexList[c+1]]);
		V2T_To_Bsp(&tr[2], &src[indexList[c+2]]);

		// original vertex number
		int ivt = 3;



		BspClipVertex *res = tr;
		BspClipVertex *res2 = temp;
		BspClipVertex *chg = 0;
		// clip the vertex

		for (int x = 0; x < 4; x++)
		{
			if (frustVis[x] == 0)
			{

				ivt = clipToPlane ( res2, res, ivt, &myVFrustum.FrustrumPlane[x] ); // bottom
				if (ivt < 3 || !res2) break;

				chg = res2;
				res2 = res;
				res = chg;

			}
		}


		if (ivt > 2 && res)
		{
			memcpy(&dst[o], &res[0], sizeof(BspClipVertex) * ivt);

			for (int k = 0; k <= ivt - 3; k++)
			{ index[l++] = s; index[l++] = s + k + 1; index[l++] = s + k + 2;  }
			o += ivt;
			s += ivt;

		}

	}
	*output += l;
	*vcount += o;

}
#endif
}



inline int isPointVisible( Quake3BspPlane* pPlane, f32 x, f32 y, f32 z)
{


	f32 output =  x*pPlane->normal.x+y*pPlane->normal.y+pPlane->normal.z*z;
	output += pPlane->dist;

	if( output >= 0 )
		return 1;
	return 0;
}


inline int isVertexVisible(ViewFrustum* pFrustum, const void *vtx)
{
	BspClipVertex* pPolygonV1;
	int iCurrVert = 0;
	int i = 0;
	pPolygonV1 = (BspClipVertex *) vtx;

	for (int i = 0; i < 5; i++)
	{

		if( !isPointVisible( &pFrustum->FrustrumPlane[i], pPolygonV1[0].x,
		                     pPolygonV1[0].y, pPolygonV1[0].z) )
			return 0;
	}

	return 1;
}



//! Draws an indexed triangle list.
void CSoftwareDriver2::drawIndexedTriangleList(const S3DVertexTangents* vertices,
                                               s32 vertexCount, const u16* indexList, s32 triangleCount)
{

	if (!checkPrimitiveCount(triangleCount))
		return;

	CNullDriver::drawIndexedTriangleList(vertices, vertexCount, indexList, triangleCount);

	setRenderStates3DMode();


	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY );
	glEnableClientState(GL_NORMAL_ARRAY );

	// convert colors to gl color format.

	const S3DVertexTangents* p = vertices;
	ColorBuffer.set_used(vertexCount);
	for (s32 i=0; i<vertexCount; ++i)
	{
		ColorBuffer[i] = p->Color.toOpenGLColor();
		++p;
	}

	// draw everything

	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(video::SColor), &ColorBuffer[0]);
	glNormalPointer(GL_FLOAT, sizeof(S3DVertexTangents), &vertices[0].Normal);
	glVertexPointer(3, GL_FLOAT, sizeof(S3DVertexTangents),  &vertices[0].Pos);
	glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertexTangents), &vertices[0].TCoords);

	glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_SHORT, indexList);
	glFlush();
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY );
	glDisableClientState(GL_NORMAL_ARRAY );
}

#define GL_POSITION_MAPPING_PSP 0x11111


inline int __is_tris_visible( core::vector3df *poly )
{

	for (int x = 0; x < 3; x++)
		for (int u = 0; u < 6; u++)
		{

			f32 output =  poly[x].X*myVFrustum.FrustrumPlane[u].normal.x+
			             poly[x].Y*myVFrustum.FrustrumPlane[u].normal.y+
			             myVFrustum.FrustrumPlane[u].normal.z*poly[x].Z;

			output += myVFrustum.FrustrumPlane[u].dist;

			if( output < 0 )
				return 0;
		}

	return 1;
}

inline int __is_tris_completely_out( core::vector3df *poly )
{

	int planes_out[3];

	planes_out[0] = planes_out[1] = planes_out[2] = 0;


	for (int x = 0; x < 3; x++)
		for (int u = 0; u < 6; u++)
		{

			f32 output =  poly[x].X*myVFrustum.FrustrumPlane[u].normal.x+
			             poly[x].Y*myVFrustum.FrustrumPlane[u].normal.y+
			             myVFrustum.FrustrumPlane[u].normal.z*poly[x].Z;

			output += myVFrustum.FrustrumPlane[u].dist;

			if( output < 0 )
				planes_out[x]++;
		}

	return planes_out[0] && planes_out[1] && planes_out[2] ;
}

inline f32 shadowDotProduct( Quake3BspPlane *plane,  const core::vector3df *p)
{
	return p->X*plane->normal.x +
	       p->Y*plane->normal.y +
	       p->Z*plane->normal.z +
	       plane->dist;
}
inline void shadowInterpolate ( core::vector3df *out, const core::vector3df  *b, const core::vector3df  *a, f32 inv)
{
	out->X = b->X + ( inv * ( a->X - b->X ) );
	out->Y = b->Y + ( inv * ( a->Y - b->Y ) );
	out->Z = b->Z + ( inv * ( a->Z - b->Z ) );
}
inline f32 shadowFraction(f32 out, f32 c)
{
  #define d_clampf(x) x < 0.f ? 0.0f : x > 1.0f ? 1.0f : x
	f32 t =  c / out;
	return /*d_clampf(*/ t /*)*/;
}
inline u32 shadowClipToPlane ( core::vector3df * dest, const core::vector3df * source, u32 inCount, Quake3BspPlane* plane )
{
	u32 outCount;
	core::vector3df  * out;

	const core::vector3df  * vEnd;
	const core::vector3df  * vStart;

	f32 endDotPlane;
	f32 startDotPlane;

	out = dest;
	outCount = 0;
	vStart = source; // PolygonV1
	startDotPlane = shadowDotProduct ( plane, vStart );

	for( u32 i = 1; i < inCount + 1; ++i)
	{

		vEnd = &source[i%inCount]; // PolygonV2
		endDotPlane = shadowDotProduct(plane, vEnd);

		// current point inside
		if ( startDotPlane > 0.f )
		{
			// copy current to out
			memcpy(out, vStart, sizeof(core::vector3df));
			out += 1;
			outCount += 1;

			// last point outside
			if ( endDotPlane < startDotPlane && endDotPlane <= 0.0f)
			{
				// intersect line segment with plane
				shadowInterpolate (out, vStart, vEnd, startDotPlane/(startDotPlane-endDotPlane) );
				out += 1;
				outCount += 1;

			}

		}
		else
		{
			// current point outside
			if ( endDotPlane > startDotPlane && endDotPlane > 0.0f)
			{
				// previous was inside
				// intersect line segment with plane
				shadowInterpolate (out, vStart, vEnd,  startDotPlane/(startDotPlane-endDotPlane) );

				out += 1;
				outCount += 1;
			}
		}
		vStart = vEnd;
		startDotPlane = endDotPlane;
	}

	return outCount;
}

inline u32 shadowClipToFrustrum ( core::vector3df *v0, core::vector3df * v1, u32 vIn, ViewFrustum *pFrustum )
{
	u32 vOut;

	vOut = vIn;


	vOut = shadowClipToPlane ( v1, v0, vOut, &pFrustum->FrustrumPlane[0] );         // right
	if (vOut < 3) return 0;

	vOut = shadowClipToPlane ( v0, v1, vOut, &pFrustum->FrustrumPlane[1] );         // left
	if (vOut < 3) return 0;

	vOut = shadowClipToPlane ( v1, v0, vOut, &pFrustum->FrustrumPlane[2] );         // top
	if (vOut < 3) return 0;

	vOut = shadowClipToPlane ( v0, v1, vOut, &pFrustum->FrustrumPlane[3] );         // bottom
	if (vOut < 3) return 0;
	/*
	       vOut = shadowClipToPlane ( v1, v0, vOut, &pFrustum->FrustrumPlane[4] );		// near
	   if (vOut < 3) return 0;

	       vOut = shadowClipToPlane ( v0, v1, vOut, &pFrustum->FrustrumPlane[5] );		// far
	 */
	return vOut;
}

core::CMemory *CSoftwareDriver2::getMemoryBuffer()
{
	mem.CFlushMemory();
	return &mem;
}



void CSoftwareDriver2::drawClippedShadow(int triangleCount, core::vector3df* vertices, u16 *indices)
{

	mem.CFlushMemory();
	core::vector3df *ptr = (core::vector3df*)mem.CMemoryAlloc(0);
	int b = triangleCount*3, l = 0, i = 0;
	bool run = true;

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(core::vector3df),&ptr[0].X);

	core::vector3df clipped[10], temp[10];

	int current_count = 0;
	int culled = 0;
	for (int x = 0; x < triangleCount * 3; x+=3)
	{

		clipped[0] = vertices[indices[x]];
		clipped[1] = vertices[indices[x+1]];
		clipped[2] = vertices[indices[x+2]];

		if (__is_tris_visible(clipped))
		{

			memcpy(&ptr[current_count], clipped, sizeof(core::vector3df) * 3);
			current_count += 3;

		}
		else
		{

			int vOut = shadowClipToFrustrum(clipped, temp, 3, &myVFrustum);

			if (vOut > 2)
				for (int i = 0; i <= vOut - 3; i++)
				{
					memcpy(&ptr[current_count], &clipped[0], sizeof(core::vector3df));
					current_count++;

					memcpy(&ptr[current_count], &clipped[i+1], sizeof(core::vector3df) * 2);
					current_count+=2;

				}



			if (43690 - current_count < 10)
				break;

		}

	}


	glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
	glCullFace(GL_BACK);
	glDrawArrays(GL_TRIANGLES, 0, current_count);
	// glFlush();


	glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
	glCullFace(GL_FRONT);
	glDrawArrays(GL_TRIANGLES, 0, current_count);
	glFlush();
	glDisableClientState(GL_VERTEX_ARRAY);
}



//! draws an indexed triangle list
void CSoftwareDriver2::drawIndexedTriangleList( const S3DVertex* vertices, s32 vertexCount,
                                                const u16* indexList, s32 triangleCount
                                                )
{

	int depthFunc;
	if (Material.DepthLess == true || Material.Flags[EMF_DEPTHLESS] == true)
		depthFunc = GL_LESS;
	else
		depthFunc = GL_LEQUAL;

	glDepthFunc(depthFunc);

	CNullDriver::drawIndexedTriangleList(vertices, vertexCount, indexList, triangleCount);
	setRenderStates3DMode();
	if (!checkPrimitiveCount(triangleCount) || !vertexCount)

		return;
	const int VERTEX_PER_NODE = 27;


	bool multitexturing = false;
	if (__second_texture != 0 && multipass)
		multitexturing = multipass->setPass(0);

	if ( (Material.Clipping == false && Material.Flags[EMF_CLIPPING] == false) || enableClipping == false)
	{

		if (Material.Flags[EMF_DMA]  == true || Material.StaticMesh == true) {

			//	sceKernelDcacheWritebackAll();
			sceKernelDcacheWritebackRange(vertices, sizeof(S3DVertex) * vertexCount);
			sceKernelDcacheWritebackRange(indexList, sizeof(u16) * triangleCount * 3);

			__pspgl_context_render_prim(pspgl_curctx, GE_TRIANGLES, triangleCount * 3, GE_COLOR_8888|GE_VERTEX_32BITF|GE_VINDEX_16BIT|GE_NORMAL_32BITF|GE_TEXTURE_32BITF|GE_TRANSFORM_3D,
			                            vertices, indexList);

			if (Material.DisableFlush == false && Material.Flags[EMF_DISABLEFLUSH] == false)
				glFlush();

			if (multitexturing == true)
			{
				glDepthFunc( GL_EQUAL);
				multipass->setPass(1);
				glBindTexture(GL_TEXTURE_2D,
				              ((COpenGLTexture*)__second_texture)->getOpenGLTextureName());

				__pspgl_context_render_prim(pspgl_curctx, GE_TRIANGLES, triangleCount * 3, GE_COLOR_8888|GE_VERTEX_32BITF|GE_VINDEX_16BIT|GE_NORMAL_32BITF|GE_TEXTURE_32BITF|GE_TRANSFORM_3D,
				                            vertices, indexList);

				if (Material.DisableFlush == false && Material.Flags[EMF_DISABLEFLUSH] == false)
					glFlush();



				glDepthFunc( depthFunc );
			}
			return;

		} else {
prova:
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY );
			glEnableClientState(GL_NORMAL_ARRAY );

			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(S3DVertex), &vertices[0].Color);
			glNormalPointer(GL_FLOAT, sizeof(S3DVertex), &vertices[0].Normal);
			glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), &vertices[0].TCoords);
			glVertexPointer(3, GL_FLOAT, sizeof(S3DVertex),  &vertices[0].Pos);

			glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_SHORT, indexList);

			if (Material.DisableFlush == false && Material.Flags[EMF_DISABLEFLUSH] == false)
				glFlush();
			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY );
			glDisableClientState(GL_NORMAL_ARRAY );

			if (multitexturing == true)
			{
				glDepthFunc( GL_EQUAL);
				multipass->setPass(1);
				glBindTexture(GL_TEXTURE_2D,
				              ((COpenGLTexture*)__second_texture)->getOpenGLTextureName());
				glEnableClientState(GL_COLOR_ARRAY);
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY );
				glEnableClientState(GL_NORMAL_ARRAY );

				glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(S3DVertex), &vertices[0].Color);
				glNormalPointer(GL_FLOAT, sizeof(S3DVertex), &vertices[0].Normal);
				glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), &vertices[0].TCoords);
				glVertexPointer(3, GL_FLOAT, sizeof(S3DVertex),  &vertices[0].Pos);

				glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_SHORT, indexList);
				if (Material.DisableFlush == false && Material.Flags[EMF_DISABLEFLUSH] == false)
					glFlush();

				glDisableClientState(GL_COLOR_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY );
				glDisableClientState(GL_NORMAL_ARRAY );
				glDepthFunc( GL_LEQUAL );
			}
			return;

		}

	}

	u16 *indexes;
	BspClipVertex *ptr;
	int b = triangleCount*3, l = 0, i = 0;
	bool run = true;


	ptr = &vertexStack[__lte_current_size_vbo1];
	indexes = &vertexStackIndex[__lte_current_size_vbo2];


	while ( run )
	{
		int o  = VERTEX_PER_NODE;
		if (b < VERTEX_PER_NODE) o = b;

		if (( __lte_current_size_vbo2+l>32768-o*10) || (__lte_current_size_vbo1+i + o*10 > 29127))
		{

			__lte_current_size_vbo2 = 0;
			__lte_current_size_vbo1 = 0;

			sceKernelDcacheWritebackInvalidateRange(ptr, sizeof(BspClipVertex) * i);
			sceKernelDcacheWritebackInvalidateRange(indexes, l * 2);

			__pspgl_context_render_prim(pspgl_curctx, GE_TRIANGLES, l, GE_COLOR_8888|GE_VERTEX_32BITF|GE_VINDEX_16BIT|GE_NORMAL_32BITF|GE_TEXTURE_32BITF|GE_TRANSFORM_3D,
			                            ptr, indexes);
			glFinish();


			l = 0;
			i = 0;
		}
		if (b > VERTEX_PER_NODE ) {

			b-= VERTEX_PER_NODE;
			BSP_Step2(&ptr[i], &indexes[l], (S3DVertex2TCoords*)vertices, &indexList[b], VERTEX_PER_NODE, &l, &i);
		} else {

			run = false;
			BSP_Step2(&ptr[i], &indexes[l], (S3DVertex2TCoords*)vertices, &indexList[0], b, &l, &i);
		}
	}


	sceKernelDcacheWritebackInvalidateRange(ptr, sizeof(BspClipVertex) * i);
	sceKernelDcacheWritebackInvalidateRange(indexes, l * 2);

	__pspgl_context_render_prim(pspgl_curctx, GE_TRIANGLES, l, GE_COLOR_8888|GE_VERTEX_32BITF|GE_VINDEX_16BIT|GE_NORMAL_32BITF|GE_TEXTURE_32BITF|GE_TRANSFORM_3D,
	                            ptr, indexes);

	if (Material.DisableFlush == false && Material.Flags[EMF_DISABLEFLUSH] == false)
		glFlush();
	__lte_current_size_vbo1+=i;
	__lte_current_size_vbo2+=l;


#if 1
	if (multitexturing == true)
	{
		glDepthFunc( GL_EQUAL );
		multipass->setPass(1);
		glBindTexture(GL_TEXTURE_2D,
		              ((COpenGLTexture*)__second_texture)->getOpenGLTextureName());

		__pspgl_context_render_prim(pspgl_curctx, GE_TRIANGLES, l, GE_COLOR_8888|GE_VERTEX_32BITF|GE_VINDEX_16BIT|GE_NORMAL_32BITF|GE_TEXTURE_32BITF|GE_TRANSFORM_3D,
		                            ptr, indexes);

		if (Material.DisableFlush == false && Material.Flags[EMF_DISABLEFLUSH] == false)
			glFlush();


		glDepthFunc( GL_LEQUAL );
	}

   #endif

}


//! draws an indexed triangle list
void CSoftwareDriver2::drawIndexedTriangleList(const S3DVertex2TCoords* vertices, s32 vertexCount,
                                               const u16* indexList, s32 triangleCount)
{

	drawIndexedTriangleList((S3DVertex*)vertices, vertexCount, indexList, triangleCount);

}




//! draws an indexed triangle fan
void CSoftwareDriver2::drawIndexedTriangleFan(const S3DVertex* vertices,
                                              s32 vertexCount, const u16* indexList, s32 triangleCount)
{
	if (!checkPrimitiveCount(triangleCount))
		return;

	CNullDriver::drawIndexedTriangleFan(vertices, vertexCount, indexList, triangleCount);

	setRenderStates3DMode();



	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY );
	glEnableClientState(GL_NORMAL_ARRAY );

	// convert colors to gl color format.

	const S3DVertex* p = vertices;
	ColorBuffer.set_used(vertexCount);
	for (s32 i=0; i<vertexCount; ++i)
	{
		ColorBuffer[i] = p->Color.toOpenGLColor();
		++p;
	}

	// draw everything

	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(video::SColor), &ColorBuffer[0]);
	glNormalPointer(GL_FLOAT, sizeof(S3DVertex), &vertices[0].Normal);
	glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex), &vertices[0].TCoords);
	glVertexPointer(3, GL_FLOAT, sizeof(S3DVertex),  &vertices[0].Pos);

	glDrawElements(GL_TRIANGLE_FAN, triangleCount+2, GL_UNSIGNED_SHORT, indexList);
	glFlush();
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY );
	glDisableClientState(GL_NORMAL_ARRAY );
}



//! draws an indexed triangle fan
void CSoftwareDriver2::drawIndexedTriangleFan(const S3DVertex2TCoords* vertices, s32 vertexCount, const u16* indexList, s32 triangleCount)
{
	if (!checkPrimitiveCount(triangleCount))
		return;

	CNullDriver::drawIndexedTriangleFan(vertices, vertexCount, indexList, triangleCount);

	setRenderStates3DMode();

	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY );
	glEnableClientState(GL_NORMAL_ARRAY );

	// convert colors to gl color format.

	const S3DVertex2TCoords* p = vertices;
	ColorBuffer.set_used(vertexCount);
	for (s32 i=0; i<vertexCount; ++i)
	{
		ColorBuffer[i] = p->Color.toOpenGLColor();
		++p;
	}

	// draw everything

	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(video::SColor), &ColorBuffer[0]);
	glNormalPointer(GL_FLOAT, sizeof(S3DVertex2TCoords), &vertices[0].Normal);
	glVertexPointer(3, GL_FLOAT, sizeof(S3DVertex2TCoords),  &vertices[0].Pos);

	// texture coordiantes
	glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), &vertices[0].TCoords);

	glDrawElements(GL_TRIANGLE_FAN, triangleCount+2, GL_UNSIGNED_SHORT, indexList);

	glFlush();
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);


	glDisableClientState(GL_TEXTURE_COORD_ARRAY );

	glDisableClientState(GL_NORMAL_ARRAY );
}


//! Draws a 2d line.
void CSoftwareDriver2::draw2DLine(const core::position2d<s32>& start,
                                  const core::position2d<s32>& end,
                                  SColor color)
{
	// thanks to Vash TheStampede who sent in his implementation

	setRenderStates2DMode(color.getAlpha() < 255, false, false);
	setTexture(0,0);

	core::dimension2d<s32> currentRendertargetSize = getCurrentRenderTargetSize();
	const s32 xPlus = -(currentRendertargetSize.Width>>1);
	const f32 xFact = 1.0f / (currentRendertargetSize.Width>>1);

	const s32 yPlus =
	        currentRendertargetSize.Height-(currentRendertargetSize.Height>>1);
	const f32 yFact = 1.0f / (currentRendertargetSize.Height>>1);

	core::position2d<f32> npos_start;
	npos_start.X  = (f32)(start.X + xPlus) * xFact;
	npos_start.Y  = (f32)(yPlus - start.Y) * yFact;

	core::position2d<f32> npos_end;
	npos_end.X  = (f32)(end.X + xPlus) * xFact;
	npos_end.Y  = (f32)(yPlus - end.Y) * yFact;

	glBegin(GL_LINES);
	glColor4ub(color.getRed(), color.getGreen(),
	           color.getBlue(),
	           color.getAlpha());
	glVertex2f(npos_start.X, npos_start.Y);
	glVertex2f(npos_end.X,   npos_end.Y);
	glEnd();
}


//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
void CSoftwareDriver2::draw2DImage(video::ITexture* texture, const core::position2d<s32>& pos,
                                   const core::rect<s32>& sourceRect,
                                   const core::rect<s32>* clipRect, SColor color,
                                   bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;


	if (sourceRect.UpperLeftCorner.X >= sourceRect.LowerRightCorner.X ||
	    sourceRect.UpperLeftCorner.Y >= sourceRect.LowerRightCorner.Y)
		return;

	core::position2d<s32> targetPos = pos;
	core::position2d<s32> sourcePos = sourceRect.UpperLeftCorner;
	core::dimension2d<s32> sourceSize(sourceRect.getWidth(), sourceRect.getHeight());
	core::dimension2d<s32> currentRendertargetSize = getCurrentRenderTargetSize();

	const core::dimension2d<s32> targetSurfaceSize = currentRendertargetSize;

	if (clipRect)
	{
		if (targetPos.X < clipRect->UpperLeftCorner.X)
		{
			sourceSize.Width += targetPos.X - clipRect->UpperLeftCorner.X;
			if (sourceSize.Width <= 0)
				return;

			sourcePos.X -= targetPos.X - clipRect->UpperLeftCorner.X;
			targetPos.X = clipRect->UpperLeftCorner.X;
		}
		if (targetPos.X + sourceSize.Width > clipRect->LowerRightCorner.X)
		{
			sourceSize.Width -= (targetPos.X + sourceSize.Width) - clipRect->LowerRightCorner.X;
			if (sourceSize.Width <= 0)
				return;
		}

		if (targetPos.Y < clipRect->UpperLeftCorner.Y)
		{
			sourceSize.Height += targetPos.Y - clipRect->UpperLeftCorner.Y;
			if (sourceSize.Height <= 0)
				return;

			sourcePos.Y -= targetPos.Y - clipRect->UpperLeftCorner.Y;
			targetPos.Y = clipRect->UpperLeftCorner.Y;
		}

		if (targetPos.Y + sourceSize.Height > clipRect->LowerRightCorner.Y)
		{
			sourceSize.Height -= (targetPos.Y + sourceSize.Height) - clipRect->LowerRightCorner.Y;
			if (sourceSize.Height <= 0)
				return;
		}
	}


	// clip these coordinates

	if (targetPos.X<0)
	{
		sourceSize.Width += targetPos.X;
		if (sourceSize.Width <= 0)
			return;

		sourcePos.X -= targetPos.X;
		targetPos.X = 0;
	}

	if (targetPos.X + sourceSize.Width > targetSurfaceSize.Width)
	{
		sourceSize.Width -= (targetPos.X + sourceSize.Width) - targetSurfaceSize.Width;
		if (sourceSize.Width <= 0)
			return;
	}

	if (targetPos.Y<0)
	{
		sourceSize.Height += targetPos.Y;
		if (sourceSize.Height <= 0)
			return;

		sourcePos.Y -= targetPos.Y;
		targetPos.Y = 0;
	}

	if (targetPos.Y + sourceSize.Height > targetSurfaceSize.Height)
	{
		sourceSize.Height -= (targetPos.Y + sourceSize.Height) - targetSurfaceSize.Height;
		if (sourceSize.Height <= 0)
			return;
	}

	// ok, we've clipped everything.
	// now draw it.

	setTexture(0, texture);
	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());

	if (useAlphaChannelOfTexture)
		setRenderStates2DMode(false, true, true);
	else
		setRenderStates2DMode(false, true, false);
	//setRenderStates2DMode(false,true,false);


	if (sourceSize.Height >= 271)
		sourceSize.Height++;

	core::rect<s32> poss(targetPos, sourceSize);

	s32 xPlus = currentRendertargetSize.Width/2;
	f32 xFact = 1.0f / (currentRendertargetSize.Width>>1);

	s32 yPlus = currentRendertargetSize.Height/2;
	f32 yFact = 1.0f / ((currentRendertargetSize.Height)>>1);

	const core::dimension2d<s32> sourceSurfaceSize = texture->getOriginalSize();
	core::rect<f32> tcoords;
	tcoords.UpperLeftCorner.X = (((f32)sourcePos.X)) / texture->getOriginalSize().Width ;
	tcoords.UpperLeftCorner.Y = (((f32)sourcePos.Y)) / texture->getOriginalSize().Height;
	tcoords.LowerRightCorner.X = (((f32)sourcePos.X  + (f32)sourceSize.Width)) / texture->getOriginalSize().Width;
	tcoords.LowerRightCorner.Y = (((f32)sourcePos.Y + (f32)sourceSize.Height)) / texture->getOriginalSize().Height;

	core::rect<f32> npos;

	float WidthMid = targetSurfaceSize.Width/2;
	float HeightMid = targetSurfaceSize.Height/2;

	npos.UpperLeftCorner.X = poss.UpperLeftCorner.X/WidthMid - 1.f;
	npos.UpperLeftCorner.Y = (f32)(1.0f-poss.UpperLeftCorner.Y/HeightMid);
	npos.LowerRightCorner.X = (f32)(poss.LowerRightCorner.X/WidthMid-1.f);
	npos.LowerRightCorner.Y = (f32)(1.0f-poss.LowerRightCorner.Y/HeightMid);

    #if 0
	struct {
		float u,v;
		float x,y,z;
	} __maxvtx[64];


	const float slice_step = 0.0625;
	int num_slices = (int)((fabs(npos.UpperLeftCorner.X-npos.LowerRightCorner.X))/slice_step);

	float uv_step = 0;

	if (num_slices)
		uv_step = (fabs(tcoords.UpperLeftCorner.X-tcoords.LowerRightCorner.X)) / num_slices;

	float __start_x = npos.UpperLeftCorner.X;
	float __start_u = tcoords.UpperLeftCorner.X;

	for (int x = 0; x < num_slices*2 ; x+=2)
	{

		__maxvtx[x].x = __start_x;
		__maxvtx[x].y = npos.UpperLeftCorner.Y;

		__start_x += slice_step;

		__maxvtx[x+1].x = __start_x;
		__maxvtx[x+1].y = npos.LowerRightCorner.Y;

		__maxvtx[x].z = 0;
		__maxvtx[x+1].z = 0;

		__maxvtx[x].v = tcoords.UpperLeftCorner.Y;
		__maxvtx[x+1].v = tcoords.LowerRightCorner.Y;

		__maxvtx[x].u = __start_u;

		__start_u += uv_step;

		__maxvtx[x+1].u = __start_u;

	}

	if (__start_x != npos.LowerRightCorner.X)
	{
		__maxvtx[ num_slices * 2 ].z = __maxvtx[ num_slices * 2 + 1].z = 0;

		__maxvtx[ num_slices * 2 ].x = __start_x;
		__maxvtx[ num_slices * 2 + 1 ].x = npos.LowerRightCorner.X;

		__maxvtx[ num_slices * 2 ].y = npos.UpperLeftCorner.Y;
		__maxvtx[ num_slices * 2 + 1 ].y = npos.LowerRightCorner.Y;

		__maxvtx[ num_slices * 2 ].u = __start_u;
		__maxvtx[ num_slices * 2 + 1 ].u = tcoords.LowerRightCorner.X;

		__maxvtx[ num_slices * 2 ].v = tcoords.UpperLeftCorner.Y;
		__maxvtx[ num_slices * 2 + 1 ].v = tcoords.LowerRightCorner.Y;

		num_slices += 2;

	}


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer(2, GL_FLOAT, 20, (GLvoid*)(0 + (unsigned int)__maxvtx));
	glVertexPointer(3, GL_FLOAT, 20, (GLvoid*)(8 + (unsigned int)__maxvtx));
	glDrawArrays(GL_SPRITES_PSP, 0, num_slices * 2);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY );
	glDisableClientState(GL_VERTEX_ARRAY);


  #else
	glBegin(GL_SPRITES_PSP);

	glTexCoord2f(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	glVertex2f(npos.UpperLeftCorner.X, npos.UpperLeftCorner.Y);

	glTexCoord2f(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	glVertex2f(npos.LowerRightCorner.X, npos.LowerRightCorner.Y);

	glEnd();
	#endif
}


void CSoftwareDriver2::draw2DStretchedImage(video::ITexture* texture, const core::position2d<s32>& pos,
                                            const core::dimension2d<s32>& size, core::rect<s32>& sourceRect,
                                            SColor color,
                                            bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	core::dimension2d<s32> currentRendertargetSize = getCurrentRenderTargetSize();

	const core::dimension2d<s32> targetSurfaceSize = currentRendertargetSize;


	setTexture(0, texture);
	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());

	if (useAlphaChannelOfTexture)
		setRenderStates2DMode(false, true, true);
	else
		setRenderStates2DMode(false, true, false);

	core::rect<f32> tcoords;
	tcoords.UpperLeftCorner.X = (((f32)sourceRect.UpperLeftCorner.X)) / texture->getOriginalSize().Width ;
	tcoords.UpperLeftCorner.Y = (((f32)sourceRect.UpperLeftCorner.Y)) / texture->getOriginalSize().Height;
	tcoords.LowerRightCorner.X = (((f32)sourceRect.LowerRightCorner.X )) / texture->getOriginalSize().Width;
	tcoords.LowerRightCorner.Y = (((f32)sourceRect.LowerRightCorner.Y)) / texture->getOriginalSize().Height;

	core::rect<f32> npos;

	float WidthMid = targetSurfaceSize.Width/2;
	float HeightMid = targetSurfaceSize.Height/2;

	npos.UpperLeftCorner.X = pos.X/WidthMid - 1.f;
	npos.UpperLeftCorner.Y = (f32)(1.0f-pos.Y/HeightMid);
	npos.LowerRightCorner.X = (f32)((pos.X+size.Width)/WidthMid-1.f);
	npos.LowerRightCorner.Y = (f32)(1.0f-(pos.Y+size.Height)/HeightMid);

	glBegin(GL_SPRITES_PSP);

	glTexCoord2f(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	glVertex2f(npos.UpperLeftCorner.X, npos.UpperLeftCorner.Y);

	glTexCoord2f(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	glVertex2f(npos.LowerRightCorner.X, npos.LowerRightCorner.Y);

	glEnd();

}

//! draws an 2d image
void CSoftwareDriver2::draw2DImage(video::ITexture* texture, const core::position2d<s32>& destPos)
{
	if (!texture)
		return;

	draw2DImage(texture, destPos, core::rect<s32>(core::position2d<s32>(0,0),
	                                              texture->getOriginalSize()));
}

//! Draws a 3d line.
void CSoftwareDriver2::draw3DLine(const core::vector3df& start,
                                  const core::vector3df& end, SColor color)
{
	setRenderStates3DMode();

	glBegin(GL_LINES);
	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	glVertex3f(start.X, start.Y, start.Z);

	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	glVertex3f(end.X, end.Y, end.Z);
	glEnd();
}




//! sets the needed renderstates
void CSoftwareDriver2::setRenderStates3DMode()
{
	if (CurrentRenderMode != ERM_3D)
	{
		// switch back the matrices
		GLfloat glmat[16], glmat2[16];

		createGLMatrix(glmat, Matrizes[ETS_VIEW] * Matrizes[ETS_WORLD]);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glmat);

		createGLMatrix(glmat2, Matrizes[ETS_PROJECTION]);
		glmat2[12]*= -1.0f;
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glmat2);

		ResetRenderStates = true;
		f32 modelMatrix[16], projectionMatrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, modelMatrix);
		glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
		CalculateViewFrustum( &myVFrustum, projectionMatrix, modelMatrix );
	}

	if (ResetRenderStates)
		glFrontFace(GL_CW);

	if ( ResetRenderStates || LastMaterial != Material)
	{
		// unset old material

		if (LastMaterial.MaterialType != Material.MaterialType &&
		    LastMaterial.MaterialType >= 0 && LastMaterial.MaterialType < (s32)MaterialRenderers.size())
			MaterialRenderers[LastMaterial.MaterialType]->OnUnsetMaterial();

		// set new material.

		if (Material.MaterialType >= 0 && Material.MaterialType < (s32)MaterialRenderers.size())
			MaterialRenderers[Material.MaterialType]->OnSetMaterial(
			        Material, LastMaterial, ResetRenderStates, this);
	}

	bool shaderOK = true;

	if (Material.MaterialType >= 0 && Material.MaterialType < (s32)MaterialRenderers.size())
		shaderOK = MaterialRenderers[Material.MaterialType]->OnRender(this, video::EVT_STANDARD);

	LastMaterial = Material;

	ResetRenderStates = false;

	CurrentRenderMode = ERM_3D;
}

//!Draws an 2d rectangle with a gradient.
void CSoftwareDriver2::draw2DRectangle(const core::rect<s32>& position,
                                       SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
                                       const core::rect<s32>* clip)
{
	setRenderStates2DMode(colorLeftUp.getAlpha() < 255 ||
	                      colorRightUp.getAlpha() < 255 ||
	                      colorLeftDown.getAlpha() < 255 ||
	                      colorRightDown.getAlpha() < 255, false, false);

	setTexture(0,0);

	core::rect<s32> pos = position;

	if (clip)
		pos.clipAgainst(*clip);

	if (!pos.isValid())
		return;

	core::dimension2d<s32> currentRendertargetSize = getCurrentRenderTargetSize();
	s32 xPlus = -(currentRendertargetSize.Width>>1);
	f32 xFact = 1.0f / (currentRendertargetSize.Width>>1);

	s32 yPlus = currentRendertargetSize.Height-(currentRendertargetSize.Height>>1);
	f32 yFact = 1.0f / (currentRendertargetSize.Height>>1);

	core::rect<f32> npos;
	npos.UpperLeftCorner.X = (f32)(pos.UpperLeftCorner.X+xPlus) * xFact;
	npos.UpperLeftCorner.Y = (f32)(yPlus-pos.UpperLeftCorner.Y) * yFact;
	npos.LowerRightCorner.X = (f32)(pos.LowerRightCorner.X+xPlus) * xFact;
	npos.LowerRightCorner.Y = (f32)(yPlus-pos.LowerRightCorner.Y) * yFact;

//  __draw_quad_col(npos, colorLeftUp,colorLeftDown, colorRightUp,colorRightDown);

	glBegin(GL_QUADS);
	glColor4ub(colorLeftUp.getRed(), colorLeftUp.getGreen(),
	           colorLeftUp.getBlue(), colorLeftUp.getAlpha());
	glVertex2f(npos.UpperLeftCorner.X, npos.UpperLeftCorner.Y);

	glColor4ub(colorRightUp.getRed(), colorRightUp.getGreen(),
	           colorRightUp.getBlue(), colorRightUp.getAlpha());
	glVertex2f(npos.LowerRightCorner.X, npos.UpperLeftCorner.Y);

	glColor4ub(colorRightDown.getRed(), colorRightDown.getGreen(),
	           colorRightDown.getBlue(), colorRightDown.getAlpha());
	glVertex2f(npos.LowerRightCorner.X, npos.LowerRightCorner.Y);

	glColor4ub(colorLeftDown.getRed(), colorLeftDown.getGreen(),
	           colorLeftDown.getBlue(), colorLeftDown.getAlpha());
	glVertex2f(npos.UpperLeftCorner.X, npos.LowerRightCorner.Y);

	glEnd();
}



void CSoftwareDriver2::draw2DRectangle(SColor color, const core::rect<s32>& position,
                                       const core::rect<s32>* clip)
{
	setRenderStates2DMode(color.getAlpha() < 255, false, false);
	setTexture(0,0);
	core::rect<s32> pos = position;

	if (clip)
		pos.clipAgainst(*clip);

	if (!pos.isValid())
		return;

	core::dimension2d<s32> currentRendertargetSize = getCurrentRenderTargetSize();
	s32 xPlus = -(currentRendertargetSize.Width>>1);
	f32 xFact = 1.0f / (currentRendertargetSize.Width>>1);

	s32 yPlus = currentRendertargetSize.Height-(currentRendertargetSize.Height>>1);
	f32 yFact = 1.0f / (currentRendertargetSize.Height>>1);

	core::rect<f32> npos;
	npos.UpperLeftCorner.X = (f32)(pos.UpperLeftCorner.X+xPlus) * xFact;
	npos.UpperLeftCorner.Y = (f32)(yPlus-pos.UpperLeftCorner.Y) * yFact;
	npos.LowerRightCorner.X = (f32)(pos.LowerRightCorner.X+xPlus) * xFact;
	npos.LowerRightCorner.Y = (f32)(yPlus-pos.LowerRightCorner.Y) * yFact;

	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
// __draw_quad_empty(npos, color.toOpenGLColor());

	glBegin(GL_SPRITES_PSP);

	glVertex2f(npos.UpperLeftCorner.X, npos.UpperLeftCorner.Y);
	//glVertex2f(npos.LowerRightCorner.X, npos.UpperLeftCorner.Y);
	glVertex2f(npos.LowerRightCorner.X, npos.LowerRightCorner.Y);
	//glVertex2f(npos.UpperLeftCorner.X, npos.LowerRightCorner.Y);
	glEnd();
}


//! \return Returns the name of the video driver. Example: In case of the DirectX8
//! driver, it would return "Direct3D8.1".
const wchar_t* CSoftwareDriver2::getName()
{

	return L"PSPGL 1.0";

}

//! Returns type of video driver
E_DRIVER_TYPE CSoftwareDriver2::getDriverType()
{
	return EDT_OPENGL;
}

//! Returns the transformation set by setTransform
core::matrix4 CSoftwareDriver2::getTransform(E_TRANSFORMATION_STATE state)
{
	return Matrizes[state];
}



//! Clears the ZBuffer.
void CSoftwareDriver2::clearZBuffer()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}


//! returns a device dependent texture from a software surface (IImage)
//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
ITexture* CSoftwareDriver2::createDeviceDependentTexture(IImage* surface)
{

	bool generateMipLevels = getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);

	return new COpenGLTexture(surface, generateMipLevels);
	//return new COpenGLTexture(surface, false);
}


//! creates a video driver
IVideoDriver* createSoftwareDriver2(const core::dimension2d<s32>& windowSize, bool fullscreen, io::IFileSystem* io, video::IImagePresenter* presenter, gui::ICursorControl* CursorControl, bool showLogo)
{
	return new CSoftwareDriver2(windowSize, fullscreen, io, presenter, CursorControl, showLogo);
}


} // end namespace video
} // end namespace engine
