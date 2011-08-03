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
 

#ifndef __C_OPENGL_MATERIAL_RENDERER_H_INCLUDED__
#define __C_OPENGL_MATERIAL_RENDERER_H_INCLUDED__

#include "engineCompileConfig.h"


#include <GL/gl.h>
#include <GL/glu.h>


#include "CSoftwareDriver2.h"
#include "IMaterialRenderer.h"
/* Color Macros, maps 8 bit unsigned channels into one 32-bit value */
#define GU_ABGR(a,b,g,r)        (((a) << 24)|((b) << 16)|((g) << 8)|(r))
#define GU_ARGB(a,r,g,b)        GU_ABGR((a),(b),(g),(r))
#define GU_RGBA(r,g,b,a)        GU_ARGB((a),(r),(g),(b))

/* Color Macro, maps floating point channels (0..1) into one 32-bit value */
#define GU_COLOR(r,g,b,a)       GU_RGBA((u32)((r) * 255.0f),(u32)((g) * 255.0f),(u32)((b) * 255.0f),(u32)((a) * 255.0f))


extern "C"
{
	void glBlendFuncFixPSP (GLuint srccolor, GLuint dstcolor);
}
namespace engine
{
namespace video
{

//! Base class for all internal D3D9 material renderers
class COpenGLMaterialRenderer : public IMaterialRenderer
{
public:

//! Constructor
COpenGLMaterialRenderer(video::CSoftwareDriver2* driver)
	: Driver(driver)
{
}

protected:

video::CSoftwareDriver2* Driver;
};


class COpenGLMultipassRender {

public:
virtual bool setPass(int i);
};


class COpenGLMultipassRender_dummy : public COpenGLMultipassRender
{
virtual bool setPass(int i)
{
	switch (i)
	{
	case 0:
		return true;
		break;
	case 1:
		return false;
		break;
	}
}
};


class COpenGLMultipassRender_REFLECTION_2_LAYER : public COpenGLMultipassRender
{
virtual bool setPass(int i)
{
	switch (i)
	{
	case 0:
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);


		return true;
		break;

	case 1:
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);

		glBlendFuncFixPSP(
		        GU_COLOR(0.3f, 0.3f, 0.3f, 0.f),
		        GU_COLOR(0.7f, 0.7f, 0.7f, 0.f));

		glEnable(GL_BLEND);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		return true;
		break;
	}
}
};


class COpenGLMultipassRender_TRANSPARENT_REFLECTION_2_LAYER : public COpenGLMultipassRender
{
virtual bool setPass(int i)
{
	switch (i)
	{
	case 0:
		glDisable(GL_ALPHA_TEST);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
		glEnable(GL_BLEND);

		return true;
		break;

	case 1:
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		return true;
		break;
	}
}
};

class COpenGLMultipassRender_SOLID_2_LAYER : public COpenGLMultipassRender
{
virtual bool setPass(int i)
{
	switch (i)
	{
	case 0:
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);


		return true;
		break;

	case 1:
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
		glEnable(GL_BLEND);
		return true;
		break;
	}
}
};

//! Solid material renderer
class COpenGLMaterialRenderer_SOLID : public COpenGLMaterialRenderer
{
public:

COpenGLMaterialRenderer_SOLID(video::CSoftwareDriver2* d)
	: COpenGLMaterialRenderer(d) {
}

virtual void OnSetMaterial(SMaterial& material, const SMaterial& lastMaterial,
                           bool resetAllRenderstates, IMaterialRendererServices* services)
{
	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	{

		// thanks to Murphy, the following line removed some bugs with several OpenGL
		// implementations.

		        #if 1
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
      #endif
	}
	Driver->setMultipassRender(0);
	services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}
};


//! Solid 2 layer material renderer
class COpenGLMaterialRenderer_SOLID_2_LAYER : public COpenGLMaterialRenderer
{
public:

COpenGLMaterialRenderer_SOLID_2_LAYER(video::CSoftwareDriver2* d)
	: COpenGLMaterialRenderer(d) {
}

virtual void OnSetMaterial(SMaterial& material, const SMaterial& lastMaterial,
                           bool resetAllRenderstates, IMaterialRendererServices* services)
{
	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	{
	}
	Driver->setMultipassRender(3);
	services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}
};


//! Transparent add color material renderer
class COpenGLMaterialRenderer_TRANSPARENT_ADD_COLOR : public COpenGLMaterialRenderer
{
public:

COpenGLMaterialRenderer_TRANSPARENT_ADD_COLOR(video::CSoftwareDriver2* d)
	: COpenGLMaterialRenderer(d) {
}

virtual void OnSetMaterial(SMaterial& material, const SMaterial& lastMaterial,
                           bool resetAllRenderstates, IMaterialRendererServices* services)
{
	//if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	{
     #if 1
		glDisable(GL_ALPHA_TEST);

		Driver->setTexture(1,0);

		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_BLEND);
     #endif
	}
	Driver->setMultipassRender(0);
	material.ZWriteEnable = false;
	services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}

//! Returns if the material is transparent.
virtual bool isTransparent()
{
	return true;
}
};


//! Transparent vertex alpha material renderer
class COpenGLMaterialRenderer_TRANSPARENT_VERTEX_ALPHA : public COpenGLMaterialRenderer
{
public:

COpenGLMaterialRenderer_TRANSPARENT_VERTEX_ALPHA(video::CSoftwareDriver2* d)
	: COpenGLMaterialRenderer(d) {
}

virtual void OnSetMaterial(SMaterial& material, const SMaterial& lastMaterial,
                           bool resetAllRenderstates, IMaterialRendererServices* services)
{
	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	{
      #if 1
		glDisable(GL_ALPHA_TEST);

		Driver->setTexture(1,0);


		glDisable(GL_ALPHA_TEST);

		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
      #endif
	}
	Driver->setMultipassRender(0);
	material.ZWriteEnable = false;
	services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}

virtual void OnUnsetMaterial()
{
	// default values
    #if 1
	glDisable(GL_BLEND);
    #endif
}

//! Returns if the material is transparent.
virtual bool isTransparent()
{
	return true;
}
};


//! Transparent alpha channel material renderer
class COpenGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL : public COpenGLMaterialRenderer
{
public:

COpenGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL(video::CSoftwareDriver2* d)
	: COpenGLMaterialRenderer(d) {
}

virtual void OnSetMaterial(SMaterial& material, const SMaterial& lastMaterial,
                           bool resetAllRenderstates, IMaterialRendererServices* services)
{
	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	{


      #if 1
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		glEnable(GL_ALPHA_TEST);

		f32 refValue = material.MaterialTypeParam;
		if ( refValue == 0 )
			refValue = 0.5f;

		glAlphaFunc(GL_GREATER, refValue);
      #endif
	}
	Driver->setMultipassRender(0);
	//material.ZWriteEnable = false;
	services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}

virtual void OnUnsetMaterial()
{
	glDisable(GL_ALPHA_TEST);
}

//! Returns if the material is transparent.
virtual bool isTransparent()
{
	return true;
}
};



//! Transparent alpha channel material renderer
class COpenGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF : public COpenGLMaterialRenderer
{
public:

COpenGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF(video::CSoftwareDriver2* d)
	: COpenGLMaterialRenderer(d) {
}

virtual void OnSetMaterial(SMaterial& material, const SMaterial& lastMaterial,
                           bool resetAllRenderstates, IMaterialRendererServices* services)
{
	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	{
      #if 1
		glEnable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);

		glAlphaFunc(GL_GREATER, 0.5);
      #endif
	}
	Driver->setMultipassRender(0);
	//material.ZWriteEnable = false;
	services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}

virtual void OnUnsetMaterial()
{
	#if 1
	glDisable(GL_ALPHA_TEST);
	#endif
}

//! Returns if the material is transparent.
virtual bool isTransparent()
{
	return false;          // this material is not really transparent because it does no blending.
}
};


//! material renderer for all kinds of linghtmaps
class COpenGLMaterialRenderer_LIGHTMAP : public COpenGLMaterialRenderer
{
public:

COpenGLMaterialRenderer_LIGHTMAP(video::CSoftwareDriver2* d)
	: COpenGLMaterialRenderer(d) {
}

virtual void OnSetMaterial(SMaterial& material, const SMaterial& lastMaterial,
                           bool resetAllRenderstates, IMaterialRendererServices* services)
{
	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	{

	}
	Driver->setMultipassRender(0);
	services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}
};



//! material renderer for all kinds of linghtmaps
class COpenGLMaterialRenderer_DETAIL_MAP : public COpenGLMaterialRenderer
{
public:

COpenGLMaterialRenderer_DETAIL_MAP(video::CSoftwareDriver2* d)
	: COpenGLMaterialRenderer(d) {
}

virtual void OnSetMaterial(SMaterial& material, const SMaterial& lastMaterial,
                           bool resetAllRenderstates, IMaterialRendererServices* services)
{
	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	{

	}
	Driver->setMultipassRender(0);
	services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}
};


//! sphere map material renderer
class COpenGLMaterialRenderer_SPHERE_MAP : public COpenGLMaterialRenderer
{
public:

COpenGLMaterialRenderer_SPHERE_MAP(video::CSoftwareDriver2* d)
	: COpenGLMaterialRenderer(d) {
}

virtual void OnSetMaterial(SMaterial& material, const SMaterial& lastMaterial,
                           bool resetAllRenderstates, IMaterialRendererServices* services)
{
	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	{

      #if 1
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);

		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
      #endif
	}
	Driver->setMultipassRender(0);
	services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}

virtual void OnUnsetMaterial()
{
    #if 1
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
    #endif
}
};


//! reflection 2 layer material renderer
class COpenGLMaterialRenderer_REFLECTION_2_LAYER : public COpenGLMaterialRenderer
{
public:

COpenGLMaterialRenderer_REFLECTION_2_LAYER(video::CSoftwareDriver2* d)
	: COpenGLMaterialRenderer(d) {
}

virtual void OnSetMaterial(SMaterial& material, const SMaterial& lastMaterial,
                           bool resetAllRenderstates, IMaterialRendererServices* services)
{
	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	{
		Driver->setMultipassRender(1);
	}
	services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}

virtual void OnUnsetMaterial()
{

	    #if 1
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
      #endif

}
};


//! reflection 2 layer material renderer
class COpenGLMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER : public COpenGLMaterialRenderer
{
public:

COpenGLMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER(video::CSoftwareDriver2* d)
	: COpenGLMaterialRenderer(d) {
}

virtual void OnSetMaterial(SMaterial& material, const SMaterial& lastMaterial,
                           bool resetAllRenderstates, IMaterialRendererServices* services)
{
	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	{

      #if 1
      #endif
	}
	Driver->setMultipassRender(2);
	material.ZWriteEnable = false;
	services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}

virtual void OnUnsetMaterial()
{
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
}

//! Returns if the material is transparent.
virtual bool isTransparent()
{
	return true;
}
};

} // end namespace video
} // end namespace engine

#endif
