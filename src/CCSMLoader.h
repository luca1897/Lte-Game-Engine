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
 
//
// This Loader has been originally written by Saurav Mohapatra. I (Nikolaus Gebhardt)
// modified some minor things and integrated it into engine 0.9. Thanks a lot
// to Saurav Mohapatra for his work on this and that he gave me his permission to
// add it into engine.
// I did some changes to Saurav Mohapatra's loader, so I'm writing this down here:
// - Replaced all dependencies to STL and stdio with engine:: methods/constructs.
// - Moved everything into namespace engine::scene
// - Replaced logging with engine's internal logger.
// - Removed dependency to engineDevice
// - Moved all internal structures into CCSMLoader.cpp
// - Made the texture root parameter dependent on a ISceneManager string parameter
// - removed exceptions
// - Implemented CCCSMLoader as IMeshLoader
// - Fixed some problems with memory leaks
// - Fixed bounding box calculation
//
// The original readme of this file looks like this:
//
// This component provides a loader for the Cartography shop 4.x .csm maps for engine Engine.
// This is a part of the M_TRIX Project.
// This is licensed under the ZLib/LibPNG license
// The engineCSM library is written by Saurav Mohapatra.
//
// Features
//
// The engineCSM library features the following capabilities
//
//  * Loads the .csm 4.0 and 4.1 files transparently
//  * Presents the loaded file as engine::scene::IAnimatedMesh for easy creation of IOctTreeSceneNode
//  * Loads the textures given the correct texture root. hence map and textures can be in separate directories
//
// For more informations go to http://www.geocities.com/standard_template/enginecsm/downloads.html

#ifndef __CSM_LOADER_H_INCLUDED__
#define __CSM_LOADER_H_INCLUDED__

#include "engineArray.h"
#include "IMesh.h"
#include "engineString.h"
#include "IFileSystem.h"
#include "IMeshLoader.h"

namespace engine
{
namespace scene
{
class CSMFile;
class ISceneManager;

class CCSMLoader : public scene::IMeshLoader
{
public:

CCSMLoader(ISceneManager* manager, io::IFileSystem* fs);
virtual ~CCSMLoader();

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
virtual bool isALoadableFileExtension(const c8* fileName);

//! creates/loads an animated mesh from the file.
virtual IAnimatedMesh* createMesh(engine::io::IReadFile* file);

private:

scene::IMesh* createCSMMesh(engine::io::IReadFile* file);

scene::IMesh* createengineMesh(const CSMFile* csmFile,
                               core::stringc textureRoot, const c8* lmprefix);

scene::ISceneManager* SceneManager;
io::IFileSystem* FileSystem;
};

} // end namespace
} // end namespace

#endif

