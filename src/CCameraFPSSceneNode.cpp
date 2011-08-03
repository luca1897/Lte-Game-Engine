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
 
#include "CCameraFPSSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "os.h"
#include <pspdebug.h>
#include <fmathv.h>
#include <float.h>
#include "Keycodes.h"
#include "stdio.h"
#include "quaternion.h"
#include <stdio.h>

#define printf pspDebugScreenPrintf
namespace engine
{
namespace scene
{

const f32 MAX_VERTICAL_ANGLE = 89.0f;

//! constructor
CCameraFPSSceneNode::CCameraFPSSceneNode(ISceneNode* parent, ISceneManager* mgr,
                                         gui::ICursorControl* cursorControl, s32 id, f32 rotateSpeed, f32 moveSpeed,
                                         SKeyMap* keyMapArray, s32 keyMapSize, bool noVerticalMovement, int moveStyle)
	: CCameraSceneNode(parent, mgr, id), CursorControl(cursorControl),
	MoveSpeed(moveSpeed), RotateSpeed(rotateSpeed), firstUpdate(true),
	NoVerticalMovement(noVerticalMovement), MoveStyle(moveStyle)
{
	#ifdef _DEBUG
	setDebugName("CCameraFPSSceneNode");
	#endif

	if (CursorControl)
		CursorControl->grab();

	MoveSpeed /= 1000.0f;

	recalculateViewArea();

	allKeysUp();

	if (MoveStyle != 0 && MoveStyle != 1 && MoveStyle != 2)
		MoveStyle = 0;

	// create key map
	if (!keyMapArray || !keyMapSize)
	{
		// create default key map
		KeyMap.push_back(SCamKeyMap(0, engine::KEY_UP));
		KeyMap.push_back(SCamKeyMap(1, engine::KEY_DOWN));
		KeyMap.push_back(SCamKeyMap(2, engine::KEY_LEFT));
		KeyMap.push_back(SCamKeyMap(3, engine::KEY_RIGHT));
	}
	else
	{
		// create custom key map

		for (s32 i=0; i<keyMapSize; ++i)
		{
			switch(keyMapArray[i].Action)
			{
			case EKA_MOVE_FORWARD: KeyMap.push_back(SCamKeyMap(0, keyMapArray[i].KeyCode));
				break;
			case EKA_MOVE_BACKWARD: KeyMap.push_back(SCamKeyMap(1, keyMapArray[i].KeyCode));
				break;
			case EKA_STRAFE_LEFT: KeyMap.push_back(SCamKeyMap(2, keyMapArray[i].KeyCode));
				break;
			case EKA_STRAFE_RIGHT: KeyMap.push_back(SCamKeyMap(3, keyMapArray[i].KeyCode));
				break;
			} // end switch
		} // end for
	} // end if
}

//! destructor
CCameraFPSSceneNode::~CCameraFPSSceneNode()
{
	if (CursorControl)
		CursorControl->drop();
}


//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for
//! example with scene::ISceneManager::addMayaCameraSceneNode or
//! scene::ISceneManager::addMeshViewerCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever.
bool CCameraFPSSceneNode::OnEvent(SEvent event)
{
	if (!InputReceiverEnabled)
		return false;

	if (event.EventType == EET_KEY_INPUT_EVENT)
	{
		s32 cnt = (s32)KeyMap.size();
		for (s32 i=0; i<cnt; ++i)
			if (KeyMap[i].keycode == event.KeyInput.Key)
			{
				CursorKeys[KeyMap[i].action] = event.KeyInput.PressedDown;
				return true;
			}
	}

	return false;
}



//! onPostRender is called just after rendering the whole scene.
//! nodes may calculate or store animations here, and may do other useful things,
//! dependent on what they are.
void CCameraFPSSceneNode::OnPostRender(u32 timeMs)
{
	animate();
	core::list<ISceneNodeAnimator*>::Iterator ait = Animators.begin();
	for (; ait != Animators.end(); ++ait)
		(*ait)->animateNode(this, timeMs);

	updateAbsolutePosition();
	Target = getPosition() + TargetVector;

	core::list<ISceneNode*>::Iterator it = Children.begin();
	for (; it != Children.end(); ++it)
		(*it)->OnPostRender(timeMs);
}

inline f32 __check__sign(f32 c) {
	return c >= 0 ? 1 : -1;
}

void CCameraFPSSceneNode::animate()
{
	if (SceneManager->getActiveCamera() != this)
		return;

	if (firstUpdate)
	{
		if (CursorControl)
			CursorControl->setPosition(0.5f, 0.5f);

		LastAnimationTime = os::Timer::getTime();

		firstUpdate = false;
		return;
	}

	// get time

	s32 now = os::Timer::getTime();
	s32 timeDiff = now - LastAnimationTime;
	LastAnimationTime = now;

	// Update rotation

	Target.set(0,0,1);

	if (!CursorControl)
		return;

	RelativeRotation.X *= -1.0f;
	RelativeRotation.Y *= -1.0f;

	// set target

	core::position2d<f32> cursorpos = CursorControl->getRelativePosition();
	if (!InputReceiverEnabled)
		return;

	if (MoveStyle == 0) {


		if (!core::equals(cursorpos.X, 0.5f) ||
		    !core::equals(cursorpos.Y, 0.5f))
		{
			RelativeRotation.Y += (0.5f - cursorpos.X) * RotateSpeed;
			RelativeRotation.X += (0.5f - cursorpos.Y) * RotateSpeed;
		}

	}

	if (MoveStyle == 1)
	{

		if (CursorKeys[0])
			RelativeRotation.X += 0.04f * RotateSpeed;

		if (CursorKeys[1])
			RelativeRotation.X -= 0.04f * RotateSpeed;

		if (CursorKeys[2])
			RelativeRotation.Y += 0.04f * RotateSpeed;
		if (CursorKeys[3])
			RelativeRotation.Y -= 0.04f * RotateSpeed;


	}

	if (MoveStyle == 2)
	{

		if (CursorKeys[0])
			RelativeRotation.X += 0.04f * RotateSpeed;

		if (CursorKeys[1])
			RelativeRotation.X -= 0.04f * RotateSpeed;

		if (cursorpos.X <= 0.46f || cursorpos.X >= 0.54f)
			RelativeRotation.Y += (0.5f - cursorpos.X) * RotateSpeed;

	}
	if (RelativeRotation.Y > 179.0f) RelativeRotation.Y = -179.0f;
	if (RelativeRotation.Y < -179.0f) RelativeRotation.Y = 179.0f;
	if (RelativeRotation.X > MAX_VERTICAL_ANGLE) RelativeRotation.X = MAX_VERTICAL_ANGLE;
	if (RelativeRotation.X < -MAX_VERTICAL_ANGLE) RelativeRotation.X = -MAX_VERTICAL_ANGLE;


	CursorControl->setPosition(0.5f, 0.5f);
	core::matrix4 mat;
	// update position
	core::vector3df pos = getPosition();
	mat.setRotationDegrees(core::vector3df(-RelativeRotation.X, -RelativeRotation.Y, 0));
	mat.transformVect(Target);


	core::vector3df movedir = Target;

	if (NoVerticalMovement)
		movedir.Y = 0.0f;

	movedir.normalize();

	if (MoveStyle == 1)
	{

		if (!core::equals(cursorpos.X, 0.5f) ||
		    !core::equals(cursorpos.Y, 0.5f))
		{
			pos += movedir * MoveSpeed  * (f32)timeDiff * __check__sign((0.5f - cursorpos.Y));
			core::vector3df strafevect = Target;
			strafevect = strafevect.crossProduct(UpVector);

			if (NoVerticalMovement)
				strafevect.Y = 0.0f;

			strafevect.normalize();

			pos += strafevect * MoveSpeed * (f32)timeDiff * __check__sign((0.5f - cursorpos.X));


		}
	}

	if (MoveStyle == 2)
	{
		if (!core::equals(cursorpos.Y, 0.5f))
			pos += movedir * MoveSpeed * (f32)timeDiff * __check__sign((0.5f - cursorpos.Y));

		core::vector3df strafevect = Target;
		strafevect = strafevect.crossProduct(UpVector);

		if (NoVerticalMovement)
			strafevect.Y = 0.0f;

		strafevect.normalize();

		if (CursorKeys[2])
			pos += strafevect * (f32)timeDiff * MoveSpeed;

		if (CursorKeys[3])
			pos -= strafevect * (f32)timeDiff * MoveSpeed;


	}

	if (MoveStyle == 0)
	{
		if (CursorKeys[0])
			pos += movedir * (f32)timeDiff * MoveSpeed;

		if (CursorKeys[1])
			pos -= movedir * (f32)timeDiff * MoveSpeed;

		// strafing

		core::vector3df strafevect = Target;
		strafevect = strafevect.crossProduct(UpVector);

		if (NoVerticalMovement)
			strafevect.Y = 0.0f;

		strafevect.normalize();

		if (CursorKeys[2])
			pos += strafevect * (f32)timeDiff * MoveSpeed;

		if (CursorKeys[3])
			pos -= strafevect * (f32)timeDiff * MoveSpeed;
	}

	// write translation

	setPosition(pos);

	// write right target

	TargetVector = Target;
	Target += pos;
	RelativeRotation.X *= -1.0f;
	RelativeRotation.Y *= -1.0f;

}


void CCameraFPSSceneNode::allKeysUp()
{
	for (s32 i=0; i<4; ++i)
		CursorKeys[i] = false;
}


//! sets the look at target of the camera
//! \param pos: Look at target of the camera.
void CCameraFPSSceneNode::setTarget(const core::vector3df& tgt)
{
	updateAbsolutePosition();
	core::vector3df vect = tgt - getAbsolutePosition();
	vect = vect.getHorizontalAngle();
	RelativeRotation.X = vect.X;
	RelativeRotation.Y = vect.Y;

	if (RelativeRotation.X > MAX_VERTICAL_ANGLE)
		RelativeRotation.X -= 360.0f;
}


} // end namespace
} // end namespace

