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
 
#include "CSceneNodeAnimatorFlyStraight.h"

namespace engine
{
namespace scene
{


//! constructor
CSceneNodeAnimatorFlyStraight::CSceneNodeAnimatorFlyStraight(const core::vector3df& startPoint,
                                                             const core::vector3df& endPoint, u32 timeForWay,
                                                             bool loop, u32 now)
	: Start(startPoint), End(endPoint), StartTime(now), TimeForWay(timeForWay), Loop(loop)
{
	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorFlyStraight");
	#endif

	EndTime = now + timeForWay;

	Vector = End - Start;
	WayLength = (f32)Vector.getLength();
	Vector.normalize();

	TimeFactor = WayLength / TimeForWay;
}



//! destructor
CSceneNodeAnimatorFlyStraight::~CSceneNodeAnimatorFlyStraight()
{
}



//! animates a scene node
void CSceneNodeAnimatorFlyStraight::animateNode(ISceneNode* node, u32 timeMs)
{
	if (!node)
		return;

	u32 t = (timeMs-StartTime);

	core::vector3df pos = Start;

	if (!Loop && t >= TimeForWay)
		pos = End;
	else
		pos += Vector * (f32)fmod((f32)t, (f32)TimeForWay) * TimeFactor;

	node->setPosition(pos);
}



} // end namespace scene
} // end namespace engine
