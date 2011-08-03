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
 
#include "CGUIMeshViewer.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IAnimatedMesh.h"
#include "engineMath.h"
#include "os.h"
#include "IGUISkin.h"

namespace engine
{

namespace gui
{


//! constructor
CGUIMeshViewer::CGUIMeshViewer(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
	: IGUIMeshViewer(environment, parent, id, rectangle), Mesh(0)
{
	#ifdef _DEBUG
	setDebugName("CGUIMeshViewer");
	#endif
}



//! destructor
CGUIMeshViewer::~CGUIMeshViewer()
{
	if (Mesh)
		Mesh->drop();
}



//! sets the mesh to be shown
void CGUIMeshViewer::setMesh(scene::IAnimatedMesh* mesh)
{
	if (Mesh)
		Mesh->drop();

	Mesh = mesh;
	if (!Mesh)
		return;

	core::vector3df center(0.0f,0.0f,0.0f);
	core::aabbox3d<f32> box;

	if (mesh->getFrameCount())
	{
		box = mesh->getMesh(0)->getBoundingBox();
		center = (box.MaxEdge + box.MinEdge) / 2;
	}

	if (Mesh)
		Mesh->grab();
}



//! sets the material
void CGUIMeshViewer::setMaterial(const video::SMaterial& material)
{
	Material = material;
}



//! gets the material
const video::SMaterial& CGUIMeshViewer::getMaterial()
{
	return Material;
}



//! called if an event happened.
bool CGUIMeshViewer::OnEvent(SEvent event)
{
	return Parent ? Parent->OnEvent(event) : false;
}



//! draws the element and its children
void CGUIMeshViewer::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	video::IVideoDriver* driver = Environment->getVideoDriver();
	core::rect<s32> viewPort = AbsoluteRect;
	viewPort.LowerRightCorner.X -= 1;
	viewPort.LowerRightCorner.Y -= 1;
	viewPort.UpperLeftCorner.X += 1;
	viewPort.UpperLeftCorner.Y += 1;

	viewPort.clipAgainst(AbsoluteClippingRect);

	// draw the frame

	core::rect<s32> frameRect(AbsoluteRect);
	frameRect.LowerRightCorner.Y = frameRect.UpperLeftCorner.Y + 1;
	driver->draw2DRectangle(skin->getColor(EGDC_3D_SHADOW), frameRect, &AbsoluteClippingRect);

	frameRect.LowerRightCorner.Y = AbsoluteRect.LowerRightCorner.Y;
	frameRect.LowerRightCorner.X = frameRect.UpperLeftCorner.X + 1;
	driver->draw2DRectangle(skin->getColor(EGDC_3D_SHADOW), frameRect, &AbsoluteClippingRect);

	frameRect = AbsoluteRect;
	frameRect.UpperLeftCorner.X = frameRect.LowerRightCorner.X - 1;
	driver->draw2DRectangle(skin->getColor(EGDC_3D_HIGH_LIGHT), frameRect, &AbsoluteClippingRect);

	frameRect = AbsoluteRect;
	frameRect.UpperLeftCorner.Y = AbsoluteRect.LowerRightCorner.Y - 1;
	frameRect.LowerRightCorner.Y = AbsoluteRect.LowerRightCorner.Y;
	driver->draw2DRectangle(skin->getColor(EGDC_3D_HIGH_LIGHT), frameRect, &AbsoluteClippingRect);

	// draw the mesh

	if (Mesh)
	{
		//TODO: if outside of screen, dont draw.
		// - why is the absolute clipping rect not already the screen?

		core::rect<s32> oldViewPort = driver->getViewPort();

		driver->setViewPort(viewPort);

		core::matrix4 mat;

		//CameraControl->calculateProjectionMatrix(mat);
		//driver->setTransform(video::TS_PROJECTION, mat);

		mat.makeIdentity();
		mat.setTranslation(core::vector3df(0,0,0));
		driver->setTransform(video::ETS_WORLD, mat);

		//CameraControl->calculateViewMatrix(mat);
		//driver->setTransform(video::TS_VIEW, mat);

		driver->setMaterial(Material);

		scene::IMesh* m = Mesh->getMesh(os::Timer::getTime()/20);
		for (s32 i=0; i<m->getMeshBufferCount(); ++i)
		{
			scene::IMeshBuffer* mb = m->getMeshBuffer(i);

			switch(mb->getVertexType())
			{
			case video::EVT_STANDARD:
				driver->drawIndexedTriangleList((video::S3DVertex*)mb->getVertices(), mb->getVertexCount(), mb->getIndices(), mb->getIndexCount()/ 3);
				break;
			case video::EVT_2TCOORDS:
				driver->drawIndexedTriangleList((video::S3DVertex2TCoords*)mb->getVertices(), mb->getVertexCount(), mb->getIndices(), mb->getIndexCount()/ 3);
				break;
			}
		}

		driver->setViewPort(oldViewPort);
	}

	IGUIElement::draw();
}


} // end namespace gui
} // end namespace engine


