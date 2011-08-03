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
 
#include "COpenGLDriver.h"

#include "engineCompileConfig.h"
#ifdef _engine_COMPILE_WITH_OPENGL_

#include "COpenGLTexture.h"
#include "COpenGLMaterialRenderer.h"
#include "COpenGLShaderMaterialRenderer.h"
#include "COpenGLSLMaterialRenderer.h"
#include "COpenGLNormalMapRenderer.h"
#include "COpenGLParallaxMapRenderer.h"
#include "os.h"
#include <stdlib.h>
#include <string.h>

namespace engine
{
namespace video
{

// -----------------------------------------------------------------------
// WINDOWS CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _engine_WINDOWS_
//! Windows constructor and init code
COpenGLDriver::COpenGLDriver(const core::dimension2d<s32>& screenSize, HWND window, bool fullscreen, bool stencilBuffer, io::IFileSystem* io)
	: CNullDriver(io, screenSize), HDc(0), HRc(0), Window(window),
	CurrentRenderMode(ERM_NONE), ResetRenderStates(true), StencilBuffer(stencilBuffer),
	Transformation3DChanged(true), LastSetLight(-1), MultiTextureExtension(false),
	MaxTextureUnits(1), pGlActiveTextureARB(0), pGlClientActiveTextureARB(0),
	ARBVertexProgramExtension(false), ARBFragmentProgramExtension(false),
	pGlGenProgramsARB(0), pGlBindProgramARB(0), pGlProgramStringARB(0),
	pGlDeleteProgramsARB(0), pGlProgramLocalParameter4fvARB(0),
	ARBShadingLanguage100Extension(false),
	RenderTargetTexture(0), MaxAnisotropy(1), AnisotropyExtension(false),
	CurrentRendertargetSize(0,0), pGlCreateShaderObjectARB(0), pGlShaderSourceARB(0),
	pGlCompileShaderARB(0), pGlCreateProgramObjectARB(0), pGlAttachObjectARB(0),
	pGlLinkProgramARB(0), pGlUseProgramObjectARB(0), pGlDeleteObjectARB(0),
	pGlGetObjectParameterivARB(0), pGlGetUniformLocationARB(0), pGlUniform4fvARB(0),
	pGlUniform1fvARB(0), pGlUniform2fvARB(0), pGlUniform3fvARB(0), pGlUniformMatrix2fvARB(0),
	pGlUniformMatrix3fvARB(0), pGlUniformMatrix4fvARB(0), pGlGetActiveUniformARB(0),
	wglSwapIntervalEXT(0)
{
	#ifdef _DEBUG
	setDebugName("COpenGLDriver");
	#endif

	printVersion();
}

//! inits the open gl driver
bool COpenGLDriver::initDriver(const core::dimension2d<s32>& screenSize, HWND window,
                               bool fullscreen, bool vsync)
{
	static PIXELFORMATDESCRIPTOR pfd =     {
		sizeof(PIXELFORMATDESCRIPTOR),  // Size Of This Pixel Format Descriptor
		1,                                                      // Version Number
		PFD_DRAW_TO_WINDOW |            // Format Must Support Window
		PFD_SUPPORT_OPENGL |            // Format Must Support OpenGL
		PFD_DOUBLEBUFFER,                       // Must Support Double Buffering
		PFD_TYPE_RGBA,                          // Request An RGBA Format
		16,                                                     // Select Our Color Depth
		0, 0, 0, 0, 0, 0,                       // Color Bits Ignored
		0,                                                      // No Alpha Buffer
		0,                                                      // Shift Bit Ignored
		0,                                                      // No Accumulation Buffer
		0, 0, 0, 0,                                     // Accumulation Bits Ignored
		16,                                                     // 16Bit Z-Buffer (Depth Buffer)
		StencilBuffer ? 1 : 0,          // Stencil Buffer
		0,                                                      // No Auxiliary Buffer
		PFD_MAIN_PLANE,                         // Main Drawing Layer
		0,                                                      // Reserved
		0, 0, 0                                         // Layer Masks Ignored
	};

	for (int i=0; i<3; ++i)
	{
		if (i == 1)
		{
			os::Printer::log("Cannot create a GL device with stencil buffer, disabling stencil shadows.", ELL_WARNING);
			StencilBuffer = false;
			pfd.cStencilBits = 0;
		}
		else
		if (i == 2)
		{
			os::Printer::log("Cannot create a GL device context.", ELL_ERROR);
			return false;
		}

		// get hdc
		if (!(HDc=GetDC(window)))
		{
			os::Printer::log("Cannot create a GL device context.", ELL_ERROR);
			continue;
		}

		GLuint PixelFormat;

		// choose pixelformat
		if (!(PixelFormat = ChoosePixelFormat(HDc, &pfd)))
		{
			os::Printer::log("Cannot find a suitable pixelformat.", ELL_ERROR);
			continue;
		}

		// set pixel format
		if(!SetPixelFormat(HDc, PixelFormat, &pfd))
		{
			os::Printer::log("Cannot set the pixel format.", ELL_ERROR);
			continue;
		}

		// create rendering context
		if (!(HRc=wglCreateContext(HDc)))
		{
			os::Printer::log("Cannot create a GL rendering context.", ELL_ERROR);
			continue;
		}

		// activate rendering context
		if(!wglMakeCurrent(HDc, HRc))
		{
			os::Printer::log("Cannot activate GL rendering context", ELL_ERROR);
			continue;
		}

		break;
	}

	// print renderer information
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	if (renderer && vendor)
		os::Printer::log((const c8*)renderer, (const c8*)vendor, ELL_INFORMATION);


	// load extensions
	loadExtensions();

	glViewport(0, 0, screenSize.Width, screenSize.Height); // Reset The Current Viewport
	glShadeModel(GL_SMOOTH);
	setAmbientLight(SColor(0,0,0,0));
	glClearDepth(1.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glDepthFunc(GL_LEQUAL);

	// set vsync
	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT(vsync ? 1 : 0);

	// create material renderers
	createMaterialRenderers();

	// set the renderstates
	ResetRenderStates = true;
	setRenderStates3DMode();

	// set fog mode
	setFog(FogColor, LinearFog, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

	// set exposed data
	ExposedData.OpenGLWin32.HDc = reinterpret_cast<s32>(HDc);
	ExposedData.OpenGLWin32.HRc = reinterpret_cast<s32>(HRc);
	ExposedData.OpenGLWin32.HWnd = reinterpret_cast<s32>(Window);

	return true;
}



//! win32 destructor
COpenGLDriver::~COpenGLDriver()
{
	deleteMaterialRenders();

	// I get a blue screen on my laptop, when I do not delete the
	// textures manually before releasing the dc. Oh how I love this.

	deleteAllTextures();

	if (HRc)
	{
		if (!wglMakeCurrent(0, 0))
			os::Printer::log("Release of dc and rc failed.", ELL_WARNING);

		if (!wglDeleteContext(HRc))
			os::Printer::log("Release of rendering context failed.", ELL_WARNING);

		HRc = 0;
	}

	if (HDc)
		ReleaseDC(Window, HDc);

	HDc = 0;
}
#endif

// -----------------------------------------------------------------------
// MACOSX CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef MACOSX
//! Windows constructor and init code
COpenGLDriver::COpenGLDriver(const core::dimension2d<s32>& screenSize, bool fullscreen, bool stencilBuffer, CengineDeviceMacOSX *device, io::IFileSystem* io)
	: CNullDriver(io, screenSize),
	CurrentRenderMode(ERM_NONE), ResetRenderStates(true), StencilBuffer(stencilBuffer),
	Transformation3DChanged(true), LastSetLight(-1), MultiTextureExtension(false),
	MaxTextureUnits(1), ARBVertexProgramExtension(false), ARBFragmentProgramExtension(false), ARBShadingLanguage100Extension(false),
	RenderTargetTexture(0), MaxAnisotropy(1), AnisotropyExtension(false),
	CurrentRendertargetSize(0,0), _device(device)
{
	#ifdef _DEBUG
	setDebugName("COpenGLDriver");
	#endif

	printVersion();

	loadExtensions();

	// create material renderers
	createMaterialRenderers();

	// set fog mode
	setFog(FogColor, LinearFog, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);
}

COpenGLDriver::~COpenGLDriver()
{
	deleteAllTextures();
}

#endif

// -----------------------------------------------------------------------
// LINUX CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef LINUX
//! Linux constructor and init code
COpenGLDriver::COpenGLDriver(const core::dimension2d<s32>& screenSize, bool fullscreen, bool doublebuffer, bool stencilBuffer, Window window, Display* display, io::IFileSystem* io)
	: CNullDriver(io, screenSize),
	CurrentRenderMode(ERM_NONE), ResetRenderStates(true), StencilBuffer(stencilBuffer),
	Transformation3DChanged(true), LastSetLight(-1), MultiTextureExtension(false),
	MaxTextureUnits(1), DoubleBuffered(doublebuffer), XWindow(window), XDisplay(display),
	ARBVertexProgramExtension(false), ARBFragmentProgramExtension(false),
	ARBShadingLanguage100Extension(false),
	RenderTargetTexture(0), MaxAnisotropy(1), AnisotropyExtension(false),
	CurrentRendertargetSize(0,0)
{
	#ifdef _DEBUG
	setDebugName("COpenGLDriver");
	#endif

	printVersion();

	loadExtensions();

	// create material renderers
	createMaterialRenderers();

	// set fog mode
	setFog(FogColor, LinearFog, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

	ExposedData.OpenGLLinux.Window = XWindow;
}

//! linux destructor
COpenGLDriver::~COpenGLDriver()
{
	deleteAllTextures();
}

// extensions
#ifdef _engine_LINUX_OPENGL_USE_EXTENSIONS_
PFNGLACTIVETEXTUREARBPROC pGlActiveTextureARB = 0;
PFNGLCLIENTACTIVETEXTUREARBPROC pGlClientActiveTextureARB = 0;
//PFNGLACTIVETEXTUREPROC pGlActiveTextureARB = 0;
//PFNGLCLIENTACTIVETEXTUREPROC pGlClientActiveTextureARB = 0;
PFNGLGENPROGRAMSARBPROC pGlGenProgramsARB = 0;
PFNGLBINDPROGRAMARBPROC pGlBindProgramARB = 0;
PFNGLPROGRAMSTRINGARBPROC pGlProgramStringARB = 0;
PFNGLDELETEPROGRAMSNVPROC pGlDeleteProgramsARB = 0;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC pGlProgramLocalParameter4fvARB = 0;
#endif // _engine_LINUX_OPENGL_USE_EXTENSIONS_

#endif // LINUX




// -----------------------------------------------------------------------
// METHODS
// -----------------------------------------------------------------------

void COpenGLDriver::createMaterialRenderers()
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
	//addAndDropMaterialRenderer(new COpenGLMaterialRenderer_SOLID( this));
	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER( this));

	// add normal map renderers
	s32 tmp = 0;
	video::IMaterialRenderer* renderer = 0;
	renderer = new COpenGLNormalMapRenderer(this, tmp, MaterialRenderers[EMT_SOLID]);
	renderer->drop();
	renderer = new COpenGLNormalMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR]);
	renderer->drop();
	renderer = new COpenGLNormalMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA]);
	renderer->drop();

	// add parallax map renderers
	renderer = new COpenGLParallaxMapRenderer(this, tmp, MaterialRenderers[EMT_SOLID]);
	renderer->drop();
	renderer = new COpenGLParallaxMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR]);
	renderer->drop();
	renderer = new COpenGLParallaxMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA]);
	renderer->drop();
}

void COpenGLDriver::loadExtensions()
{
	if (atof((c8*)glGetString(GL_VERSION)) >= 1.2)
	{
		os::Printer::log("OpenGL driver version is 1.2 or better.", ELL_INFORMATION);
	}
	else
		os::Printer::log("OpenGL driver version is not 1.2 or better.", ELL_WARNING);

	const GLubyte* t = glGetString(GL_EXTENSIONS);
	s32 len = (s32)strlen((const char*)t);
	c8 *str = new c8[len+1];
	c8* p = str;

	for (s32 i=0; i<len; ++i)
	{
		str[i] = (char)t[i];

		if (str[i] == ' ')
		{
			str[i] = 0;
			if (strstr(p, "GL_ARB_multitexture"))
				MultiTextureExtension = true;
			else
			if (strstr(p, "GL_ARB_vertex_program"))
				ARBVertexProgramExtension = true;
			else
			if (strstr(p, "GL_ARB_fragment_program"))
				ARBFragmentProgramExtension = true;
			else
			if (strstr(p, "GL_ARB_shading_language_100"))
				ARBShadingLanguage100Extension = true;
			else
			if (strstr(p, "GL_EXT_texture_filter_anisotropic"))
				AnisotropyExtension = true;

			p = p + strlen(p) + 1;
		}
	}

	delete [] str;

	if (MultiTextureExtension)
	{
		#ifdef _engine_WINDOWS_

		// Windows
		// get multitextureing function pointers

		pGlActiveTextureARB   = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
		pGlClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
		if (!pGlActiveTextureARB || !pGlClientActiveTextureARB)
		{
			MultiTextureExtension = false;
			os::Printer::log("Failed to load OpenGL's multitexture extension, proceeding without.", ELL_WARNING);
		}

		// get fragment and vertex program function pointers
		pGlGenProgramsARB = (PFNGLGENPROGRAMSARBPROC) wglGetProcAddress("glGenProgramsARB");
		pGlBindProgramARB = (PFNGLBINDPROGRAMARBPROC) wglGetProcAddress("glBindProgramARB");
		pGlProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC) wglGetProcAddress("glProgramStringARB");
		pGlDeleteProgramsARB = (PFNGLDELETEPROGRAMSNVPROC) wglGetProcAddress("glDeleteProgramsARB");
		pGlProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC) wglGetProcAddress("glProgramLocalParameter4fvARB");
		pGlCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC) wglGetProcAddress("glCreateShaderObjectARB");
		pGlShaderSourceARB = (PFNGLSHADERSOURCEARBPROC) wglGetProcAddress("glShaderSourceARB");
		pGlCompileShaderARB = (PFNGLCOMPILESHADERARBPROC) wglGetProcAddress("glCompileShaderARB");
		pGlCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC) wglGetProcAddress("glCreateProgramObjectARB");
		pGlAttachObjectARB = (PFNGLATTACHOBJECTARBPROC) wglGetProcAddress("glAttachObjectARB");
		pGlLinkProgramARB = (PFNGLLINKPROGRAMARBPROC) wglGetProcAddress("glLinkProgramARB");
		pGlUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC) wglGetProcAddress("glUseProgramObjectARB");
		pGlDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC) wglGetProcAddress("glDeleteObjectARB");
		pGlGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC) wglGetProcAddress("glGetObjectParameterivARB");
		pGlGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)  wglGetProcAddress("glGetUniformLocationARB");
		pGlUniform4fvARB = (PFNGLUNIFORM4FVARBPROC) wglGetProcAddress("glUniform4fvARB");
		pGlUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)     wglGetProcAddress("glUniform1fvARB");
		pGlUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)     wglGetProcAddress("glUniform2fvARB");
		pGlUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)     wglGetProcAddress("glUniform3fvARB");
		pGlUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC) wglGetProcAddress("glUniformMatrix2fvARB");
		pGlUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC) wglGetProcAddress("glUniformMatrix3fvARB");
		pGlUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC) wglGetProcAddress("glUniformMatrix4fvARB");
		pGlGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMARBPROC) wglGetProcAddress("glGetActiveUniformARB");

		// get vsync extension
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

		#else
		        #ifdef MACOSX

		if (MultiTextureExtension)
			os::Printer::log("Multittexturing active.");
		        #else
		        #ifdef _engine_LINUX_OPENGL_USE_EXTENSIONS_

		// Linux

		if (MultiTextureExtension)
			os::Printer::log("Multittexturing active.");

		        #ifdef GLX_VERSION_1_4
		                #define engine_OGL_LOAD_EXTENSION glXGetProcAddress
		                #else
		                #define engine_OGL_LOAD_EXTENSION glXGetProcAddressARB
		        #endif

		pGlActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)
		                      engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glActiveTextureARB"));

		pGlClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)
		                            engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glClientActiveTextureARB"));

		// get fragment and vertex program function pointers
		pGlGenProgramsARB = (PFNGLGENPROGRAMSARBPROC)
		                    engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGenProgramsARB"));

		pGlBindProgramARB = (PFNGLBINDPROGRAMARBPROC)
		                    engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBindProgramARB"));

		pGlProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC)
		                      engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glProgramStringARB"));

		pGlDeleteProgramsARB = (PFNGLDELETEPROGRAMSNVPROC)
		                       engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteProgramsARB"));

		pGlProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)
		                                 engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glProgramLocalParameter4fvARB"));

		pGlCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)
		                           engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCreateShaderObjectARB"));

		pGlShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)
		                     engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glShaderSourceARB"));

		pGlCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)
		                      engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCompileShaderARB"));

		pGlCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)
		                            engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCreateProgramObjectARB"));

		pGlAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)
		                     engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glAttachObjectARB"));

		pGlLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)
		                    engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glLinkProgramARB"));

		pGlUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)
		                         engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUseProgramObjectARB"));

		pGlDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)
		                     engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteObjectARB"));

		pGlGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)
		                             engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetObjectParameterivARB"));

		pGlGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)
		                           engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetUniformLocationARB"));

		pGlUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)
		                   engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform4fvARB"));

		pGlUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)
		                   engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform1fvARB"));

		pGlUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)
		                   engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform2fvARB"));

		pGlUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)
		                   engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform3fvARB"));

		pGlUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)
		                   engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform4fvARB"));

		pGlUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC)
		                         engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniformMatrix2fvARB"));

		pGlUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC)
		                         engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniformMatrix3fvARB"));

		pGlUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)
		                         engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniformMatrix4fvARB"));

		pGlGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMARBPROC)
		                         engine_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetActiveUniformARB"));

		//ARBVertexProgramExtension = false;
		//ARBFragmentProgramExtension = false;

		        #else // _engine_LINUX_OPENGL_USE_EXTENSIONS_

		MultiTextureExtension = false;
		ARBVertexProgramExtension = false;
		ARBFragmentProgramExtension = false;
		os::Printer::log("Extentions disabled.", ELL_WARNING);

		        #endif // _engine_LINUX_OPENGL_USE_EXTENSIONS_
		        #endif // MACOSX
		#endif // _engine_WINDOWS_

		// load common extensions

		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &MaxTextureUnits);
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &MaxAnisotropy);
	}

	if (MaxTextureUnits < 2)
	{
		MultiTextureExtension = false;
		os::Printer::log("Warning: OpenGL device only has one texture unit. Disabling multitexturing.", ELL_WARNING);
	}
}



//! presents the rendered scene on the screen, returns false if failed
bool COpenGLDriver::endScene( s32 windowId, core::rect<s32>* sourceRect )
{
	CNullDriver::endScene( windowId );

#ifdef _engine_WINDOWS_
	return SwapBuffers(HDc) == TRUE;
#endif

#ifdef LINUX
	if (DoubleBuffered)
		glXSwapBuffers(XDisplay, XWindow);
	return true;
#endif

#ifdef MACOSX
	_device->flush();
	return true;
#endif
}



//! clears the zbuffer
bool COpenGLDriver::beginScene(bool backBuffer, bool zBuffer, SColor color)
{
	CNullDriver::beginScene(backBuffer, zBuffer, color);

	GLbitfield mask = 0;

	if (backBuffer)
	{
		f32 inv = 1.0f / 255.0f;
		glClearColor(color.getRed() * inv, color.getGreen() * inv,
		             color.getBlue() * inv, color.getAlpha() * inv);

		mask |= GL_COLOR_BUFFER_BIT;
	}

	if (zBuffer)
	{
		glDepthMask(GL_TRUE);
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	glClear(mask);
	return true;
}



//! Returns the transformation set by setTransform
core::matrix4 COpenGLDriver::getTransform(E_TRANSFORMATION_STATE state)
{
	return Matrizes[state];
}



//! sets transformation
void COpenGLDriver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
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

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glmat);
	}
		break;
	}
}



//! draws an indexed triangle list
void COpenGLDriver::drawIndexedTriangleList(const S3DVertex* vertices, s32 vertexCount, const u16* indexList, s32 triangleCount)
{
	if (!checkPrimitiveCount(vertexCount) || !vertexCount)
		return;

	CNullDriver::drawIndexedTriangleList(vertices, vertexCount, indexList, triangleCount);

	setRenderStates3DMode();

	extGlClientActiveTextureARB(GL_TEXTURE0_ARB);

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

	glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_SHORT, indexList);

	glFlush();

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY );
	glDisableClientState(GL_NORMAL_ARRAY );
}



//! draws an indexed triangle list
void COpenGLDriver::drawIndexedTriangleList(const S3DVertex2TCoords* vertices, s32 vertexCount,
                                            const u16* indexList, s32 triangleCount)
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
	if (MultiTextureExtension)
	{
		extGlClientActiveTextureARB(GL_TEXTURE0_ARB);
		glEnableClientState ( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), &vertices[0].TCoords);

		extGlClientActiveTextureARB(GL_TEXTURE1_ARB);
		glEnableClientState ( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), &vertices[0].TCoords2);
	}
	else
		glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), &vertices[0].TCoords);

	glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_SHORT, indexList);

	glFlush();

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	if (MultiTextureExtension)
	{
		extGlClientActiveTextureARB(GL_TEXTURE0_ARB);
		glDisableClientState ( GL_TEXTURE_COORD_ARRAY );

		extGlClientActiveTextureARB(GL_TEXTURE1_ARB);
		glDisableClientState ( GL_TEXTURE_COORD_ARRAY );
	}
	else
		glDisableClientState(GL_TEXTURE_COORD_ARRAY );

	glDisableClientState(GL_NORMAL_ARRAY );
}


//! Draws an indexed triangle list.
void COpenGLDriver::drawIndexedTriangleList(const S3DVertexTangents* vertices,
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

	extGlClientActiveTextureARB(GL_TEXTURE0_ARB);
	glEnableClientState ( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertexTangents), &vertices[0].TCoords);

	extGlClientActiveTextureARB(GL_TEXTURE1_ARB);
	glEnableClientState ( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer(3, GL_FLOAT, sizeof(S3DVertexTangents), &vertices[0].Tangent);

	extGlClientActiveTextureARB(GL_TEXTURE2_ARB);
	glEnableClientState ( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer(3, GL_FLOAT, sizeof(S3DVertexTangents), &vertices[0].Binormal);

	glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_SHORT, indexList);

	glFlush();

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY );
	glDisableClientState(GL_NORMAL_ARRAY );

	extGlClientActiveTextureARB(GL_TEXTURE1_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY );
	extGlClientActiveTextureARB(GL_TEXTURE2_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY );
}



//! draws an indexed triangle fan
void COpenGLDriver::drawIndexedTriangleFan(const S3DVertex* vertices, s32 vertexCount,
                                           const u16* indexList, s32 triangleCount)
{
	if (!checkPrimitiveCount(triangleCount))
		return;

	CNullDriver::drawIndexedTriangleFan(vertices, vertexCount, indexList, triangleCount);

	setRenderStates3DMode();

	extGlClientActiveTextureARB(GL_TEXTURE0_ARB);

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
void COpenGLDriver::drawIndexedTriangleFan(const S3DVertex2TCoords* vertices,
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
	if (MultiTextureExtension)
	{
		extGlClientActiveTextureARB(GL_TEXTURE0_ARB);
		glEnableClientState ( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), &vertices[0].TCoords);

		extGlClientActiveTextureARB(GL_TEXTURE1_ARB);
		glEnableClientState ( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), &vertices[0].TCoords2);
	}
	else
		glTexCoordPointer(2, GL_FLOAT, sizeof(S3DVertex2TCoords), &vertices[0].TCoords);

	glDrawElements(GL_TRIANGLE_FAN, triangleCount+2, GL_UNSIGNED_SHORT, indexList);

	glFlush();

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	if (MultiTextureExtension)
	{
		extGlClientActiveTextureARB(GL_TEXTURE0_ARB);
		glDisableClientState ( GL_TEXTURE_COORD_ARRAY );

		extGlClientActiveTextureARB(GL_TEXTURE1_ARB);
		glDisableClientState ( GL_TEXTURE_COORD_ARRAY );
	}
	else
		glDisableClientState(GL_TEXTURE_COORD_ARRAY );

	glDisableClientState(GL_NORMAL_ARRAY );
}



//! draws an 2d image
void COpenGLDriver::draw2DImage(video::ITexture* texture,
                                const core::position2d<s32>& destPos)
{
	if (!texture)
		return;

	draw2DImage(texture, destPos, core::rect<s32>(core::position2d<s32>(0,0),
	                                              texture->getOriginalSize()));
}



//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
void COpenGLDriver::draw2DImage(video::ITexture* texture, const core::position2d<s32>& pos,
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

	core::rect<s32> poss(targetPos, sourceSize);

	s32 xPlus = -(currentRendertargetSize.Width>>1);
	f32 xFact = 1.0f / (currentRendertargetSize.Width>>1);

	s32 yPlus = currentRendertargetSize.Height-(currentRendertargetSize.Height>>1);
	f32 yFact = 1.0f / (currentRendertargetSize.Height>>1);

	const core::dimension2d<s32> sourceSurfaceSize = texture->getOriginalSize();
	core::rect<f32> tcoords;
	tcoords.UpperLeftCorner.X = (((f32)sourcePos.X)+0.5f) / texture->getOriginalSize().Width ;
	tcoords.UpperLeftCorner.Y = (((f32)sourcePos.Y)+0.5f) / texture->getOriginalSize().Height;
	tcoords.LowerRightCorner.X = (((f32)sourcePos.X +0.5f + (f32)sourceSize.Width)) / texture->getOriginalSize().Width;
	tcoords.LowerRightCorner.Y = (((f32)sourcePos.Y +0.5f + (f32)sourceSize.Height)) / texture->getOriginalSize().Height;

	core::rect<f32> npos;
	npos.UpperLeftCorner.X = (f32)(poss.UpperLeftCorner.X+xPlus+0.5f) * xFact;
	npos.UpperLeftCorner.Y = (f32)(yPlus-poss.UpperLeftCorner.Y+0.5f) * yFact;
	npos.LowerRightCorner.X = (f32)(poss.LowerRightCorner.X+xPlus+0.5f) * xFact;
	npos.LowerRightCorner.Y = (f32)(yPlus-poss.LowerRightCorner.Y+0.5f) * yFact;

	glBegin(GL_QUADS);

	glTexCoord2f(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	glVertex2f(npos.UpperLeftCorner.X, npos.UpperLeftCorner.Y);

	glTexCoord2f(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	glVertex2f(npos.LowerRightCorner.X, npos.UpperLeftCorner.Y);

	glTexCoord2f(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	glVertex2f(npos.LowerRightCorner.X, npos.LowerRightCorner.Y);

	glTexCoord2f(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);
	glVertex2f(npos.UpperLeftCorner.X, npos.LowerRightCorner.Y);

	glEnd();
}


void COpenGLDriver::draw2DImage(video::ITexture* texture, const core::rect<s32>& destRect,
                                const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect,
                                video::SColor* colors, bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	core::rect<s32> trgRect=destRect;
	core::rect<s32> srcRect=sourceRect;

	core::dimension2d<s32> currentRendertargetSize = getCurrentRenderTargetSize();
	const core::dimension2d<s32> targetSurfaceSize=currentRendertargetSize;

	const core::dimension2d<s32> ss = texture->getOriginalSize();
	f32 ssw=1.0f/ss.Width;
	f32 ssh=1.0f/ss.Height;

	core::rect<f32> tcoords;
	tcoords.UpperLeftCorner.X = (((f32)srcRect.UpperLeftCorner.X)+0.5f) * ssw ;
	tcoords.UpperLeftCorner.Y = (((f32)srcRect.UpperLeftCorner.Y)+0.5f) * ssh;
	tcoords.LowerRightCorner.X = (((f32)srcRect.UpperLeftCorner.X +0.5f + (f32)srcRect.getWidth())) * ssw;
	tcoords.LowerRightCorner.Y = (((f32)srcRect.UpperLeftCorner.Y +0.5f + (f32)srcRect.getHeight())) * ssh;

	s32 xPlus = -(currentRendertargetSize.Width>>1);
	f32 xFact = 1.0f / (currentRendertargetSize.Width>>1);

	s32 yPlus = currentRendertargetSize.Height-(currentRendertargetSize.Height>>1);
	f32 yFact = 1.0f / (currentRendertargetSize.Height>>1);

	core::rect<f32> npos;
	npos.UpperLeftCorner.X = (f32)(trgRect.UpperLeftCorner.X+xPlus+0.5f) * xFact;
	npos.UpperLeftCorner.Y = (f32)(yPlus-trgRect.UpperLeftCorner.Y+0.5f) * yFact;
	npos.LowerRightCorner.X = (f32)(trgRect.LowerRightCorner.X+xPlus+0.5f) * xFact;
	npos.LowerRightCorner.Y = (f32)(yPlus-trgRect.LowerRightCorner.Y+0.5f) * yFact;

	setTexture(0, texture);

	if (useAlphaChannelOfTexture)
		setRenderStates2DMode(false, true, true);
	else
		setRenderStates2DMode(false, true, false);

	bool bTempColors=false;

	if(colors==NULL)
	{
		colors=new SColor[4];
		for(int i=0; i<4; i++) colors[i]=SColor(0,255,255,255);
		bTempColors=true;
	}

	glBegin(GL_QUADS);

	glColor4ub(colors[0].getRed(), colors[0].getGreen(), colors[0].getBlue(), colors[0].getAlpha());
	glTexCoord2f(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	glVertex2f(npos.UpperLeftCorner.X, npos.UpperLeftCorner.Y);

	glColor4ub(colors[3].getRed(), colors[3].getGreen(), colors[3].getBlue(), colors[3].getAlpha());
	glTexCoord2f(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	glVertex2f(npos.LowerRightCorner.X, npos.UpperLeftCorner.Y);

	glColor4ub(colors[2].getRed(), colors[2].getGreen(), colors[2].getBlue(), colors[2].getAlpha());
	glTexCoord2f(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	glVertex2f(npos.LowerRightCorner.X, npos.LowerRightCorner.Y);

	glColor4ub(colors[1].getRed(), colors[1].getGreen(), colors[1].getBlue(), colors[1].getAlpha());
	glTexCoord2f(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);
	glVertex2f(npos.UpperLeftCorner.X, npos.LowerRightCorner.Y);

	glEnd();

	if(bTempColors)
		delete [] colors;
}


//! Draws a 2d line.
void COpenGLDriver::draw2DLine(const core::position2d<s32>& start,
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

//! draw an 2d rectangle
void COpenGLDriver::draw2DRectangle(SColor color, const core::rect<s32>& position,
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

	glBegin(GL_QUADS);
	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	glVertex2f(npos.UpperLeftCorner.X, npos.UpperLeftCorner.Y);
	glVertex2f(npos.LowerRightCorner.X, npos.UpperLeftCorner.Y);
	glVertex2f(npos.LowerRightCorner.X, npos.LowerRightCorner.Y);
	glVertex2f(npos.UpperLeftCorner.X, npos.LowerRightCorner.Y);
	glEnd();
}



//! draw an 2d rectangle
void COpenGLDriver::draw2DRectangle(const core::rect<s32>& position,
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



//! queries the features of the driver, returns true if feature is available
bool COpenGLDriver::queryFeature(E_VIDEO_DRIVER_FEATURE feature)
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
	case EVDF_ARB_VERTEX_PROGRAM_1:
		return ARBVertexProgramExtension;
	case EVDF_ARB_FRAGMENT_PROGRAM_1:
		return ARBFragmentProgramExtension;
	case EVDF_ARB_GLSL:
		return ARBShadingLanguage100Extension;
	};

	return false;
}


//! sets the current Texture
void COpenGLDriver::setTexture(s32 stage, video::ITexture* texture)
{
	if (stage > 1)
		return;

	if (MultiTextureExtension)
		extGlActiveTextureARB(stage == 0 ? GL_TEXTURE0_ARB : GL_TEXTURE1_ARB);
	else
	if (stage != 0)
		return;

	if (texture && texture->getDriverType() != EDT_OPENGL)
	{
		glDisable(GL_TEXTURE_2D);
		os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
		return;
	}

	if (texture == 0)
		glDisable(GL_TEXTURE_2D);
	else
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,
		              ((COpenGLTexture*)texture)->getOpenGLTextureName());
	}
}


//! returns a device dependent texture from a software surface (IImage)
//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
video::ITexture* COpenGLDriver::createDeviceDependentTexture(IImage* surface)
{
	bool generateMipLevels = getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);

	return new COpenGLTexture(surface, generateMipLevels);
}



//! Sets a material. All 3d drawing functions draw geometry now
//! using this material.
//! \param material: Material to be used from now on.
void COpenGLDriver::setMaterial(const SMaterial& material)
{
	Material = material;

	setTexture(0, Material.Texture1);
	setTexture(1, Material.Texture2);
}



//! prints error if an error happened.
inline void COpenGLDriver::printGLError()
{
#ifdef _DEBUG
	GLenum g = glGetError();
	switch(g)
	{
	case GL_NO_ERROR:
		break;
	case GL_INVALID_ENUM:
		os::Printer::log("GL_INVALID_ENUM", ELL_ERROR); break;
	case GL_INVALID_VALUE:
		os::Printer::log("GL_INVALID_VALUE", ELL_ERROR); break;
	case GL_INVALID_OPERATION:
		os::Printer::log("GL_INVALID_OPERATION", ELL_ERROR); break;
	case GL_STACK_OVERFLOW:
		os::Printer::log("GL_STACK_OVERFLOW", ELL_ERROR); break;
	case GL_STACK_UNDERFLOW:
		os::Printer::log("GL_STACK_UNDERFLOW", ELL_ERROR); break;
	case GL_OUT_OF_MEMORY:
		os::Printer::log("GL_OUT_OF_MEMORY", ELL_ERROR); break;
	};
#endif
}



//! sets the needed renderstates
void COpenGLDriver::setRenderStates3DMode()
{
	if (CurrentRenderMode != ERM_3D)
	{
		// switch back the matrices
		GLfloat glmat[16];

		createGLMatrix(glmat, Matrizes[ETS_VIEW] * Matrizes[ETS_WORLD]);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glmat);

		createGLMatrix(glmat, Matrizes[ETS_PROJECTION]);
		glmat[12]*= -1.0f;
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glmat);

		ResetRenderStates = true;
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


//! Can be called by an IMaterialRenderer to make its work easier.
void COpenGLDriver::setBasicRenderStates(const SMaterial& material, const SMaterial& lastmaterial,
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

		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0); //Material.Shininess);
	}

	// bilinear
	glDisable(GL_CULL_FACE);
	if (resetAllRenderstates ||
	    lastmaterial.BilinearFilter != material.BilinearFilter ||
	    lastmaterial.AnisotropicFilter != material.AnisotropicFilter )
	{
		if (MultiTextureExtension)
		{
			extGlActiveTextureARB(GL_TEXTURE1_ARB);

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
			                 Material.BilinearFilter ? GL_LINEAR : GL_NEAREST);

			if (AnisotropyExtension)
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
				                material.AnisotropicFilter ? MaxAnisotropy : 1.0f );

			extGlActiveTextureARB(GL_TEXTURE0_ARB);
		}

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		                 Material.BilinearFilter ? GL_LINEAR : GL_NEAREST);

		if (AnisotropyExtension)
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
			                material.AnisotropicFilter ? MaxAnisotropy : 1.0f );
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
		//if (material.BackfaceCulling)
		//	glEnable(GL_CULL_FACE);
		//else

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
void COpenGLDriver::setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel)
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

		if (MultiTextureExtension)
		{
			extGlActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);

			extGlActiveTextureARB(GL_TEXTURE0_ARB);
		}

		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);

		glDisable(GL_ALPHA_TEST);
		glCullFace(GL_BACK);

		// unset last 3d material
		if (CurrentRenderMode == ERM_3D &&
		    Material.MaterialType >= 0 && Material.MaterialType < (s32)MaterialRenderers.size())
			MaterialRenderers[Material.MaterialType]->OnUnsetMaterial();
	}

	if (texture)
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		if (alphaChannel)
		{
			glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_REPLACE);
			glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_TEXTURE);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);

			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
			glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
			glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
			glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PRIMARY_COLOR_EXT);
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


//! \return Returns the name of the video driver. Example: In case of the Direct3D8
//! driver, it would return "Direct3D8.1".
const wchar_t* COpenGLDriver::getName()
{
	return L"OpenGL 1.5";
}


//! deletes all dynamic lights there are
void COpenGLDriver::deleteAllDynamicLights()
{
	for (s32 i=0; i<LastSetLight+1; ++i)
		glDisable(GL_LIGHT0 + i);

	LastSetLight = -1;

	CNullDriver::deleteAllDynamicLights();
}


//! adds a dynamic light
void COpenGLDriver::addDynamicLight(const SLight& light)
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


//! returns the maximal amount of dynamic lights the device can handle
s32 COpenGLDriver::getMaximalDynamicLightAmount()
{
	return GL_MAX_LIGHTS;
}


//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
void COpenGLDriver::setAmbientLight(const SColorf& color)
{
	GLfloat data[4] = {color.r, color.g, color.b, color.a};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, data);
}


// this code was sent in by Oliver Klems, thank you! (I modified the glVieport
// method just a bit.
void COpenGLDriver::setViewPort(const core::rect<s32>& area)
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

//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
//! this: Frist, draw all geometry. Then use this method, to draw the shadow
//! volume. Then, use IVideoDriver::drawStencilShadow() to visualize the shadow.
void COpenGLDriver::drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail)
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

	// store current OpenGL   state
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT |
	             GL_POLYGON_BIT   | GL_STENCIL_BUFFER_BIT   );

	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_STENCIL_TEST);
	glColorMask(GL_FALSE, GL_FALSE,   GL_FALSE, GL_FALSE ); // no color buffer drawing
	glStencilFunc(GL_ALWAYS, 1,   0xFFFFFFFFL   );
	glColorMask(0, 0, 0, 0);
	glEnable(GL_CULL_FACE);


	if (!zfail)
	{
		// ZPASS Method

		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		glCullFace(GL_BACK);
		glBegin(GL_TRIANGLES);

		s32 i;
		for(i=0; i<count; ++i)
			glVertex3f(triangles[i].X, triangles[i].Y, triangles[i].Z);

		glEnd();

		glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
		glCullFace(GL_FRONT);

		glBegin(GL_TRIANGLES);
		for( i=0; i<count; ++i)
			glVertex3f(triangles[i].X, triangles[i].Y, triangles[i].Z);

		glEnd();
	}
	else
	{
		// ZFAIL Method

		glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
		glCullFace(GL_FRONT);

		glBegin(GL_TRIANGLES);

		s32 i;
		for(i=0; i<count; ++i)
			glVertex3f(triangles[i].X, triangles[i].Y, triangles[i].Z);

		glEnd();

		glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
		glCullFace(GL_BACK);

		glBegin(GL_TRIANGLES);

		for(i=0; i<count; ++i)
			glVertex3f(triangles[i].X, triangles[i].Y, triangles[i].Z);

		glEnd();
	}

	glPopAttrib();
}



void COpenGLDriver::drawStencilShadow(bool clearStencilBuffer, video::SColor leftUpEdge,
                                      video::SColor rightUpEdge, video::SColor leftDownEdge, video::SColor rightDownEdge)
{
	if (!StencilBuffer)
		return;

	setTexture(0,0);
	setTexture(1,0);
	glPushAttrib( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT );

	glDisable( GL_LIGHTING );
	glDepthMask(GL_FALSE);
	glDepthFunc( GL_LEQUAL );
	glEnable( GL_STENCIL_TEST );

	glFrontFace( GL_CCW );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glStencilFunc(GL_NOTEQUAL, 0, 0xFFFFFFFFL);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glDisable(GL_FOG);

	glPushMatrix();
	glLoadIdentity();

	// draw A shadowing rectangle covering the entire screen

	glBegin(GL_TRIANGLE_STRIP);

	glColor4ub (leftUpEdge.getRed(), leftUpEdge.getGreen(), leftUpEdge.getBlue(), leftUpEdge.getAlpha() );
	glVertex3f(-10.1f, 10.1f,0.90f);

	glColor4ub (leftDownEdge.getRed(), leftDownEdge.getGreen(), leftDownEdge.getBlue(), leftDownEdge.getAlpha() );
	glVertex3f(-10.1f,-10.1f,0.90f);

	glColor4ub (rightUpEdge.getRed(), rightUpEdge.getGreen(), rightUpEdge.getBlue(), rightUpEdge.getAlpha() );
	glVertex3f( 10.1f, 10.1f,0.90f);

	glColor4ub (rightDownEdge.getRed(), rightDownEdge.getGreen(), rightDownEdge.getBlue(), rightDownEdge.getAlpha() );
	glVertex3f( 10.1f,-10.1f,0.90f);

	glEnd();

	glPopMatrix();
	glPopAttrib();

	if (clearStencilBuffer)
		glClear(GL_STENCIL_BUFFER_BIT);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}


//! Sets the fog mode.
void COpenGLDriver::setFog(SColor c, bool linearFog, f32 start,
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


//! Draws a 3d line.
void COpenGLDriver::draw3DLine(const core::vector3df& start,
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

//! Only used by the internal engine. Used to notify the driver that
//! the window was resized.
void COpenGLDriver::OnResize(const core::dimension2d<s32>& size)
{
	CNullDriver::OnResize(size);
	glViewport(0, 0, size.Width, size.Height);
}

//! Returns type of video driver
E_DRIVER_TYPE COpenGLDriver::getDriverType()
{
	return EDT_OPENGL;
}

void COpenGLDriver::extGlActiveTextureARB(GLenum texture)
{
#ifdef MACOSX
	if (MultiTextureExtension) glActiveTextureARB(texture);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (MultiTextureExtension && pGlActiveTextureARB)
		pGlActiveTextureARB(texture);
#endif
}

void COpenGLDriver::extGlClientActiveTextureARB(GLenum texture)
{
#ifdef MACOSX
	if (MultiTextureExtension) glClientActiveTextureARB(texture);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (MultiTextureExtension && pGlClientActiveTextureARB)
		pGlClientActiveTextureARB(texture);
#endif
}

void COpenGLDriver::extGlGenProgramsARB(GLsizei n, GLuint *programs)
{
#ifdef MACOSX
	glGenProgramsARB(n,programs);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlGenProgramsARB)
		pGlGenProgramsARB(n, programs);
#endif
}

void COpenGLDriver::extGlBindProgramARB(GLenum target, GLuint program)
{
#ifdef MACOSX
	glBindProgramARB(target, program);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlBindProgramARB)
		pGlBindProgramARB(target, program);
#endif
}

void COpenGLDriver::extGlProgramStringARB(GLenum target, GLenum format, GLsizei len, const GLvoid *string)
{
#ifdef MACOSX
	glProgramStringARB(target,format,len,string);
#endif

#if defined (WIN32) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlProgramStringARB)
		pGlProgramStringARB(target, format, len, string);
#endif
}

void COpenGLDriver::extGlDeleteProgramsARB(GLsizei n, const GLuint *programs)
{
#ifdef MACOSX
	glDeleteProgramsARB(n,programs);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlDeleteProgramsARB)
		pGlDeleteProgramsARB(n, programs);
#endif
}

void COpenGLDriver::extGlProgramLocalParameter4fvARB(GLenum n, GLuint i, const GLfloat * f)
{
#ifdef MACOSX
	glProgramLocalParameter4fvARB(n,i,f);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlProgramLocalParameter4fvARB)
		pGlProgramLocalParameter4fvARB(n,i,f);
#endif
}

GLhandleARB COpenGLDriver::extGlCreateShaderObjectARB(GLenum shaderType)
{
#ifdef MACOSX
	return (glCreateShaderObjectARB(shaderType));
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlCreateShaderObjectARB)
		return pGlCreateShaderObjectARB(shaderType);
#endif

	return 0;
}

void COpenGLDriver::extGlShaderSourceARB(GLhandleARB shader, int numOfStrings, const char **strings, int *lenOfStrings)
{
#ifdef MACOSX
	glShaderSourceARB(shader, numOfStrings, strings, (GLint *)lenOfStrings);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlShaderSourceARB)
		pGlShaderSourceARB(shader, numOfStrings, strings, lenOfStrings);
#endif
}

void COpenGLDriver::extGlCompileShaderARB(GLhandleARB shader)
{
#ifdef MACOSX
	glCompileShaderARB(shader);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlCompileShaderARB)
		pGlCompileShaderARB(shader);
#endif
}

GLhandleARB COpenGLDriver::extGlCreateProgramObjectARB(void)
{
#ifdef MACOSX
	return (glCreateProgramObjectARB());
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlCreateProgramObjectARB)
		return pGlCreateProgramObjectARB();
#endif

	return 0;
}

void COpenGLDriver::extGlAttachObjectARB(GLhandleARB program, GLhandleARB shader)
{
#ifdef MACOSX
	glAttachObjectARB(program, shader);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlAttachObjectARB)
		pGlAttachObjectARB(program, shader);
#endif
}

void COpenGLDriver::extGlLinkProgramARB(GLhandleARB program)
{
#ifdef MACOSX
	glLinkProgramARB(program);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlLinkProgramARB)
		pGlLinkProgramARB(program);
#endif
}

void COpenGLDriver::extGlUseProgramObjectARB(GLhandleARB prog)
{
#ifdef MACOSX
	glUseProgramObjectARB(prog);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlUseProgramObjectARB)
		pGlUseProgramObjectARB(prog);
#endif
}

void COpenGLDriver::extGlDeleteObjectARB(GLhandleARB object)
{
#ifdef MACOSX
	glDeleteObjectARB(object);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlDeleteObjectARB)
		pGlDeleteObjectARB(object);
#endif
}

void COpenGLDriver::extGlGetObjectParameterivARB(GLhandleARB object, GLenum type, int *param)
{
#ifdef MACOSX
	glGetObjectParameterivARB(object, type, (GLint *)param);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlGetObjectParameterivARB)
		pGlGetObjectParameterivARB(object, type, param);
#endif
}

GLint COpenGLDriver::extGlGetUniformLocationARB(GLhandleARB program, const char *name)
{
#ifdef MACOSX
	return (glGetUniformLocationARB(program, name));
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlGetUniformLocationARB)
		return pGlGetUniformLocationARB(program, name);
#endif

	return 0;
}

void COpenGLDriver::extGlUniform4fvARB(GLint location, GLsizei count, const GLfloat *v)
{
#ifdef MACOSX
	glUniform4fvARB(location, count, v);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlUniform4fvARB)
		pGlUniform4fvARB(location, count, v);
#endif
}

void COpenGLDriver::extGlUniform1fvARB (GLint loc, GLsizei count, const GLfloat *v)
{
#ifdef MACOSX
	glUniform1fvARB(loc, count, v);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlUniform1fvARB)
		pGlUniform1fvARB(loc, count, v);
#endif
}

void COpenGLDriver::extGlUniform2fvARB (GLint loc, GLsizei count, const GLfloat *v)
{
#ifdef MACOSX
	glUniform2fvARB(loc, count, v);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlUniform2fvARB)
		pGlUniform2fvARB(loc, count, v);
#endif
}

void COpenGLDriver::extGlUniform3fvARB (GLint loc, GLsizei count, const GLfloat *v)
{
#ifdef MACOSX
	glUniform3fvARB(loc, count, v);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlUniform3fvARB)
		pGlUniform3fvARB(loc, count, v);
#endif
}

void COpenGLDriver::extGlUniformMatrix2fvARB (GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v)
{
#ifdef MACOSX
	glUniformMatrix2fvARB(loc, count, transpose, v);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlUniformMatrix2fvARB)
		pGlUniformMatrix2fvARB(loc, count, transpose, v);
#endif
}

void COpenGLDriver::extGlUniformMatrix3fvARB (GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v)
{
#ifdef MACOSX
	glUniformMatrix3fvARB(loc, count, transpose, v);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlUniformMatrix3fvARB)
		pGlUniformMatrix3fvARB(loc, count, transpose, v);
#endif
}

void COpenGLDriver::extGlUniformMatrix4fvARB (GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v)
{
#ifdef MACOSX
	glUniformMatrix4fvARB(loc, count, transpose, v);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlUniformMatrix4fvARB)
		pGlUniformMatrix4fvARB(loc, count, transpose, v);
#endif
}

void COpenGLDriver::extGlGetActiveUniformARB (GLhandleARB program, GLuint index, GLsizei maxlength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name)
{
#ifdef MACOSX
	glGetActiveUniformARB(program, index, maxlength, length, size, type, name);
#endif

#if defined (_engine_WINDOWS_) || defined (_engine_LINUX_OPENGL_USE_EXTENSIONS_)
	if (pGlGetActiveUniformARB)
		pGlGetActiveUniformARB(program, index, maxlength, length, size, type, name);
#endif
}

bool COpenGLDriver::hasMultiTextureExtension()
{
	return MultiTextureExtension;
}

//! Sets a vertex shader constant.
void COpenGLDriver::setVertexShaderConstant(f32* data, s32 startRegister, s32 constantAmount)
{
	for (int i=0; i<constantAmount; ++i)
		extGlProgramLocalParameter4fvARB(GL_VERTEX_PROGRAM_ARB, startRegister+i, &data[i*4]);
}

//! Sets a pixel shader constant.
void COpenGLDriver::setPixelShaderConstant(f32* data, s32 startRegister, s32 constantAmount)
{
	for (int i=0; i<constantAmount; ++i)
		extGlProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, startRegister+i, &data[i*4]);
}

//! Sets a constant for the vertex shader based on a name.
bool COpenGLDriver::setVertexShaderConstant(const c8* name, const f32* floats, int count)
{
	//pass this along, as in GLSL the same routine is used for both vertex and fragment shaders
	return setPixelShaderConstant(name, floats, count);
}

//! Sets a constant for the pixel shader based on a name.
bool COpenGLDriver::setPixelShaderConstant(const c8* name, const f32* floats, int count)
{
	os::Printer::log("Error: Please call services->setPixelShaderConstant(), not VideoDriver->setPixelShaderConstant().");
	return false;
}


//! Adds a new material renderer to the VideoDriver, using pixel and/or
//! vertex shaders to render geometry.
s32 COpenGLDriver::addShaderMaterial(const c8* vertexShaderProgram,
                                     const c8* pixelShaderProgram,
                                     IShaderConstantSetCallBack* callback,
                                     E_MATERIAL_TYPE baseMaterial, s32 userData)
{
	s32 nr = -1;
	COpenGLShaderMaterialRenderer* r = new COpenGLShaderMaterialRenderer(
	        this, nr, vertexShaderProgram, pixelShaderProgram,
	        callback, getMaterialRenderer(baseMaterial), userData);

	r->drop();
	return nr;
}

//! Adds a new material renderer to the VideoDriver, using GLSL to render geometry.
s32 COpenGLDriver::addHighLevelShaderMaterial(
        const c8* vertexShaderProgram,
        const c8* vertexShaderEntryPointName,
        E_VERTEX_SHADER_TYPE vsCompileTarget,
        const c8* pixelShaderProgram,
        const c8* pixelShaderEntryPointName,
        E_PIXEL_SHADER_TYPE psCompileTarget,
        IShaderConstantSetCallBack* callback,
        E_MATERIAL_TYPE baseMaterial,
        s32 userData)
{
	s32 nr = -1;

	COpenGLSLMaterialRenderer* r = new COpenGLSLMaterialRenderer(
	        this, nr, vertexShaderProgram, vertexShaderEntryPointName,
	        vsCompileTarget, pixelShaderProgram, pixelShaderEntryPointName, psCompileTarget,
	        callback,getMaterialRenderer(baseMaterial), userData);

	r->drop();
	return nr;
}

//! Returns a pointer to the IVideoDriver interface. (Implementation for
//! IMaterialRendererServices)
IVideoDriver* COpenGLDriver::getVideoDriver()
{
	return this;
}


//! Returns pointer to the IGPUProgrammingServices interface.
IGPUProgrammingServices* COpenGLDriver::getGPUProgrammingServices()
{
	return this;
}

ITexture* COpenGLDriver::createRenderTargetTexture(core::dimension2d<s32> size)
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

bool COpenGLDriver::setRenderTarget(video::ITexture* texture, bool clearBackBuffer,
                                    bool clearZBuffer, SColor color)
{
	// check for right driver type

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
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
			                    RenderTargetTexture->getSize().Width, RenderTargetTexture->getSize().Height);

			glViewport(0,0,ScreenSize.Width,ScreenSize.Height);
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
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
			                    RenderTargetTexture->getSize().Width,
			                    RenderTargetTexture->getSize().Height);

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


//! Clears the ZBuffer.
void COpenGLDriver::clearZBuffer()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}



// returns the current size of the screen or rendertarget
core::dimension2d<s32> COpenGLDriver::getCurrentRenderTargetSize()
{
	if ( CurrentRendertargetSize.Width == 0 )
		return ScreenSize;
	else
		return CurrentRendertargetSize;
}



} // end namespace
} // end namespace

#endif // _engine_COMPILE_WITH_OPENGL_


		namespace engine
		{
		namespace video
		{


// -----------------------------------
// WINDOWS VERSION
// -----------------------------------
#ifdef _engine_WINDOWS_
		IVideoDriver* createOpenGLDriver(const core::dimension2d<s32>& screenSize,
		                                 HWND window, bool fullscreen, bool stencilBuffer, bool vsync, io::IFileSystem* io)
		{
#ifdef _engine_COMPILE_WITH_OPENGL_
			COpenGLDriver* ogl =  new COpenGLDriver(screenSize, window, fullscreen, stencilBuffer, io);
			if (!ogl->initDriver(screenSize, window, fullscreen, vsync))
			{
				ogl->drop();
				ogl = 0;
			}
			return ogl;
#else
			return 0;
#endif // _engine_COMPILE_WITH_OPENGL_
		}
#endif // _engine_WINDOWS_

// -----------------------------------
// MACOSX VERSION
// -----------------------------------
#ifdef MACOSX
		IVideoDriver* createOpenGLDriver(const core::dimension2d<s32>& screenSize,
		                                 bool fullscreen, bool stencilBuffer, CengineDeviceMacOSX *device, io::IFileSystem* io)
		{
#ifdef _engine_COMPILE_WITH_OPENGL_
			return new COpenGLDriver(screenSize, fullscreen, stencilBuffer,
			                         device,  io);
#else
			return 0;
#endif //  _engine_COMPILE_WITH_OPENGL_
		}
#endif // MACOSX

// -----------------------------------
// LINUX VERSION
// -----------------------------------
#ifdef LINUX
		IVideoDriver* createOpenGLDriver(const core::dimension2d<s32>& screenSize,
		                                 bool fullscreen, bool doublebuffer, bool stencilBuffer, Window window,
		                                 Display* display, io::IFileSystem* io)
		{
#ifdef _engine_COMPILE_WITH_OPENGL_
			return new COpenGLDriver(screenSize, fullscreen, doublebuffer, stencilBuffer,
			                         window, display, io);
#else
			return 0;
#endif //  _engine_COMPILE_WITH_OPENGL_
		}
#endif // LINUX

		} // end namespace
		} // end namespace

