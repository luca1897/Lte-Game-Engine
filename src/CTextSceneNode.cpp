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
 
#include "CTextSceneNode.h"
#include "ISceneManager.h"


namespace engine
{
namespace scene
{

//! constructor
CTextSceneNode::CTextSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
                               gui::IGUIFont* font, scene::ISceneCollisionManager* coll,
                               const core::vector3df& position, const wchar_t* text,
                               video::SColor color)
	: ITextSceneNode(parent, mgr, id, position), Font(font), Coll(coll), Color(color)
{
	#ifdef _DEBUG
	setDebugName("CTextSceneNode");
	#endif

	Text = text;
	setAutomaticCulling(false);

	if (Font)
		Font->grab();
}

//! destructor
CTextSceneNode::~CTextSceneNode()
{
	if (Font)
		Font->drop();
}
#include <pspdebug.h>
#define printf pspDebugScreenPrintf

void CTextSceneNode::OnPreRender()
{
//	printf("text scene\n");
	if (IsVisible)
		SceneManager->registerNodeForRendering(this, ESNRP_SHADOW);

	ISceneNode::OnPreRender();
}

//! renders the node.
void CTextSceneNode::render()
{
	if (!Font || !Coll)
		return;

	core::position2d<s32> pos = Coll->getScreenCoordinatesFrom3DPosition(getAbsolutePosition(),
	                                                                     SceneManager->getActiveCamera());

	core::rect<s32> r(pos, core::dimension2d<s32>(1,1));
	Font->draw(Text.c_str(), r, Color, true, true);
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CTextSceneNode::getBoundingBox() const
{
	return Box;
}

//! returns amount of materials used by this scene node.
s32 CTextSceneNode::getMaterialCount()
{
	return 0;
}

//! sets the text string
void CTextSceneNode::setText(const wchar_t* text)
{
	Text = text;
}

//! sets the color of the text
void CTextSceneNode::setTextColor(video::SColor color)
{
	Color = color;
}


} // end namespace scene
} // end namespace engine

