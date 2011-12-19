/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __engine_I_VIDEO_MODE_LIST_H_INCLUDED__
#define __engine_I_VIDEO_MODE_LIST_H_INCLUDED__

#include "IUnknown.h"
#include "dimension2d.h"

namespace engine
{
namespace video
{

	//! A list of all available video modes.
	/** You can get a list via engineDevice::getVideoModeList(). If you are confused 
	now, because you think you have to create an engine Device with a video
	mode before being able to get the video mode list, let me tell you that
	there is no need to start up an engine Device with EDT_DIRECT3D8, EDT_OPENGL or
	DT_SOFTWARE: For this (and for lots of other reasons) the null device,
	DT_NULL exists.*/
	class IVideoModeList : public IUnknown
	{
	public:

		//! destructor
		virtual ~IVideoModeList() {}

		//! Gets amount of video modes in the list.
		//! \return Returns amount of video modes.
		virtual s32 getVideoModeCount() const = 0;

		//! Returns the screen size of a video mode in pixels.
		//! \param modeNumber: zero based index of the video mode.
		//! \return Returns size of screen in pixels of the specified video mode.
		virtual core::dimension2d<s32> getVideoModeResolution(s32 modeNumber) const = 0;

		//! Returns the pixel depth of a video mode in bits.
		//! \param modeNumber: zero based index of the video mode.
		//! \return Returns the size of each pixel of the specified video mode in bits.
		virtual s32 getVideoModeDepth(s32 modeNumber) const = 0;

		//! Returns current desktop screen resolution.
		//! \return Returns size of screen in pixels of the current desktop video mode.
		virtual core::dimension2d<s32> getDesktopResolution() const = 0;

		//! Returns the pixel depth of a video mode in bits.
		//! \return Returns the size of each pixel of the current desktop video mode in bits.
		virtual s32 getDesktopDepth() const = 0;
	};

} // end namespace video
} // end namespace engine


#endif


