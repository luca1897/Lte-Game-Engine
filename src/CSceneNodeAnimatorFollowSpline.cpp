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
 
#include "CSceneNodeAnimatorFollowSpline.h"

namespace engine
{
namespace scene
{

//! constructor
CSceneNodeAnimatorFollowSpline::CSceneNodeAnimatorFollowSpline(u32 time,
                                                               const core::array< core::vector3df >& points, f32 speed,
                                                               f32 tightness)
	: Points(points), Speed(speed), StartTime(time), Tightness(tightness)
{
	NumPoints = Points.size();

	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorFollowSpline");
	#endif
}



//! destructor
CSceneNodeAnimatorFollowSpline::~CSceneNodeAnimatorFollowSpline()
{
}



inline s32 CSceneNodeAnimatorFollowSpline::clamp(s32 idx, s32 size)
{
	return ( idx<0 ? size+idx : ( idx>=size ? idx-size : idx ) );
}


//! animates a scene node
void CSceneNodeAnimatorFollowSpline::animateNode(ISceneNode* node, u32 timeMs)
{
	core::vector3df p, p0, p1, p2, p3;
	core::vector3df t1, t2;

	f32 dt = ( (timeMs-StartTime) * Speed );
	s32 idx = static_cast< s32 >( 0.001f * dt ) % NumPoints;
	f32 u = 0.001f * fmodf( dt, 1000.0f );

	p0 = Points[ clamp( idx - 1, NumPoints ) ];
	p1 = Points[ clamp( idx, NumPoints ) ];
	p2 = Points[ clamp( idx + 1, NumPoints ) ];
	p3 = Points[ clamp( idx + 2, NumPoints ) ];

	// hermite polynomials
	f32 h1 = 2.0f * u * u * u - 3.0f * u * u + 1.0f;
	f32 h2 = -2.0f * u * u * u + 3.0f * u * u;
	f32 h3 = u * u * u - 2.0f * u * u + u;
	f32 h4 = u * u * u - u * u;

	// tangents
	t1 = ( p2 - p0 ) * Tightness;
	t2 = ( p3 - p1 ) * Tightness;

	// interpolated point
	p = p1 * h1 + p2 * h2 + t1 * h3 + t2 * h4;

	node->setPosition(p);
}


} // end namespace scene
} // end namespace engine

