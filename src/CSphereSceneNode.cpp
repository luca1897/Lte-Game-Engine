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
 
// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "enginelicht Engine".
// For conditions of distribution and use, see copyright notice in enginelicht.h

#include "CSphereSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "S3DVertex.h"
#include "os.h"

namespace engine
{
namespace scene
{

//! constructor
CSphereSceneNode::CSphereSceneNode(f32 Radius, s32 polyCount, ISceneNode* parent, ISceneManager* mgr, s32 id,
                                   const core::vector3df& position, const core::vector3df& rotation, const core::vector3df& scale)
	: ISceneNode(parent, mgr, id, position, rotation, scale), Radius(Radius),
	PolyCount(polyCount), Vertices(0), Indices(0)
{
	#ifdef _DEBUG
	setDebugName("CSphereSceneNode");
	#endif

	setSizeAndPolys();
}



//! destructor
CSphereSceneNode::~CSphereSceneNode()
{
	delete [] Vertices;
	delete [] Indices;
}


void CSphereSceneNode::setSizeAndPolys()
{
	// thanks to Alfaz93 who made his code available for enginelicht on which
	// this one is based!

	// we are creating the sphere mesh here.

	if (PolyCount < 2)
		PolyCount = 2;
	else
	if (PolyCount > 181)  // prevent u16 overflow
		PolyCount = 181;

	delete [] Vertices;
	delete [] Indices;

	VertexCount = (PolyCount * PolyCount) + 2;
	Vertices = new video::S3DVertex[VertexCount];

	IndexCount = (PolyCount * PolyCount) * 6;
	Indices = new u16[IndexCount];

	video::SColor clr(100, 255,255,255);

	int i=0;
	int level = 0;

	for (int p1=0; p1<PolyCount-1; ++p1)
	{
		level = p1 * PolyCount;
		int p2;

		for (p2 = 0; p2 < PolyCount - 1; ++p2)
		{
			Indices[i] = level + p2 + PolyCount;
			Indices[++i] = level + p2;
			Indices[++i] = level + p2 + 1;
			++i;
		}

		Indices[i] = level + PolyCount - 1 + PolyCount;
		Indices[++i] = level + PolyCount - 1;
		Indices[++i] = level;
		++i;

		Indices[i] = level + PolyCount - 1 + PolyCount;
		Indices[++i] = level;
		Indices[++i] = level + PolyCount;
		++i;

		for (p2 = 1; p2 <= PolyCount - 1; ++p2)
		{
			Indices[i] = level + p2 - 1 + PolyCount;
			Indices[++i] = level + p2;
			Indices[++i] = level + p2 + PolyCount;
			++i;
		}
	}

	int PolyCountSq = PolyCount * PolyCount;
	int PolyCountSq1 = PolyCountSq + 1;
	int PolyCountSqM1 = (PolyCount - 1) * PolyCount;

	for (int p2 = 0; p2 < PolyCount - 1; ++p2)
	{
		// create triangles which are at the top of the sphere

		Indices[i] = PolyCountSq;
		Indices[++i] = p2 + 1;
		Indices[++i] = p2;
		++i;

		// create triangles which are at the bottom of the sphere

		Indices[i] = PolyCountSqM1 + p2;
		Indices[++i] = PolyCountSqM1 + p2 + 1;
		Indices[++i] = PolyCountSq1;
		++i;
	}

	// create a triangle which is at the top of the sphere

	Indices[i] = PolyCountSq;
	Indices[++i] = 0;
	Indices[++i] = PolyCount - 1;
	++i;

	// create a triangle which is at the bottom of the sphere

	Indices[i] = PolyCountSqM1 + PolyCount - 1;
	Indices[++i] = PolyCountSqM1;
	Indices[++i] = PolyCountSq1;

	// calculate the angle which separates all points in a circle

	float Angle = 2 * core::PI / (f32)PolyCount;
	float sinay;
	float cosay;
	float sinaxz;
	float cosaxz;

	i = 0;
	float axz;

	// we don't start at 0.

	float ay = -Angle / 4;

	for (int y = 0; y < PolyCount; ++y)
	{
		ay += Angle / 2;
		axz = 0;

		for (int xz = 0; xz < PolyCount; ++xz)
		{
			// calculate points position

			axz += Angle;
			sinay = sin(ay) * Radius;
			cosay = cos(ay) * Radius;
			cosaxz = cos(axz);
			sinaxz = sin(axz);

			core::vector3df pos(cosaxz * sinay, cosay, sinaxz * sinay);
			core::vector3df normal(pos);
			normal.normalize();

			Vertices[i] = video::S3DVertex(pos.X, pos.Y, pos.Z,
			                               normal.X, normal.Y, normal.Z,
			                               clr,
			                               asin(normal.X)/core::PI*2 + 0.5f,
			                               acos(normal.Y)/core::PI*2 + 0.5f);

			++i;
		}
	}

	// the vertex at the top of the sphere
	Vertices[i] = video::S3DVertex(0.0f,Radius,0.0f, 1.0f,1.0f,1.0f, clr, 0.5f, 0.5f);

	// the vertex at the bottom of the sphere
	++i;
	Vertices[i] = video::S3DVertex(0.0f,-Radius,0.0f, -1.0f,-1.0f,-1.0f, clr, 0.5f, 0.5f);

	// recalculate bounding box

	Box.reset(Vertices[i].Pos);
	Box.addInternalPoint(Vertices[i-1].Pos);
	Box.addInternalPoint(Radius,0.0f,0.0f);
	Box.addInternalPoint(-Radius,0.0f,0.0f);
	Box.addInternalPoint(0.0f,0.0f,Radius);
	Box.addInternalPoint(0.0f,0.0f,-Radius);
}



//! renders the node.
void CSphereSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	if (VertexCount && IndexCount)
	{
		driver->setMaterial(Material);
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		driver->drawIndexedTriangleList(Vertices, VertexCount, Indices, IndexCount/3);
		if (DebugDataVisible)
		{
			video::SMaterial m;
			m.Lighting = false;
			driver->setMaterial(m);
			driver->draw3DBox(Box, video::SColor(255,255,255,255));
		}
	}
}


video::S3DVertex* CSphereSceneNode::getVertices() {
	return Vertices;
}
u16* CSphereSceneNode::getIndices() {
	return Indices;
}

s32 CSphereSceneNode::getIndexCount()  {
	return IndexCount;
}
s32 CSphereSceneNode::getVertexCount() {
	return VertexCount;
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CSphereSceneNode::getBoundingBox() const
{
	return Box;
}


void CSphereSceneNode::OnPreRender()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(this);

	ISceneNode::OnPreRender();
}


//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hirachy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
video::SMaterial& CSphereSceneNode::getMaterial(s32 i)
{
	return Material;
}


//! returns amount of materials used by this scene node.
s32 CSphereSceneNode::getMaterialCount()
{
	return 1;
}


} // end namespace scene
} // end namespace engine

