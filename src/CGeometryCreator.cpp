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
 
#include "CGeometryCreator.h"
#include "SAnimatedMesh.h"
#include "SMeshBuffer.h"
#include "SMesh.h"
#include "IVideoDriver.h"
#include "CImage.h"
#include <stdio.h>
#include "os.h"

namespace engine
{
namespace scene
{

// creates a hill plane
IAnimatedMesh* CGeometryCreator::createHillPlaneMesh(const core::dimension2d<f32>& tileSize, const core::dimension2d<s32>& tc,
                                                     video::SMaterial* material,     f32 hillHeight, const core::dimension2d<f32>& ch,
                                                     const core::dimension2d<f32>& textureRepeatCount)
{
	core::dimension2d<s32> tileCount = tc;
	tileCount.Height += 1;
	tileCount.Width += 1;

	core::dimension2d<f32> countHills = ch;

	SMeshBuffer* buffer = new SMeshBuffer();
	SMesh* mesh = new SMesh();
	video::S3DVertex vtx;
	vtx.Color.set(255,255,255,255);
	vtx.Normal.set(0,0,0);

	if (countHills.Width < 0.01f) countHills.Width = 1;
	if (countHills.Height < 0.01f) countHills.Height = 1;

	f32 halfX = (tileSize.Width * tileCount.Width) / 2;
	f32 halfY = (tileSize.Height * tileCount.Height) / 2;

	// create vertices

	s32 x = 0;
	s32 y = 0;

	core::dimension2d<f32> tx;
	tx.Width = 1.0f / (tileCount.Width / textureRepeatCount.Width);
	tx.Height = 1.0f / (tileCount.Height / textureRepeatCount.Height);


	for (x=0; x<tileCount.Width; ++x)
		for (y=0; y<tileCount.Height; ++y)
		{
			vtx.Pos.set(tileSize.Width * x - halfX, 0, tileSize.Height * y - halfY);
			vtx.TCoords.set(-(f32)x * tx.Width, (f32)y * tx.Height);

			if (hillHeight)
				vtx.Pos.Y = (f32)(sin(vtx.Pos.X * countHills.Width * engine::core::PI / halfX) *
				                  cos(vtx.Pos.Z * countHills.Height * engine::core::PI / halfY))
				            *hillHeight;

			buffer->Vertices.push_back(vtx);
		}

	// create indices

	for (x=0; x<tileCount.Width-1; ++x)
		for (y=0; y<tileCount.Height-1; ++y)
		{
			s32 current = y*tileCount.Width + x;

			buffer->Indices.push_back(current);
			buffer->Indices.push_back(current + 1);
			buffer->Indices.push_back(current + tileCount.Width);

			buffer->Indices.push_back(current + 1);
			buffer->Indices.push_back(current + 1 + tileCount.Width);
			buffer->Indices.push_back(current + tileCount.Width);
		}

	// recalculate normals
	for (s32 i=0; i<(s32)buffer->Indices.size(); i+=3)
	{
		core::plane3d<f32> p(
		        buffer->Vertices[buffer->Indices[i+0]].Pos,
		        buffer->Vertices[buffer->Indices[i+1]].Pos,
		        buffer->Vertices[buffer->Indices[i+2]].Pos);

		p.Normal.normalize();

		buffer->Vertices[buffer->Indices[i+0]].Normal = p.Normal;
		buffer->Vertices[buffer->Indices[i+1]].Normal = p.Normal;
		buffer->Vertices[buffer->Indices[i+2]].Normal = p.Normal;
	}

	if (material)
		buffer->Material = *material;

	buffer->recalculateBoundingBox();

	SAnimatedMesh* animatedMesh = new SAnimatedMesh();
	mesh->addMeshBuffer(buffer);
	mesh->recalculateBoundingBox();
	animatedMesh->addMesh(mesh);
	animatedMesh->recalculateBoundingBox();

	mesh->drop();
	buffer->drop();

	return animatedMesh;
}



IAnimatedMesh* CGeometryCreator::createTerrainMesh(video::IImage* texture,
                                                   video::IImage* heightmap, const core::dimension2d<f32>& stretchSize,
                                                   f32 maxHeight, video::IVideoDriver* driver,
                                                   const core::dimension2d<s32> maxVtxBlockSize,
                                                   bool debugBorders)
{
	u32 tm = os::Timer::getRealTime()/1000;

	if (!texture || !heightmap)
		return 0;

	video::SMaterial material;
	c8 textureName[64];
	c8 tmp[255];


	// debug border
	s32 borderSkip = debugBorders ? 0 : 1;

	video::S3DVertex vtx;
	vtx.Color.set(255,255,255,255);

	SMesh* mesh = new SMesh();

	core::dimension2d<s32> hMapSize= heightmap->getDimension();
	core::dimension2d<s32> tMapSize= texture->getDimension();
	core::position2d<f32> thRel((f32)tMapSize.Width / (s32)hMapSize.Width, (f32)tMapSize.Height / (s32)hMapSize.Height);
	core::position2d<s32> processed(0,0);

	while (processed.Y<hMapSize.Height)
	{
		while(processed.X<hMapSize.Width)
		{
			core::dimension2d<s32> blockSize = maxVtxBlockSize;
			if (processed.X + blockSize.Width > hMapSize.Width)
				blockSize.Width = hMapSize.Width - processed.X;
			if (processed.Y + blockSize.Height > hMapSize.Height)
				blockSize.Height = hMapSize.Height - processed.Y;

			SMeshBuffer* buffer = new SMeshBuffer();
			s32 x,y;

			// add vertices of vertex block
			for (y=0; y<blockSize.Height; ++y)
				for (x=0; x<blockSize.Width; ++x)
				{
					video::SColor clr = heightmap->getPixel(x+processed.X, y+processed.Y);
					f32 height = ((clr.getRed() + clr.getGreen() + clr.getBlue()) / 3.0f)/255.0f * maxHeight;

					vtx.Pos.set((f32)(x+processed.X) * stretchSize.Width,
					            height, (f32)(y+processed.Y) * stretchSize.Height);

					vtx.TCoords.set((f32)(x+0.5f) / ((f32)blockSize.Width),
					                (f32)(y+0.5f) / ((f32)blockSize.Height));

					buffer->Vertices.push_back(vtx);
				}

			// add indices of vertex block
			for (y=0; y<blockSize.Height-1; ++y)
				for (x=0; x<blockSize.Width-1; ++x)
				{
					s32 c = (y*blockSize.Width) + x;

					buffer->Indices.push_back(c);
					buffer->Indices.push_back(c + blockSize.Width);
					buffer->Indices.push_back(c + 1);

					buffer->Indices.push_back(c + 1);
					buffer->Indices.push_back(c + blockSize.Width);
					buffer->Indices.push_back(c + 1 + blockSize.Width);
				}

			// recalculate normals
			for (s32 i=0; i<(s32)buffer->Indices.size(); i+=3)
			{
				core::plane3d<f32> p(
				        buffer->Vertices[buffer->Indices[i+0]].Pos,
				        buffer->Vertices[buffer->Indices[i+1]].Pos,
				        buffer->Vertices[buffer->Indices[i+2]].Pos);
				p.Normal.normalize();

				buffer->Vertices[buffer->Indices[i+0]].Normal = p.Normal;
				buffer->Vertices[buffer->Indices[i+1]].Normal = p.Normal;
				buffer->Vertices[buffer->Indices[i+2]].Normal = p.Normal;
			}

			if (buffer->Vertices.size())
			{
				// create texture for this block
				video::IImage* img = new video::CImage(texture,
				                                       core::position2d<s32>((s32)(processed.X*thRel.X), (s32)(processed.Y*thRel.Y)),
				                                       core::dimension2d<s32>((s32)(blockSize.Width*thRel.X), (s32)(blockSize.Height*thRel.Y)));

				sprintf(textureName, "terrain%d_%d", tm, mesh->getMeshBufferCount());

				material.Texture1 = driver->addTexture(textureName, img);

				if (material.Texture1)
				{
					sprintf(tmp, "Generated terrain texture (%dx%d): %s",
					        material.Texture1->getSize().Width,
					        material.Texture1->getSize().Height,
					        textureName);
					os::Printer::log(tmp);
				}
				else
					os::Printer::log("Could not create terrain texture.", textureName, ELL_ERROR);

				buffer->Material = material;
				img->drop();
			}

			buffer->recalculateBoundingBox();
			mesh->addMeshBuffer(buffer);
			buffer->drop();

			// keep on processing
			processed.X += maxVtxBlockSize.Width - borderSkip;
		}

		// keep on processing
		processed.X = 0;
		processed.Y += maxVtxBlockSize.Height - borderSkip;
	}

	SAnimatedMesh* animatedMesh = new SAnimatedMesh();
	mesh->recalculateBoundingBox();
	animatedMesh->addMesh(mesh);
	animatedMesh->recalculateBoundingBox();

	mesh->drop();

	return animatedMesh;
}


} // end namespace scene
} // end namespace engine
