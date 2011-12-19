/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __S_VIEW_FRUSTRUM_H_INCLUDED__
#define __S_VIEW_FRUSTRUM_H_INCLUDED__

#include "plane3d.h"
#include "vector3d.h"
#include "aabbox3d.h"
#include "matrix4.h"

namespace engine
{
namespace scene
{

	//! Defines the view frustrum. Thats the space viewed by the camera.
	/** The view frustrum is enclosed by 6 planes. These six planes share
	four points. A bounding box around these four points is also stored in
	this structure.
	*/
	struct SViewFrustrum
	{
		enum VFPLANES
		{
			//! Far plane of the frustrum. Thats the plane farest away from the eye.
			VF_FAR_PLANE = 0,
			//! Near plane of the frustrum. Thats the plane nearest to the eye.
			VF_NEAR_PLANE,
			//! Left plane of the frustrum.
			VF_LEFT_PLANE,
			//! Right plane of the frustrum.
			VF_RIGHT_PLANE,
			//! Bottom plane of the frustrum.
			VF_BOTTOM_PLANE,
			//! Top plane of the frustrum.
			VF_TOP_PLANE,

			//! Amount of planes enclosing the view frustum. Should be 6.
			VF_PLANE_COUNT
		};

		//! Default Constructor
		SViewFrustrum() {};

		//! This constructor creates a view frustrum based on a projection and/or
		//! view matrix.
		SViewFrustrum(const core::matrix4& mat);

		//! the position of the camera
		core::vector3df cameraPosition;

		//! all planes enclosing the view frustrum.
		core::plane3d<f32> planes[VF_PLANE_COUNT];

		//! bouding box around the view frustrum
		core::aabbox3d<f32> boundingBox;

		//! transforms the furstum by the matrix
		//! \param Matrix by which the view frustrum is transformed.
		void transform(const core::matrix4 &mat);

		//! returns the point which is on the far left upper corner inside the
		//! the view frustrum.
		core::vector3df getFarLeftUp() const;

		//! returns the point which is on the far left bottom corner inside the
		//! the view frustrum.
		core::vector3df getFarLeftDown() const;

		//! returns the point which is on the far right top corner inside the
		//! the view frustrum.
		core::vector3df getFarRightUp() const;

		//! returns the point which is on the far right bottom corner inside the
		//! the view frustrum.
		core::vector3df getFarRightDown() const;

		//! returns a bounding box enclosing the whole view frustrum
		core::aabbox3d<f32> getBoundingBox() const;

		//! recalculates the bounding box member based on the planes
		void recalculateBoundingBox();
	};



} // end namespace scene
} // end namespace engine

#endif


