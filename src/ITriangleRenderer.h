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
 
#ifndef __I_TRIANGLE_RENDERER_H_INCLUDED__
#define __I_TRIANGLE_RENDERER_H_INCLUDED__

#include "IUnknown.h"
#include "IImage.h"
#include "S2DVertex.h"
#include "rect.h"
#include "IZBuffer.h"
#include "IImage.h"

namespace engine
{
namespace video
{

enum ETriangleRenderer
{
	ETR_FLAT = 0,
	ETR_FLAT_WIRE,
	ETR_GOURAUD,
	ETR_GOURAUD_WIRE,
	ETR_TEXTURE_FLAT,
	ETR_TEXTURE_FLAT_WIRE,
	ETR_TEXTURE_GOURAUD,
	ETR_TEXTURE_GOURAUD_WIRE,
	ETR_TEXTURE_GOURAUD_NOZ,
	ETR_TEXTURE_GOURAUD_ADD,
	ETR_COUNT
};

class ITriangleRenderer : public IUnknown
{
public:

//! destructor
virtual ~ITriangleRenderer() {
};

//! sets a render target
virtual void setRenderTarget(video::IImage* surface, const core::rect<s32>& viewPort) = 0;

//! en or disables the backface culling
virtual void setBackfaceCulling(bool enabled = true) = 0;

//! sets the Texture
virtual void setTexture(video::IImage* texture) = 0;

//! draws an indexed triangle list
virtual void drawIndexedTriangleList(S2DVertex* vertices, s32 vertexCount, const u16* indexList, s32 triangleCount) = 0;
};


ITriangleRenderer* createTriangleRendererTextureGouraud(IZBuffer* zbuffer);
ITriangleRenderer* createTriangleRendererTextureGouraudWire(IZBuffer* zbuffer);
ITriangleRenderer* createTriangleRendererGouraud(IZBuffer* zbuffer);
ITriangleRenderer* createTriangleRendererGouraudWire(IZBuffer* zbuffer);
ITriangleRenderer* createTriangleRendererTextureFlat(IZBuffer* zbuffer);
ITriangleRenderer* createTriangleRendererTextureFlatWire(IZBuffer* zbuffer);
ITriangleRenderer* createTriangleRendererFlat(IZBuffer* zbuffer);
ITriangleRenderer* createTriangleRendererFlatWire(IZBuffer* zbuffer);
ITriangleRenderer* createTriangleRendererTextureGouraudNoZ();
ITriangleRenderer* createTriangleRendererTextureGouraudAdd(IZBuffer* zbuffer);


} // end namespace video
} // end namespace engine

#endif

