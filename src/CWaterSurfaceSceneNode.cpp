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
 
#include "CWaterSurfaceSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "S3DVertex.h"
#include "SViewFrustrum.h"
#include "ICameraSceneNode.h"
#include "IMeshManipulator.h"
#include "SMesh.h"
#include "os.h"
#include <math.h>
#include "fastmath.h"


namespace engine
{
namespace scene
{



//! constructor
CWaterSurfaceSceneNode::CWaterSurfaceSceneNode(f32 waveHeight, f32 waveSpeed, f32 waveLength,
                                               IMesh* mesh, ISceneNode* parent, ISceneManager* mgr, s32 id,
                                               const core::vector3df& position, const core::vector3df& rotation,
                                               const core::vector3df& scale)
	: CMeshSceneNode(mesh, parent, mgr, id, position, rotation, scale), OriginalMesh(0)
{
	#ifdef _DEBUG
	setDebugName("CWaterSurfaceSceneNode");
	#endif

	WaveSpeed = waveSpeed; //300.0f;
	WaveLength = waveLength; //10.0f;
	WaveHeight = waveHeight; //2.0f;

	// create copy of the mesh
	if (!mesh)
		return;

	IMesh* clone = SceneManager->getMeshManipulator()->createMeshCopy(mesh);
	OriginalMesh = Mesh;
	Mesh = clone;
}



//! destructor
CWaterSurfaceSceneNode::~CWaterSurfaceSceneNode()
{
	if (OriginalMesh)
		OriginalMesh->drop();
}


#include <pspdebug.h>
#define printf pspDebugScreenPrintf

//! frame
void CWaterSurfaceSceneNode::OnPreRender()
{
	int d= os::Timer::getRealTime();
	if (IsVisible)
		SceneManager->registerNodeForRendering(this);

	animateWaterSurface();

	CMeshSceneNode::OnPreRender();
}


void CWaterSurfaceSceneNode::animateWaterSurface()
{
	if (!Mesh)
		return;
	s32 meshBufferCount = Mesh->getMeshBufferCount();
	f32 time = os::Timer::getTime() / WaveSpeed;

	for (s32 b=0; b<meshBufferCount; ++b)
	{
		s32 vtxCnt = Mesh->getMeshBuffer(b)->getVertexCount();

		switch(Mesh->getMeshBuffer(b)->getVertexType())
		{
		case video::EVT_STANDARD:
		{
			video::S3DVertex* v =
			        (video::S3DVertex*)Mesh->getMeshBuffer(b)->getVertices();

			video::S3DVertex* v2 =
			        (video::S3DVertex*)OriginalMesh->getMeshBuffer(b)->getVertices();

			for (s32 i=0; i<vtxCnt; ++i)
			{
				v[i].Pos.Y = v2[i].Pos.Y +
				             ((f32)FastSin(((v2[i].Pos.X/WaveLength) + time)) * WaveHeight) +
				             ((f32)FastCos(((v2[i].Pos.Z/WaveLength) + time)) * WaveHeight);

			}
//printf("water2c %d %d\n", os::Timer::getRealTime() -d, vtxCnt);
		}
			break;
		case video::EVT_2TCOORDS:
		{
			video::S3DVertex2TCoords* v =
			        (video::S3DVertex2TCoords*)Mesh->getMeshBuffer(b)->getVertices();

			video::S3DVertex2TCoords* v2 =
			        (video::S3DVertex2TCoords*)OriginalMesh->getMeshBuffer(b)->getVertices();

			for (s32 i=0; i<vtxCnt; ++i)
			{
				v[i].Pos.Y = v2[i].Pos.Y +
				             ((f32)FastSin(((v2[i].Pos.X/WaveLength) + time)) * WaveHeight) +
				             ((f32)FastCos(((v2[i].Pos.Z/WaveLength) + time)) * WaveHeight);
			}
//printf("water2c %d %d\n", os::Timer::getRealTime() -d, vtxCnt);

		}
			break;
		} // end switch

	} // end for all mesh buffers

	SceneManager->getMeshManipulator()->recalculateNormals(Mesh);
}


} // end namespace scene
} // end namespace engine

