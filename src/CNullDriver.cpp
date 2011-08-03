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
 
#include "CNullDriver.h"
#include "CSoftwareTexture.h"
#include "os.h"
#include <stdio.h>
#include "CImage.h"
#include "IMeshBuffer.h"
#include "ISceneManager.h"

#include <string.h>

#include <pspdebug.h>
#define printf pspDebugScreenPrintf

namespace engine
{
namespace video
{

//! creates a loader which is able to load windows bitmaps
IImageLoader* createImageLoaderBmp();
//! creates a loader which is able to load jpeg images
IImageLoader* createImageLoaderJPG();
//! creates a loader which is able to load targa images
IImageLoader* createImageLoaderTGA();
//! creates a loader which is able to load psd images
IImageLoader* createImageLoaderPSD();
//! creates a loader which is able to load pcx images
IImageLoader* createImageLoaderPCX();
//! creates a loader which is able to load png images
IImageLoader* createImageLoaderPNG();


void CNullDriver::enableClippingFeature(bool enable)
{
	enableClipping = enable;
}

//! constructor
CNullDriver::CNullDriver(io::IFileSystem* io, const core::dimension2d<s32>& screenSize)
	: ScreenSize(screenSize), ViewPort(0,0,0,0),
	FileSystem(io), PrimitivesDrawn(0), TextureCreationFlags(0),enableClipping(true)
{
	#ifdef _DEBUG
	setDebugName("CNullDriver");
	#endif

	setFog();

	setTextureCreationFlag(ETCF_ALWAYS_32_BIT, true);
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, true);

	ViewPort = core::rect<s32>(core::position2d<s32>(0,0), screenSize);

	if (FileSystem)
		FileSystem->grab();

	// create surface loader

	SurfaceLoader.push_back(video::createImageLoaderBmp());
	SurfaceLoader.push_back(video::createImageLoaderJPG());
	SurfaceLoader.push_back(video::createImageLoaderTGA());
	SurfaceLoader.push_back(video::createImageLoaderPSD());
	SurfaceLoader.push_back(video::createImageLoaderPCX());
	SurfaceLoader.push_back(video::createImageLoaderPNG());

	// set ExposedData to 0
	memset(&ExposedData, 0, sizeof(ExposedData));
}



//! destructor
CNullDriver::~CNullDriver()
{
	// delete file system

	if (FileSystem)
		FileSystem->drop();

	// delete textures

	deleteAllTextures();

	// delete surface loader

	int i=0;
	for (i=0; i<(int)SurfaceLoader.size(); ++i)
		SurfaceLoader[i]->drop();


	deleteMaterialRenders();
}


void CNullDriver::draw2DStretchedImage(video::ITexture* texture, const core::position2d<s32>& pos,
                                       const core::dimension2d<s32>& size, core::rect<s32>& sourceRect, SColor color,
                                       bool useAlphaChannelOfTexture )
{
}

//! Adds an external surface loader to the engine.
void CNullDriver::addExternalImageLoader(IImageLoader* loader)
{
	if (!loader)
		return;

	loader->grab();
	SurfaceLoader.push_back(loader);
}

void CNullDriver::setSceneManager(void *smgr) {
	return;
}
//! deletes all textures
void CNullDriver::deleteAllTextures()
{
	for (u32 i=0; i<Textures.size(); ++i)
		Textures[i].Surface->drop();

	Textures.clear();
}



//! applications must call this method before performing any rendering. returns false if failed.
bool CNullDriver::beginScene(bool backBuffer, bool zBuffer, SColor color)
{
	PrimitivesDrawn = 0;
	return true;
}



//! applications must call this method after performing any rendering. returns false if failed.
bool CNullDriver::endScene( s32 windowId, core::rect<s32>* sourceRect )
{
	FPSCounter.registerFrame(os::Timer::getRealTime());
	return true;
}



//! queries the features of the driver, returns true if feature is available
bool CNullDriver::queryFeature(E_VIDEO_DRIVER_FEATURE feature)
{
	return false;
}



//! sets transformation
void CNullDriver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
{
}


//! Returns the transformation set by setTransform
core::matrix4 CNullDriver::getTransform(E_TRANSFORMATION_STATE state)
{
	return core::matrix4();
}



//! sets a material
void CNullDriver::setMaterial(const SMaterial& material)
{
}



//! Removes a texture from the texture cache and deletes it, freeing lot of
//! memory.
void CNullDriver::removeTexture(ITexture* texture)
{
	for (u32 i=0; i<Textures.size(); ++i)
		if (Textures[i].Surface == texture)
		{
			texture->drop();
			Textures.erase(i);
		}
}


//! Removes all texture from the texture cache and deletes them, freeing lot of
//! memory.
void CNullDriver::removeAllTextures()
{
	deleteAllTextures();
}

ITexture *CNullDriver::getTextureByIndex(u32 Index)
{

	if (Textures.size() > Index && Index >= 0)
		return Textures[Index].Surface;

	return 0;

}

u32 CNullDriver::getTextureCount()
{

	return Textures.size();

}
//! loads a Texture
ITexture* CNullDriver::getTexture(const c8* filename)
{
	ITexture* texture = findTexture(filename);

	if (texture)
		return texture;
	io::IReadFile* file = FileSystem->createAndOpenFile(filename);

	if (file)
	{
		texture = loadTextureFromFile(file);
		file->drop();

		if (texture)
		{
			addTexture(texture, filename);
			texture->drop(); // drop it becaus we created it, one grab to much
		}
	}
	else
		os::Printer::log("Could not open file of texture", filename, ELL_ERROR);

	if (!texture)
		os::Printer::log("Could not load texture", filename, ELL_ERROR);
	return texture;
}



//! loads a Texture
ITexture* CNullDriver::getTexture(io::IReadFile* file)
{
	ITexture* texture = 0;

	if (file)
	{
		texture = findTexture(file->getFileName());

		if (texture)
			return texture;

		texture = loadTextureFromFile(file);

		if (texture)
		{
			addTexture(texture, file->getFileName());
			texture->drop(); // drop it because we created it, one grab to much
		}
	}

	if (!texture)
		os::Printer::log("Could not load texture", file->getFileName(), ELL_ERROR);

	return texture;
}



//! opens the file and loads it into the surface
video::ITexture* CNullDriver::loadTextureFromFile(io::IReadFile* file)
{
	ITexture* texture = 0;
	IImage* image = createImageFromFile(file);

	if (image)
	{
		// create texture from surface
		texture = createDeviceDependentTexture(image);
		os::Printer::log("Loaded texture", file->getFileName());
		image->drop();
	}

	return texture;
}



//! adds a surface, not loaded or created by the engine Engine
void CNullDriver::addTexture(video::ITexture* texture, const c8* filename)
{
	if (texture)
	{
		if (!filename)
			filename = "";

		SSurface s;
		s.Filename = filename;
		s.Filename.make_lower();
		s.Surface = texture;
		texture->grab();

		Textures.push_back(s);
	}
}



//! looks if the image is already loaded
video::ITexture* CNullDriver::findTexture(const c8* filename)
{
	SSurface s;
	if (!filename)
		filename = "";

	s.Filename = filename;
	s.Filename.make_lower();

	s32 index = Textures.binary_search(s);
	if (index != -1)
		return Textures[index].Surface;

	return 0;
}



//! Creates a texture from a loaded IImage.
ITexture* CNullDriver::addTexture(const c8* name, IImage* image)
{
	if (!name || !image)
		return 0;

	ITexture* t = createDeviceDependentTexture(image);
	addTexture(t, name);
	t->drop();
	return t;
}



//! creates a Texture
ITexture* CNullDriver::addTexture(const core::dimension2d<s32>& size,
                                  const c8* name,
                                  ECOLOR_FORMAT format)
{
	if (!name)
		return 0;

	IImage* image = new CImage(format, size);
	ITexture* t = createDeviceDependentTexture(image);
	image->drop();
	addTexture(t, name);

	if (t)
		t->drop();

	return t;
}



//! returns a device dependent texture from a software surface (IImage)
//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
ITexture* CNullDriver::createDeviceDependentTexture(IImage* surface)
{
	return new CSoftwareTexture(surface);
}



//! sets a render target
bool CNullDriver::setRenderTarget(video::ITexture* texture, bool clearBackBuffer,
                                  bool clearZBuffer, SColor color)
{
	return false;
}



//! sets a viewport
void CNullDriver::setViewPort(const core::rect<s32>& area)
{
}



//! gets the area of the current viewport
const core::rect<s32>& CNullDriver::getViewPort() const
{
	return ViewPort;
}



//! draws an indexed triangle list
void CNullDriver::drawIndexedTriangleList(const S3DVertex* vertices, s32 vertexCount, const u16* indexList, s32 triangleCount)
{
	PrimitivesDrawn += triangleCount;
}



//! draws an indexed triangle list
void CNullDriver::drawIndexedTriangleList(const S3DVertex2TCoords* vertices, s32 vertexCount, const u16* indexList, s32 triangleCount)
{
	PrimitivesDrawn += triangleCount;
}


//! Draws an indexed triangle list.
void CNullDriver::drawIndexedTriangleList(const S3DVertexTangents* vertices,
                                          s32 vertexCount, const u16* indexList, s32 triangleCount)
{
	PrimitivesDrawn += triangleCount;
}


//! Draws an indexed triangle list.
void CNullDriver::drawIndexedTriangleFan(const S3DVertex2TCoords* vertices,
                                         s32 vertexCount, const u16* indexList, s32 triangleCount)
{
	PrimitivesDrawn += triangleCount;
}


//! Draws an indexed triangle fan.
void CNullDriver::drawIndexedTriangleFan(const S3DVertex* vertices,
                                         s32 vertexCount, const u16* indexList, s32 triangleCount)
{
	PrimitivesDrawn += triangleCount;
}


//! Draws a 3d line.
void CNullDriver::draw3DLine(const core::vector3df& start,
                             const core::vector3df& end, SColor color)
{
	core::vector3df vect = start.crossProduct(end);
	vect.normalize();
	vect *= 1.0f;

	S3DVertex vtx[4];

	vtx[0].Color = color.toOpenGLColor();
	vtx[1].Color = color.toOpenGLColor();
	vtx[2].Color = color.toOpenGLColor();
	vtx[3].Color = color.toOpenGLColor();

	vtx[0].Pos = start;
	vtx[1].Pos = end;

	vtx[2].Pos = start + vect;
	vtx[3].Pos = end + vect;

	u16 idx[12] = {0,1,2, 0,2,1,   0,1,3, 0,3,1};

	drawIndexedTriangleList(vtx, 4, idx, 4);
}



//! Draws a 3d triangle.
void CNullDriver::draw3DTriangle(const core::triangle3df& triangle, SColor color)
{
	draw3DLine(triangle.pointA, triangle.pointB, color);
	draw3DLine(triangle.pointB, triangle.pointC, color);
	draw3DLine(triangle.pointC, triangle.pointA, color);
}



//! Draws a 3d axis aligned box.
void CNullDriver::draw3DBox(const core::aabbox3d<f32> box, SColor color)
{
	core::vector3df edges[8];
	box.getEdges(edges);

	// TODO: optimize into one big drawIndexePrimitive call.

	draw3DLine(edges[5], edges[1], color);
	draw3DLine(edges[1], edges[3], color);
	draw3DLine(edges[3], edges[7], color);
	draw3DLine(edges[7], edges[5], color);
	draw3DLine(edges[0], edges[2], color);
	draw3DLine(edges[2], edges[6], color);
	draw3DLine(edges[6], edges[4], color);
	draw3DLine(edges[4], edges[0], color);
	draw3DLine(edges[1], edges[0], color);
	draw3DLine(edges[3], edges[2], color);
	draw3DLine(edges[7], edges[6], color);
	draw3DLine(edges[5], edges[4], color);
}



//! draws an 2d image
void CNullDriver::draw2DImage(video::ITexture* texture, const core::position2d<s32>& destPos)
{
}

void CNullDriver::changeCursor(video::ITexture *texture)
{
}


//! Draws a part of the texture into the rectangle.
void CNullDriver::draw2DImage(video::ITexture* texture, const core::rect<s32>& destRect,
                              const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect,
                              video::SColor* colors, bool useAlphaChannelOfTexture)
{

}


//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
void CNullDriver::draw2DImage(video::ITexture* texture, const core::position2d<s32>& destPos,
                              const core::rect<s32>& sourceRect,
                              const core::rect<s32>* clipRect, SColor color,
                              bool useAlphaChannelOfTexture)
{
}



//! draw an 2d rectangle
void CNullDriver::draw2DRectangle(SColor color, const core::rect<s32>& pos, const core::rect<s32>* clip)
{
}


//!Draws an 2d rectangle with a gradient.
void CNullDriver::draw2DRectangle(const core::rect<s32>& pos,
                                  SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
                                  const core::rect<s32>* clip)
{
}


//! Draws a 2d line.
void CNullDriver::draw2DLine(const core::position2d<s32>& start,
                             const core::position2d<s32>& end,
                             SColor color)
{
}


//! Draws a non filled concyclic reqular 2d polyon.
void CNullDriver::draw2DPolygon(core::position2d<s32> center,
                                f32 radius, video::SColor color, s32 count)
{
	if (count < 2)
		return;

	core::position2d<s32> first;
	core::position2d<s32> a,b;

	for (s32 j=0; j<count; ++j)
	{
		b = a;

		f32 p = j / (f32)count * (core::PI*2);
		a = center + core::position2d<s32>((s32)(sin(p)*radius), (s32)(cos(p)*radius));

		if (j==0)
			first = a;
		else
			draw2DLine(a, b, color);
	}

	draw2DLine(a, first, color);
}




//! returns screen size
core::dimension2d<s32> CNullDriver::getScreenSize()
{
	return ScreenSize;
}



// returns current frames per second value
s32 CNullDriver::getFPS()
{
	return FPSCounter.getFPS();
}



//! returns amount of primitives (mostly triangles) were drawn in the last frame.
//! very useful method for statistics.
u32 CNullDriver::getPrimitiveCountDrawn()
{
	return PrimitivesDrawn;
}



//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
void CNullDriver::setAmbientLight(const SColorf& color)
{
}



//! \return Returns the name of the video driver. Example: In case of the DIRECT3D8
//! driver, it would return "Direct3D8".
const wchar_t* CNullDriver::getName()
{
	return L"engine NullDevice";
}



//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
//! this: Frist, draw all geometry. Then use this method, to draw the shadow
//! volume. Then, use IVideoDriver::drawStencilShadow() to visualize the shadow.
void CNullDriver::drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail)
{
}


//! Fills the stencil shadow with color. After the shadow volume has been drawn
//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
//! to draw the color of the shadow.
void CNullDriver::drawStencilShadow(bool clearStencilBuffer, video::SColor leftUpEdge,
                                    video::SColor rightUpEdge,      video::SColor leftDownEdge,     video::SColor rightDownEdge)
{
}


//! deletes all dynamic lights there are
void CNullDriver::deleteAllDynamicLights()
{
	Lights.set_used(0);
}



//! adds a dynamic light
void CNullDriver::addDynamicLight(const SLight& light)
{
	Lights.push_back(light);
}



//! returns the maximal amount of dynamic lights the device can handle
s32 CNullDriver::getMaximalDynamicLightAmount()
{
	return 0;
}

//! Returns current amount of dynamic lights set
//! \return Current amount of dynamic lights set
s32 CNullDriver::getDynamicLightCount()
{
	return Lights.size();
}

//! Returns light data which was previously set with IVideDriver::addDynamicLight().
//! \param idx: Zero based index of the light. Must be greater than 0 and smaller
//! than IVideoDriver()::getDynamicLightCount.
//! \return Light data.
const SLight& CNullDriver::getDynamicLight(s32 idx)
{
	if (idx<0 || idx>=(s32)Lights.size())
		return *((SLight*)0);

	return Lights[idx];
}


//! Creates an 1bit alpha channel of the texture based of an color key.
void CNullDriver::makeColorKeyTexture(video::ITexture* texture, video::SColor color)
{
	if (!texture)
		return;

	if (texture->getColorFormat() != ECF_A1R5G5B5 &&
	    texture->getColorFormat() != ECF_A8R8G8B8 )
	{
		os::Printer::log("Error: Unsupported texture color format for making color key channel.", ELL_ERROR);
		return;
	}

	if (texture->getColorFormat() == ECF_A1R5G5B5)
	{
		s16 *p = (s16*)texture->lock();
		if (!p)
		{
			os::Printer::log("Could not lock texture for making color key channel.", ELL_ERROR);
			return;
		}

		core::dimension2d<s32> dim = texture->getSize();
		s32 pitch = texture->getPitch() / 2;

		s16 ref = (0x0<<15) | (~(0x1<<15) & color.toA1R5G5B5());
		s16 blackalpha = (0x0<<15) | (~(0x1<<15) & 0);

		for (s32 x=0; x<pitch; ++x)
			for (s32 y=0; y<dim.Height; ++y)
			{
				s16 c = (0x0<<15) | (~(0x1<<15) & p[y*pitch + x]);
				p[y*pitch + x] = (c == ref) ? blackalpha : ((0x1<<15) | (~(0x1<<15) & c));
			}

		texture->unlock();
	}
	else
	{
		s32 *p = (s32*)texture->lock();

		if (!p)
		{
			os::Printer::log("Could not lock texture for making color key channel.", ELL_ERROR);
			return;
		}

		core::dimension2d<s32> dim = texture->getSize();
		s32 pitch = texture->getPitch() / 4;

		s32 ref = (0x0<<24) | (~(0xFF<<24) & color.color);
		s32 blackalpha = (0x0<<24) | (~(0xFF<<24) & 0);

		for (s32 x=0; x<pitch; ++x)
			for (s32 y=0; y<dim.Height; ++y)
			{
				s32 c = (0x0<<24) | (~(0xFF<<24) & p[y*pitch + x]);
				p[y*pitch + x] = (c == ref) ? blackalpha : ((0xFF<<24) | (~(0xFF<<24) & c));
			}

		texture->unlock();
	}
}



//! Creates an 1bit alpha channel of the texture based of an color key position.
void CNullDriver::makeColorKeyTexture(video::ITexture* texture,
                                      core::position2d<s32> colorKeyPixelPos)
{
	if (!texture)
		return;

	if (texture->getColorFormat() != ECF_A1R5G5B5 &&
	    texture->getColorFormat() != ECF_A8R8G8B8 )
	{
		os::Printer::log("Error: Unsupported texture color format for making color key channel.", ELL_ERROR);
		return;
	}

	if (texture->getColorFormat() == ECF_A1R5G5B5)
	{
		s16 *p = (s16*)texture->lock();

		if (!p)
		{
			os::Printer::log("Could not lock texture for making color key channel.", ELL_ERROR);
			return;
		}

		core::dimension2d<s32> dim = texture->getSize();
		s32 pitch = texture->getPitch() / 2;

		s16 ref = (0x0<<15) | (~(0x1<<15) & p[colorKeyPixelPos.Y*dim.Width + colorKeyPixelPos.X]);
		s16 blackalpha = (0x0<<15) | (~(0x1<<15) & 0);

		for (s32 x=0; x<pitch; ++x)
			for (s32 y=0; y<dim.Height; ++y)
			{
				s16 c = (0x0<<15) | (~(0x1<<15) & p[y*pitch + x]);
				p[y*pitch + x] = (c == ref) ? blackalpha : ((0x1<<15) | (~(0x1<<15) & c));
			}

		texture->unlock();
	}
	else
	{
		s32 *p = (s32*)texture->lock();

		if (!p)
		{
			os::Printer::log("Could not lock texture for making color key channel.", ELL_ERROR);
			return;
		}

		core::dimension2d<s32> dim = texture->getSize();
		s32 pitch = texture->getPitch() / 4;

#ifdef PPC
		s32 ref = (0x0) | (~(0xFF) & p[colorKeyPixelPos.Y*dim.Width + colorKeyPixelPos.X]);
		s32 blackalpha = (0x0) | (~(0xFF) & 0);
#else
		s32 ref = (0x0<<24) | (~(0xFF<<24) & p[colorKeyPixelPos.Y*dim.Width + colorKeyPixelPos.X]);
		s32 blackalpha = (0x0<<24) | (~(0xFF<<24) & 0);
#endif

		for (s32 x=0; x<pitch; ++x)
			for (s32 y=0; y<dim.Height; ++y)
			{
#ifdef PPC
				s32 c = (0x0) | (~(0xFF) & p[y*pitch + x]);
				p[y*pitch + x] = (c == ref) ? blackalpha : ((0xFF) | (~(0xFF) & c));
#else
				s32 c = (0x0<<24) | (~(0xFF<<24) & p[y*pitch + x]);
				p[y*pitch + x] = (c == ref) ? blackalpha : ((0xFF<<24) | (~(0xFF<<24) & c));
#endif
			}

		texture->unlock();
	}
}


//! Creates a normal map from a height map texture.
//! \param amplitude: Constant value by which the height information is multiplied.
void CNullDriver::makeNormalMapTexture(video::ITexture* texture, f32 amplitude)
{
	if (!texture)
		return;

	if (texture->getColorFormat() != ECF_A1R5G5B5 &&
	    texture->getColorFormat() != ECF_A8R8G8B8 )
	{
		os::Printer::log("Error: Unsupported texture color format for making normal map.", ELL_ERROR);
		return;
	}

	core::dimension2d<s32> dim = texture->getSize();
	amplitude = amplitude / 255.0f;
	f32 vh = dim.Height / (f32)dim.Width;
	f32 hh = dim.Width / (f32)dim.Height;

	if (texture->getColorFormat() == ECF_A8R8G8B8)
	{
		// ECF_A8R8G8B8 version

		s32 *p = (s32*)texture->lock();

		if (!p)
		{
			os::Printer::log("Could not lock texture for making normal map.", ELL_ERROR);
			return;
		}

		// copy texture

		s32 pitch = texture->getPitch() / 4;

		s32* in = new s32[dim.Height * pitch];
		memcpy(in, p, dim.Height * pitch * 4);

		for (s32 x=0; x<pitch; ++x)
			for (s32 y=0; y<dim.Height; ++y)
			{
				// TODO: this could be optimized really a lot

				core::vector3df h1((x-1)*hh, nml32(x-1, y, pitch, dim.Height, in)*amplitude, y*vh);
				core::vector3df h2((x+1)*hh, nml32(x+1, y, pitch, dim.Height, in)*amplitude, y*vh);
				//core::vector3df v1(x*hh, nml32(x, y-1, pitch, dim.Height, in)*amplitude, (y-1)*vh);
				//core::vector3df v2(x*hh, nml32(x, y+1, pitch, dim.Height, in)*amplitude, (y+1)*vh);
				core::vector3df v1(x*hh, nml32(x, y+1, pitch, dim.Height, in)*amplitude, (y-1)*vh);
				core::vector3df v2(x*hh, nml32(x, y-1, pitch, dim.Height, in)*amplitude, (y+1)*vh);

				core::vector3df v = v1-v2;
				core::vector3df h = h1-h2;

				core::vector3df n = v.crossProduct(h);
				n.normalize();
				n *= 0.5f;
				n += core::vector3df(0.5f,0.5f,0.5f); // now between 0 and 1
				n *= 255.0f;

				s32 height = (s32)nml32(x, y, pitch, dim.Height, in);
				p[y*pitch + x] = video::SColor(
				        height, // store height in alpha
				        (s32)n.X, (s32)n.Z, (s32)n.Y).color;
			}

		delete [] in;
		texture->unlock();
	}
	else
	{
		// ECF_A1R5G5B5 version

		s16 *p = (s16*)texture->lock();

		if (!p)
		{
			os::Printer::log("Could not lock texture for making normal map.", ELL_ERROR);
			return;
		}

		s32 pitch = texture->getPitch() / 2;

		// copy texture

		s16* in = new s16[dim.Height * pitch];
		memcpy(in, p, dim.Height * pitch * 2);

		for (s32 x=0; x<pitch; ++x)
			for (s32 y=0; y<dim.Height; ++y)
			{
				// TODO: this could be optimized really a lot

				core::vector3df h1((x-1)*hh, nml16(x-1, y, pitch, dim.Height, in)*amplitude, y*vh);
				core::vector3df h2((x+1)*hh, nml16(x+1, y, pitch, dim.Height, in)*amplitude, y*vh);
				core::vector3df v1(x*hh, nml16(x, y-1, pitch, dim.Height, in)*amplitude, (y-1)*vh);
				core::vector3df v2(x*hh, nml16(x, y+1, pitch, dim.Height, in)*amplitude, (y+1)*vh);

				core::vector3df v = v1-v2;
				core::vector3df h = h1-h2;

				core::vector3df n = v.crossProduct(h);
				n.normalize();
				n *= 0.5f;
				n += core::vector3df(0.5f,0.5f,0.5f); // now between 0 and 1
				n *= 255.0f;

				p[y*pitch + x] = video::RGB16((s32)n.X, (s32)n.Z, (s32)n.Y);
			}

		delete [] in;
		texture->unlock();
	}

	texture->regenerateMipMapLevels();
}


//! Returns the maximum amount of primitives (mostly vertices) which
//! the device is able to render with one drawIndexedTriangleList
//! call.
s32 CNullDriver::getMaximalPrimitiveCount()
{
	return 65535;
}


//! checks triangle count and print warning if wrong
bool CNullDriver::checkPrimitiveCount(s32 prmCount)
{
	s32 m = getMaximalPrimitiveCount();
	if ((prmCount-1) > m)
	{
		char tmp[1024];
		sprintf(tmp,"Could not draw triangles, too many primitives(%d), maxium is %d.", prmCount, m);
		os::Printer::log(tmp, ELL_ERROR);
		return false;
	}

	return true;
}

//! Enables or disables a texture creation flag.
void CNullDriver::setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled)
{
	if (enabled && ((flag == ETCF_ALWAYS_16_BIT) || (flag == ETCF_ALWAYS_32_BIT)
	                || (flag == ETCF_OPTIMIZED_FOR_QUALITY) || (flag == ETCF_OPTIMIZED_FOR_SPEED)))
	{
		// disable other formats
		setTextureCreationFlag(ETCF_ALWAYS_16_BIT, false);
		setTextureCreationFlag(ETCF_ALWAYS_32_BIT, false);
		setTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY, false);
		setTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED, false);
	}

	// set flag
	TextureCreationFlags = (TextureCreationFlags & (~flag)) |
	                       ((((u32) !enabled)-1) & flag);
}


//! Returns if a texture creation flag is enabled or disabled.
bool CNullDriver::getTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag)
{
	return (TextureCreationFlags & flag)!=0;
}


//! Creates a software image from a file.
IImage* CNullDriver::createImageFromFile(const char* filename)
{
	IImage* image = 0;
	io::IReadFile* file = FileSystem->createAndOpenFile(filename);

	if (file)
	{
		image = createImageFromFile(file);
		file->drop();
	}
	else
		os::Printer::log("Could not open file of image", filename, ELL_ERROR);

	return image;
}


//! Creates a software image from a file.
IImage* CNullDriver::createImageFromFile(io::IReadFile* file)
{
	IImage* image = 0;
	// try to load file based on file extension

	u32 i;

	for (i=0; i<SurfaceLoader.size(); ++i)
		if (SurfaceLoader[i]->isALoadableFileExtension(file->getFileName()))
		{

			image = SurfaceLoader[i]->loadImage(file);
			if (image)
				break;
		}

	// try to load file based on what is in it
	if (!image)
		for (i=0; i<SurfaceLoader.size(); ++i)
		{
			if (i!=0)
				file->seek(0);

			if (SurfaceLoader[i]->isALoadableFileFormat(file))
			{
				file->seek(0);
				image = SurfaceLoader[i]->loadImage(file);
				if (image)
					break;
			}
		}

	return image;
}

//! Creates a software image from a byte array.
IImage* CNullDriver::createImageFromData(ECOLOR_FORMAT format,
                                         const core::dimension2d<s32>& size, void *data,
                                         bool ownForeignMemory)
{
	return new CImage(format, size, data, ownForeignMemory);
}


//! Sets the fog mode.
void CNullDriver::setFog(SColor color, bool linearFog, f32 start, f32 end, f32 density,
                         bool pixelFog, bool rangeFog)
{
	FogColor = color;
	LinearFog = linearFog;
	FogStart = start;
	FogEnd = end;
	FogDensity = density;
	PixelFog = pixelFog;
	RangeFog = rangeFog;
}


#define printf pspDebugScreenPrintf

//! Draws a mesh buffer
void CNullDriver::drawMeshBuffer(scene::IMeshBuffer* mb)
{
	if (!mb)
		return;

	switch(mb->getVertexType())
	{
	case video::EVT_STANDARD:
//int d = os::Timer::getRealTime();
		drawIndexedTriangleList((video::S3DVertex*)mb->getVertices(),
		                        mb->getVertexCount(), mb->getIndices(), mb->getIndexCount()/ 3);
//printf("rr2 %d\n", os::Timer::getRealTime() - d);
		break;

	case video::EVT_2TCOORDS:

		drawIndexedTriangleList((video::S3DVertex2TCoords*)mb->getVertices(),
		                        mb->getVertexCount(), mb->getIndices(), mb->getIndexCount()/ 3);
		break;

	case video::EVT_TANGENTS:

		drawIndexedTriangleList((video::S3DVertexTangents*)mb->getVertices(),
		                        mb->getVertexCount(), mb->getIndices(), mb->getIndexCount()/ 3);
		break;
	}

}



//! Only used by the internal engine. Used to notify the driver that
//! the window was resized.
void CNullDriver::OnResize(const core::dimension2d<s32>& size)
{
	if (ViewPort.getWidth() == ScreenSize.Width &&
	    ViewPort.getHeight() == ScreenSize.Height)
		ViewPort = core::rect<s32>(core::position2d<s32>(0,0), size);

	ScreenSize = size;
}

// adds a material renderer and drops it afterwards. To be used for internal creation
s32 CNullDriver::addAndDropMaterialRenderer(IMaterialRenderer* m)
{
	s32 i = addMaterialRenderer(m);

	if (m)
		m->drop();

	return i;
}


//! Adds a new material renderer to the video device.
s32 CNullDriver::addMaterialRenderer(IMaterialRenderer* renderer)
{
	if (!renderer)
		return -1;

	MaterialRenderers.push_back(renderer);
	renderer->grab();

	return MaterialRenderers.size()-1;
}


//! Returns driver and operating system specific data about the IVideoDriver.
SExposedVideoData CNullDriver::getExposedVideoData()
{
	return ExposedData;
}


//! Returns type of video driver
E_DRIVER_TYPE CNullDriver::getDriverType()
{
	return EDT_NULL;
}

//! deletes all material renderers
void CNullDriver::deleteMaterialRenders()
{
	// delete material renderers
	for (int i=0; i<(int)MaterialRenderers.size(); ++i)
		if (MaterialRenderers[i])
			MaterialRenderers[i]->drop();

	MaterialRenderers.clear();
}

//! Returns pointer to material renderer or null
IMaterialRenderer* CNullDriver::getMaterialRenderer(s32 idx)
{
	if (idx < 0 || idx >= (s32)MaterialRenderers.size())
		return 0;

	return MaterialRenderers[idx];
}



//! Returns amount of currently available material renderers.
s32 CNullDriver::getMaterialRendererCount()
{
	return MaterialRenderers.size();
}


//! Returns pointer to the IGPUProgrammingServices interface.
IGPUProgrammingServices* CNullDriver::getGPUProgrammingServices()
{
	return 0;
}

//! Adds a new material renderer to the VideoDriver, based on a high level shading
//! language. Currently only HLSL in D3D9 is supported.
s32 CNullDriver::addHighLevelShaderMaterial(
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
	os::Printer::log("High level shader materials not available (yet) in this driver, sorry");
	return -1;
}

//! Like IGPUProgrammingServices::addShaderMaterial() (look there for a detailed description),
//! but tries to load the programs from files.
s32 CNullDriver::addHighLevelShaderMaterialFromFiles(
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
	io::IReadFile* vsfile = 0;
	io::IReadFile* psfile = 0;

	if (vertexShaderProgram)
	{
		vsfile = FileSystem->createAndOpenFile(vertexShaderProgram);
		if (!vsfile)
		{
			os::Printer::log("Could not open vertex shader program file",
			                 vertexShaderProgram, ELL_WARNING);
			return -1;
		}
	}

	if (pixelShaderProgram)
	{
		psfile = FileSystem->createAndOpenFile(pixelShaderProgram);
		if (!psfile)
		{
			os::Printer::log("Could not open pixel shader program file",
			                 pixelShaderProgram, ELL_WARNING);
			if (vsfile)
				vsfile->drop();
			return -1;
		}
	}

	s32 result = addHighLevelShaderMaterialFromFiles(
	        vsfile, vertexShaderEntryPointName, vsCompileTarget,
	        psfile, pixelShaderEntryPointName, psCompileTarget,
	        callback, baseMaterial, userData);

	if (psfile)
		psfile->drop();

	if (vsfile)
		vsfile->drop();

	return result;
}

//! Like IGPUProgrammingServices::addShaderMaterial() (look there for a detailed description),
//! but tries to load the programs from files.
s32 CNullDriver::addHighLevelShaderMaterialFromFiles(
        io::IReadFile* vertexShaderProgram,
        const c8* vertexShaderEntryPointName,
        E_VERTEX_SHADER_TYPE vsCompileTarget,
        io::IReadFile* pixelShaderProgram,
        const c8* pixelShaderEntryPointName,
        E_PIXEL_SHADER_TYPE psCompileTarget,
        IShaderConstantSetCallBack* callback,
        E_MATERIAL_TYPE baseMaterial,
        s32 userData)
{
	c8* vs = 0;
	c8* ps = 0;

	if (vertexShaderProgram)
	{
		s32 size = vertexShaderProgram->getSize();
		if (size)
		{
			vs = new c8[size+1];
			vertexShaderProgram->read(vs, size);
			vs[size] = 0;
		}
	}

	if (pixelShaderProgram)
	{
		s32 size = pixelShaderProgram->getSize();
		if (size)
		{
			ps = new c8[size+1];
			pixelShaderProgram->read(ps, size);
			ps[size] = 0;
		}
	}

	s32 result = this->addHighLevelShaderMaterial(
	        vs, vertexShaderEntryPointName, vsCompileTarget,
	        ps, pixelShaderEntryPointName, psCompileTarget,
	        callback, baseMaterial, userData);

	delete [] vs;
	delete [] ps;

	return result;
}

//! Adds a new material renderer to the VideoDriver, using pixel and/or
//! vertex shaders to render geometry.
s32 CNullDriver::addShaderMaterial(const c8* vertexShaderProgram,
                                   const c8* pixelShaderProgram,
                                   IShaderConstantSetCallBack* callback,
                                   E_MATERIAL_TYPE baseMaterial,
                                   s32 userData)
{
	os::Printer::log("Shader materials not implemented yet in this driver, sorry.");
	return -1;
}

//! Like IGPUProgrammingServices::addShaderMaterial(), but tries to load the
//! programs from files.
s32 CNullDriver::addShaderMaterialFromFiles(io::IReadFile* vertexShaderProgram,
                                            io::IReadFile* pixelShaderProgram,
                                            IShaderConstantSetCallBack* callback,
                                            E_MATERIAL_TYPE baseMaterial,
                                            s32 userData)
{
	c8* vs = 0;
	c8* ps = 0;

	if (vertexShaderProgram)
	{
		s32 size = vertexShaderProgram->getSize();
		if (size)
		{
			vs = new c8[size+1];
			vertexShaderProgram->read(vs, size);
			vs[size] = 0;
		}
	}

	if (pixelShaderProgram)
	{
		s32 size = pixelShaderProgram->getSize();
		if (size)
		{
			ps = new c8[size+1];
			pixelShaderProgram->read(ps, size);
			ps[size] = 0;
		}
	}

	s32 result = addShaderMaterial(vs, ps, callback, baseMaterial, userData);

	delete [] vs;
	delete [] ps;

	return result;
}



//! Like IGPUProgrammingServices::addShaderMaterial(), but tries to load the
//! programs from files.
s32 CNullDriver::addShaderMaterialFromFiles(const c8* vertexShaderProgramFileName,
                                            const c8* pixelShaderProgramFileName,
                                            IShaderConstantSetCallBack* callback,
                                            E_MATERIAL_TYPE baseMaterial,
                                            s32 userData)
{
	io::IReadFile* vsfile = 0;
	io::IReadFile* psfile = 0;

	if (vertexShaderProgramFileName)
	{
		vsfile = FileSystem->createAndOpenFile(vertexShaderProgramFileName);
		if (!vsfile)
		{
			os::Printer::log("Could not open vertex shader program file",
			                 vertexShaderProgramFileName, ELL_WARNING);
			return -1;
		}
	}

	if (pixelShaderProgramFileName)
	{
		psfile = FileSystem->createAndOpenFile(pixelShaderProgramFileName);
		if (!psfile)
		{
			os::Printer::log("Could not open pixel shader program file",
			                 pixelShaderProgramFileName, ELL_WARNING);
			if (vsfile)
				vsfile->drop();
			return -1;
		}
	}

	s32 result = addShaderMaterialFromFiles(vsfile, psfile, callback,
	                                        baseMaterial, userData);

	if (psfile)
		psfile->drop();

	if (vsfile)
		vsfile->drop();

	return result;
}

//! Creates a render target texture.
ITexture* CNullDriver::createRenderTargetTexture(core::dimension2d<s32> size)
{
	return 0;
}

//! Clears the ZBuffer.
void CNullDriver::clearZBuffer()
{
}

// prints renderer version
void CNullDriver::printVersion()
{
	core::stringc namePrint = "Using renderer: ";
	namePrint += getName();
	os::Printer::log(namePrint.c_str(), ELL_INFORMATION);
}


//! creates a video driver
IVideoDriver* createNullDriver(io::IFileSystem* io, const core::dimension2d<s32>& screenSize)
{
	return new CNullDriver(io, screenSize);
}


} // end namespace
} // end namespace

