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
 
#ifndef __C_CAMERA_FPS_SCENE_NODE_H_INCLUDED__
#define __C_CAMERA_FPS_SCENE_NODE_H_INCLUDED__

#include "ICursorControl.h"
#include "CCameraSceneNode.h"
#include "vector2d.h"
#include "SKeyMap.h"
#include "engineArray.h"

namespace engine
{
namespace scene
{

class CCameraFPSSceneNode : public CCameraSceneNode
{
public:

//! constructor
CCameraFPSSceneNode(ISceneNode* parent, ISceneManager* mgr,
                    gui::ICursorControl* cursorControl, s32 id,
                    f32 rotateSpeed, f32 moveSpeed,
                    SKeyMap* keyMapArray, s32 keyMapSize, bool noVerticalMovement = false, int moveStyle = 0 );

//! destructor
virtual ~CCameraFPSSceneNode();

//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for
//! example with scene::ISceneManager::addMayaCameraSceneNode or
//! scene::ISceneManager::addMeshViewerCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever.
virtual bool OnEvent(SEvent event);

//! onPostRender is called just after rendering the whole scene.
//! nodes may calculate or store animations here, and may do other useful things,
//! dependent on what they are.
virtual void OnPostRender(u32 timeMs);

//! sets the look at target of the camera
//! \param pos: Look at target of the camera.
virtual void setTarget(const core::vector3df& pos);

//! Returns type of the scene node
virtual ESCENE_NODE_TYPE getType() {
	return ESNT_CAMERA_FPS;
}

private:

struct SCamKeyMap
{
	SCamKeyMap() {
	};
	SCamKeyMap(s32 a, EKEY_CODE k) : action(a), keycode(k) {
	}

	s32 action;
	EKEY_CODE keycode;
};

void allKeysUp();
void animate();

int MoveStyle;

bool CursorKeys[4];

f32 MoveSpeed;
f32 RotateSpeed;

//f32 RotationX;
//f32 RotationY;

gui::ICursorControl* CursorControl;

bool firstUpdate;
s32 LastAnimationTime;

core::vector3df TargetVector;
core::array<SCamKeyMap> KeyMap;

bool NoVerticalMovement;
};

} // end namespace
} // end namespace

#endif

