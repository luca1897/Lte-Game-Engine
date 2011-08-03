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
 
#ifndef __C_TRIANGLE_RENDERER_TEXTURE_GOURAUD_H_INCLUDED__
#define __C_TRIANGLE_RENDERER_TEXTURE_GOURAUD_H_INCLUDED__

#include "ITriangleRenderer.h"
#include "rect.h"

namespace engine
{
namespace video
{

class CTRTextureGouraud : public ITriangleRenderer
{
public:

//! constructor
CTRTextureGouraud(IZBuffer* zbuffer);

//! destructor
virtual ~CTRTextureGouraud();

//! sets a render target
virtual void setRenderTarget(video::IImage* surface, const core::rect<s32>& viewPort);

//! draws an indexed triangle list
virtual void drawIndexedTriangleList(S2DVertex* vertices, s32 vertexCount, const u16* indexList, s32 triangleCount);

//! en or disables the backface culling
virtual void setBackfaceCulling(bool enabled = true);

//! sets the Texture
virtual void setTexture(video::IImage* texture);

protected:

//! vertauscht zwei vertizen
inline void swapVertices(const S2DVertex** v1, const S2DVertex** v2)
{
	const S2DVertex* b = *v1;
	*v1 = *v2;
	*v2 = b;
}

video::IImage* RenderTarget;
core::rect<s32> ViewPortRect;

IZBuffer* ZBuffer;

s32 SurfaceWidth;
s32 SurfaceHeight;
bool BackFaceCullingEnabled;
TZBufferType* lockedZBuffer;
s16* lockedSurface;
s16* lockedTexture;
s32 lockedTextureWidth;
s32 textureXMask, textureYMask;
video::IImage* Texture;
};

} // end namespace video
} // end namespace engine

#endif

