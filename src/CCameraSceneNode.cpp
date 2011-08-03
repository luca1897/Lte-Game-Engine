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
 
#include "CCameraSceneNode.h"
#include "ISceneManager.h"
#include "IVideoDriver.h"
#include "os.h"
extern struct pspvfpu_context *ctx;
int inormalize;

namespace engine
{
namespace scene
{


//! constructor
CCameraSceneNode::CCameraSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
                                   const core::vector3df& position, const core::vector3df& lookat)
	: ICameraSceneNode(parent, mgr, id, position, core::vector3df(0.0f, 0.0f, 0.0f),
	                   core::vector3df(1.0f, 1.0f, 1.0f)), InputReceiverEnabled(true)
{
	#ifdef _DEBUG
	setDebugName("CCameraSceneNode");
	#endif

	BBox.reset(0,0,0);

	// set default view

	UpVector.set(0.0f, 1.0f, 0.0f);
	Target.set(lookat);

	// set default projection

	fovy = 90.f * (core::PI / 180.f);       // Field of view, in radians.
	aspect = 9.f / 16.f;    // Aspect ratio.
	zn = 1.0f;                              // value of the near view-plane.
	zf = 3000.0f;                   // Z-value of the far view-plane.

	video::IVideoDriver* d = mgr->getVideoDriver();
	if (d)
	{
		screenDim.Width = (f32)d->getScreenSize().Width;
		screenDim.Height = (f32)d->getScreenSize().Height;
		//aspect = screenDim.Height / screenDim.Width;
	}

	recalculateProjectionMatrix();
}



//! destructor
CCameraSceneNode::~CCameraSceneNode()
{
}


//! Disables or enables the camera to get key or mouse inputs.
void CCameraSceneNode::setInputReceiverEnabled(bool enabled)
{
	InputReceiverEnabled = enabled;
}


//! Returns if the input receiver of the camera is currently enabled.
bool CCameraSceneNode::isInputReceiverEnabled()
{
	_engine_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return InputReceiverEnabled;
}


//! Sets the projection matrix of the camera. The core::matrix4 class has some methods
//! to build a projection matrix. e.g: core::matrix4::buildProjectionMatrixPerspectiveFovLH
//! \param projection: The new projection matrix of the camera.
void CCameraSceneNode::setProjectionMatrix(const core::matrix4& projection)
{
	Projection = projection;
}



//! Gets the current projection matrix of the camera
//! \return Returns the current projection matrix of the camera.
const core::matrix4& CCameraSceneNode::getProjectionMatrix()
{
	return Projection;
}



//! Gets the current view matrix of the camera
//! \return Returns the current view matrix of the camera.
const core::matrix4& CCameraSceneNode::getViewMatrix()
{
	return View;
}



//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for
//! example with scene::ISceneManager::addMayaCameraSceneNode or
//! scene::ISceneManager::addMeshViewerCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever.
bool CCameraSceneNode::OnEvent(SEvent event)
{
	return false;
}



//! sets the look at target of the camera
//! \param pos: Look at target of the camera.
void CCameraSceneNode::setTarget(const core::vector3df& pos)
{
	Target = pos;
}



//! Gets the current look at target of the camera
//! \return Returns the current look at target of the camera
core::vector3df CCameraSceneNode::getTarget() const
{
	return Target;
}



//! sets the up vector of the camera
//! \param pos: New upvector of the camera.
void CCameraSceneNode::setUpVector(const core::vector3df& pos)
{
	UpVector = pos;
}



//! Gets the up vector of the camera.
//! \return Returns the up vector of the camera.
core::vector3df CCameraSceneNode::getUpVector() const
{
	return UpVector;
}


f32 CCameraSceneNode::getNearValue()
{
	return zn;
}

f32 CCameraSceneNode::getFarValue()
{
	return zf;
}

f32 CCameraSceneNode::getAspectRatio()
{
	return aspect;
}

f32 CCameraSceneNode::getFOV()
{
	return fovy;
}

void CCameraSceneNode::setNearValue(f32 f)
{
	zn = f;
	recalculateProjectionMatrix();
}

void CCameraSceneNode::setFarValue(f32 f)
{
	zf = f;
	recalculateProjectionMatrix();
}

void CCameraSceneNode::setAspectRatio(f32 f)
{
	aspect = f;
	recalculateProjectionMatrix();
}

void CCameraSceneNode::setFOV(f32 f)
{
	fovy = f;
	recalculateProjectionMatrix();
}

void CCameraSceneNode::recalculateProjectionMatrix()
{
	Projection.buildProjectionMatrixPerspectiveFovLH(fovy, aspect, zn, zf);
	//recalculateViewArea();
}


#include <pspdebug.h>
#define printf pspDebugScreenPrintf

//! prerender
void CCameraSceneNode::OnPreRender()
{

	//printf("camera\n");
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	if (!driver)
		return;


	if (SceneManager->getActiveCamera() == this)
	{




		screenDim.Width = (f32)driver->getScreenSize().Width;
		screenDim.Height = (f32)driver->getScreenSize().Height;
		driver->setTransform(video::ETS_PROJECTION, Projection);
		//printf("2\n");
		// if upvector and vector to the target are the same, we have a
		// problem. so solve this problem:

		core::vector3df pos = getAbsolutePosition();
		core::vector3df tgtv = Target - pos;

		inormalize=1;

		tgtv.normalize();
		inormalize=0;
		core::vector3df up = UpVector;

		//printf("sto per normalizzare \n");
		inormalize=1;
		up.normalize();
		inormalize=0;

		f32 dp = tgtv.dotProduct(up);
		if ((dp > -1.0001f && dp < -0.9999f) ||
		    (dp < 1.0001f && dp > 0.9999f))
			up.X += 1.0f;

		//printf("7\n");
		View.buildCameraLookAtMatrixLH(pos, Target, up);
		//printf("8\n");
		recalculateViewArea();
		//printf("9\n");

		SceneManager->registerNodeForRendering(this, ESNRP_LIGHT_AND_CAMERA);
	}

	ISceneNode::OnPreRender();
	//printf("camera ok\n");
}
#undef printf

//! render
void CCameraSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	if (!driver)
		return;

	driver->setTransform(video::ETS_VIEW, View);
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CCameraSceneNode::getBoundingBox() const
{
	return BBox;
}



//! returns the view frustrum. needed sometimes by bsp or lod render nodes.
const SViewFrustrum* CCameraSceneNode::getViewFrustrum()
{
	return &ViewArea;
}


void CCameraSceneNode::recalculateViewArea()
{

	core::matrix4 mat = Projection * View;

	ViewArea = SViewFrustrum(mat);

	ViewArea.cameraPosition = getAbsolutePosition();

	ViewArea.recalculateBoundingBox();
}



} // end namespace
} // end namespace

