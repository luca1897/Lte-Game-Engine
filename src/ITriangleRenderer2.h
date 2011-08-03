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

// Copyright (C) 2002-2006 Nikolaus Gebhardt/Alten Thomas
// This file is part of the "engine Engine".
// For conditions of distribution and use, see copyright notice in engine.h

#ifndef __I_TRIANGLE_2_RENDERER_H_INCLUDED__
#define __I_TRIANGLE_2_RENDERER_H_INCLUDED__

#include "SoftwareDriver2_compile_config.h"
#include "IUnknown.h"
#include "IImage.h"
#include "S2DVertex.h"
#include "rect.h"
#include "IZBuffer2.h"
#include "S4DVertex.h"



namespace engine
{


namespace video
{

enum ETriangleRenderer2
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
	ETR_TEXTURE_GOURAUD_ADD_NO_Z,

	ETR_TEXTURE_GOURAUD_VERTEX_ALPHA,

	ETR_TEXTURE_GOURAUD_LIGHTMAP,
	ETR_TEXTURE_GOURAUD_LIGHTMAP_M2,
	ETR_TEXTURE_GOURAUD_LIGHTMAP_M4,

	ETR_TEXTURE_GOURAUD_LIGHTMAP_ADD,

	ETR_TEXTURE_GOURAUD_DETAIL_MAP,

	ETR_GOURAUD_ALPHA,
	ETR_GOURAUD_ALPHA_NOZ,

	ETR_INVALID,

	ETR2_COUNT
};

class ITriangleRenderer2 : public IUnknown
{
public:
ITriangleRenderer2(IZBuffer2* zbuffer)
	: RenderTarget(0),ZBuffer(zbuffer)
{
	IT[0].Texture = 0;
	IT[1].Texture = 0;

	                #ifdef _DEBUG
	setDebugName("CTRTextureLightMap2_M1");
	                #endif

	if (ZBuffer)
		zbuffer->grab();
}


//! destructor
virtual ~ITriangleRenderer2()
{
	if (RenderTarget)
		RenderTarget->drop();

	if (ZBuffer)
		ZBuffer->drop();

	if ( IT[0].Texture )
		IT[0].Texture->drop();

	if ( IT[1].Texture )
		IT[1].Texture->drop();
};

//! sets a render target
virtual void setRenderTarget(video::IImage* surface, const core::rect<s32>& viewPort)
{
	if (RenderTarget)
		RenderTarget->drop();

	RenderTarget = surface;

	if (RenderTarget)
	{
		SurfaceWidth = RenderTarget->getDimension().Width;
		RenderTarget->grab();
	}
}



//! sets the Texture
virtual void setTexture( u32 stage, video::IImage* texture)
{
	sInternalTexture *it = &IT[stage];

	if ( it->Texture == texture )
		return;

	if ( it->Texture)
		it->Texture->drop();

	it->Texture = texture;

	if ( it->Texture)
	{
		it->Texture->grab();

		// prepare for optimal fixpoint
		it->pitch = (it->Texture->getPitch() );

		it->textureXMask = s32_to_fixPoint ( it->Texture->getDimension().Width - 1 );
		it->textureYMask = s32_to_fixPoint ( it->Texture->getDimension().Height - 1 );

		it->textureXMask |= FIX_POINT_FRACT_MASK;
		it->textureYMask |= FIX_POINT_FRACT_MASK;
	}
}


virtual void drawTriangle ( const s4DVertex *a,const s4DVertex *b,const s4DVertex *c ) = 0;
virtual void drawLine ( const s4DVertex *a,const s4DVertex *b) {
};


protected:

video::IImage* RenderTarget;

IZBuffer2* ZBuffer;

s32 SurfaceWidth;
TZBufferType2* lockedZBuffer;
tVideoSample* lockedSurface;

sInternalTexture IT[2];



};


ITriangleRenderer2* createTriangleRendererTextureGouraud2(IZBuffer2* zbuffer);
ITriangleRenderer2* createTriangleRendererTextureLightMap2_M1(IZBuffer2* zbuffer);
ITriangleRenderer2* createTriangleRendererTextureLightMap2_M2(IZBuffer2* zbuffer);
ITriangleRenderer2* createTriangleRendererTextureLightMap2_M4(IZBuffer2* zbuffer);
ITriangleRenderer2* createTriangleRendererTextureLightMap2_Add(IZBuffer2* zbuffer);
ITriangleRenderer2* createTriangleRendererTextureDetailMap2(IZBuffer2* zbuffer);
ITriangleRenderer2* createTriangleRendererTextureVertexAlpha2(IZBuffer2* zbuffer);


ITriangleRenderer2* createTriangleRendererTextureGouraudWire2(IZBuffer2* zbuffer);
ITriangleRenderer2* createTriangleRendererGouraud2(IZBuffer2* zbuffer);
ITriangleRenderer2* createTriangleRendererGouraudAlpha2(IZBuffer2* zbuffer);
ITriangleRenderer2* createTriangleRendererGouraudAlphaNoZ2(IZBuffer2* zbuffer);
ITriangleRenderer2* createTriangleRendererGouraudWire2(IZBuffer2* zbuffer);
ITriangleRenderer2* createTriangleRendererTextureFlat2(IZBuffer2* zbuffer);
ITriangleRenderer2* createTriangleRendererTextureFlatWire2(IZBuffer2* zbuffer);
ITriangleRenderer2* createTRFlat2(IZBuffer2* zbuffer);
ITriangleRenderer2* createTRFlatWire2(IZBuffer2* zbuffer);
ITriangleRenderer2* createTRTextureGouraudNoZ2();
ITriangleRenderer2* createTRTextureGouraudAdd2(IZBuffer2* zbuffer);
ITriangleRenderer2* createTRTextureGouraudAddNoZ2(IZBuffer2* zbuffer);



} // end namespace video
} // end namespace engine

#endif

