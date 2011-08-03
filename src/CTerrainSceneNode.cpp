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
 
// The code for the TerrainSceneNode is based on the GeoMipMapSceneNode
// developed by Spinz. He made it available for engine and allowed it to be
// distributed under this licence. I only modified some parts. A lot of thanks go to him.

#include "CTerrainSceneNode.h"
#include "CTerrainTriangleSelector.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "ICameraSceneNode.h"
#include "SMeshBufferLightMap.h"
#include <math.h>
#include <stdio.h>
#include "os.h"
#include "fastmath.h"

extern bool __using_bezier_patches;
extern int __bezier_mesh_size;


namespace engine
{
namespace scene
{

//! constructor
CTerrainSceneNode::CTerrainSceneNode(ISceneNode* parent, ISceneManager* mgr,
                                     s32 id, s32 maxLOD, E_TERRAIN_PATCH_SIZE patchSize, const core::vector3df& position,
                                     const core::vector3df& rotation, const core::vector3df& scale)
	: ITerrainSceneNode(parent, mgr, id, position, rotation, scale),
	OverrideDistanceThreshold(false),
	DynamicSelectorUpdate(false),
	UseDefaultRotationPivot(true)
{
	        #ifdef _DEBUG
	setDebugName("CTerrainSceneNode");
	        #endif

	TerrainData.PatchSize = patchSize;
	TerrainData.CalcPatchSize = patchSize - 1;
	TerrainData.MaxLOD = maxLOD;
	TerrainData.Scale = scale;
	TerrainData.Position = position;
	TerrainData.Rotation = rotation;

	OldCameraPosition = core::vector3df(-99999.9f, -99999.9f, -99999.9f );
	OldCameraRotation = core::vector3df(-99999.9f, -99999.9f, -99999.9f );
	CameraMovementDelta = 10.0f;
	CameraRotationDelta = 1.0f;

	setAutomaticCulling( false );
}

//! destructor
CTerrainSceneNode::~CTerrainSceneNode ( )
{
	if (TerrainData.LODDistanceThreshold)
		delete [] TerrainData.LODDistanceThreshold;

	if (TerrainData.Patches)
		delete [] TerrainData.Patches;
}

//! Initializes the terrain data.  Loads the vertices from the heightMapFile
bool CTerrainSceneNode::loadHeightMap( io::IReadFile* file, video::SColor vertexColor )
{
	if( !file )
		return false;

	u32 startTime = os::Timer::getTime();
	video::IImage* heightMap = SceneManager->getVideoDriver()->createImageFromFile( file );

	if( !heightMap )
	{
		os::Printer::print( "Was not able to load heightmap." );
		return false;
	}

	// Get the dimension of the heightmap data
	TerrainData.Size = heightMap->getDimension().Width;

	// Make sure the maximum level of detail is compatible with the heightmap size
	if( TerrainData.Size <= 17 && TerrainData.MaxLOD > 1 )
		TerrainData.MaxLOD = 1;
	else
	if( TerrainData.Size <= 33 && TerrainData.MaxLOD > 2 )
		TerrainData.MaxLOD = 2;
	else
	if( TerrainData.Size <= 65 && TerrainData.MaxLOD > 3 )
		TerrainData.MaxLOD = 3;
	else
	if( TerrainData.Size <= 129 && TerrainData.MaxLOD > 4 )
		TerrainData.MaxLOD = 4;
	else
	if( TerrainData.Size <= 257 && TerrainData.MaxLOD > 4 )
		TerrainData.MaxLOD = 5;
	else
	if( TerrainData.Size <= 513 && TerrainData.MaxLOD > 6 )
		TerrainData.MaxLOD = 6;
	else
	if( TerrainData.Size <= 1025 && TerrainData.MaxLOD > 7 )
		TerrainData.MaxLOD = 7;

	// --- Generate vertex data from heightmap ----
	// resize the vertex array for the mesh buffer one time ( makes loading faster )
	SMeshBufferLightMap* pMeshBuffer = new SMeshBufferLightMap();
	pMeshBuffer->Vertices.reallocate( TerrainData.Size * TerrainData.Size );
	pMeshBuffer->Vertices.set_used( TerrainData.Size * TerrainData.Size );

	video::S3DVertex2TCoords vertex;
	core::vector3df normal = core::vector3df( 0.0f, 1.0f, 0.0f );
	vertex.Color = vertexColor;

	__bezier_mesh_size = TerrainData.Size;

	// Read the heightmap to get the vertex data
	// Apply positions changes, scaling changes
	for( s32 x = 0; x < TerrainData.Size; ++x )
	{
		for( s32 z = 0; z < TerrainData.Size; ++z )
		{
			s32 index = x * TerrainData.Size + z;

			vertex.Pos.X = (f32)x;
			vertex.Pos.Y = (f32)( heightMap->getPixel(x,z).getRed() + heightMap->getPixel(x,z).getGreen() + heightMap->getPixel(x,z).getBlue() ) / 3.0f;
			vertex.Pos.Z = (f32)z;

			vertex.Normal = normal;

			vertex.TCoords.X = x / (f32)TerrainData.Size;
			vertex.TCoords.Y = z / (f32)TerrainData.Size;

//				vertex.TCoords2.X = x / (f32)TerrainData.Size;
			//			vertex.TCoords2.Y = z / (f32)TerrainData.Size;

			pMeshBuffer->Vertices[index] = vertex;
		}
	}

	// drop heightMap, no longer needed
	heightMap->drop();

	// calculate smooth normals for the vertices
	calculateNormals( pMeshBuffer );

	// add the MeshBuffer to the mesh
	Mesh.addMeshBuffer( pMeshBuffer );
	s32 vertexCount = pMeshBuffer->getVertexCount();

	// We copy the data to the renderBuffer, after the normals have been calculated.
	RenderBuffer.Vertices.reallocate( vertexCount );
	RenderBuffer.Vertices.set_used( vertexCount );

	for( s32 i = 0; i < vertexCount; i++ )
	{
		RenderBuffer.Vertices[i] = pMeshBuffer->Vertices[i];
		RenderBuffer.Vertices[i].Pos *= TerrainData.Scale;
		RenderBuffer.Vertices[i].Pos += TerrainData.Position;
	}

	// We no longer need the pMeshBuffer
	pMeshBuffer->drop();

	// calculate all the necessary data for the patches and the terrain
	calculateDistanceThresholds();
	createPatches();
	calculatePatchData();

	// set the default rotation pivot point to the terrain nodes center
	TerrainData.RotationPivot = TerrainData.Center;

	// Rotate the vertices of the terrain by the rotation specified.  Must be done
	// after calculating the terrain data, so we know what the current center of the
	// terrain is.
	setRotation( TerrainData.Rotation );

	// Pre-allocate memory for indices
	RenderBuffer.Indices.reallocate( TerrainData.PatchCount * TerrainData.PatchCount *
	                                 TerrainData.CalcPatchSize * TerrainData.CalcPatchSize * 6 );
	RenderBuffer.Indices.set_used( TerrainData.PatchCount * TerrainData.PatchCount *
	                               TerrainData.CalcPatchSize * TerrainData.CalcPatchSize * 6 );

	u32 endTime = os::Timer::getTime();

	c8 tmp[255];
	sprintf(tmp, "Generated terrain data (%dx%d) in %.4f seconds",
	        TerrainData.Size, TerrainData.Size, ( endTime - startTime ) / 1000.0f );
	os::Printer::print( tmp );

	return true;
}

//! Initializes the terrain data.  Loads the vertices from the heightMapFile
bool CTerrainSceneNode::loadHeightMapRAW( io::IReadFile* file, s32 bitsPerPixel, video::SColor vertexColor )
{
	if( !file )
		return false;

	// start reading
	u32 startTime = os::Timer::getTime();

	// get file size
	s32 fileSize = file->getSize();
	s32 bytesPerPixel = bitsPerPixel / 8;
	s32 heightMapSize = (s32)FastSqrt( (f32)( fileSize / bytesPerPixel ) );

	// Get the dimension of the heightmap data
	TerrainData.Size = heightMapSize;

	// Make sure the maximum level of detail is compatible with the heightmap size
	if( TerrainData.Size <= 17 && TerrainData.MaxLOD > 1 )
		TerrainData.MaxLOD = 1;
	else
	if( TerrainData.Size <= 33 && TerrainData.MaxLOD > 2 )
		TerrainData.MaxLOD = 2;
	else
	if( TerrainData.Size <= 65 && TerrainData.MaxLOD > 3 )
		TerrainData.MaxLOD = 3;
	else
	if( TerrainData.Size <= 129 && TerrainData.MaxLOD > 4 )
		TerrainData.MaxLOD = 4;
	else
	if( TerrainData.Size <= 257 && TerrainData.MaxLOD > 4 )
		TerrainData.MaxLOD = 5;
	else
	if( TerrainData.Size <= 513 && TerrainData.MaxLOD > 6 )
		TerrainData.MaxLOD = 6;
	else
	if( TerrainData.Size <= 1025 && TerrainData.MaxLOD > 7 )
		TerrainData.MaxLOD = 7;

	// --- Generate vertex data from heightmap ----
	// resize the vertex array for the mesh buffer one time ( makes loading faster )
	SMeshBufferLightMap* pMeshBuffer = new SMeshBufferLightMap();
	pMeshBuffer->Vertices.reallocate( TerrainData.Size * TerrainData.Size );
	pMeshBuffer->Vertices.set_used( TerrainData.Size * TerrainData.Size );

	video::S3DVertex2TCoords vertex;
	core::vector3df normal = core::vector3df( 0.0f, 1.0f, 0.0f );
	vertex.Color = vertexColor;

	// Read the heightmap to get the vertex data
	// Apply positions changes, scaling changes
	for( s32 x = 0; x < TerrainData.Size; ++x )
	{
		for( s32 z = 0; z < TerrainData.Size; ++z )
		{
			vertex.Pos.X = (f32)x;

			if( file->read( &vertex.Pos.Y, bytesPerPixel ) != bytesPerPixel )
			{
				os::Printer::print("Error reading heightmap RAW file.");
				pMeshBuffer->drop();
				return false;
			}

			vertex.Pos.Z = (f32)z;

			vertex.Normal = normal;

			vertex.TCoords.X = x / (f32)TerrainData.Size;
			vertex.TCoords.Y = z / (f32)TerrainData.Size;

//				vertex.TCoords2.X = x / (f32)TerrainData.Size;
			//			vertex.TCoords2.Y = z / (f32)TerrainData.Size;

			pMeshBuffer->Vertices.push_back( vertex );
		}
	}

	// calculate smooth normals for the vertices
	calculateNormals( pMeshBuffer );

	// add the MeshBuffer to the mesh
	Mesh.addMeshBuffer( pMeshBuffer );
	s32 vertexCount = pMeshBuffer->getVertexCount();

	// We copy the data to the renderBuffer, after the normals have been calculated.
	RenderBuffer.Vertices.reallocate( vertexCount );
	RenderBuffer.Vertices.set_used( vertexCount );

	for( s32 i = 0; i < vertexCount; i++ )
	{
		RenderBuffer.Vertices[i] = pMeshBuffer->Vertices[i];
		RenderBuffer.Vertices[i].Pos *= TerrainData.Scale;
		RenderBuffer.Vertices[i].Pos += TerrainData.Position;
	}

	// We no longer need the pMeshBuffer
	pMeshBuffer->drop();

	// calculate all the necessary data for the patches and the terrain
	calculateDistanceThresholds();
	createPatches();
	calculatePatchData();

	// set the default rotation pivot point to the terrain nodes center
	TerrainData.RotationPivot = TerrainData.Center;

	// Rotate the vertices of the terrain by the rotation specified.  Must be done
	// after calculating the terrain data, so we know what the current center of the
	// terrain is.
	setRotation( TerrainData.Rotation );

	// Pre-allocate memory for indices
	RenderBuffer.Indices.reallocate( TerrainData.PatchCount * TerrainData.PatchCount *
	                                 TerrainData.CalcPatchSize * TerrainData.CalcPatchSize * 6 );
	RenderBuffer.Indices.set_used( TerrainData.PatchCount * TerrainData.PatchCount *
	                               TerrainData.CalcPatchSize * TerrainData.CalcPatchSize * 6 );

	u32 endTime = os::Timer::getTime();

	c8 tmp[255];
	sprintf( tmp, "Generated terrain data (%dx%d) in %.4f seconds",
	         TerrainData.Size, TerrainData.Size, (endTime - startTime) / 1000.0f );
	os::Printer::print( tmp );

	return true;
}

//! Sets the scale of the scene node.
//! \param scale: New scale of the node
void CTerrainSceneNode::setScale(const core::vector3df& scale)
{
	TerrainData.Scale = scale;
	applyTransformation();
}

//! Sets the rotation of the node. This only modifies
//! the relative rotation of the node.
//! \param rotation: New rotation of the node in degrees.
void CTerrainSceneNode::setRotation(const core::vector3df& rotation)
{
	TerrainData.Rotation = rotation;
	applyTransformation();
}

//! Sets the pivot point for rotation of this node.  This is useful for the TiledTerrainManager to
//! rotate all terrain tiles around a global world point.
//! NOTE: The default for the RotationPivot will be the center of the individual tile.
void CTerrainSceneNode::setRotationPivot( const core::vector3df& pivot )
{
	UseDefaultRotationPivot = false;
	TerrainData.RotationPivot = pivot;
}

//! Sets the position of the node.
//! \param newpos: New postition of the scene node.
void CTerrainSceneNode::setPosition ( const core::vector3df& newpos )
{
	TerrainData.Position = newpos;
	applyTransformation();
}

//! Apply transformation changes( scale, position, rotation )
void CTerrainSceneNode::applyTransformation()
{
	if( !Mesh.getMeshBufferCount() )
		return;

	TerrainData.Position = TerrainData.Position;
	video::S3DVertex2TCoords* meshVertices = (video::S3DVertex2TCoords*)Mesh.getMeshBuffer( 0 )->getVertices();
	s32 vtxCount = Mesh.getMeshBuffer( 0 )->getVertexCount();
	core::matrix4 rotMatrix;
	rotMatrix.setRotationDegrees( TerrainData.Rotation );

	for( s32 i = 0; i < vtxCount; ++i )
	{
		RenderBuffer.Vertices[i].Pos = meshVertices[i].Pos * TerrainData.Scale + TerrainData.Position;

		RenderBuffer.Vertices[i].Pos -= TerrainData.RotationPivot;
		rotMatrix.inverseRotateVect( RenderBuffer.Vertices[i].Pos );
		RenderBuffer.Vertices[i].Pos += TerrainData.RotationPivot;
	}

	calculateDistanceThresholds( true );
	calculatePatchData();
}

//! Updates the scene nodes indices if the camera has moved or rotated by a certain
//! threshold, which can be changed using the SetCameraMovementDeltaThreshold and
//! SetCameraRotationDeltaThreshold functions.  This also determines if a given patch
//! for the scene node is within the view frustrum and if it's not the indices are not
//! generated for that patch.

#include <pspdebug.h>
#define printf pspDebugScreenPrintf
void CTerrainSceneNode::OnPreRender()
{

	if (!IsVisible || !SceneManager->getActiveCamera())
		return;

	preRenderLODCalculations();
	preRenderIndicesCalculations();

	//		printf("terrain %d\n", os::Timer::getRealTime() -d);
}

void CTerrainSceneNode::preRenderLODCalculations()
{
	SceneManager->registerNodeForRendering( this );
	// Do Not call ISceneNode::OnPreRender ( ), this node should have no children

	// Determine the camera rotation, based on the camera direction.
	core::line3d<f32> line;
	line.start = SceneManager->getActiveCamera()->getAbsolutePosition();
	line.end = SceneManager->getActiveCamera()->getTarget();
	core::vector3df cameraRotation = line.getVector().getHorizontalAngle();
	core::vector3df cameraPosition = SceneManager->getActiveCamera()->getPosition ( );

	// Only check on the Camera's Y Rotation
	if (( fabs(cameraRotation.X - OldCameraRotation.X) < CameraRotationDelta) &&
	    ( fabs(cameraRotation.Y - OldCameraRotation.Y) < CameraRotationDelta))
	{
		if ((fabs(cameraPosition.X - OldCameraPosition.X) < CameraMovementDelta) &&
		    (fabs(cameraPosition.Y - OldCameraPosition.Y) < CameraMovementDelta) &&
		    (fabs(cameraPosition.Z - OldCameraPosition.Z) < CameraMovementDelta))
		{
			return;
		}
	}

	OldCameraPosition = cameraPosition;
	OldCameraRotation = cameraRotation;
	const SViewFrustrum* frustrum = SceneManager->getActiveCamera()->getViewFrustrum();

	// Determine each patches LOD based on distance from camera ( and whether or not they are in
	// the view frustrum ).
	for( s32 j = 0; j < TerrainData.PatchCount * TerrainData.PatchCount; ++j )
	{
		if( frustrum->getBoundingBox().intersectsWithBox( TerrainData.Patches[j].BoundingBox ) )
		{
			f32 distance = (cameraPosition.X - TerrainData.Patches[j].Center.X) * (cameraPosition.X - TerrainData.Patches[j].Center.X) +
			               (cameraPosition.Y - TerrainData.Patches[j].Center.Y) * (cameraPosition.Y - TerrainData.Patches[j].Center.Y) +
			               (cameraPosition.Z - TerrainData.Patches[j].Center.Z) * (cameraPosition.Z - TerrainData.Patches[j].Center.Z);

			for( s32 i = TerrainData.MaxLOD - 1; i >= 0; --i )
			{
				if( distance >= TerrainData.LODDistanceThreshold[i] )
				{
					TerrainData.Patches[j].CurrentLOD = i;
					break;
				}
				//else if( i == 0 )
				{
					// If we've turned off a patch from viewing, because of the frustrum, and now we turn around and it's
					// too close, we need to turn it back on, at the highest LOD.  The if above doesn't catch this.
					TerrainData.Patches[j].CurrentLOD = 0;
				}
			}
		}
		else
		{
			TerrainData.Patches[j].CurrentLOD = -1;
		}
	}

}

void CTerrainSceneNode::preRenderIndicesCalculations()
{
	IndicesToRender = 0;
	s32 index11;
	s32 index21;
	s32 index12;
	s32 index22;

	// Then generate the indices for all patches that are visible.
	for( s32 i = 0; i < TerrainData.PatchCount; ++i )
	{
		for( s32 j = 0; j < TerrainData.PatchCount; ++j )
		{
			s32 index = i * TerrainData.PatchCount + j;
			if( TerrainData.Patches[index].CurrentLOD >= 0 )
			{
				s32 x = 0;
				s32 z = 0;

				// calculate the step we take this patch, based on the patches current LOD
				s32 step = 1 << TerrainData.Patches[index].CurrentLOD;

				// Loop through patch and generate indices
				while( z < TerrainData.CalcPatchSize )
				{
					index11 = getIndex( j, i, index, x, z );
					index21 = getIndex( j, i, index, x + step, z );
					index12 = getIndex( j, i, index, x, z + step );
					index22 = getIndex( j, i, index, x + step, z + step );

					RenderBuffer.Indices[IndicesToRender++] = index12;
					RenderBuffer.Indices[IndicesToRender++] = index11;
					RenderBuffer.Indices[IndicesToRender++] = index22;
					RenderBuffer.Indices[IndicesToRender++] = index22;
					RenderBuffer.Indices[IndicesToRender++] = index11;
					RenderBuffer.Indices[IndicesToRender++] = index21;

					// increment index position horizontally
					x += step;

					if ( x >= TerrainData.CalcPatchSize )         // we've hit an edge
					{
						x = 0;
						z += step;
					}
				}
			}
		}
	}

	if ( DynamicSelectorUpdate && TriangleSelector )
	{
		CTerrainTriangleSelector* selector = (CTerrainTriangleSelector*)TriangleSelector;
		selector->setTriangleData ( this, -1 );
	}
}


//! Render the scene node
void CTerrainSceneNode::render()
{

	if (!IsVisible || !SceneManager->getActiveCamera())
		return;


	core::matrix4 identity;
	SceneManager->getVideoDriver()->setTransform (video::ETS_WORLD, identity);

	SceneManager->getVideoDriver()->setMaterial(Mesh.getMeshBuffer(0)->getMaterial());

	__using_bezier_patches = true;

	// For use with geomorphing
	SceneManager->getVideoDriver()->drawIndexedTriangleList(
	        (video::S3DVertex2TCoords*)RenderBuffer.getVertices(),
	        RenderBuffer.getVertexCount(),
	        RenderBuffer.getIndices(),
	        IndicesToRender / 3
	        );
	__using_bezier_patches = false;

}

//! Return the bounding box of the entire terrain.
const core::aabbox3d<f32>& CTerrainSceneNode::getBoundingBox() const
{
	return TerrainData.BoundingBox;
}

//! Return the bounding box of a patch
const core::aabbox3d<f32>& CTerrainSceneNode::getBoundingBox( s32 patchX, s32 patchZ ) const
{
	return TerrainData.Patches[patchX * TerrainData.PatchCount + patchZ].BoundingBox;
}

//! Gets the meshbuffer data based on a specified Level of Detail.
//! \param mb: A reference to an SMeshBuffer object
//! \param LOD: The Level Of Detail you want the indices from.
void CTerrainSceneNode::getMeshBufferForLOD(SMeshBufferLightMap& mb, s32 LOD )
{
	if ( LOD < 0 )
		LOD = 0;
	else if ( LOD > TerrainData.MaxLOD - 1 )
		LOD = TerrainData.MaxLOD - 1;

	s32 numVertices = Mesh.getMeshBuffer( 0 )->getVertexCount ( );
	mb.Vertices.reallocate ( numVertices );
	video::S3DVertex2TCoords* vertices = (video::S3DVertex2TCoords*)Mesh.getMeshBuffer ( 0 )->getVertices ( );

	s32 i;
	for (i=0; i<numVertices; ++i)
		mb.Vertices.push_back(vertices[i]);

	// calculate the step we take for all patches, since LOD is the same
	s32 step = 1 << LOD;
	s32 index11;
	s32 index21;
	s32 index12;
	s32 index22;

	// Generate the indices for all patches at the specified LOD
	for (i=0; i<TerrainData.PatchCount; ++i)
	{
		for (s32 j=0; j<TerrainData.PatchCount; ++j)
		{
			s32 index = i*TerrainData.PatchCount + j;
			s32 x = 0;
			s32 z = 0;

			// Loop through patch and generate indices
			while (z < TerrainData.CalcPatchSize)
			{
				index11 = getIndex( j, i, index, x, z );
				index21 = getIndex( j, i, index, x + step, z );
				index12 = getIndex( j, i, index, x, z + step );
				index22 = getIndex( j, i, index, x + step, z + step );

				mb.Indices.push_back( index12 );
				mb.Indices.push_back( index11 );
				mb.Indices.push_back( index22 );
				mb.Indices.push_back( index22 );
				mb.Indices.push_back( index11 );
				mb.Indices.push_back( index21 );

				// increment index position horizontally
				x += step;

				if (x >= TerrainData.CalcPatchSize)         // we've hit an edge
				{
					x = 0;
					z += step;
				}
			}
		}
	}
}

//! Gets the indices for a specified patch at a specified Level of Detail.
//! \param mb: A reference to an array of u32 indices.
//! \param patchX: Patch x coordinate.
//! \param patchZ: Patch z coordinate.
//! \param LOD: The level of detail to get for that patch.  If -1, then get
//! the CurrentLOD.  If the CurrentLOD is set to -1, meaning it's not shown,
//! then it will retrieve the triangles at the highest LOD ( 0 ).
//! \return: Number if indices put into the buffer.
s32 CTerrainSceneNode::getIndicesForPatch(core::array<u32>& indices, s32 patchX, s32 patchZ, s32 LOD )
{
	if ( patchX < 0 || patchX > TerrainData.PatchCount - 1 || patchZ < 0 || patchZ > TerrainData.PatchCount - 1 )
		return -1;

	if ( LOD < -1 || LOD > TerrainData.MaxLOD - 1 )
		return -1;

	s32 rv = 0;
	core::array<s32> cLODs;
	bool setLODs = false;

	// If LOD of -1 was passed in, use the CurrentLOD of the patch specified
	if ( LOD == -1 )
	{
		LOD = TerrainData.Patches[patchX * TerrainData.PatchCount + patchZ].CurrentLOD;
	}
	else
	{
		getCurrentLODOfPatches(cLODs);
		setCurrentLODOfPatches(LOD);
		setLODs = true;
	}

	if ( LOD < 0 )
		return -2;         // Patch not visible, don't generate indices.

	// calculate the step we take for this LOD
	s32 step = 1 << LOD;

	// Generate the indices for the specified patch at the specified LOD
	s32 index = patchX * TerrainData.PatchCount + patchZ;

	s32 x = 0;
	s32 z = 0;
	s32 index11;
	s32 index21;
	s32 index12;
	s32 index22;

	indices.set_used ( TerrainData.PatchSize * TerrainData.PatchSize * 6 );

	// Loop through patch and generate indices
	while (z<TerrainData.CalcPatchSize)
	{
		index11 = getIndex( patchZ, patchX, index, x, z );
		index21 = getIndex( patchZ, patchX, index, x + step, z );
		index12 = getIndex( patchZ, patchX, index, x, z + step );
		index22 = getIndex( patchZ, patchX, index, x + step, z + step );

		indices[rv++] = index12;
		indices[rv++] = index11;
		indices[rv++] = index22;
		indices[rv++] = index22;
		indices[rv++] = index11;
		indices[rv++] = index21;

		// increment index position horizontally
		x += step;

		if (x >= TerrainData.CalcPatchSize)         // we've hit an edge
		{
			x = 0;
			z += step;
		}
	}

	if ( setLODs )
		setCurrentLODOfPatches (cLODs);

	return rv;
}

//! Populates an array with the CurrentLOD of each patch.
//! \param LODs: A reference to a core::array<s32> to hold the values
//! \return Returns the number of elements in the array
s32 CTerrainSceneNode::getCurrentLODOfPatches(core::array<s32>& LODs)
{
	s32 numLODs;
	LODs.clear ( );

	for ( numLODs = 0; numLODs < TerrainData.PatchCount * TerrainData.PatchCount; numLODs++ )
		LODs.push_back ( TerrainData.Patches[numLODs].CurrentLOD );

	return LODs.size();
}


//! Manually sets the LOD of a patch
//! \param patchX: Patch x coordinate.
//! \param patchZ: Patch z coordinate.
//! \param LOD: The level of detail to set the patch to.
void CTerrainSceneNode::setLODOfPatch( s32 patchX, s32 patchZ, s32 LOD )
{
	TerrainData.Patches[patchX * TerrainData.PatchCount + patchZ].CurrentLOD = LOD;
}


//! Override the default generation of distance thresholds for determining the LOD a patch
//! is rendered at.
bool CTerrainSceneNode::overrideLODDistance(s32 LOD, f32 newDistance)
{
	OverrideDistanceThreshold = true;

	if ( LOD < 0 || LOD > TerrainData.MaxLOD - 1 )
		return false;

	TerrainData.LODDistanceThreshold[LOD] = newDistance * newDistance;

	return true;
}

//! Creates a planar texture mapping on the terrain
//! \param resolution: resolution of the planar mapping. This is the value
//! specifying the releation between world space and texture coordinate space.
void CTerrainSceneNode::scaleTexture(f32 resolution, f32 resolution2)
{
	for (s32 x=0; x<TerrainData.Size; ++x)
	{
		for (s32 z=0; z<TerrainData.Size; ++z)
		{
			s32 index = x * TerrainData.Size + z;
			RenderBuffer.Vertices[index].TCoords.X = x / (f32)TerrainData.Size * resolution;
			RenderBuffer.Vertices[index].TCoords.Y = z / (f32)TerrainData.Size * resolution;


			if ( resolution2 == 0 )
			{
//					RenderBuffer.Vertices[index].TCoords2 = RenderBuffer.Vertices[index].TCoords;
			}
			else
			{
				//				RenderBuffer.Vertices[index].TCoords2.X = x / (f32)TerrainData.Size * resolution2;
				//			RenderBuffer.Vertices[index].TCoords2.Y = z / (f32)TerrainData.Size * resolution2;
			}
		}
	}
}

//! used to get the indices when generatin index data for patches at varying levels of detail.
u32 CTerrainSceneNode::getIndex(const s32& PatchX, const s32& PatchZ,
                                const s32& PatchIndex, u32 vX, u32 vZ)
{
	// top border
	if (vZ == 0)
	{
		if (TerrainData.Patches[PatchIndex].Top &&
		    TerrainData.Patches[PatchIndex].CurrentLOD < TerrainData.Patches[PatchIndex].Top->CurrentLOD &&
		    (vX % ( 1 << TerrainData.Patches[PatchIndex].Top->CurrentLOD)) != 0 )
		{
			vX = vX - vX % ( 1 << TerrainData.Patches[PatchIndex].Top->CurrentLOD );
		}
	}
	else
	if ( vZ == (u32)TerrainData.CalcPatchSize )         // bottom border
	{
		if (TerrainData.Patches[PatchIndex].Bottom &&
		    TerrainData.Patches[PatchIndex].CurrentLOD < TerrainData.Patches[PatchIndex].Bottom->CurrentLOD &&
		    (vX % ( 1 << TerrainData.Patches[PatchIndex].Bottom->CurrentLOD)) != 0)
		{
			vX = vX - vX % ( 1 << TerrainData.Patches[PatchIndex].Bottom->CurrentLOD );
		}
	}

	// left border
	if ( vX == 0 )
	{
		if (TerrainData.Patches[PatchIndex].Left &&
		    TerrainData.Patches[PatchIndex].CurrentLOD < TerrainData.Patches[PatchIndex].Left->CurrentLOD &&
		    ( vZ % ( 1 << TerrainData.Patches[PatchIndex].Left->CurrentLOD ) ) != 0)
		{
			vZ = vZ - vZ % ( 1 << TerrainData.Patches[PatchIndex].Left->CurrentLOD );
		}
	}
	else
	if  ( vX == (u32)TerrainData.CalcPatchSize )         // right border
	{
		if (TerrainData.Patches[PatchIndex].Right &&
		    TerrainData.Patches[PatchIndex].CurrentLOD < TerrainData.Patches[PatchIndex].Right->CurrentLOD &&
		    ( vZ %  ( 1 << TerrainData.Patches[PatchIndex].Right->CurrentLOD ) ) != 0)
		{
			vZ = vZ - vZ % ( 1 << TerrainData.Patches[PatchIndex].Right->CurrentLOD );
		}
	}

	if ( vZ >= (u32)TerrainData.PatchSize )
		vZ = TerrainData.CalcPatchSize;

	if ( vX >= (u32)TerrainData.PatchSize )
		vX = TerrainData.CalcPatchSize;

	return (vZ + ((TerrainData.CalcPatchSize) * PatchZ)) * TerrainData.Size +
	       (vX + ((TerrainData.CalcPatchSize) * PatchX));
}

//! calculate smooth normals
void CTerrainSceneNode::calculateNormals ( SMeshBufferLightMap* pMeshBuffer )
{
	s32 count;
	core::vector3df a, b, c, t;
	core::vector3df normal = core::vector3df ( 0.0f, 1.0f, 0.0f );

	for (s32 x=0; x<TerrainData.Size; ++x)
		for (s32 z=0; z<TerrainData.Size; ++z)
		{
			count = 0;
			normal = core::vector3df ( 0.0f, 0.0f, 0.0f );

			// top left
			if (x>0 && z>0)
			{
				a = pMeshBuffer->Vertices[(x-1)*TerrainData.Size+z-1].Pos;
				b = pMeshBuffer->Vertices[(x-1)*TerrainData.Size+z].Pos;
				c = pMeshBuffer->Vertices[x*TerrainData.Size+z].Pos;
				b -= a;
				c -= a;
				t = b.crossProduct ( c );
				t.normalize ( );
				normal += t;

				a = pMeshBuffer->Vertices[(x-1)*TerrainData.Size+z-1].Pos;
				b = pMeshBuffer->Vertices[x*TerrainData.Size+z-1].Pos;
				c = pMeshBuffer->Vertices[x*TerrainData.Size+z].Pos;
				b -= a;
				c -= a;
				t = b.crossProduct ( c );
				t.normalize ( );
				normal += t;

				count += 2;
			}

			// top right
			if (x>0 && z<TerrainData.Size-1)
			{
				a = pMeshBuffer->Vertices[(x-1)*TerrainData.Size+z].Pos;
				b = pMeshBuffer->Vertices[(x-1)*TerrainData.Size+z+1].Pos;
				c = pMeshBuffer->Vertices[x*TerrainData.Size+z+1].Pos;
				b -= a;
				c -= a;
				t = b.crossProduct ( c );
				t.normalize ( );
				normal += t;

				a = pMeshBuffer->Vertices[(x-1)*TerrainData.Size+z].Pos;
				b = pMeshBuffer->Vertices[x*TerrainData.Size+z+1].Pos;
				c = pMeshBuffer->Vertices[x*TerrainData.Size+z].Pos;
				b -= a;
				c -= a;
				t = b.crossProduct ( c );
				t.normalize ( );
				normal += t;

				count += 2;
			}

			// bottom right
			if (x<TerrainData.Size-1 && z<TerrainData.Size-1)
			{
				a = pMeshBuffer->Vertices[x*TerrainData.Size+z+1].Pos;
				b = pMeshBuffer->Vertices[x*TerrainData.Size+z+1].Pos;
				c = pMeshBuffer->Vertices[(x+1)*TerrainData.Size+z+1].Pos;
				b -= a;
				c -= a;
				t = b.crossProduct ( c );
				t.normalize ( );
				normal += t;

				a = pMeshBuffer->Vertices[x*TerrainData.Size+z+1].Pos;
				b = pMeshBuffer->Vertices[(x+1)*TerrainData.Size+z+1].Pos;
				c = pMeshBuffer->Vertices[(x+1)*TerrainData.Size+z].Pos;
				b -= a;
				c -= a;
				t = b.crossProduct ( c );
				t.normalize ( );
				normal += t;

				count += 2;
			}

			// bottom left
			if (x<TerrainData.Size-1 && z>0)
			{
				a = pMeshBuffer->Vertices[x*TerrainData.Size+z-1].Pos;
				b = pMeshBuffer->Vertices[x*TerrainData.Size+z].Pos;
				c = pMeshBuffer->Vertices[(x+1)*TerrainData.Size+z].Pos;
				b -= a;
				c -= a;
				t = b.crossProduct ( c );
				t.normalize ( );
				normal += t;

				a = pMeshBuffer->Vertices[x*TerrainData.Size+z-1].Pos;
				b = pMeshBuffer->Vertices[(x+1)*TerrainData.Size+z].Pos;
				c = pMeshBuffer->Vertices[(x+1)*TerrainData.Size+z-1].Pos;
				b -= a;
				c -= a;
				t = b.crossProduct ( c );
				t.normalize ( );
				normal += t;

				count += 2;
			}

			if ( count != 0 )
			{
				normal /= (f32)count;
				normal.normalize ( );
			}
			else
			{
				normal = core::vector3df ( 0.0f, 1.0f, 0.0f );
			}

			pMeshBuffer->Vertices[x * TerrainData.Size + z].Normal = normal;
		}
}

//! create patches, stuff that needs to only be done once for patches goes here.
void CTerrainSceneNode::createPatches()
{
	TerrainData.PatchCount = (TerrainData.Size - 1) / ( TerrainData.CalcPatchSize );

	if (TerrainData.Patches)
		delete [] TerrainData.Patches;

	TerrainData.Patches = new SPatch[TerrainData.PatchCount * TerrainData.PatchCount];
}

//! used to calculate the internal STerrainData structure both at creation and after scaling/position calls.
void CTerrainSceneNode::calculatePatchData()
{
	// Reset the Terrains Bounding Box for re-calculation
	TerrainData.BoundingBox = core::aabbox3df ( 999999.9f, 999999.9f, 999999.9f, -999999.9f, -999999.9f, -999999.9f );

	for( s32 x = 0; x < TerrainData.PatchCount; ++x )
	{
		for( s32 z = 0; z < TerrainData.PatchCount; ++z )
		{
			s32 index = x * TerrainData.PatchCount + z;
			TerrainData.Patches[index].CurrentLOD = 0;

			// For each patch, calculate the bounding box ( mins and maxes )
			TerrainData.Patches[index].BoundingBox =  core::aabbox3df (999999.9f, 999999.9f, 999999.9f,
			                                                           -999999.9f, -999999.9f, -999999.9f );

			for( s32 xx = x*(TerrainData.CalcPatchSize); xx < ( x + 1 ) * TerrainData.CalcPatchSize; ++xx )
				for( s32 zz = z*(TerrainData.CalcPatchSize); zz < ( z + 1 ) * TerrainData.CalcPatchSize; ++zz )
					TerrainData.Patches[index].BoundingBox.addInternalPoint( RenderBuffer.Vertices[xx * TerrainData.Size + zz].Pos );

			// Reconfigure the bounding box of the terrain as a whole
			TerrainData.BoundingBox.addInternalBox( TerrainData.Patches[index].BoundingBox );

			// get center of Patch
			TerrainData.Patches[index].Center = TerrainData.Patches[index].BoundingBox.getCenter();

			// Assign Neighbors
			// Top
			if( x > 0 )
				TerrainData.Patches[index].Top = &TerrainData.Patches[(x-1) * TerrainData.PatchCount + z];
			else
				TerrainData.Patches[index].Top = 0;

			// Bottom
			if( x < TerrainData.PatchCount - 1 )
				TerrainData.Patches[index].Bottom = &TerrainData.Patches[(x+1) * TerrainData.PatchCount + z];
			else
				TerrainData.Patches[index].Bottom = 0;

			// Left
			if( z > 0 )
				TerrainData.Patches[index].Left = &TerrainData.Patches[x * TerrainData.PatchCount + z - 1];
			else
				TerrainData.Patches[index].Left = 0;

			// Right
			if( z < TerrainData.PatchCount - 1 )
				TerrainData.Patches[index].Right = &TerrainData.Patches[x * TerrainData.PatchCount + z + 1];
			else
				TerrainData.Patches[index].Right = 0;
		}
	}

	// get center of Terrain
	TerrainData.Center = TerrainData.BoundingBox.getCenter();

	// if the default rotation pivot is still being used, update it.
	if( UseDefaultRotationPivot )
	{
		TerrainData.RotationPivot = TerrainData.Center;
	}
}


//! used to calculate or recalculate the distance thresholds
void CTerrainSceneNode::calculateDistanceThresholds(bool scalechanged)
{
	// Initialize Patch Data
	s32 i;

	// Only update the LODDistanceThreshold if it's not manually changed
	if (!OverrideDistanceThreshold)
	{
		if( TerrainData.LODDistanceThreshold )
		{
			delete [] TerrainData.LODDistanceThreshold;
		}

		// Determine new distance threshold for determining what LOD to draw patches at
		TerrainData.LODDistanceThreshold = new f32[TerrainData.MaxLOD];

		for (i=0; i<TerrainData.MaxLOD; ++i)
		{
			TerrainData.LODDistanceThreshold[i] =
			        (TerrainData.PatchSize * TerrainData.PatchSize) *
			        (TerrainData.Scale.X * TerrainData.Scale.Z) *
			        ((i+1+ i / 2) * (i+1+ i / 2));
		}
	}
}

void CTerrainSceneNode::setCurrentLODOfPatches(s32 lod)
{
	for (s32 i=0; i< TerrainData.PatchCount * TerrainData.PatchCount; ++i)
		TerrainData.Patches[i].CurrentLOD = lod;
}

void CTerrainSceneNode::setCurrentLODOfPatches(core::array<s32>& lodarray)
{
	for (s32 i=0; i<TerrainData.PatchCount * TerrainData.PatchCount; ++i)
		TerrainData.Patches[i].CurrentLOD = lodarray[i];
}


} // end namespace scene
} // end namespace engine

