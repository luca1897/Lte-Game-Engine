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
 
#include "SViewFrustrum.h"
#include <pspdebug.h>
#define printf pspDebugScreenPrintf
namespace engine
{

namespace scene
{
//! transforms the furstum by the matrix
//! \param Matrix by which the view frustrum is transformed.
void SViewFrustrum::transform(const core::matrix4 &mat)
{
	for (int i=0; i<VF_PLANE_COUNT; ++i)
		mat.transformPlane(planes[i]);

	mat.transformVect(cameraPosition);

	recalculateBoundingBox();
}


//! returns the point which is on the far left upper corner inside the
//! the view frustrum.
core::vector3df SViewFrustrum::getFarLeftUp() const
{
	core::vector3df p;
	planes[scene::SViewFrustrum::VF_FAR_PLANE].getIntersectionWithPlanes(
	        planes[scene::SViewFrustrum::VF_TOP_PLANE],
	        planes[scene::SViewFrustrum::VF_LEFT_PLANE], p);

	return p;
}

//! returns the point which is on the far left bottom corner inside the
//! the view frustrum.
core::vector3df SViewFrustrum::getFarLeftDown() const
{
	core::vector3df p;
	planes[scene::SViewFrustrum::VF_FAR_PLANE].getIntersectionWithPlanes(
	        planes[scene::SViewFrustrum::VF_BOTTOM_PLANE],
	        planes[scene::SViewFrustrum::VF_LEFT_PLANE], p);

	return p;
}

//! returns the point which is on the far right top corner inside the
//! the view frustrum.
core::vector3df SViewFrustrum::getFarRightUp() const
{
	core::vector3df p;
	planes[scene::SViewFrustrum::VF_FAR_PLANE].getIntersectionWithPlanes(
	        planes[scene::SViewFrustrum::VF_TOP_PLANE],
	        planes[scene::SViewFrustrum::VF_RIGHT_PLANE], p);

	return p;
}

//! returns the point which is on the far right bottom corner inside the
//! the view frustrum.
core::vector3df SViewFrustrum::getFarRightDown() const
{
	core::vector3df p;
	planes[scene::SViewFrustrum::VF_FAR_PLANE].getIntersectionWithPlanes(
	        planes[scene::SViewFrustrum::VF_BOTTOM_PLANE],
	        planes[scene::SViewFrustrum::VF_RIGHT_PLANE], p);

	return p;
}


//! returns a bounding box encosing the whole view frustrum
core::aabbox3d<f32> SViewFrustrum::getBoundingBox() const
{
	return boundingBox;
}


//! recalculates the bounding box member based on the planes
void SViewFrustrum::recalculateBoundingBox()
{
	core::aabbox3d<f32> box(cameraPosition);

	box.addInternalPoint(getFarLeftUp());
	box.addInternalPoint(getFarRightUp());
	box.addInternalPoint(getFarLeftDown());
	box.addInternalPoint(getFarRightDown());

	boundingBox = box;
}


//! This constructor creates a view frustrum based on a projection and/or
//! view matrix.
SViewFrustrum::SViewFrustrum(const core::matrix4& mat)
{
	        #define sw(a,b)         (mat((b),(a)))


	// left clipping plane
	planes[SViewFrustrum::VF_LEFT_PLANE].Normal.X = -(sw(0,3) + sw(0,0)) ;
	planes[SViewFrustrum::VF_LEFT_PLANE].Normal.Y = -(sw(1,3) + sw(1,0)) ;
	planes[SViewFrustrum::VF_LEFT_PLANE].Normal.Z = -(sw(2,3) + sw(2,0)) ;
	planes[SViewFrustrum::VF_LEFT_PLANE].D =                  -(sw(3,3) + sw(3,0)) ;
//	              printf("1.1\n");
	// right clipping plane
	planes[SViewFrustrum::VF_RIGHT_PLANE].Normal.X = -(sw(0,3) - sw(0,0)) ;
	planes[SViewFrustrum::VF_RIGHT_PLANE].Normal.Y = -(sw(1,3) - sw(1,0)) ;
	planes[SViewFrustrum::VF_RIGHT_PLANE].Normal.Z = -(sw(2,3) - sw(2,0)) ;
	planes[SViewFrustrum::VF_RIGHT_PLANE].D =        -(sw(3,3) - sw(3,0)) ;

	// top clipping plane
	planes[SViewFrustrum::VF_TOP_PLANE].Normal.X = -(sw(0,3) - sw(0,1)) ;
	planes[SViewFrustrum::VF_TOP_PLANE].Normal.Y = -(sw(1,3) - sw(1,1)) ;
	planes[SViewFrustrum::VF_TOP_PLANE].Normal.Z = -(sw(2,3) - sw(2,1)) ;
	planes[SViewFrustrum::VF_TOP_PLANE].D =        -(sw(3,3) - sw(3,1)) ;

	// bottom clipping plane
	planes[SViewFrustrum::VF_BOTTOM_PLANE].Normal.X = -(sw(0,3) + sw(0,1)) ;
	planes[SViewFrustrum::VF_BOTTOM_PLANE].Normal.Y = -(sw(1,3) + sw(1,1)) ;
	planes[SViewFrustrum::VF_BOTTOM_PLANE].Normal.Z = -(sw(2,3) + sw(2,1)) ;
	planes[SViewFrustrum::VF_BOTTOM_PLANE].D =        -(sw(3,3) + sw(3,1)) ;

	// near clipping plane
	planes[SViewFrustrum::VF_NEAR_PLANE].Normal.X = -sw(0,2) ;
	planes[SViewFrustrum::VF_NEAR_PLANE].Normal.Y = -sw(1,2) ;
	planes[SViewFrustrum::VF_NEAR_PLANE].Normal.Z = -sw(2,2) ;
	planes[SViewFrustrum::VF_NEAR_PLANE].D =        -sw(3,2) ;

	// far clipping plane
	planes[SViewFrustrum::VF_FAR_PLANE].Normal.X = -(sw(0,3) - sw(0,2)) ;
	planes[SViewFrustrum::VF_FAR_PLANE].Normal.Y = -(sw(1,3) - sw(1,2)) ;
	planes[SViewFrustrum::VF_FAR_PLANE].Normal.Z = -(sw(2,3) - sw(2,2)) ;
	planes[SViewFrustrum::VF_FAR_PLANE].D =        -(sw(3,3) - sw(3,2)) ;
	// normalize normals

	for (s32 i=0; i<6; ++i)
	{
		if ( planes[i].Normal.getLength() == 0 ) {
			planes[i].Normal *= 0;
			planes[i].D *= 0;
		} else {

			f32 len = (f32)(1.0f / planes[i].Normal.getLength());
			planes[i].Normal *= len;
			planes[i].D *= len;

		}
	}

	// make bounding box
	recalculateBoundingBox();
}

}
}
