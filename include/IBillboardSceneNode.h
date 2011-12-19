/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __I_BILLBOARD_SCENE_NODE_H_INCLUDED__
#define __I_BILLBOARD_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"

namespace engine
{
namespace scene
{

//! A billboard scene node.
/** A billboard is like a 3d sprite: A 2d element,
which always looks to the camera. It is usually used for explosions, fire,
lensflares, particles and things like that.
*/
class IBillboardSceneNode : public ISceneNode
{
public:

	//! constructor
	IBillboardSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id, 
		const core::vector3df& position = core::vector3df(0,0,0))
		: ISceneNode(parent, mgr, id, position) {}

	//! Sets the size of the billboard.
	virtual void setSize(const core::dimension2d<f32>& size) = 0;

	//! Returns the size of the billboard.
	virtual const core::dimension2d<f32>& getSize() = 0;
};

} // end namespace scene
} // end namespace engine


#endif


