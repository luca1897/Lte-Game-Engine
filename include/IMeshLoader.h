/*

  LTE Game Engine SDK:

   Copyright (C) 2006, SiberianSTAR <haxormail@gmail.com>

  Based on Irrlicht 1.0:
 
   Copyright (C) 2002-2006 Nikolaus Gebhardt

  For conditions of distribution and use, see copyright notice in
  engine.h
 
  http://www.ltestudios.com

*/

#ifndef __I_MESH_LOADER_H_INCLUDED__
#define __I_MESH_LOADER_H_INCLUDED__

#include "IUnknown.h"
#include "IReadFile.h"
#include "IAnimatedMesh.h"

namespace engine
{
namespace scene
{

//!	Class which is able to load an animated mesh from a file.
/** If you want the engine Engine be able to load meshes of 
currently unsupported file formats (e.g .cob), then implement
this and add your new Surface loader with 
ISceneManager::addExternalMeshLoader() to the engine. */
class IMeshLoader : public IUnknown
{
public:

	//! destructor
	virtual ~IMeshLoader() {};

	//! Returns true if the file maybe is able to be loaded by this class.
	/** This decision should be based only on the file extension (e.g. ".cob") */
	virtual bool isALoadableFileExtension(const c8* fileName) = 0;

	//! Creates/loads an animated mesh from the file.
	/** \return Pointer to the created mesh. Returns 0 if loading failed.
	If you no longer need the mesh, you should call IAnimatedMesh::drop().
	See IUnknown::drop() for more information. */
	virtual IAnimatedMesh* createMesh(engine::io::IReadFile* file) = 0;
};


} // end namespace scene
} // end namespace engine

#endif


