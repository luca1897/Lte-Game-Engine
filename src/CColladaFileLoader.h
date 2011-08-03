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
 
#ifndef __C_COLLADA_MESH_FILE_LOADER_H_INCLUDED__
#define __C_COLLADA_MESH_FILE_LOADER_H_INCLUDED__

#include "IMeshLoader.h"
#include "IFileSystem.h"
#include "IVideoDriver.h"
#include "engineString.h"
#include "SMesh.h"
#include "SMeshBuffer.h"
#include "ISceneManager.h"

namespace engine
{
namespace scene
{

#ifdef _DEBUG
//#define COLLADA_READER_DEBUG
#endif

class IColladaPrefab;

enum ECOLLADA_PARAM_NAME
{
	ECPN_COLOR = 0,
	ECPN_AMBIENT,
	ECPN_DIFFUSE,
	ECPN_SPECULAR,
	ECPN_SHININESS,
	ECPN_YFOV,
	ECPN_ZNEAR,
	ECPN_ZFAR,

	ECPN_COUNT
};

enum ECOLLADA_PARAM_TYPE
{
	ECPT_FLOAT = 0,
	ECPT_FLOAT2,
	ECPT_FLOAT3,
	ECPT_FLOAT4,

	ECPT_COUNT
};

//! Collada Parameter
struct SColladaParam
{
	SColladaParam()
		: Name(ECPN_COUNT), Type(ECPT_COUNT)
	{
		for (int i=0; i<4; ++i) Floats[i] = 0;
	}

	ECOLLADA_PARAM_NAME Name;
	ECOLLADA_PARAM_TYPE Type;

	f32 Floats[4];
};

enum ECOLLADA_INPUT_SEMANTIC
{
	ECIS_POSITION = 0,
	ECIS_VERTEX,
	ECIS_NORMAL,
	ECIS_TEXCOORD,
	ECIS_UV,
	ECIS_TANGENT,

	ECIS_COUNT
};

//! Collada Input
struct SColladaInput
{
	SColladaInput()
		: Semantic(ECIS_COUNT)
	{
	}

	ECOLLADA_INPUT_SEMANTIC Semantic;
	core::stringc Source;
};

//! Collada material
struct SColladaMaterial
{
	video::SMaterial Mat;
	core::stringc Id;
};


struct SNumberArray // for stroring f32 and int arrays
{
	core::stringc Name;
	core::array<f32> Data;
};

struct SAccessor
{
	SAccessor()
		: Count(0), Offset(0), Stride(1) {
	}
	// I don't store the source of the accessor here because I assume
	// it to use the array of the source this accessor is located in.

	int Count;
	int Offset;
	int Stride;

	core::array<SColladaParam> Parameters; // parameters defining the accessor
};

struct SSource
{
	core::stringc Id;
	SNumberArray Array;
	core::array<SAccessor> Accessors;
};


//! Meshloader capable of loading COLLADA meshes and scene descriptions into engine.
class CColladaFileLoader : public IMeshLoader
{
public:

//! Constructor
CColladaFileLoader(video::IVideoDriver* driver,
                   scene::ISceneManager* smgr, io::IFileSystem* fs);

//! destructor
virtual ~CColladaFileLoader();

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".cob")
virtual bool isALoadableFileExtension(const c8* fileName);

//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IUnknown::drop() for more information.
virtual IAnimatedMesh* createMesh(engine::io::IReadFile* file);

private:

//! skips an (unknown) section in the collada document
void skipSection(io::IXMLReaderUTF8* reader, bool reportSkipping);

//! reads the <COLLADA> section and its content
void readColladaSection(io::IXMLReaderUTF8* reader);

//! reads a <library> section and its content
void readLibrarySection(io::IXMLReaderUTF8* reader);

//! reads a <scene> section and its content
void readSceneSection(io::IXMLReaderUTF8* reader);

//! reads a <asset> section and its content
void readAssetSection(io::IXMLReaderUTF8* reader);

//! reads a <node> section and its content
void readNodeSection(io::IXMLReaderUTF8* reader, scene::ISceneNode* parent);

//! reads a <lookat> element and its content and creates a matrix from it
core::matrix4 readLookAtNode(io::IXMLReaderUTF8* reader);

//! reads a <matrix> element and its content and creates a matrix from it
core::matrix4 readMatrixNode(io::IXMLReaderUTF8* reader);

//! reads a <perspective> element and its content and creates a matrix from it
core::matrix4 readPerspectiveNode(io::IXMLReaderUTF8* reader);

//! reads a <rotate> element and its content and creates a matrix from it
core::matrix4 readRotateNode(io::IXMLReaderUTF8* reader);

//! reads a <skew> element and its content and creates a matrix from it
core::matrix4 readSkewNode(io::IXMLReaderUTF8* reader);

//! reads a <scale> element and its content and creates a matrix from it
core::matrix4 readScaleNode(io::IXMLReaderUTF8* reader);

//! reads a <translate> element and its content and creates a matrix from it
core::matrix4 readTranslateNode(io::IXMLReaderUTF8* reader);

//! reads a <instance> node and creates a scene node from it
void readInstanceNode(io::IXMLReaderUTF8* reader, scene::ISceneNode* parent,
                      scene::ISceneNode** outNode);

//! reads a <light> element and stores it as prefab
void readLightPrefab(io::IXMLReaderUTF8* reader);

//! reads a <camera> element and stores it as prefab
void readCameraPrefab(io::IXMLReaderUTF8* reader);

//! reads a <material> element and stores it in the material section
void readMaterial(io::IXMLReaderUTF8* reader);

//! reads a <geometry> element and stores it as mesh if possible
void readGeometry(io::IXMLReaderUTF8* reader);

//! parses a f32 from a char pointer and moves the pointer to
//! the end of the parsed f32
inline f32 readFloat(c8** p);

//! parses an int from a char pointer and moves the pointer to
//! the end of the parsed f32
inline s32 readInt(c8** p);

//! places pointer to next begin of a token
void findNextNoneWhiteSpace(c8** p);

//! reads floats from inside of xml element until end of xml element
void readFloatsInsideElement(io::IXMLReaderUTF8* reader, f32* floats, s32 count);

//! clears all loaded data
void clearData();

//! parses all collada parameters inside an element and stores them in Parameters
void readColladaParameters(io::IXMLReaderUTF8* reader, const core::stringc& parentName);

//! returns a collada parameter or none if not found
SColladaParam* getColladaParameter(ECOLLADA_PARAM_NAME name);

//! parses all collada inuts inside an element and stores them in Inputs. Reads
//! until first tag which is not an input tag or the end of the parent is reached
void readColladaInputs(io::IXMLReaderUTF8* reader, const core::stringc& parentName);

//! reads a collada input tag and adds it to the input parameter
void readColladaInput(io::IXMLReaderUTF8* reader);

//! returns a collada input or none if not found
SColladaInput* getColladaInput(ECOLLADA_INPUT_SEMANTIC input);

//! changes the XML URI into an internal id
void uriToId(core::stringc& str);

//! reads a polygons section and creates a mesh from it
void readPolygonSection(io::IXMLReaderUTF8* reader,
                        core::stringc vertexPositionSource, core::array<SSource>& sources,
                        scene::SMesh* mesh);

video::IVideoDriver* Driver;
scene::ISceneManager* SceneManager;
io::IFileSystem* FileSystem;

scene::IAnimatedMesh* DummyMesh;
core::stringc CurrentlyLoadingMesh;

core::array<IColladaPrefab*> Prefabs;
core::array<SColladaParam> Parameters;
core::array<SColladaMaterial> Materials;
core::array<SColladaInput> Inputs;

bool CreateInstances;
};



//! following class is for holding and createing instances of library objects,
//! named prefabs in this loader.
class IColladaPrefab : public IUnknown
{
public:

//! creates an instance of this prefab
virtual scene::ISceneNode* addInstance(scene::ISceneNode* parent,
                                       scene::ISceneManager* mgr) = 0;

//! returns id of this prefab
virtual const c8* getId() = 0;
};


} // end namespace scene
} // end namespace engine

#endif

