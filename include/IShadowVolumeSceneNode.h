/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __I_SHADOW_VOLUME_SCENE_NODE_H_INCLUDED__
#define __I_SHADOW_VOLUME_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"
#include "IMesh.h"

namespace engine
{
namespace scene
{

	enum ESV_SHADOWTYPE {
		
		//! Shadow volume is generated in render-time with the mesh (slow)
		ESV_AUTOMATIC = 0,
		
		//! A Sphere is used to make a fake shadow (fast)
		ESV_SPHERE,
		
		//! A Cube is used to make a fake shadow (fast)
		ESV_CUBE, 
		
		//! A Custom Mesh is used to generate the shadow
		ESV_CUSTOM,
		
		//! A pre-built shadow volume that needs only to be extruded
		ESV_PREBUILT
		
	};

	//! Scene node for rendering a shadow volume into a stencil buffer.
	class IShadowVolumeSceneNode : public ISceneNode
	{
	public:

		//! constructor
		IShadowVolumeSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
			: ISceneNode(parent, mgr, id) {}
		
		//! sets a custom model matrix (that will be multiplicated with the original
		//  model matrix) to transform the shadow volume.
	  virtual void setCustomModelMatrix(core::matrix4& mat);

		//! sets the mesh from which the shadow volume should be generated.
		virtual void setMeshToRenderFrom(IMesh* mesh) = 0;
	};

} // end namespace scene
} // end namespace engine

#endif


