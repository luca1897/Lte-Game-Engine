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
 
#ifndef __C_CAMERA_SCENE_NODE_H_INCLUDED__
#define __C_CAMERA_SCENE_NODE_H_INCLUDED__

#include "ICameraSceneNode.h"

namespace engine
{
namespace scene
{

class CCameraSceneNode : public ICameraSceneNode
{
public:

//! constructor
CCameraSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
                 const core::vector3df& position = core::vector3df(0,0,0),
                 const core::vector3df& lookat = core::vector3df(0,0,100));

//! destructor
virtual ~CCameraSceneNode();

//! Sets the projection matrix of the camera. The core::matrix4 class has some methods
//! to build a projection matrix. e.g: core::matrix4::buildProjectionMatrixPerspectiveFovLH
//! \param projection: The new projection matrix of the camera.
virtual void setProjectionMatrix(const core::matrix4& projection);

//! Gets the current projection matrix of the camera
//! \return Returns the current projection matrix of the camera.
virtual const core::matrix4& getProjectionMatrix();

//! Gets the current view matrix of the camera
//! \return Returns the current view matrix of the camera.
virtual const core::matrix4& getViewMatrix();

//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for
//! example with scene::ISceneManager::addMayaCameraSceneNode or
//! scene::ISceneManager::addMeshViewerCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever.
virtual bool OnEvent(SEvent event);

//! sets the look at target of the camera
//! \param pos: Look at target of the camera.
virtual void setTarget(const core::vector3df& pos);

//! Gets the current look at target of the camera
//! \return Returns the current look at target of the camera
virtual core::vector3df getTarget() const;

//! sets the up vector of the camera
//! \param pos: New upvector of the camera.
virtual void setUpVector(const core::vector3df& pos);

//! Gets the up vector of the camera.
//! \return Returns the up vector of the camera.
virtual core::vector3df getUpVector() const;

//! \return returns value of the near plane of the camera
virtual f32 getNearValue();

//! \return returns value of the far plane of the camera
virtual f32 getFarValue();

//! \return returns aspect ration of the camera
virtual f32 getAspectRatio();

//! \return returns field of view of the camera
virtual f32 getFOV();

//! sets the value of the near clipping plane. (default: 1.0f)
virtual void setNearValue(f32 zn);

//! sets the value of the far clipping plane (default: 2000.0f)
virtual void setFarValue(f32 zf);

//! sets the aspect ratio (default: 4.0f / 3.0f)
virtual void setAspectRatio(f32 aspect);

//! sets the field of view (Default: PI / 3.5f)
virtual void setFOV(f32 fovy);

//! pre render event
virtual void OnPreRender();

//! render
virtual void render();

//! returns the axis aligned bounding box of this node
virtual const core::aabbox3d<f32>& getBoundingBox() const;

//! returns the view area. needed sometimes by bsp or lod render nodes.
virtual const SViewFrustrum* getViewFrustrum();

//! Disables or enables the camera to get key or mouse inputs.
//! If this is set to true, the camera will respond to key inputs
//! otherwise not.
virtual void setInputReceiverEnabled(bool enabled);

//! Returns if the input receiver of the camera is currently enabled.
virtual bool isInputReceiverEnabled();

//! Returns type of the scene node
virtual ESCENE_NODE_TYPE getType() {
	return ESNT_CAMERA;
}

protected:

void recalculateProjectionMatrix();
void recalculateViewArea();

//core::vector3df Pos;
core::vector3df Target;
core::vector3df UpVector;

core::matrix4 Projection;
core::matrix4 View;

core::aabbox3d<f32> BBox;

f32 fovy;                               // Field of view, in radians.
f32 aspect;                     // Aspect ratio.
f32 zn;                         // value of the near view-plane.
f32 zf;                         // Z-value of the far view-plane.

core::dimension2d<f32> screenDim;

SViewFrustrum ViewArea;

bool InputReceiverEnabled;
};

} // end namespace
} // end namespace

#endif

