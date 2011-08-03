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
 
#include "CVideoModeList.h"

namespace engine
{
namespace video
{

//! constructor
CVideoModeList::CVideoModeList()
{
	Desktop.depth = 0;
	Desktop.size = core::dimension2d<s32>(0,0);
}


//! destructor
CVideoModeList::~CVideoModeList()
{
}



void CVideoModeList::setDesktop(s32 desktopDepth, const core::dimension2d<s32>& desktopSize)
{
	Desktop.depth = desktopDepth;
	Desktop.size = desktopSize;
}


//! Gets amount of video modes in the list.
s32 CVideoModeList::getVideoModeCount() const
{
	return (s32)VideoModes.size();
}



//! Returns the screen size of a video mode in pixels.
core::dimension2d<s32> CVideoModeList::getVideoModeResolution(s32 modeNumber) const
{
	if (modeNumber < 0 || modeNumber > (s32)VideoModes.size())
		return core::dimension2d<s32>(0,0);

	return VideoModes[modeNumber].size;
}



//! Returns the pixel depth of a video mode in bits.
s32 CVideoModeList::getVideoModeDepth(s32 modeNumber) const
{
	if (modeNumber < 0 || modeNumber > (s32)VideoModes.size())
		return 0;

	return VideoModes[modeNumber].depth;
}


//! Returns current desktop screen resolution.
core::dimension2d<s32> CVideoModeList::getDesktopResolution() const
{
	return Desktop.size;
}


//! Returns the pixel depth of a video mode in bits.
s32 CVideoModeList::getDesktopDepth() const
{
	return Desktop.depth;
}



//! adds a new mode to the list
void CVideoModeList::addMode(const core::dimension2d<s32>& size, s32 depth)
{
	SVideoMode m;
	m.depth = depth;
	m.size = size;

	for (s32 i=0; i<(s32)VideoModes.size(); ++i)
	{
		if (VideoModes[i] == m)
			return;
	}

	VideoModes.push_back(m);
	VideoModes.sort(); // TODO: could be replaced by inserting into right place
}


} // end namespace video
} // end namespace engine

