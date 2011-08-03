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
 
#include "CAnimatedMeshSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "S3DVertex.h"
#include "os.h"
#include "CShadowVolumeSceneNode.h"
#include "SViewFrustrum.h"
#include "ICameraSceneNode.h"
#include "IAnimatedMeshMS3D.h"
#include "IAnimatedMeshX.h"
#include "IDummyTransformationSceneNode.h"
#include "CCelshadingSceneNode.h"
#include "IMaterialRenderer.h"
#include <pspdebug.h>
#include <stdio.h>
#define printf pspDebugScreenPrintf
extern engine::scene::IMesh *__llodshadow_sphere;
extern engine::scene::IMesh *__llodshadow_cube;
namespace engine
{
namespace scene
{



//! constructor
CAnimatedMeshSceneNode::CAnimatedMeshSceneNode(IAnimatedMesh* mesh, ISceneNode* parent, ISceneManager* mgr, s32 id,
                                               const core::vector3df& position, const core::vector3df& rotation,       const core::vector3df& scale)
	: IAnimatedMeshSceneNode(parent, mgr, id, position, rotation, scale), Mesh(mesh),
	BeginFrameTime(0), StartFrame(0), EndFrame(0), FramesPerSecond(100), celshading(0),
	Shadow(0), Looping(true), LoopCallBack(0), shadowCustomMesh(0), shadType(ESV_AUTOMATIC), indexOfMD2(0),
	PlanarShadow(0)
{
	#ifdef _DEBUG
	setDebugName("CAnimatedMeshSceneNode");
	#endif
	framenr=0;

	BeginFrameTime = os::Timer::getTime();

	if (Mesh)
	{
		// get materials.
		Box = Mesh->getBoundingBox();

		IMesh* m = Mesh->getMesh(0,0);

		if (m)
		{
			video::SMaterial mat;
			for (s32 i=0; i<m->getMeshBufferCount(); ++i)
			{
				IMeshBuffer* mb = m->getMeshBuffer(i);
				if (mb)
					mat = mb->getMaterial();

				Materials.push_back(mat);
			}
		}

		// get start and begin time

		StartFrame = 0;
		EndFrame = Mesh->getFrameCount();

		// grab the mesh
		useCBones = false;
		Mesh->grab();

	}
}


void CAnimatedMeshSceneNode::addPlanarShadow(core::plane3df plane, bool animated, bool depthTest, float Divisor)
{

	if (PlanarShadow == 0)
		PlanarShadow = new CPlanarShadow(this, SceneManager, Mesh->getMesh(0,0), Divisor);


	PlanarShadow->setPlane(plane);
	PlanarShadow->setAnimated(animated);
	PlanarShadow->setDepth(depthTest);


}

void CAnimatedMeshSceneNode::removePlanarShadow()
{

	if (PlanarShadow)
		PlanarShadow->drop();

	PlanarShadow = 0;
}


//! destructor
CAnimatedMeshSceneNode::~CAnimatedMeshSceneNode()

{
	if (Mesh)
		Mesh->drop();

	if (Shadow)
		Shadow->drop();

	if (PlanarShadow)
		PlanarShadow->drop();

	if (shadType == ESV_CUSTOM || shadType == ESV_PREBUILT)
		shadowCustomMesh->drop();


	for (s32 i=0; i<(s32)JointChildSceneNodes.size(); ++i)
		if (JointChildSceneNodes[i])
			JointChildSceneNodes[i]->drop();

	if (LoopCallBack)
		LoopCallBack->drop();

	if ( (indexOfMD2 != 0) )
		if (((IAnimatedMeshMD2*)Mesh)->isInterpolationBuffer(indexOfMD2))
			((IAnimatedMeshMD2*)Mesh)->dropInterpolationBuffer(indexOfMD2);

}



//! sets the current frame. from now on the animation is played from this frame.
void CAnimatedMeshSceneNode::setCurrentFrame(s32 frame)
{
}

#include <pspdebug.h>
#define printf pspDebugScreenPrintf
const void CAnimatedMeshSceneNode::useCustomBones(bool use){
	useCBones = use;
}
const core::array<core::matrix4>* CAnimatedMeshSceneNode::getCustomBones(){
	return &CBones;
}
const core::array<core::matrix4>* CAnimatedMeshSceneNode::makeCustomBones(){
	CBones.clear();
	if(Mesh) if (Mesh->getMeshType() == EAMT_X) ((IAnimatedMeshX*)Mesh)->getBones(&CBones);
	return &CBones;
}
//! frame
void CAnimatedMeshSceneNode::OnPreRender()
{


	int d;
	if (IsVisible)
	{
		// because this node supports rendering of mixed mode meshes consisting of
		// transparent and solid material at the same time, we need to go through all
		// materials, check of what type they are and register this node for the right
		// render pass according to that.

		video::IVideoDriver* driver = SceneManager->getVideoDriver();

		PassCount = 0;
		int transparentCount = 0;
		int solidCount = 0;
//d= os::Timer::getRealTime();
		// count transparent and solid materials in this scene node
		for (u32 i=0; i<Materials.size(); ++i)
		{
			video::IMaterialRenderer* rnd =
			        driver->getMaterialRenderer(Materials[i].MaterialType);

			if (rnd && rnd->isTransparent())
				++transparentCount;
			else
				++solidCount;

			if (solidCount && transparentCount)
				break;
		}
//printf("pr-1 %d\n", os::Timer::getRealTime() -d);
		// register according to material types counted
//d= os::Timer::getRealTime();
		if (solidCount)
		{

			if (PlanarShadow == 0)
				SceneManager->registerNodeForRendering(this, scene::ESNRP_SOLID);
			else
				SceneManager->registerNodeForRendering(this, scene::ESNRP_PLANARSHADOW);

		}
		if (transparentCount)
			SceneManager->registerNodeForRendering(this, scene::ESNRP_TRANSPARENT);




	}

	if ( Mesh->getMeshType() == EAMT_MD2)
		if (getMaterial(0).Flags[video::EMF_DMA] == true)
		{

			if (indexOfMD2 != 0) {

				if ( ((IAnimatedMeshMD2*)Mesh)->isInterpolationBuffer(indexOfMD2) == false )
				{
					indexOfMD2 = ((IAnimatedMeshMD2*)Mesh)->createInterpolationBuffer();
					((IAnimatedMeshMD2*)Mesh)->setInterpolationBuffer(indexOfMD2);

				}
				else
				{
					((IAnimatedMeshMD2*)Mesh)->setInterpolationBuffer(indexOfMD2);
				}

			}
			else {

				indexOfMD2 = ((IAnimatedMeshMD2*)Mesh)->createInterpolationBuffer();
				((IAnimatedMeshMD2*)Mesh)->setInterpolationBuffer(indexOfMD2);
			}
		}


	ISceneNode::OnPreRender();


	int tempo;

	if (IsVisible)
	{
		for (s32 i=0; i<(s32)JointChildSceneNodes.size(); ++i)
			if (JointChildSceneNodes[i])
				JointChildSceneNodes[i]->OnPreRender();

		framenr = getFrameNr();
		scene::IMesh* m  = 0;




		if (Shadow && Mesh) {
			m = Mesh->getMesh(framenr, 255, StartFrame, EndFrame);
			if (shadType == ESV_AUTOMATIC)
				Shadow->setMeshToRenderFrom(m);

			else if (shadowCustomMesh)
			{

				if (shadType != ESV_PREBUILT && shadType != ESV_CUSTOM)
				{
					core::matrix4 mat;
					core::aabbox3d<f32> box = m->getBoundingBox();

					mat.setTranslation(box.getCenter());

					core::vector3df ext = box.getExtent();

					if (ext.X < 0) ext.X *= -1;
					if (ext.Y < 0) ext.Y *= -1;
					if (ext.Z < 0) ext.Z *= -1;

					mat.setScale(ext);
					Shadow->setCustomModelMatrix(mat);
				}

				if (shadType == ESV_PREBUILT)
				{

					core::matrix4 mat;
					core::aabbox3d<f32> box = m->getBoundingBox();

					mat.setTranslation(box.getCenter());
					Shadow->setCustomModelMatrix(mat);

				}
				if (shadowCustomMesh)
				{

					Shadow->setMeshToRenderFrom(shadowCustomMesh);
				}
			}
		}
	}



}


inline s32 CAnimatedMeshSceneNode::getFrameNr()
{
	s32 frame = 0;


	/* QUESTO CODICE NON C'ERA PRIMA: I FRAME VENIVANO CALCOLATI IN BASE AL TEMPO PASSATO DALL'ULTIMA SCENA */
	//if (framenr > EndFrame) { framenr=StartFrame-1; }
	//framenr++;
	//return framenr;
	/* ----------------- FINO QUI -------------------- */


	s32 len = EndFrame - StartFrame;

	if (!len)
		return StartFrame;

	if (Looping)
	{
		// play animation looped
		frame = StartFrame + ((s32)((os::Timer::getTime() - BeginFrameTime)
		                            * (FramesPerSecond/1000.0f)) % len);
	}
	else
	{
		// play animation non looped
		frame = StartFrame + ((s32)((os::Timer::getTime() - BeginFrameTime)
		                            * (FramesPerSecond/1000.0f)));

		if (frame > EndFrame)
		{
			frame = EndFrame;
			StartFrame = EndFrame;
			if (LoopCallBack)
				LoopCallBack->OnAnimationEnd(this);
		}
	}

	return frame;
}



//! OnPostRender() is called just after rendering the whole scene.
void CAnimatedMeshSceneNode::OnPostRender(u32 timeMs)
{
	s32 frameNr = getFrameNr();

	if (IsVisible)
	{
		// animate this node with all animators

		core::list<ISceneNodeAnimator*>::Iterator ait = Animators.begin();
		for (; ait != Animators.end(); ++ait)
			(*ait)->animateNode(this, timeMs);

		// update absolute position
		updateAbsolutePosition();

		// update all dummy transformation nodes
		if (!JointChildSceneNodes.empty() && Mesh &&
		    (Mesh->getMeshType() == EAMT_MS3D || Mesh->getMeshType() == EAMT_X))
		{
			IAnimatedMeshMS3D* amm = (IAnimatedMeshMS3D*)Mesh;
			core::matrix4* mat;

			for (s32 i=0; i<(s32)JointChildSceneNodes.size(); ++i)
				if (JointChildSceneNodes[i])
				{
					mat = amm->getMatrixOfJoint(i, frameNr);
					if (mat)
						JointChildSceneNodes[i]->getRelativeTransformationMatrix() = *mat;
				}
		}

		core::list<ISceneNode*>::Iterator it = Children.begin();
		for (; it != Children.end(); ++it)
			(*it)->OnPostRender(timeMs);
	}
}


//! renders the node.
void CAnimatedMeshSceneNode::render()
{

	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	if (!Mesh || !driver)
		return;


	if(Mesh->getMeshType() == EAMT_X) ((IAnimatedMeshX*)Mesh)->useCMeshBones(&CBones, useCBones);
	bool isTransparentPass =
	        SceneManager->getSceneNodeRenderPass() == scene::ESNRP_TRANSPARENT;

	++PassCount;


	if ( Mesh->getMeshType() == EAMT_MD2)
		if (getMaterial(0).Flags[video::EMF_DMA] == true)
		{
			if (indexOfMD2 != 0)
			{
				((IAnimatedMeshMD2*)Mesh)->setInterpolationBuffer(indexOfMD2);
			}
		}

	scene::IMesh* m = Mesh->getMesh(framenr, 255, StartFrame, EndFrame);




	if (m)
	{


		if (PlanarShadow)
		{
			PlanarShadow->setMeshToRenderFrom(m);
			PlanarShadow->setTransform(AbsoluteTransformation);
			PlanarShadow->render();
		}

		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

		Box = m->getBoundingBox();



		video::IVideoDriver *driver = SceneManager->getVideoDriver();

		if (celshading && PassCount==1)
			celshading->setMeshToRenderFrom(m);

		//printf("getMeshBufferCount() :: %d\n", m->getMeshBufferCount());

		for (s32 i=0; i<m->getMeshBufferCount(); ++i)
		{
			video::IMaterialRenderer* rnd = driver->getMaterialRenderer(Materials[i].MaterialType);
			bool transparent = (rnd && rnd->isTransparent());

			// only render transparent buffer if this is the transparent render pass
			// and solid only in solid pass
			if (transparent == isTransparentPass)
			{

				scene::IMeshBuffer* mb = m->getMeshBuffer(i);

				driver->setMaterial(Materials[i]);

				driver->drawMeshBuffer(mb);


			}
		}

	}

}



//! sets the frames between the animation is looped.
//! the default is 0 - MaximalFrameCount of the mesh.
bool CAnimatedMeshSceneNode::setFrameLoop(s32 begin, s32 end)
{
	if (!Mesh)
		return false;

	s32 frameCount = Mesh->getFrameCount();

	if (!(begin <= end && begin < frameCount && end < frameCount))
		return false;

	StartFrame = begin;
	EndFrame = end;
	BeginFrameTime = os::Timer::getTime();

	return true;
}



//! sets the speed with witch the animation is played
void CAnimatedMeshSceneNode::setAnimationSpeed(s32 framesPerSecond)
{
	FramesPerSecond = framesPerSecond;
}




//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CAnimatedMeshSceneNode::getBoundingBox() const
{
	return Box; //Mesh ? Mesh->getBoundingBox() : Box;
}


//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hirachy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
video::SMaterial& CAnimatedMeshSceneNode::getMaterial(s32 i)
{
	if (i < 0 || i >= (s32)Materials.size())
		return ISceneNode::getMaterial(i);

	return Materials[i];
}



//! returns amount of materials used by this scene node.
s32 CAnimatedMeshSceneNode::getMaterialCount()
{
	return Materials.size();
}



//! Creates shadow volume scene node as child of this node
//! and returns a pointer to it.
IShadowVolumeSceneNode* CAnimatedMeshSceneNode::addShadowVolumeSceneNode(s32 id,
                                                                         f32 infinity, bool removeHardVertices, ESV_SHADOWTYPE shadowType, scene::IMesh *customShadow )
{
	if (!SceneManager->getVideoDriver()->queryFeature(video::EVDF_STENCIL_BUFFER))
		return 0;

	if (Shadow)
	{
		os::Printer::log("This node already has a shadow.", ELL_WARNING);
		return 0;
	}

	if (shadowType == ESV_CUSTOM && customShadow == 0)
		return 0;

	shadType = shadowType;
	shadowCustomMesh = customShadow;



	if (shadType == ESV_SPHERE || shadType == ESV_CUBE)
		removeHardVertices  =  false;

	if (shadType == ESV_SPHERE)
		shadowCustomMesh = __llodshadow_sphere;

	if (shadType == ESV_CUBE)
		shadowCustomMesh = __llodshadow_cube;

	if (shadType == ESV_CUSTOM || shadType == ESV_PREBUILT)
		shadowCustomMesh->grab();


	Shadow = new CShadowVolumeSceneNode(this, SceneManager, -1,  true, infinity, removeHardVertices, shadType == ESV_PREBUILT, shadowCustomMesh);
	return Shadow;
}


ICelshadingSceneNode* CAnimatedMeshSceneNode::addCelshadingSceneNode(float *shader, int width, bool recalc_normals )
{
	if (celshading)
	{
		os::Printer::log("This node already has a celshading scene node.", ELL_WARNING);
		return 0;
	}

	celshading = new CCelshadingSceneNode(this, SceneManager, -1, shader, width, recalc_normals);
	return celshading;
}

void CAnimatedMeshSceneNode::removeCelshadingSceneNode()
{
	if (!celshading)
	{
		os::Printer::log("This node hasn't a celshading scene node.", ELL_WARNING);
		return;
	}

	delete celshading;
	celshading = 0;
}



//! Returns a pointer to a child node, wich has the same transformation as
//! the corrsesponding joint, if the mesh in this scene node is a ms3d mesh.
ISceneNode* CAnimatedMeshSceneNode::getMS3DJointNode(const c8* jointName)
{
	if (!Mesh || Mesh->getMeshType() != EAMT_MS3D)
		return 0;

	IAnimatedMeshMS3D* amm = (IAnimatedMeshMS3D*)Mesh;
	s32 jointCount = amm->getJointCount();
	s32 number = amm->getJointNumber(jointName);

	if (number == -1)
	{
		os::Printer::log("Joint with specified name not found in ms3d mesh.", jointName, ELL_WARNING);
		return 0;
	}

	if (JointChildSceneNodes.empty())
	{
		// allocate joints for the first time.
		JointChildSceneNodes.set_used(jointCount);
		for (s32 i=0; i<jointCount; ++i)
			JointChildSceneNodes[i] = 0;
	}

	if (JointChildSceneNodes[number] == 0)
	{
		JointChildSceneNodes[number] =
		        SceneManager->addDummyTransformationSceneNode(this);
		JointChildSceneNodes[number]->grab();
	}

	return JointChildSceneNodes[number];
}


//! Returns a pointer to a child node, wich has the same transformation as
//! the corrsesponding joint, if the mesh in this scene node is a ms3d mesh.
ISceneNode* CAnimatedMeshSceneNode::getXJointNode(const c8* jointName)
{
	if (!Mesh || Mesh->getMeshType() != EAMT_X)
		return 0;

	IAnimatedMeshX* amm = (IAnimatedMeshX*)Mesh;
	s32 jointCount = amm->getJointCount();
	s32 number = amm->getJointNumber(jointName);

	if (number == -1)
	{
		os::Printer::log("Joint with specified name not found in x mesh.", jointName, ELL_WARNING);
		return 0;
	}

	if (JointChildSceneNodes.empty())
	{
		// allocate joints for the first time.
		JointChildSceneNodes.set_used(jointCount);
		for (s32 i=0; i<jointCount; ++i)
			JointChildSceneNodes[i] = 0;
	}

	if (JointChildSceneNodes[number] == 0)
	{
		JointChildSceneNodes[number] =
		        SceneManager->addDummyTransformationSceneNode(this);
		JointChildSceneNodes[number]->grab();
	}

	return JointChildSceneNodes[number];
}



//! Removes a child from this scene node.
//! Implemented here, to be able to remove the shadow properly, if there is one,
//! or to remove attached childs.
bool CAnimatedMeshSceneNode::removeChild(ISceneNode* child)
{
	if (child && Shadow == child)
	{
		Shadow->drop();
		Shadow = 0;

		if (shadType == ESV_CUSTOM || shadType == ESV_PREBUILT)
		{
			shadowCustomMesh->drop();
			shadType = ESV_AUTOMATIC;
		}

		return true;
	}

	if (ISceneNode::removeChild(child))
	{
		for (s32 i=0; i<(s32)JointChildSceneNodes.size(); ++i)
			if (JointChildSceneNodes[i] == child)
			{
				JointChildSceneNodes[i]->drop();
				JointChildSceneNodes[i] = 0;
				return true;
			}

		return true;
	}

	return false;
}


//! Starts a MD2 animation.
bool CAnimatedMeshSceneNode::setMD2Animation(EMD2_ANIMATION_TYPE anim)
{
	if (!Mesh || Mesh->getMeshType() != EAMT_MD2)
		return false;

	IAnimatedMeshMD2* m = (IAnimatedMeshMD2*)Mesh;

	s32 begin, end, speed;
	m->getFrameLoop(anim, begin, end, speed);

	setAnimationSpeed(speed);
	setFrameLoop(begin, end);
	return true;
}


//! Starts a special MD2 animation.
bool CAnimatedMeshSceneNode::setMD2Animation(const c8* animationName)
{
	if (!Mesh || Mesh->getMeshType() != EAMT_MD2)
		return false;

	IAnimatedMeshMD2* m = (IAnimatedMeshMD2*)Mesh;

	s32 begin, end, speed;
	if (!m->getFrameLoop(animationName, begin, end, speed))
		return false;

	setAnimationSpeed(speed);
	setFrameLoop(begin, end);
	return true;
}


//! Sets looping mode which is on by default. If set to false,
//! animations will not be looped.
void CAnimatedMeshSceneNode::setLoopMode(bool playAnimationLooped)
{
	Looping = playAnimationLooped;
}

//! Sets a callback interface which will be called if an animation
//! playback has ended. Set this to 0 to disable the callback again.
void CAnimatedMeshSceneNode::setAnimationEndCallback(IAnimationEndCallBack* callback)
{
	if (LoopCallBack)
		LoopCallBack->drop();

	LoopCallBack = callback;

	if (LoopCallBack)
		LoopCallBack->grab();
}


} // end namespace scene
} // end namespace engine

