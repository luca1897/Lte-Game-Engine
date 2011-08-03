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
 
#include "CSceneNodeAnimatorTexture.h"

namespace engine
{
namespace scene
{


//! constructor
CSceneNodeAnimatorTexture::CSceneNodeAnimatorTexture(const core::array<video::ITexture*>& textures,
                                                     s32 timePerFrame, bool loop, u32 now)
	: Loop(loop), StartTime(now), TimePerFrame(timePerFrame)
{
	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorTexture");
	#endif

	for (u32 i=0; i<textures.size(); ++i)
	{
		if (textures[i])
			textures[i]->grab();

		Textures.push_back(textures[i]);
	}

	EndTime = now + (timePerFrame * Textures.size());
}



//! destructor
CSceneNodeAnimatorTexture::~CSceneNodeAnimatorTexture()
{
	for (u32 i=0; i<Textures.size(); ++i)
		if (Textures[i])
			Textures[i]->drop();
}



//! animates a scene node
void CSceneNodeAnimatorTexture::animateNode(ISceneNode* node, u32 timeMs)
{
	u32 t = (timeMs-StartTime);

	s32 idx = 0;

	if (!Loop && timeMs >= EndTime)
		idx = Textures.size() - 1;
	else
		idx = (t/TimePerFrame) % Textures.size();

	if (idx < (s32)Textures.size())
		node->setMaterialTexture(0, Textures[idx]);
}



} // end namespace scene
} // end namespace engine
