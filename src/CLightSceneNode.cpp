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
 
#include "CLightSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "os.h"

namespace engine
{
namespace scene
{

//! constructor
CLightSceneNode::CLightSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
                                 const core::vector3df& position, video::SColorf color,f32 radius)
	: ILightSceneNode(parent, mgr, id, position)
{
	#ifdef _DEBUG
	setDebugName("CLightSceneNode");
	#endif

	AutomaticCullingEnabled = false;
	LightData.Radius = radius;
	LightData.DiffuseColor = color;
	LightData.Position = position;

	// set some useful specular color
	LightData.SpecularColor = color.getInterpolated(video::SColor(255,255,255,255),0.5f);
}

CLightSceneNode::~CLightSceneNode()
{
}


#include <pspdebug.h>
#define printf pspDebugScreenPrintf
//! pre render event
void CLightSceneNode::OnPreRender()
{
	int d= os::Timer::getRealTime();



//	printf("light scene\n");
	if (IsVisible)
	{
		SceneManager->registerNodeForRendering(this, ESNRP_LIGHT_AND_CAMERA);
		ISceneNode::OnPreRender();
	}
//	printf("light %d\n", os::Timer::getRealTime() -d);
}


//! render
void CLightSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	if (!driver)
		return;

	switch ( LightData.Type )
	{
	case video::ELT_POINT:
		LightData.Position = getAbsolutePosition();
		break;

	case video::ELT_DIRECTIONAL:
		// misuse Position as direction..
		LightData.Position = getAbsolutePosition();
		LightData.Position.invert ();
		if ( LightData.Position.getLengthSQ() == 0.0 )
		{
			LightData.Position.set ( 0.f, -1.f, 0.f );
			os::Printer::log ( "Invalid Directionial Light Direction" );
		}
		else
		{
			LightData.Position.normalize();
		}
		break;
	}
#if 0
	if (0)
	{
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		video::SMaterial m;
		m.Lighting = false;
		driver->setMaterial(m);
//		driver->draw3DBox(BBox, LightData.DiffuseColor.toSColor());

		core::aabbox3df radius = BBox;
		switch ( LightData.Type )
		{
		case video::ELT_POINT:
			radius.MaxEdge *= LightData.Radius;
			radius.MinEdge *= LightData.Radius;
			driver->draw3DBox(radius, LightData.DiffuseColor.toSColor());
			break;

		case video::ELT_DIRECTIONAL:
			driver->draw3DLine (    core::vector3df ( 0.f, 0.f, 0.f ),
			                        core::vector3df ( 0.f, 0.f, 0.f ) + (LightData.Position * 10.f ),
			                        LightData.DiffuseColor.toSColor()
			                        );
			break;
		}

	}
#endif

	driver->addDynamicLight(LightData);
}


//! returns the light data
video::SLight& CLightSceneNode::getLightData()
{
	return LightData;
}

//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CLightSceneNode::getBoundingBox() const
{
	return BBox;
}

} // end namespace scene
} // end namespace engine
