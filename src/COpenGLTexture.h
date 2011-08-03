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
 
#ifndef __C_OPEN_GL_TEXTURE_H_INCLUDED__
#define __C_OPEN_GL_TEXTURE_H_INCLUDED__

#include "ITexture.h"
#include "IImage.h"

#include "engineCompileConfig.h"
#include "engineTypes.h"
#include "CImage.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>


namespace engine
{
namespace video
{

//! OpenGL texture.
class COpenGLTexture : public ITexture
{
public:

//! constructor
COpenGLTexture(IImage* surface, bool generateMipLevels);

//! destructor
virtual ~COpenGLTexture();

//! lock function
virtual void* lock();

//! unlock function
virtual void unlock();

//! Returns original size of the texture.
virtual const core::dimension2d<s32>& getOriginalSize();

//! Returns (=size) of the texture.
virtual const core::dimension2d<s32>& getSize();

//! returns driver type of texture (=the driver, who created the texture)
virtual E_DRIVER_TYPE getDriverType();

//! returns color format of texture
virtual ECOLOR_FORMAT getColorFormat();

//! returns pitch of texture (in bytes)
virtual s32 getPitch();

//! return open gl texture name
GLuint getOpenGLTextureName();

//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
virtual void regenerateMipMapLevels();

private:

void getImageData(IImage* image);

//! copies the the texture into an open gl texture.
void copyTexture();

//! returns the size of a texture which would be the optimize size for rendering it
inline s32 getTextureSizeFromSurfaceSize(s32 size);

//! test if an error occurred, prints the problem, and returns
//! true if an error happened
inline bool testError();

core::dimension2d<s32> ImageSize;
core::dimension2d<s32> OriginalSize;
s32 Pitch;
bool SufaceHasSameSize;         // true if Surface has the same dimension as texture.
s32* ImageData;
GLuint TextureName;
bool hasMipMaps;
};


} // end namespace video
} // end namespace engine

#endif
