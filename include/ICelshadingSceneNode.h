/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __I_CEL_SHADING_SCENE_NODE_H_INCLUDED__
#define __I_CEL_SHADING_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"
#include "IMesh.h"

namespace engine
{
namespace scene
{

	//! Scene node for rendering a cel-shading effect
	class ICelshadingSceneNode : public ISceneNode
	{
	public:

		//! constructor
		ICelshadingSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
			: ISceneNode(parent, mgr, id) {}

		//! sets the mesh from which cel shading effect should be generated.
		virtual void setMeshToRenderFrom(IMesh* mesh) = 0;
	};

} // end namespace scene
} // end namespace engine

#endif


