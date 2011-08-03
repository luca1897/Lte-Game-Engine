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
 
#ifndef __C_TEXT_SCENE_NODE_H_INCLUDED__
#define __C_TEXT_SCENE_NODE_H_INCLUDED__

#include "ITextSceneNode.h"
#include "IGUIFont.h"
#include "ISceneCollisionManager.h"

namespace engine
{
namespace scene
{

class CTextSceneNode : public ITextSceneNode
{
public:

//! constructor
CTextSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
               gui::IGUIFont* font, scene::ISceneCollisionManager* coll,
               const core::vector3df& position = core::vector3df(0,0,0), const wchar_t* text=0,
               video::SColor color=video::SColor(100,0,0,0));

//! destructor
virtual ~CTextSceneNode();

virtual void OnPreRender();

//! renders the node.
virtual void render();

//! returns the axis aligned bounding box of this node
virtual const core::aabbox3d<f32>& getBoundingBox() const;

//! returns amount of materials used by this scene node.
virtual s32 getMaterialCount();

//! sets the text string
virtual void setText(const wchar_t* text);

//! sets the color of the text
virtual void setTextColor(video::SColor color);

//! Returns type of the scene node
virtual ESCENE_NODE_TYPE getType() {
	return ESNT_TEXT;
}

private:

core::aabbox3d<f32> Box;
core::stringw Text;
video::SColor Color;
gui::IGUIFont* Font;
scene::ISceneCollisionManager* Coll;
};

} // end namespace scene
} // end namespace engine

#endif

