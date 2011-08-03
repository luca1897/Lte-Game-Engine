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
 
#include "engineTypes.h"
#include "COpenGLTexture.h"
#include "os.h"
#include "CColorConverter.h"

#include "engineCompileConfig.h"
#include "glext.h"

#include <pspdebug.h>
#define printf pspDebugScreenPrintf

namespace engine
{
namespace video
{

//! constructor
COpenGLTexture::COpenGLTexture(IImage* image, bool generateMipLevels)
	: Pitch(0), ImageSize(0,0), hasMipMaps(generateMipLevels),
	ImageData(0), TextureName(0)
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture");
	#endif


	if (image)
	{
		getImageData(image);

		if (ImageData)
		{
			glGenTextures(1, &TextureName);
			copyTexture();
		}
	}
}


//! destructor
COpenGLTexture::~COpenGLTexture()
{

	glDeleteTextures(1, &TextureName);
	if (ImageData)
		delete ImageData;
}

inline unsigned int swapcolor(unsigned int o)
{
	return (o & 0xFF) + (o & 0xFF << 24) + (o & 0xFF0000) + ( (o >> 24) & 0xFF);
}

void COpenGLTexture::getImageData(IImage* image)
{
	ImageSize = image->getDimension();
	OriginalSize = ImageSize;


	core::dimension2d<s32> nImageSize;
	nImageSize.Width = getTextureSizeFromSurfaceSize(ImageSize.Width);
	nImageSize.Height = getTextureSizeFromSurfaceSize(ImageSize.Height);

	if (!nImageSize.Width || !nImageSize.Height ||
	    !ImageSize.Width || !ImageSize.Height)
	{
		os::Printer::log("Could not create OpenGL Texture.", ELL_ERROR);
		return;
	}

	ImageData = new s32[nImageSize.Width * nImageSize.Height];

	if (nImageSize == ImageSize)
	{
		/*if (image->getColorFormat() == ECF_A8R8G8B8)
		   {
		   	s32 s = nImageSize.Width * nImageSize.Height;
		   	s32 *t = (s32*)image->lock();
		   	for (s32 i=0; i<s; ++i)
		   		ImageData[i] = swapcolor(t[i]);
		   	image->unlock();
		   }
		   else
		   {*/
		// slow converting
		for (s32 x=0; x<ImageSize.Width; ++x)
			for (s32 y=0; y<ImageSize.Height; ++y)
				ImageData[y*nImageSize.Width + x] = image->getPixel(x,y).color;
		//}
	}
	else
	{
		// scale texture

		s32* source = (s32*)image->lock();

		f32 sourceXStep = (f32)ImageSize.Width / (f32)nImageSize.Width;
		f32 sourceYStep = (f32)ImageSize.Height / (f32)nImageSize.Height;
		f32 sy;

		/*if (image->getColorFormat() == ECF_A8R8G8B8)
		   {
		   	// copy texture scaling
		   	for (s32 x=0; x<nImageSize.Width; ++x)
		   	{
		   		sy = 0.0f;

		   		for (s32 y=0; y<nImageSize.Height; ++y)
		   		{
		   			ImageData[(s32)(y*nImageSize.Width + x)] = source[(s32)(((s32)sy)*ImageSize.Width + x*sourceXStep)];
		   			sy+=sourceYStep;
		   		}
		   	}
		   }
		   else
		   {*/
		// convert texture scaling, slow
		for (s32 x=0; x<nImageSize.Width; ++x)
		{
			sy = 0.0f;

			for (s32 y=0; y<nImageSize.Height; ++y)
			{
				ImageData[(s32)(y*nImageSize.Width + x)] =
				        image->getPixel((s32)(x*sourceXStep), (s32)sy).color;

				sy+=sourceYStep;
			}
		}
	}


	ImageSize = nImageSize;
}
#include <pspkernel.h>


//! test if an error occurred, prints the problem, and returns
//! true if an error happened
inline bool COpenGLTexture::testError()
{
	GLenum g = glGetError();
	switch(g)
	{
	case GL_NO_ERROR:
		return false;
	case GL_INVALID_ENUM:
		printf("texture: GL_INVALID_ENUM"); break;
	case GL_INVALID_VALUE:
		printf("texture: GL_INVALID_VALUE"); break;
	case GL_INVALID_OPERATION:
		printf("texture: GL_INVALID_OPERATION"); break;
	case GL_STACK_OVERFLOW:
		printf("texture: GL_STACK_OVERFLOW"); break;
	case GL_STACK_UNDERFLOW:
		printf("texture: GL_STACK_UNDERFLOW"); break;
	case GL_OUT_OF_MEMORY:
		printf("texture: GL_OUT_OF_MEMORY"); break;
	};
	return true;
}

int exp2(int c) {
	int z = 1;
	for (int x = 1; x < c; x++) z*=2;
	return z;
}


//! copies the the texture into an open gl texture.
void COpenGLTexture::copyTexture()
{
	bool err = false;
	glGetError();
	glBindTexture(GL_TEXTURE_2D, TextureName);
	if (testError())
	{      printf("\nCould not bind Texture %d", TextureName); err = true; }

	s32 ret = 0;
	if (hasMipMaps)
		ret = gluBuild2DMipmaps(GL_TEXTURE_2D, 4, ImageSize.Width, ImageSize.Height,
		                        GL_RGBA, GL_UNSIGNED_BYTE, ImageData);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageSize.Width,
		             ImageSize.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ImageData);

	if (testError())
	{       printf("\nCould not glTexImage2D"); err = true; }

	if (err == true) while (1) sceKernelDelayThread(10000);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	if (hasMipMaps)
	{

		s32 levels = 1;

    #if 0
		CImage *mipmaps[10];
		mipmaps[0] = texture;

		for (  levels = 1; levels < 9 ; levels++)
		{
			s32 dstw, dsth;
			dstw = ImageSize.Width / exp2(levels);
			dsth = ImageSize.Height / exp2(levels);

			if (dstw < 8)
				dstw = 8;
			if (dsth < 8)
				dsth = 8;


			CImage *n = new CImage(ECF_A8R8G8B8, core::dimension2d<s32>(dstw, dsth));
			mipmaps[levels-1]->copyToScaling(n);

			glTexImage2D(GL_TEXTURE_2D, levels, GL_RGBA, dstw,
			             dsth, 0, GL_RGBA, GL_UNSIGNED_BYTE, n->lock());

			mipmaps[levels] = n;
		}

		for ( int o = 1; o < levels; o++)
			delete mipmaps[o];
    #else

    #endif
		if (ret)
		{
		}
		else
		{
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		}

	}
}


//! returns the size of a texture which would be the optimize size for rendering it
inline s32 COpenGLTexture::getTextureSizeFromSurfaceSize(s32 size)
{
	s32 ts = 0x01;
	while(ts < size)
		ts <<= 1;

	return ts;
}


//! lock function
void* COpenGLTexture::lock()
{
	return ImageData;
}



//! unlock function
void COpenGLTexture::unlock()
{
	if (ImageData)
		copyTexture();
}


//! Returns original size of the texture.
const core::dimension2d<s32>& COpenGLTexture::getOriginalSize()
{
	return OriginalSize;
}


//! Returns (=size) of the texture.
const core::dimension2d<s32>& COpenGLTexture::getSize()
{
	return ImageSize;
}


//! returns driver type of texture (=the driver, who created the texture)
E_DRIVER_TYPE COpenGLTexture::getDriverType()
{
	return EDT_OPENGL;
}



//! returns color format of texture
ECOLOR_FORMAT COpenGLTexture::getColorFormat()
{
	return ECF_A8R8G8B8;
}



//! returns pitch of texture (in bytes)
s32 COpenGLTexture::getPitch()
{
	return ImageSize.Width * 4;
}



//! return open gl texture name
GLuint COpenGLTexture::getOpenGLTextureName()
{
	/* if (hasMipMaps) {
	   		       glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
	   		       glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	   }
	 */
	return TextureName;
}


//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
void COpenGLTexture::regenerateMipMapLevels()
{
	if (ImageData)
		if (hasMipMaps)
			copyTexture();
}


} // end namespace video
} // end namespace engine