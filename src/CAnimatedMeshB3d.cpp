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
 
//B3D file loader by Luke Hoschke, File format by Mark Sibly

#include "CAnimatedMeshB3d.h"
#include "os.h"
#include "IVideoDriver.h"
#include <string.h>
#include <pspdebug.h>
#define printf pspDebugScreenPrintf

namespace engine
{
namespace scene
{


struct B3dChunkHeader
{
	c8 name[4];
	s32 size;
};



//! constructor
CAnimatedMeshB3d::CAnimatedMeshB3d(video::IVideoDriver* driver)
	: Driver(driver)
{
	if (Driver)
		Driver->grab();

	//lastCalculatedFrame = -1;
}



//! destructor
CAnimatedMeshB3d::~CAnimatedMeshB3d()
{
	if (Driver)
		Driver->drop();
}



core::stringc CAnimatedMeshB3d::readString(io::IReadFile* file)
{
	core::stringc newstring;

	while (file->getPos() <= file->getSize() )
	{
		c8 character;
		file->read(&character,sizeof(character));

		if (character==0)
			return newstring;

		newstring.append(character);
	}
	return newstring;
}



core::stringc CAnimatedMeshB3d::stripPathString(core::string<c8> oldstring, bool keepPath)
{
	s32 lastA=oldstring.findLast('/'); // forward slash
	s32 lastB=oldstring.findLast('\\'); // back slash

	if (keepPath==false)
		if (lastA==-1 && lastB==-1) return oldstring;
		else
		if (lastA==-1 && lastB==-1) return core::stringc();


	if (lastA>lastB)
	{
		if (keepPath==false)
			return oldstring.subString(lastA+1,oldstring.size()-(lastA+1));
		else
			return oldstring.subString(0,lastA+1);
	}
	else
	{
		if (keepPath==false)
			return oldstring.subString(lastB+1,oldstring.size()-(lastB+1));
		else
			return oldstring.subString(0,lastB+1);
	}
}



void CAnimatedMeshB3d::readFloats(io::IReadFile* file, f32* vec, u32 count)
{
	file->read(vec, count*sizeof(f32));
	#ifdef __BIG_ENDIAN__
	for (u32 n=0; n<count; ++n)
		vec[n] = OSReadSwapInt32(vec,n*sizeof(f32));
	#endif
}



bool CAnimatedMeshB3d::ReadChunkTEXS(io::IReadFile* file, B3dChunk *B3dStack, s16 &B3dStackSize)
{
	//os::Printer::log("B3d TEXS Chunk"); //for debuging

	while(B3dStack[B3dStackSize].startposition + B3dStack[B3dStackSize].length>file->getPos()) //this chunk repeats
	{
		core::stringc TextureName=readString(file);

		TextureName=stripPathString(file->getFileName(),true) + stripPathString(TextureName,false);

		SB3dTexture B3dTexture;

		B3dTexture.Texture=Driver->getTexture ( TextureName.c_str() );

		file->read(&B3dTexture.Flags, sizeof(s32));
		file->read(&B3dTexture.Blend, sizeof(s32));
		readFloats(file, &B3dTexture.Xpos, 1);
		readFloats(file, &B3dTexture.Ypos, 1);
		readFloats(file, &B3dTexture.Xscale, 1);
		readFloats(file, &B3dTexture.Yscale, 1);
		readFloats(file, &B3dTexture.Angle, 1);

		#ifdef __BIG_ENDIAN__ //Never been tested
		B3dTexture.Flags = OSReadSwapInt32(&B3dTexture.Flags,0);
		B3dTexture.Blend = OSReadSwapInt32(&B3dTexture.Blend,0);
		#endif

		Textures.push_back(B3dTexture);

		/*
		   Flags:
		   1: Color (default)
		   2: Alpha
		   4: Masked
		   8: Mipmapped
		   16: Clamp U
		   32: Clamp V
		   64: Spherical environment map
		   128: Cubic environment map
		   256: Store texture in vram
		   512: Force the use of high color textures
		   65536: texture uses secondary UV values

		   Blend:
		   0: Do not blend
		   1: No blend, or Alpha (alpha when texture loaded with alpha flag - not recommended for multitexturing - see below)
		   2: Multiply (default)
		   3: Add
		   4: Dot3
		   5: Multiply 2

		 */

	}

	--B3dStackSize;

	return true;
}

bool CAnimatedMeshB3d::ReadChunkBRUS(io::IReadFile* file, B3dChunk *B3dStack, s16 &B3dStackSize)
{
	//os::Printer::log("B3d BRUS Chunk"); //for debuging

	s32 n_texs;

	file->read(&n_texs, sizeof(s32));

	#ifdef __BIG_ENDIAN__
	n_texs = OSReadSwapInt32(&n_texs,0);
	#endif

	while(B3dStack[B3dStackSize].startposition + B3dStack[B3dStackSize].length>file->getPos()) //this chunk repeats
	{
		// This is what blitz basic calls a brush, like a Irrlicht Material
		core::stringc MaterialName=readString(file); //Not used but we still need the read it

		//cout << MaterialName.c_str() <<endl;//for debuging

		SB3dMaterial B3dMaterial;

		B3dMaterial.Material=new engine::video::SMaterial();

		B3dMaterial.Textures[0]=0;
		B3dMaterial.Textures[1]=0;

		s32 texture_id[8];
		texture_id[0]=-1;
		texture_id[1]=-1;

		file->read(&B3dMaterial.red, sizeof(B3dMaterial.red));
		file->read(&B3dMaterial.green, sizeof(B3dMaterial.green));
		file->read(&B3dMaterial.blue, sizeof(B3dMaterial.blue));
		file->read(&B3dMaterial.alpha, sizeof(B3dMaterial.alpha));
		file->read(&B3dMaterial.shininess, sizeof(B3dMaterial.shininess));
		file->read(&B3dMaterial.blend, sizeof(B3dMaterial.blend));
		file->read(&B3dMaterial.fx, sizeof(B3dMaterial.fx));

		for (s32 n=0; n<=(n_texs-1); n++)
		{
			file->read(&texture_id[n], sizeof(s32)); //I'm not sure of getting the sizeof an array
			//cout << "Material is using texture id:"<< texture_id[n] <<endl;//for debuging
		}

		#ifdef __BIG_ENDIAN__ //Never been tested
		B3dMaterial.red = OSReadSwapInt32(&B3dMaterial.red,0);
		B3dMaterial.green = OSReadSwapInt32(&B3dMaterial.green,0);
		B3dMaterial.blue = OSReadSwapInt32(&B3dMaterial.blue,0);
		B3dMaterial.alpha = OSReadSwapInt32(&B3dMaterial.alpha,0);
		B3dMaterial.shininess = OSReadSwapInt32(&B3dMaterial.shininess,0);
		B3dMaterial.blend = OSReadSwapInt32(&B3dMaterial.blend,0);
		B3dMaterial.fx = OSReadSwapInt32(&B3dMaterial.fx,0);

		for (s32 n=0; n<=(n_texs-1); n++)
			texture_id[n] = OSReadSwapInt32(&texture_id[n],0);
		#endif

		if (texture_id[0]!=-1)
			B3dMaterial.Textures[0]=&Textures[texture_id[0]];
		if (texture_id[1]!=-1)
			B3dMaterial.Textures[1]=&Textures[texture_id[1]];

		if (B3dMaterial.Textures[0]!=0)
			B3dMaterial.Material->Texture1 = B3dMaterial.Textures[0]->Texture;
		if (B3dMaterial.Textures[1]!=0)
			B3dMaterial.Material->Texture2 = B3dMaterial.Textures[1]->Texture;

		//the other textures are skipped, irrlicht I think can only have 2 Textures per Material

		//Hacky code to convert blitz fx to irrlicht...
		if (B3dMaterial.Textures[1])
		{
			if (B3dMaterial.alpha==1)
			{
				if (B3dMaterial.Textures[1]->Blend &5)
				{
					//B3dMaterial.Material->MaterialType = video::EMT_LIGHTMAP;
					B3dMaterial.Material->MaterialType = video::EMT_LIGHTMAP_M2 ;
				}
				else
				{
					B3dMaterial.Material->MaterialType = video::EMT_LIGHTMAP;
				}
			}
			else
			{
				//B3dMaterial.Material->MaterialType = video::EMT_LIGHTMAP;
				//B3dMaterial.Material->MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
				//B3dMaterial.Material->MaterialTypeParam=B3dMaterial.alpha;
				B3dMaterial.Material->MaterialType = video::EMT_TRANSPARENT_VERTEX_ALPHA;
			}
		}
		else if (B3dMaterial.Textures[0])
		{
			if (B3dMaterial.Textures[0]->Flags &2)         //Alpha mapped
			{
				B3dMaterial.Material->MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
			}
			else if (B3dMaterial.alpha==1)
			{
				B3dMaterial.Material->MaterialType = video::EMT_SOLID;
			}
			else
			{
				//B3dMaterial.Material->MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
				//B3dMaterial.Material->MaterialTypeParam=B3dMaterial.alpha;
				B3dMaterial.Material->MaterialType = video::EMT_TRANSPARENT_VERTEX_ALPHA;
			}
			//Material->MaterialType =
			//EMT_TRANSPARENT_ALPHA_CHANNEL
		}
		else
		{
			if (B3dMaterial.alpha==1)
			{
				B3dMaterial.Material->MaterialType = video::EMT_SOLID;
			}
			else
			{
				B3dMaterial.Material->MaterialType = video::EMT_TRANSPARENT_VERTEX_ALPHA;
			}
		}

		//Material fx...

		if (B3dMaterial.fx &1) //full-bright
		{
			//B3dMaterial.Material->AmbientColor = video::SColorf(1, 1, 1, 1).toSColor ();
			B3dMaterial.Material->AmbientColor = video::SColorf(1, 1, 1, 1).toSColor ();    //Blitz Basic Ambient colour is 127,127,127
			B3dMaterial.Material->Lighting = false;
		}
		else
		{
			B3dMaterial.Material->AmbientColor = video::SColorf(B3dMaterial.red, B3dMaterial.green, B3dMaterial.blue, B3dMaterial.alpha).toSColor ();
		}

		if (B3dMaterial.fx &4)  //flatshaded
			B3dMaterial.Material->GouraudShading=false;

		if (B3dMaterial.fx &16)  //disable backface culling
			B3dMaterial.Material->BackfaceCulling=false;


		B3dMaterial.Material->DiffuseColor = video::SColorf(B3dMaterial.red, B3dMaterial.green, B3dMaterial.blue, B3dMaterial.alpha).toSColor ();
		//B3dMaterial.Material->EmissiveColor = video::SColorf(0, 0, 0, 0).toSColor ();
		//B3dMaterial.Material->SpecularColor = video::SColorf(0, 0, 0, 0).toSColor (); //?
		//B3dMaterial.Material->Shininess = B3dMaterial.shininess;

		Materials.push_back(B3dMaterial);
	}

	--B3dStackSize;

	return true;
}



bool CAnimatedMeshB3d::ReadChunkMESH(io::IReadFile* file, B3dChunk *B3dStack, s16 &B3dStackSize, SB3dNode *InNode)
{
	//os::Printer::log("B3d MESH Chunk"); //for debuging

	s32 Vertices_Start=Vertices.size(); //B3Ds have Vertex ID's local within the mesh I don't want this

	s32 brush_id;

	file->read(&brush_id, sizeof(brush_id));

	#ifdef __BIG_ENDIAN__
	brush_id = OSReadSwapInt32(&brush_id,0);
	#endif

	while(B3dStack[B3dStackSize].startposition + B3dStack[B3dStackSize].length>file->getPos()) //this chunk repeats
	{
		//cout << "reading chunks in mesh" << endl;

		++B3dStackSize;

		if (B3dStackSize>255) //stacks should not get this big, should they?
		{
			os::Printer::log("Stack overflow. Loading failed", file->getFileName(), ELL_ERROR);
			return false;
		}

		B3dChunkHeader header;
		file->read(&header, sizeof(header));

		#ifdef __BIG_ENDIAN__
		header.size = OSReadSwapInt32(&header.size,0);
		#endif

		B3dStack[B3dStackSize].name[0]=header.name[0];
		B3dStack[B3dStackSize].name[1]=header.name[1]; //Not sure of an easier way
		B3dStack[B3dStackSize].name[2]=header.name[2];
		B3dStack[B3dStackSize].name[3]=header.name[3];

		B3dStack[B3dStackSize].length=header.size+8;

		B3dStack[B3dStackSize].startposition=file->getPos()-8;

		bool read=false;

		if ( strncmp( B3dStack[B3dStackSize].name, "VRTS", 4 ) == 0 )
		{
			read=true;
			if(ReadChunkVRTS(file, B3dStack, B3dStackSize,InNode,0, Vertices_Start)==false)
				return false;
		}
		else if ( strncmp( B3dStack[B3dStackSize].name, "TRIS", 4 ) == 0 )
		{
			read=true;

			SB3DMeshBuffer *MeshBuffer = new SB3DMeshBuffer();

			if (brush_id!=-1)
				MeshBuffer->Material=(*Materials[brush_id].Material);

			if(ReadChunkTRIS(file, B3dStack, B3dStackSize,InNode,MeshBuffer, Vertices_Start)==false)
				return false;

			if (MeshBuffer->Material.Lighting != false)
			{
				//I don't have a pointer to MeshManipulator here
				//MeshManipulator->recalculateNormals(MeshBuffer); //Any time you would not want this? Other than loading speed loss

				//MeshBuffer->Material.NormalizeNormals=true;
			}

			//MeshBuffer->recalculateBoundingBox();

			Buffers.push_back(MeshBuffer);
		}

		if (read==false)
		{
			os::Printer::log("Mesh Unknown chunk - skipping");
			//cout << "Unknown chunk size:" << B3dStack[B3dStackSize].remaining_length << endl;
			//unknown chunk

			//file->seek(B3dStack[B3dStackSize].remaining_length-8,true);


			file->seek( (B3dStack[B3dStackSize].startposition + B3dStack[B3dStackSize].length), false);


			--B3dStackSize;
		}
	}

	--B3dStackSize;

	return true;
}

/*
   VRTS:
   int flags                   ;1=normal values present, 2=rgba values present
   int tex_coord_sets          ;texture coords per vertex (eg: 1 for simple U/V) max=8
   int tex_coord_set_size      ;components per set (eg: 2 for simple U/V) max=4
   {
   float x,y,z                 ;always present
   float nx,ny,nz              ;vertex normal: present if (flags&1)
   float red,green,blue,alpha  ;vertex color: present if (flags&2)
   float tex_coords[tex_coord_sets][tex_coord_set_size]	;tex coords
   }
 */
bool CAnimatedMeshB3d::ReadChunkVRTS(io::IReadFile* file, B3dChunk *B3dStack, s16 &B3dStackSize, SB3dNode *InNode, SB3DMeshBuffer *MeshBuffer,s32 Vertices_Start)
{
	//os::Printer::log("B3d VRTS Chunk"); //for debuging
	//cout << "B3d VRTS Chunk size:" << B3dStack[B3dStackSize].remaining_length << endl;

	s32 flags, tex_coord_sets, tex_coord_set_size;

	file->read(&flags, sizeof(flags));
	file->read(&tex_coord_sets, sizeof(tex_coord_sets));
	file->read(&tex_coord_set_size, sizeof(tex_coord_set_size));

	#ifdef __BIG_ENDIAN__
	flags = OSReadSwapInt32(&flags,0);
	tex_coord_sets = OSReadSwapInt32(&tex_coord_sets,0);
	tex_coord_set_size = OSReadSwapInt32(&tex_coord_set_size,0);
	#endif

	if (tex_coord_sets>=3 || tex_coord_set_size>=4) //Something is wrong
	{
		os::Printer::log("tex_coord_sets or tex_coord_set_size too big", file->getFileName(), ELL_ERROR);
		return false;
	}

	//cout << "tex_coord_sets:" << tex_coord_sets << endl;
	//cout << "tex_coord_set_size:" << tex_coord_set_size << endl;

	while(B3dStack[B3dStackSize].startposition + B3dStack[B3dStackSize].length>file->getPos()) //this chunk repeats
	{
		f32 x,y,z;
		f32 nx=0;
		f32 ny=0;
		f32 nz=0;
		f32 red=1;
		f32 green=1;
		f32 blue=1;
		f32 alpha=1;
		f32 tex_coords[3][4];

		file->read(&x, sizeof(x));
		file->read(&y, sizeof(y));
		file->read(&z, sizeof(z));

		if (flags&1)
		{
			file->read(&nx, sizeof(nx));
			file->read(&ny, sizeof(ny));
			file->read(&nz, sizeof(nz));
		}

		if (flags&2)
		{
			file->read(&red, sizeof(red));
			file->read(&green, sizeof(green));
			file->read(&blue, sizeof(blue));
			file->read(&alpha, sizeof(alpha));
		}

		for (s32 i=0; i<tex_coord_sets; ++i)
			for (s32 j=0; j<tex_coord_set_size; ++j)
				file->read(&tex_coords[i][j], sizeof(f32));

		#ifdef __BIG_ENDIAN__ //Never been tested
		x = OSReadSwapInt32(&x,0);
		y = OSReadSwapInt32(&y,0);
		z = OSReadSwapInt32(&z,0);

		if (flags&1)
		{
			nx = OSReadSwapInt32(&nx,0);
			ny = OSReadSwapInt32(&ny,0);
			nz = OSReadSwapInt32(&nz,0);
		}

		if (flags&2)
		{
			red = OSReadSwapInt32(&red,0);
			green = OSReadSwapInt32(&green,0);
			blue = OSReadSwapInt32(&blue,0);
			alpha = OSReadSwapInt32(&alpha,0);
		}

		for (s32 i=0; i<=tex_coord_sets-1; i++)
			for (s32 j=0; j<=tex_coord_set_size-1; j++)
				tex_coords[i][j] = OSReadSwapInt32(&tex_coords[i][j],0);
		#endif

		f32 tu=0;
		f32 tv=0;

		if (tex_coord_sets>=1 && tex_coord_set_size>=2)
		{
			tu=tex_coords[0][0];
			tv=tex_coords[0][1];
		}

		f32 tu2=0;
		f32 tv2=0;

		if (tex_coord_sets>=2 && tex_coord_set_size>=2)
		{
			tu2=tex_coords[1][0];
			tv2=tex_coords[1][1];
		}

		video::S3DVertex2TCoords *Vertex=new video::S3DVertex2TCoords
		                                  (x, y, z, video::SColorf(red, green, blue, alpha).toSColor(), tu, tv, tu2, tv2);

		Vertex->Normal=core::vector3df(nx, ny, nz);

		core::matrix4 VertexMatrix;
		VertexMatrix.setTranslation(Vertex->Pos);

		VertexMatrix=InNode->GlobalMatrix*VertexMatrix;
		Vertices_GlobalMatrix.push_back(VertexMatrix);

		Vertex->Pos=VertexMatrix.getTranslation();
		Vertices.push_back(Vertex);

		Vertices_Moved.push_back(false);

		AnimatedVertices_VertexID.push_back(-1);
		AnimatedVertices_MeshBuffer.push_back(0);
	}

	--B3dStackSize;

	return true;
}



bool CAnimatedMeshB3d::ReadChunkTRIS(io::IReadFile* file, B3dChunk *B3dStack, s16 &B3dStackSize, SB3dNode *InNode, SB3DMeshBuffer *MeshBuffer,s32 Vertices_Start)
{

	//os::Printer::log("B3d TRIS Chunk"); //for debuging
	//cout << "B3d TRIS Chunk size:" << B3dStack[B3dStackSize].remaining_length << endl;

	s32 triangle_brush_id; //irrlicht can't have different brushes for each triangle (I'm using a workaround)

	file->read(&triangle_brush_id, sizeof(triangle_brush_id));

	#ifdef __BIG_ENDIAN__
	triangle_brush_id = OSReadSwapInt32(&triangle_brush_id,0);
	#endif

	SB3dMaterial *B3dMaterial;

	if (triangle_brush_id!=-1)
		B3dMaterial=&Materials[triangle_brush_id];
	else
		B3dMaterial=0;

	if (B3dMaterial)
		MeshBuffer->Material=(*B3dMaterial->Material);

	while(B3dStack[B3dStackSize].startposition + B3dStack[B3dStackSize].length>file->getPos()) //this chunk repeats
	{
		s32 vertex_id[3];

		file->read(&vertex_id[0], sizeof(s32));
		file->read(&vertex_id[1], sizeof(s32));
		file->read(&vertex_id[2], sizeof(s32));

		#ifdef __BIG_ENDIAN__ //Never been tested
		vertex_id[0] = OSReadSwapInt32(&vertex_id[0],0);
		vertex_id[1] = OSReadSwapInt32(&vertex_id[1],0);
		vertex_id[2] = OSReadSwapInt32(&vertex_id[2],0);
		#endif


		vertex_id[0]+=Vertices_Start;
		vertex_id[1]+=Vertices_Start;
		vertex_id[2]+=Vertices_Start;

		for(s32 i=0; i<3; i++)
			if (AnimatedVertices_VertexID[ vertex_id[i] ]==-1)
			{
				MeshBuffer->Vertices.push_back(*Vertices[vertex_id[i]] );
				AnimatedVertices_VertexID[ vertex_id[i] ]=MeshBuffer->Vertices.size()-1;
				AnimatedVertices_MeshBuffer[ vertex_id[i] ]=MeshBuffer;

				//Apply Material
				engine::video::S3DVertex2TCoords *Vertex=&MeshBuffer->Vertices[MeshBuffer->Vertices.size()-1];

				Vertex->Color.setAlpha( (s32)(B3dMaterial->alpha*255.0) );

			}


		MeshBuffer->Indices.push_back( AnimatedVertices_VertexID[ vertex_id[0] ] );
		MeshBuffer->Indices.push_back( AnimatedVertices_VertexID[ vertex_id[1] ] );
		MeshBuffer->Indices.push_back( AnimatedVertices_VertexID[ vertex_id[2] ] );
	}

	--B3dStackSize;

	return true;
}



bool CAnimatedMeshB3d::ReadChunkNODE(io::IReadFile* file, B3dChunk *B3dStack, s16 &B3dStackSize, SB3dNode *InNode)
{
	core::stringc NodeName=readString(file);
	//cout << "NODE name:" << NodeName.c_str() <<endl; //for debuging
	//os::Printer::log(NodeName.c_str());

	f32 position[3];
	f32 scale[3];
	f32 rotation[4];

	s32 n;

	for (n=0; n<=2; n++)
		file->read(&position[n], sizeof(f32));

	for (n=0; n<=2; n++)
		file->read(&scale[n], sizeof(f32));

	for (n=0; n<=3; n++)
		file->read(&rotation[n], sizeof(f32));

	#ifdef __BIG_ENDIAN__ //Never been tested
	for (n=0; n<=2; n++)
		position[n] = OSReadSwapInt32(&position[n],0);

	for (n=0; n<=2; n++)
		scale[n] = OSReadSwapInt32(&scale[n],0);

	for (n=0; n<=4; n++)
		rotation[n] = OSReadSwapInt32(&rotation[n],0);
	#endif

	SB3dNode *Node=new SB3dNode();

	Node->Name=NodeName;

	Node->AddMatrix=0;

	Node->position=core::vector3df(position[0],position[1],position[2]);

	Node->scale=core::vector3df(scale[0],scale[1],scale[2]);

	Node->rotation=core::quaternion(rotation[1],rotation[2],rotation[3],rotation[0]); //meant to be in this order

	engine::core::matrix4 positionMatrix;
	positionMatrix.setTranslation(Node->position);

	engine::core::matrix4 scaleMatrix;
	scaleMatrix.setScale(Node->scale);

	engine::core::matrix4 rotationMatrix=Node->rotation.getMatrix();

	Node->LocalMatrix=positionMatrix*rotationMatrix*scaleMatrix;

	if (InNode==0)
	{
		Node->GlobalMatrix=Node->LocalMatrix;
		BaseNodes.push_back(Node);
	}
	else
	{
		Node->GlobalMatrix=InNode->GlobalMatrix*Node->LocalMatrix;
	}

	Node->GlobalInversedMatrix=Node->GlobalMatrix;
	Node->GlobalInversedMatrix.makeInverse(); //slow

	Nodes.push_back(Node);


	if (InNode!=0)
		InNode->Nodes.push_back(Node);

	while(B3dStack[B3dStackSize].startposition + B3dStack[B3dStackSize].length>file->getPos()) //this chunk repeats
	{
		//cout << "reading chunks in node" << endl;

		++B3dStackSize;

		if (B3dStackSize>255) //stacks should not get this big, should they?
		{
			os::Printer::log("Stack overflow. Loading failed", file->getFileName(), ELL_ERROR);
			return false;
		}

		B3dChunkHeader header;
		file->read(&header, sizeof(header));

		#ifdef __BIG_ENDIAN__ //Never been tested
		header.size = OSReadSwapInt32(&header.size,0);
		#endif

		B3dStack[B3dStackSize].name[0]=header.name[0];
		B3dStack[B3dStackSize].name[1]=header.name[1]; //Not sure of an easier way
		B3dStack[B3dStackSize].name[2]=header.name[2];
		B3dStack[B3dStackSize].name[3]=header.name[3];

		B3dStack[B3dStackSize].length=header.size+8;

		B3dStack[B3dStackSize].startposition=file->getPos()-8;

		bool read=false;

		if ( strncmp( B3dStack[B3dStackSize].name, "NODE", 4 ) == 0 )
		{
			read=true;
			if(ReadChunkNODE(file, B3dStack, B3dStackSize,Node)==false)
				return false;
		}
		else if ( strncmp( B3dStack[B3dStackSize].name, "MESH", 4 ) == 0 )
		{
			read=true;
			if(ReadChunkMESH(file, B3dStack, B3dStackSize,Node)==false)
				return false;
		}
		else if ( strncmp( B3dStack[B3dStackSize].name, "ANIM", 4 ) == 0 )
		{
			read=true;
			if(ReadChunkANIM(file, B3dStack, B3dStackSize,Node)==false)
				return false;
		}
		else if ( strncmp( B3dStack[B3dStackSize].name, "BONE", 4 ) == 0 )
		{
			read=true;
			if(ReadChunkBONE(file, B3dStack, B3dStackSize,Node)==false)
				return false;
		}
		else if ( strncmp( B3dStack[B3dStackSize].name, "KEYS", 4 ) == 0 )
		{
			read=true;
			if(ReadChunkKEYS(file, B3dStack, B3dStackSize,Node)==false)
				return false;
		}

		if (read==false)
		{
			os::Printer::log("Node Unknown chunk - skipping");
			//cout << "Unknown chunk size:" << B3dStack[B3dStackSize].remaining_length << endl;
			//unknown chunk

			//datalength+=B3dStack[B3dStackSize].remaining_length;		//jump to end of chunk

			file->seek( (B3dStack[B3dStackSize].startposition + B3dStack[B3dStackSize].length), false);

			--B3dStackSize;
		}
	}

	--B3dStackSize;

	return true;
}



bool CAnimatedMeshB3d::ReadChunkBONE(io::IReadFile* file, B3dChunk *B3dStack, s16 &B3dStackSize, SB3dNode *InNode)
{
	//os::Printer::log("B3d BONE Chunk"); //for debuging
	//cout << "B3d BONE Chunk size:" << B3dStack[B3dStackSize].remaining_length << endl;

	if (B3dStack[B3dStackSize].length>8)
	{
		while(B3dStack[B3dStackSize].startposition + B3dStack[B3dStackSize].length>file->getPos()) //this chunk repeats
		{
			SB3dBone Bone;

			file->read(&Bone.vertex_id, sizeof(Bone.vertex_id));
			file->read(&Bone.weight, sizeof(Bone.weight));

			#ifdef __BIG_ENDIAN__ //Never been tested
			Bone.vertex_id = OSReadSwapInt32(&Bone.vertex_id,0);
			Bone.weight = OSReadSwapInt32(&Bone.weight,0);
			#endif

			InNode->Bones.push_back(Bone);
		}
	}

	--B3dStackSize;
	return true;
}



bool CAnimatedMeshB3d::ReadChunkKEYS(io::IReadFile* file, B3dChunk *B3dStack, s16 &B3dStackSize, SB3dNode *InNode)
{
	//os::Printer::log("B3d KEYS Chunk"); //for debuging
	//cout << "B3d KEYS Chunk size:" << B3dStack[B3dStackSize].remaining_length << endl;

	s32 flags;
	file->read(&flags, sizeof(flags));
	#ifdef __BIG_ENDIAN__ //Never been tested
	flags = OSReadSwapInt32(&flags,0);
	#endif

	while(B3dStack[B3dStackSize].startposition + B3dStack[B3dStackSize].length>file->getPos()) //this chunk repeats
	{
		SB3dKey Key;

		Key.flags=flags;

		f32 position[3];
		f32 scale[3];
		f32 rotation[4];

		file->read(&Key.frame, sizeof(Key.frame));

		if (flags&1)
			readFloats(file, position, 3);

		if (flags&2)
			readFloats(file, scale, 3);

		if (flags&4)
			readFloats(file, rotation, 4);

		#ifdef __BIG_ENDIAN__ //Never been tested
		Key.frame = OSReadSwapInt32(&Key.frame,0);
		#endif

		Key.frame*=100;

		Key.position=core::vector3df(position[0],position[1],position[2]);

		Key.scale=core::vector3df(scale[0],scale[1],scale[2]);

		Key.rotation=core::quaternion(rotation[1],rotation[2],rotation[3],rotation[0]); //meant to be in this order

		InNode->Keys.push_back(Key);
	}

	--B3dStackSize;
	return true;
}



bool CAnimatedMeshB3d::ReadChunkANIM(io::IReadFile* file, B3dChunk *B3dStack, s16 &B3dStackSize, SB3dNode *InNode)
{
	//os::Printer::log("B3d ANIM Chunk"); //for debuging
	//cout << "B3d ANIM Chunk size:" << B3dStack[B3dStackSize].remaining_length << endl;

	file->read(&AnimFlags, sizeof(s32));
	file->read(&AnimFrames, sizeof(s32));
	readFloats(file, &AnimFPS, 1);

	#ifdef __BIG_ENDIAN__ //Never been tested
	AnimFlags = OSReadSwapInt32(&AnimFlags,0);
	AnimFrames = OSReadSwapInt32(&AnimFrames,0);
	#endif

	AnimFrames*=100;

	totalTime=(f32)AnimFrames;
	HasAnimation=1;
	lastCalculatedFrame=-1;

	--B3dStackSize;
	return true;
}



bool CAnimatedMeshB3d::loadFile(io::IReadFile* file)
{
	//os::Printer::log("loading b3d...");

	if (!file)
		return false;


	totalTime=0;
	HasAnimation=0;
	lastCalculatedFrame=-1;

	AnimFlags=0;
	AnimFrames=1; //how many frames in anim
	AnimFPS=0.0f;

	B3dChunkHeader header;

	file->read(&header, sizeof(header));

	#ifdef __BIG_ENDIAN__
	header.size = OSReadSwapInt32(&header.size,0);
	#endif

	if ( strncmp( header.name, "BB3D", 4 ) != 0 )
	{

		os::Printer::log("File is not a b3d file. Loading failed", file->getFileName(), ELL_ERROR);
		return false;
	}

	//header Chunk size here

	//Get file version

	B3dChunk B3dStack[255];         //should have used an irrlicht array, can someone change it

	s16 B3dStackSize=0; //starting at 0

	B3dStack[0].name[0]=header.name[0];
	B3dStack[0].name[1]=header.name[1]; //Not sure of an easier way
	B3dStack[0].name[2]=header.name[2];
	B3dStack[0].name[3]=header.name[3];

	B3dStack[B3dStackSize].startposition=file->getPos()-8;

	B3dStack[B3dStackSize].length=header.size+8;

	u32 FileVersion;

	file->read(&FileVersion, sizeof(FileVersion));
	#ifdef __BIG_ENDIAN__
	FileVersion = OSReadSwapInt32(&FileVersion,0);
	#endif

	while (B3dStack[B3dStackSize].startposition + B3dStack[B3dStackSize].length>file->getPos())
	{
		++B3dStackSize;

		if (B3dStackSize>255) //stacks should not get this big, should they?
		{
			os::Printer::log("Stack overflow. Loading failed", file->getFileName(), ELL_ERROR);
			return false;
		}

		file->read(&header, sizeof(header));

		#ifdef __BIG_ENDIAN__
		header.size = OSReadSwapInt32(&header.size,0);
		#endif

		B3dStack[B3dStackSize].name[0]=header.name[0];
		B3dStack[B3dStackSize].name[1]=header.name[1]; //Not sure of an easier way
		B3dStack[B3dStackSize].name[2]=header.name[2];
		B3dStack[B3dStackSize].name[3]=header.name[3];

		B3dStack[B3dStackSize].startposition=file->getPos()-8;

		B3dStack[B3dStackSize].length=header.size+8;

		bool read=false;

		if ( strncmp( B3dStack[B3dStackSize].name, "TEXS", 4 ) == 0 )
		{
			read=true;
			if(ReadChunkTEXS(file, B3dStack, B3dStackSize)==false)
				return false;
		}
		else if ( strncmp( B3dStack[B3dStackSize].name, "BRUS", 4 ) == 0 )
		{
			read=true;
			if(ReadChunkBRUS(file, B3dStack, B3dStackSize)==false)
				return false;
		}
		else if ( strncmp( B3dStack[B3dStackSize].name, "NODE", 4 ) == 0 )
		{
			read=true;
			if(ReadChunkNODE(file, B3dStack, B3dStackSize,(SB3dNode*)0)==false)
				return false;
		}

		if (read==false)
		{
			os::Printer::log("Base Unknown chunk - skipping");
			//cout << "Unknown chunk size:" << B3dStack[B3dStackSize].remaining_length << endl;
			//unknown chunk

			file->seek( (B3dStack[B3dStackSize].startposition + B3dStack[B3dStackSize].length), false);

			--B3dStackSize;
		}
	}

	if (!Vertices.empty())
		BoundingBox.reset(0,0,0);

	for (s32 i=0; i<(s32)Vertices.size(); ++i)
		BoundingBox.addInternalPoint(Vertices[i]->Pos);

	for (int x = 0; x < AnimatedVertices_MeshBuffer.size(); x++)
	{

		SB3DMeshBuffer* buff = AnimatedVertices_MeshBuffer[x];



		buff->getVertexArray()->reallocate ( buff->getVertexArray()->size() );
		buff->getIndexArray()->reallocate ( buff->getIndexArray()->size() );


	}

	for (int x = 0; x < Buffers.size(); x++)
	{
		SB3DMeshBuffer* buff = Buffers[x];


		buff->getVertexArray()->reallocate ( buff->getVertexArray()->size() );
		buff->getIndexArray()->reallocate ( buff->getIndexArray()->size() );

	}

	return true;
}



//! returns the amount of frames in milliseconds. If the amount is 1, it is a static (=non animated) mesh.
s32 CAnimatedMeshB3d::getFrameCount()
{
	return AnimFrames;
}



//! returns the animated mesh based on a detail level. 0 is the lowest, 255 the highest detail. Note, that some Meshes will ignore the detail level.
IMesh* CAnimatedMeshB3d::getMesh(s32 frame, s32 detailLevel, s32 startFrameLoop, s32 endFrameLoop)
{


	animate(frame,startFrameLoop, endFrameLoop);



	for (int x = 0; x < getMeshBufferCount(); x++)
	{
		Buffers[x]->recalculateBoundingBox();

		if (x == 0)
		{
			BoundingBox.reset(Buffers[x]->getBoundingBox());
		}
		else
			BoundingBox.addInternalBox(Buffers[x]->getBoundingBox());
	}


	return this;
}



//! Returns a pointer to a transformation matrix of a part of the
//! mesh based on a frame time.
core::matrix4* CAnimatedMeshB3d::getMatrixOfJoint(s32 jointNumber, s32 frame)
{
	if (!HasAnimation || jointNumber < 0 || jointNumber >= (s32)Nodes.size())
		return 0;

	animate(frame,0,0);

	return &Nodes[jointNumber]->GlobalAnimatedMatrix;
}



core::matrix4* CAnimatedMeshB3d::getMatrixOfJointUnanimated(s32 jointNumber)
{
	if (!HasAnimation || jointNumber < 0 || jointNumber >= (s32)Nodes.size())
		return 0;

	return &Nodes[jointNumber]->GlobalMatrix;
}



void CAnimatedMeshB3d::AddMatrixToJoint(s32 jointNumber, core::matrix4* matrix) //Does not work yet, causes crashes
{
	if (!HasAnimation || jointNumber < 0 || jointNumber >= (s32)Nodes.size())
		return;

	Nodes[jointNumber]->AddMatrix=matrix;
}



//! Gets joint count.
s32 CAnimatedMeshB3d::getJointCount() const
{
	return Nodes.size();
}



//! Gets the name of a joint.
const c8* CAnimatedMeshB3d::getJointName(s32 number) const
{
	if (number < 0 || number >= (s32)Nodes.size())
		return 0;
	return Nodes[number]->Name.c_str();
}



//! Gets a joint number from its name
s32 CAnimatedMeshB3d::getJointNumber(const c8* name) const
{
	for (s32 i=0; i<(s32)Nodes.size(); ++i)
		if (Nodes[i]->Name == name)
			return i;

	return -1;
}



void CAnimatedMeshB3d::animateNode(f32 frame,f32 startFrame, f32 endFrame,SB3dNode *InNode,SB3dNode *ParentNode)
{
	// Get animated matrix...

	core::matrix4 positionMatrix;
	positionMatrix.setTranslation(InNode->Animatedposition);

	core::matrix4 scaleMatrix;
	scaleMatrix.setScale(InNode->Animatedscale);

	core::matrix4 rotationMatrix;
	rotationMatrix=InNode->Animatedrotation.getMatrix();

	InNode->LocalAnimatedMatrix=positionMatrix * rotationMatrix * scaleMatrix;

	if (InNode->AddMatrix!=0)
		InNode->LocalAnimatedMatrix*= (*InNode->AddMatrix);

	if (ParentNode==0)
	{
		InNode->GlobalAnimatedMatrix=InNode->LocalAnimatedMatrix;
	}
	else
	{
		InNode->GlobalAnimatedMatrix=ParentNode->GlobalAnimatedMatrix * InNode->LocalAnimatedMatrix;

		//Driver->draw3DLine  (  ParentNode->GlobalAnimatedMatrix.getTranslation(), InNode->GlobalAnimatedMatrix.getTranslation(),  video::SColor(255, 255, 255, 255) ) ;
	}

	core::matrix4 VerticesMatrixMove= InNode->GlobalAnimatedMatrix * InNode->GlobalInversedMatrix ;

	for (s32 i=0; i<(s32)InNode->Bones.size(); ++i)
	{
		u16 VertexID = AnimatedVertices_VertexID[ InNode->Bones[i].vertex_id ];

		SB3DMeshBuffer *MeshBuffer=AnimatedVertices_MeshBuffer[ InNode->Bones[i].vertex_id ];

		video::S3DVertex2TCoords *Vertex=&MeshBuffer->Vertices[VertexID];

		core::matrix4 VertexMatrixMove=VerticesMatrixMove;

		VertexMatrixMove*=Vertices_GlobalMatrix[ InNode->Bones[i].vertex_id ];

		core::vector3df GlobalAnimatedVertexVector=VertexMatrixMove.getTranslation();

		if (Vertices_Moved[InNode->Bones[i].vertex_id]==false)
		{
			Vertices_Moved[InNode->Bones[i].vertex_id]=true;

			Vertex->Pos.X=GlobalAnimatedVertexVector.X*InNode->Bones[i].weight;
			Vertex->Pos.Y=GlobalAnimatedVertexVector.Y*InNode->Bones[i].weight;
			Vertex->Pos.Z=GlobalAnimatedVertexVector.Z*InNode->Bones[i].weight;
		}
		else
		{
			Vertex->Pos.X=Vertex->Pos.X+ GlobalAnimatedVertexVector.X*InNode->Bones[i].weight;
			Vertex->Pos.Y=Vertex->Pos.Y+ GlobalAnimatedVertexVector.Y*InNode->Bones[i].weight;
			Vertex->Pos.Z=Vertex->Pos.Z+ GlobalAnimatedVertexVector.Z*InNode->Bones[i].weight;
		}
		//MeshBuffer->recalculateBoundingBox();
	}



	for (s32 j=0; j<(s32)InNode->Nodes.size(); ++j)
	{
		animateNode(frame,startFrame, endFrame,InNode->Nodes[j],InNode);
	}




}



void CAnimatedMeshB3d::animate(s32 intframe,s32 startFrameLoop, s32 endFrameLoop) // Why cannot this be "animate(f32 frame)", it would make so much nicer animations
{
	if (!HasAnimation || lastCalculatedFrame == intframe)
		return;

	lastCalculatedFrame = intframe;

	f32 frame = (f32)intframe;

	f32 startFrame= (f32)startFrameLoop;
	f32 endFrame= (f32)endFrameLoop;

	for (s32 k=0; k<(s32)Vertices_Moved.size(); ++k)
	{
		Vertices_Moved[k]=false;
	}

	s32 i;

	for (i=0; i<(s32)Nodes.size(); ++i)
	{
		SB3dNode *Node=Nodes[i];
		for (s32 j=0; j<(s32)Node->Bones.size(); ++j)
		{
			u16 VertexID = AnimatedVertices_VertexID[ Node->Bones[j].vertex_id ];

			SB3DMeshBuffer *MeshBuffer=AnimatedVertices_MeshBuffer[ Node->Bones[j].vertex_id ];

			video::S3DVertex2TCoords *Vertex=&MeshBuffer->Vertices[VertexID];

			Vertex->Pos=Vertices[Node->Bones[j].vertex_id]->Pos;
		}
	}

	for (i=0; i<(s32)Nodes.size(); ++i)
	{
		SB3dNode *Node=Nodes[i];

		Node->Animatedposition=Node->position;
		Node->Animatedscale=Node->scale;
		Node->Animatedrotation=Node->rotation;
	}

	for (i=0; i<(s32)Nodes.size(); i++)
	{
		SB3dNode *Node=Nodes[i];

		bool foundPosition=false;
		bool foundScale=false;
		bool foundRotation=false;

		s32 LastPositionKeyID=-1;
		s32 LastScaleKeyID=-1;
		s32 LastRotationKeyID=-1;

		for (s32 j=0; j<(s32)Node->Keys.size(); j++)
		{
			SB3dKey Key=Node->Keys[j];

			if (foundPosition==false)
			{
				if (Key.flags&1)
				{
					if (Key.frame>=frame)
					{
						foundPosition=true;

						if (LastPositionKeyID==-1)
						{
							Node->Animatedposition=Key.position;
						}
						else
						{
							//interpolate

							SB3dKey LastKey=Node->Keys[LastPositionKeyID];

							f32 fd1=frame-LastKey.frame;

							f32 fd2=Key.frame-frame;

							Node->Animatedposition.X=(((Key.position.X-LastKey.position.X)/(fd1+fd2))*fd1)+LastKey.position.X;
							Node->Animatedposition.Y=(((Key.position.Y-LastKey.position.Y)/(fd1+fd2))*fd1)+LastKey.position.Y;
							Node->Animatedposition.Z=(((Key.position.Z-LastKey.position.Z)/(fd1+fd2))*fd1)+LastKey.position.Z;

						}
					}
					else
					{
						LastPositionKeyID=j;
					}
				}
			}

			if (foundScale==false)
			{
				if (Key.flags&2)
				{
					if (Key.frame>=frame)
					{
						foundScale=true;

						if (LastScaleKeyID==-1)
						{
							Node->Animatedscale=Key.scale;
						}
						else
						{
							//interpolate

							SB3dKey LastKey=Node->Keys[LastScaleKeyID];

							f32 fd1=frame-LastKey.frame;

							f32 fd2=Key.frame-frame;

							Node->Animatedscale.X=(((Key.scale.X-LastKey.scale.X)/(fd1+fd2))*fd1)+LastKey.scale.X;
							Node->Animatedscale.Y=(((Key.scale.Y-LastKey.scale.Y)/(fd1+fd2))*fd1)+LastKey.scale.Y;
							Node->Animatedscale.Z=(((Key.scale.Z-LastKey.scale.Z)/(fd1+fd2))*fd1)+LastKey.scale.Z;

						}
					}
					else
					{
						LastScaleKeyID=j;

					}
				}
			}

			if (foundRotation==false)
			{
				if (Key.flags&4)
				{
					if (Key.frame>=frame)
					{
						foundRotation=true;
						//cout << "fgfg" << endl;

						if (LastRotationKeyID==-1)
						{
							Node->Animatedrotation=Key.rotation;
						}
						else
						{
							//cout << "trying R" << endl;

							SB3dKey LastKey=Node->Keys[LastRotationKeyID];


							f32 fd1=frame-LastKey.frame;

							f32 fd2=Key.frame-frame;

							f32 t=(1.0f/(fd1+fd2))*fd1;

							Node->Animatedrotation.slerp(LastKey.rotation,Key.rotation,t);
						}
					}
					else
					{
						LastRotationKeyID=j;
					}
				}
			}
		}
	}


	for (i=0; i<(s32)BaseNodes.size(); ++i)
	{
		animateNode(frame,startFrame, endFrame,BaseNodes[i],0); //Mesh might have more than one base so BaseNode should be an array
	}


}



//! returns amount of mesh buffers.
s32 CAnimatedMeshB3d::getMeshBufferCount()
{
	return Buffers.size();
}



//! returns pointer to a mesh buffer
IMeshBuffer* CAnimatedMeshB3d::getMeshBuffer(s32 nr)
{
	return Buffers[nr];
}



//! returns an axis aligned bounding box
const core::aabbox3d<f32>& CAnimatedMeshB3d::getBoundingBox() const
{
	return BoundingBox;
}



//! returns an axis aligned bounding box
core::aabbox3d<f32>& CAnimatedMeshB3d::getBoundingBox()
{
	return BoundingBox;
}



//! sets a flag of all contained materials to a new value
void CAnimatedMeshB3d::setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
{
	for (s32 i=0; i<(int)Buffers.size(); ++i)
		Buffers[i]->Material.Flags[flag] = newvalue;
}



//! Returns the type of the animated mesh.
E_ANIMATED_MESH_TYPE CAnimatedMeshB3d::getMeshType() const
{
	return EAMTS_B3D;
}



} // end namespace scene
} // end namespace irr

