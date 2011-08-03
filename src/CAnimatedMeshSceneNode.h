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

#ifndef __C_ANIMATED_MESH_SCENE_NODE_H_INCLUDED__
#define __C_ANIMATED_MESH_SCENE_NODE_H_INCLUDED__

#include "IAnimatedMeshSceneNode.h"
#include "IAnimatedMesh.h"
#include "CPlanarShadow.h"

namespace engine
{
namespace scene
{
class IDummyTransformationSceneNode;

class CAnimatedMeshSceneNode : public IAnimatedMeshSceneNode
{
public:

//! constructor
CAnimatedMeshSceneNode(IAnimatedMesh* mesh, ISceneNode* parent, ISceneManager* mgr,     s32 id,
                       const core::vector3df& position = core::vector3df(0,0,0),
                       const core::vector3df& rotation = core::vector3df(0,0,0),
                       const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

//! destructor
virtual ~CAnimatedMeshSceneNode();

//! sets the current frame. from now on the animation is played from this frame.
virtual void setCurrentFrame(s32 frame);

//! OnPostRender() is called just after rendering the whole scene.
virtual void OnPostRender(u32 timeMs);

//! frame
virtual void OnPreRender();

//! renders the node.
virtual void render();

//! returns the axis aligned bounding box of this node
virtual const core::aabbox3d<f32>& getBoundingBox() const;

//! sets the frames between the animation is looped.
//! the default is 0 - MaximalFrameCount of the mesh.
virtual bool setFrameLoop(s32 begin, s32 end);

//! Sets looping mode which is on by default. If set to false,
//! animations will not be looped.
virtual void setLoopMode(bool playAnimationLooped);

//! Sets a callback interface which will be called if an animation
//! playback has ended. Set this to 0 to disable the callback again.
virtual void setAnimationEndCallback(IAnimationEndCallBack* callback=0);
virtual ICelshadingSceneNode* addCelshadingSceneNode(float *shader = 0, int width = 32, bool recalc_normals = false);
virtual void removeCelshadingSceneNode();

//! sets the speed with witch the animation is played
virtual void setAnimationSpeed(s32 framesPerSecond);

//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hirachy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
virtual video::SMaterial& getMaterial(s32 i);

//! returns amount of materials used by this scene node.
virtual s32 getMaterialCount();

//! Creates shadow volume scene node as child of this node
//! and returns a pointer to it.
virtual IShadowVolumeSceneNode* addShadowVolumeSceneNode(s32 id=-1,
                                                         f32 infinity=100.0f, bool removeHardVertices = true, ESV_SHADOWTYPE shadowType = ESV_AUTOMATIC, scene::IMesh *customShadow  = 0);


//! Returns a pointer to a child node, wich has the same transformation as
//! the corrsesponding joint, if the mesh in this scene node is a ms3d mesh.
virtual ISceneNode* getMS3DJointNode(const c8* jointName);

//! Returns a pointer to a child node, wich has the same transformation as
//! the corrsesponding joint, if the mesh in this scene node is a x mesh.
virtual ISceneNode* getXJointNode(const c8* jointName);

//! Removes a child from this scene node.
//! Implemented here, to be able to remove the shadow properly, if there is one,
//! or to remove attached childs.
virtual bool removeChild(ISceneNode* child);


//! Starts a MD2 animation.
virtual bool setMD2Animation(EMD2_ANIMATION_TYPE anim);

//! Starts a special MD2 animation.
virtual bool setMD2Animation(const c8* animationName);

//! Returns the current displayed frame number.
virtual s32 getFrameNr();


virtual const core::array<core::matrix4>* makeCustomBones();
virtual const core::array<core::matrix4>* getCustomBones();
virtual const void useCustomBones(bool use);

//! Returns type of the scene node
virtual ESCENE_NODE_TYPE getType() {
	return ESNT_ANIMATED_MESH;
}



//! Set planar shadow
//! \param plane: 3d plane onto project the shadow
//! \param animated: set false if the mesh is static to have greater performance
//! \param depthTest: Enable or disable depth test
virtual void addPlanarShadow(core::plane3df plane, bool animated = true, bool depthTest = false, float Divisor = 100.f) ;

//! Remove planar shadow
virtual void removePlanarShadow();

private:

core::array<video::SMaterial> Materials;
core::aabbox3d<f32> Box;
IAnimatedMesh* Mesh;
s32 BeginFrameTime;
s32 StartFrame, EndFrame;

bool useCBones;
core::array<core::matrix4> CBones;
CPlanarShadow* PlanarShadow;
s32 FramesPerSecond;
bool Looping;
IAnimationEndCallBack* LoopCallBack;
s32 PassCount;
int framenr;
IShadowVolumeSceneNode* Shadow;
ICelshadingSceneNode *celshading;
IMesh *shadowCustomMesh;
ESV_SHADOWTYPE shadType;

MD2Buffer indexOfMD2;

core::array<IDummyTransformationSceneNode* > JointChildSceneNodes;
};

} // end namespace scene
} // end namespace engine

#endif

