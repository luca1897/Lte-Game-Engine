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
 
#include "CSoftwareDriver.h"
#include "CSoftwareTexture.h"
#include "os.h"
#include "S3DVertex.h"
#include "SViewFrustrum.h"

namespace engine
{
namespace video
{


//! constructor
CSoftwareDriver::CSoftwareDriver(const core::dimension2d<s32>& windowSize, bool fullscreen, io::IFileSystem* io, video::IImagePresenter* presenter)
	: CNullDriver(io, windowSize), CurrentTriangleRenderer(0), Texture(0),
	ZBuffer(0), RenderTargetTexture(0), RenderTargetSurface(0)
{

}



//! destructor
CSoftwareDriver::~CSoftwareDriver()
{

}



//! switches to a triangle renderer
void CSoftwareDriver::switchToTriangleRenderer(ETriangleRenderer renderer)
{

}


//! void selects the right triangle renderer based on the render states.
void CSoftwareDriver::selectRightTriangleRenderer()
{

}




//! presents the rendered scene on the screen, returns false if failed
bool CSoftwareDriver::endScene( s32 windowId, core::rect<s32>* sourceRect )
{

}



//! queries the features of the driver, returns true if feature is available
bool CSoftwareDriver::queryFeature(E_VIDEO_DRIVER_FEATURE feature)
{

	return false;
}



//! sets transformation
void CSoftwareDriver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
{

}




//! sets the current Texture
void CSoftwareDriver::setTexture(video::ITexture* texture)
{

}



//! sets a material
void CSoftwareDriver::setMaterial(const SMaterial& material)
{

}


//! clears the zbuffer
bool CSoftwareDriver::beginScene(bool backBuffer, bool zBuffer, SColor color)
{

	return true;
}


//! sets a render target
bool CSoftwareDriver::setRenderTarget(video::ITexture* texture, bool clearBackBuffer,
                                      bool clearZBuffer, SColor color)
{


	return true;
}


//! sets a render target
void CSoftwareDriver::setRenderTarget(video::CImage* image)
{
}



//! sets a viewport
void CSoftwareDriver::setViewPort(const core::rect<s32>& area)
{
}


//! draws an indexed triangle list
void CSoftwareDriver::drawIndexedTriangleList(const S3DVertex* vertices, s32 vertexCount,
                                              const u16* indexList, s32 triangleCount)
{
	drawClippedIndexedTriangleListT(vertices, vertexCount, indexList, triangleCount);
}


//! Draws an indexed triangle list.
void CSoftwareDriver::drawIndexedTriangleList(const S3DVertexTangents* vertices,
                                              s32 vertexCount, const u16* indexList, s32 triangleCount)
{
	drawClippedIndexedTriangleListT(vertices, vertexCount, indexList, triangleCount);
}


//! draws an indexed triangle list
void CSoftwareDriver::drawIndexedTriangleList(const S3DVertex2TCoords* vertices, s32 vertexCount,
                                              const u16* indexList, s32 triangleCount)
{
	drawClippedIndexedTriangleListT(vertices, vertexCount, indexList, triangleCount);
}



template<class VERTEXTYPE>
void CSoftwareDriver::drawClippedIndexedTriangleListT(const VERTEXTYPE* vertices,
                                                      s32 vertexCount, const u16* indexList, s32 triangleCount)
{

}


//! draws an indexed triangle fan
void CSoftwareDriver::drawIndexedTriangleFan(const S3DVertex* vertices,
                                             s32 vertexCount, const u16* indexList, s32 triangleCount)
{
	// TODO: don't convert fan to list


}



//! draws an indexed triangle fan
void CSoftwareDriver::drawIndexedTriangleFan(const S3DVertex2TCoords* vertices, s32 vertexCount, const u16* indexList, s32 triangleCount)
{

}


//! clips a triangle agains the viewing frustrum
void CSoftwareDriver::clipTriangle(f32* transformedPos)
{

}


//! creates the clipping planes from the matrix
void CSoftwareDriver::createPlanes(core::matrix4& mat)
{
	Frustrum = scene::SViewFrustrum(mat);
}

//! draws an 2d image
void CSoftwareDriver::draw2DImage(video::ITexture* texture, const core::position2d<s32>& destPos)
{

}


//! Draws a 2d line.
void CSoftwareDriver::draw2DLine(const core::position2d<s32>& start,
                                 const core::position2d<s32>& end,
                                 SColor color)
{
	((CImage*)RenderTargetSurface)->drawLine(start, end, color );
}




//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
void CSoftwareDriver::draw2DImage(video::ITexture* texture, const core::position2d<s32>& destPos,
                                  const core::rect<s32>& sourceRect,
                                  const core::rect<s32>* clipRect, SColor color,
                                  bool useAlphaChannelOfTexture)
{

}



//! draw an 2d rectangle
void CSoftwareDriver::draw2DRectangle(SColor color, const core::rect<s32>& pos,
                                      const core::rect<s32>* clip)
{

}


//!Draws an 2d rectangle with a gradient.
void CSoftwareDriver::draw2DRectangle(const core::rect<s32>& pos,
                                      SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
                                      const core::rect<s32>* clip)
{
	// TODO: implement
	draw2DRectangle(colorLeftUp, pos, clip);
}


//! \return Returns the name of the video driver. Example: In case of the Direct3D8
//! driver, it would return "Direct3D8.1".
const wchar_t* CSoftwareDriver::getName()
{
	return L"engine Software Device 1.0";
}

//! Returns type of video driver
E_DRIVER_TYPE CSoftwareDriver::getDriverType()
{
	return EDT_SOFTWARE;
}

//! Returns the transformation set by setTransform
core::matrix4 CSoftwareDriver::getTransform(E_TRANSFORMATION_STATE state)
{
	return TransformationMatrix[state];
}

//! Creates a render target texture.
ITexture* CSoftwareDriver::createRenderTargetTexture(core::dimension2d<s32> size)
{
	CImage* img = new CImage(video::ECF_A1R5G5B5, size);
	ITexture* tex = new CSoftwareTexture(img);
	img->drop();
	return tex;
}


//! Clears the ZBuffer.
void CSoftwareDriver::clearZBuffer()
{
	if (ZBuffer)
		ZBuffer->clear();
}


//! creates a video driver
IVideoDriver* createSoftwareDriver(const core::dimension2d<s32>& windowSize, bool fullscreen, io::IFileSystem* io, video::IImagePresenter* presenter)
{
	return new CSoftwareDriver(windowSize, fullscreen, io, presenter);
}


} // end namespace video
} // end namespace engine
