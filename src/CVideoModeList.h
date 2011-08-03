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
 
#ifndef __engine_C_VIDEO_MODE_LIST_H_INCLUDED__
#define __engine_C_VIDEO_MODE_LIST_H_INCLUDED__

#include "IVideoModeList.h"
#include "dimension2d.h"
#include "engineArray.h"

namespace engine
{
namespace video
{

class CVideoModeList : public IVideoModeList
{
public:

//! constructor
CVideoModeList();

//! destructor
virtual ~CVideoModeList();

//! Gets amount of video modes in the list.
virtual s32 getVideoModeCount() const;

//! Returns the screen size of a video mode in pixels.
virtual core::dimension2d<s32> getVideoModeResolution(s32 modeNumber) const;

//! Returns the pixel depth of a video mode in bits.
virtual s32 getVideoModeDepth(s32 modeNumber) const;

//! Returns current desktop screen resolution.
virtual core::dimension2d<s32> getDesktopResolution() const;

//! Returns the pixel depth of a video mode in bits.
virtual s32 getDesktopDepth() const;

//! adds a new mode to the list
void addMode(const core::dimension2d<s32>& size, s32 depth);

void setDesktop(s32 desktopDepth, const core::dimension2d<s32>& desktopSize);

private:

struct SVideoMode
{
	core::dimension2d<s32> size;
	s32 depth;

	bool operator==(const SVideoMode& other) const
	{
		return size == other.size && depth == other.depth;
	}

	bool operator <(const SVideoMode& other) const
	{
		return (size.Width < other.size.Width ||
		        (size.Width == other.size.Width &&
		         size.Height < other.size.Height) ||
		        (size.Width == other.size.Width &&
		         size.Height == other.size.Height &&
		         depth < other.depth));
	}
};

core::array<SVideoMode> VideoModes;
SVideoMode Desktop;
};

} // end namespace video
} // end namespace engine


#endif

