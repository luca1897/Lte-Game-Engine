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
 
#include "CTRTextureGouraud.h"
#include "SColor.h"

namespace engine
{
namespace video
{

class CTRTextureFlatWire : public CTRTextureGouraud
{
public:

CTRTextureFlatWire(IZBuffer* zbuffer)
	: CTRTextureGouraud(zbuffer)
{
	        #ifdef _DEBUG
	setDebugName("CTRTextureFlatWire");
	        #endif
}

//! draws an indexed triangle list
virtual void drawIndexedTriangleList(S2DVertex* vertices, s32 vertexCount, const u16* indexList, s32 triangleCount)
{


}
};


//! creates a flat triangle renderer
ITriangleRenderer* createTriangleRendererTextureFlatWire(IZBuffer* zbuffer)
{
	return new CTRTextureFlatWire(zbuffer);
}

} // end namespace video
} // end namespace engine
