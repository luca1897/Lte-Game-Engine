/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __I_META_TRIANGLE_SELECTOR_H_INCLUDED__
#define __I_META_TRIANGLE_SELECTOR_H_INCLUDED__

#include "ITriangleSelector.h"

namespace engine
{
namespace scene
{

//! Interface for making multiple triangle selectors work as one big selector.
/** This is nothing more than a collection of one or more triangle selectors
providing together the interface of one triangle selector. In this way, 
collision tests can be done with different triangle soups in one pass.
*/
class IMetaTriangleSelector : public ITriangleSelector
{
public:

	//! Adds a triangle selector to the collection of triangle selectors
	//! in this metaTriangleSelector.
	//! \param toAdd: Pointer to an triangle selector to add to the list.
	virtual void addTriangleSelector(ITriangleSelector* toAdd) = 0; 

	//! Removes a specific triangle selector which was added before	from the collection.
	//! \param toRemove: Pointer to an triangle selector which is in the list
	//! but will be removed.
	//! \return Returns true if successful, false if not.
	virtual bool removeTriangleSelector(ITriangleSelector* toRemove) = 0;

	//! Removes all triangle selectors from the collection.
	virtual void removeAllTriangleSelectors() = 0;
};

} // end namespace scene
} // end namespace engine


#endif


