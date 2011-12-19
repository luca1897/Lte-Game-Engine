/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/
#ifndef IGUIProgressBar_H
#define IGUIProgressBar_H
#include "IGUIElement.h"
#include "engineTypes.h"
#include "SColor.h"
#include "IGUIFont.h"

namespace engine
{

namespace gui
{

//! This is a gui progressbar
class IGUIProgressBar : public IGUIElement
{
      public:
      
      //! Constructor
      IGUIProgressBar(bool horizontal, bool border, bool background, IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
      : IGUIElement(EGUIET_ELEMENT, environment, parent, id, rectangle){}
      
      //! Set max value
      virtual void setMax(s32 m) = 0;
      
      //! Set current value
      virtual void setPos(s32 p) = 0;
      
      //! Set back color
      virtual void setColorBack(video::SColor color) = 0;
      	
      //! Set front color
      virtual void setColorFront(video::SColor color) = 0;
      	
      //! Set text color
      virtual void setColorText(video::SColor color) = 0;
      	
      //! Sets gui font
      virtual void setFontText(IGUIFont* font) = 0;
};

}
}
#endif
