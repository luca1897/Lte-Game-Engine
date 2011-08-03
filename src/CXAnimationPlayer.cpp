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
 
#include "CXAnimationPlayer.h"
#include "ISceneNode.h"
#include "IVideoDriver.h"
#include "os.h"
#include "SMeshBuffer.h"
#include "IMeshManipulator.h"

#include <stdio.h>

namespace engine
{
namespace scene
{

//! constructor
CXAnimationPlayer::CXAnimationPlayer(CXFileReader* reader,
                                     video::IVideoDriver* driver,
                                     IMeshManipulator* manip,
                                     const c8* filename)
	: Reader(reader), Driver(driver), AnimatedMesh(0),
	Manipulator(manip), CurrentAnimationTime(0.0f), LastAnimationTime(1.0f),
	CurrentAnimationSet(0), IsAnimatedSkinnedMesh(false), DebugSkeletonCrossSize(1.0f)
{
	useCMBones = false;
	FileName = filename;

	if (!Reader)
		return;

	if (Driver)
		Driver->grab();

	if (Manipulator)
		Manipulator->grab();

	Reader->grab();

	createAnimationData();

}



//! destructor
CXAnimationPlayer::~CXAnimationPlayer()
{
	if (Reader)
		Reader->drop();

	if (Driver)
		Driver->drop();

	if (AnimatedMesh)
		AnimatedMesh->drop();

	if (Manipulator)
		Manipulator->drop();
}



//! Gets the frame count of the animated mesh.
s32 CXAnimationPlayer::getFrameCount()
{
	return IsAnimatedSkinnedMesh ? (s32)LastAnimationTime : 1;
}



//! Returns the IMesh interface for a frame.
IMesh* CXAnimationPlayer::getMesh(s32 frame, s32 detailLevel, s32 startFrameLoop, s32 endFrameLoop)
{
	if (!IsAnimatedSkinnedMesh)
		return &OriginalMesh;

	CurrentAnimationTime = (f32)frame;
	animateSkeleton();
	modifySkin();
	updateBoundingBoxFromAnimation();
	return AnimatedMesh;
}



//! Returns an axis aligned bounding box of the mesh.
const core::aabbox3d<f32>& CXAnimationPlayer::getBoundingBox() const
{
	return Box;
}



//! Returns the type of the animated mesh.
E_ANIMATED_MESH_TYPE CXAnimationPlayer::getMeshType() const
{
	return EAMT_X;
}

void CXAnimationPlayer::getBones(core::array<core::matrix4>* lstBones)
{
	int cnt = Joints.size();
	lstBones->clear();

	for(int n = 0; n < cnt; n++)
		lstBones->push_back(Joints[n].LocalAnimatedMatrix);
}
void CXAnimationPlayer::useCMeshBones(core::array<core::matrix4>* lstBones, bool use){
	useCMBones = use;
	CMBones = lstBones;
}

void CXAnimationPlayer::createAnimationData()
{

	// get joints from x-file
	createJointData(&Reader->getRootFrame(), -1);

	createMeshData();

	if (IsAnimatedSkinnedMesh && AnimatedMesh)
	{

		animateSkeleton();

		modifySkin();

		updateBoundingBoxFromAnimation();
		DebugSkeletonCrossSize = AnimatedMesh->getBoundingBox().getExtent().X / 20.0f;
	}
	else
		DebugSkeletonCrossSize = OriginalMesh.getBoundingBox().getExtent().X / 20.0f;
}


void CXAnimationPlayer::createMeshData()
{

	// create mesh
	addFrameToMesh(&Reader->getRootFrame());

	// recalulate box
	OriginalMesh.recalculateBoundingBox();

	// store box (fix by jox, thnx)
	Box = OriginalMesh.getBoundingBox();

	// sort weights in joints
	for (s32 j=0; j<(s32)Joints.size(); ++j)
		Joints[j].Weights.sort();

	// copy mesh
	AnimatedMesh = Manipulator->createMeshCopy(&OriginalMesh);

	// create and link animation data
	prepareAnimationData();

	// first animation
	animateSkeleton();

	modifySkin();
}


void CXAnimationPlayer::addFrameToMesh(CXFileReader::SXFrame* frame)
{

	// go through all meshes
	for (u32 m=0; m<frame->Meshes.size(); ++m)
	{

		// create mesh buffer for every material
		if (frame->Meshes[m].MaterialList.Materials.empty())
			os::Printer::log("Mesh without material found in x file.", ELL_WARNING);

		for (u32 mt=0; mt<frame->Meshes[m].MaterialList.Materials.size(); ++mt)
		{
			// create buffer
			SMeshBuffer *buf = new SMeshBuffer();
			OriginalMesh.addMeshBuffer(buf);
			buf->drop();

			// new weights buffer
			Weights.push_back( core::array<SVertexWeight>() );

			// create material
			buf->Material = getMaterialFromXMaterial(
			        frame->Meshes[m].MaterialList.Materials[mt]);

			// add all faces of this material
			addFacesToBuffer(OriginalMesh.MeshBuffers.size()-1,
			                 frame->Meshes[m], mt, frame);
			buf->recalculateBoundingBox();

		}
	}

	// add child frames
	for (u32 c=0; c<frame->ChildFrames.size(); ++c)
		addFrameToMesh(&frame->ChildFrames[c]);
}



video::SMaterial CXAnimationPlayer::getMaterialFromXMaterial(CXFileReader::SXMaterial& xmat)
{
	video::SMaterial mat;
	mat.EmissiveColor = xmat.Emissive.toSColor();
	mat.DiffuseColor = xmat.FaceColor.toSColor();
	mat.SpecularColor = xmat.Specular.toSColor();
	mat.Shininess = xmat.Power;

	//if (mat.Shininess > core::ROUNDING_ERROR)
	//	mat. TODO: enable specular highlights

	if (xmat.TextureFileName.size() != 0)
	{

		mat.Texture1 = Driver->getTexture(getTextureFileName(xmat.TextureFileName).c_str());
		if (mat.Texture1 == 0)
			mat.Texture1 = Driver->getTexture(xmat.TextureFileName.c_str());

	}

	return mat;
}



void CXAnimationPlayer::addFacesToBuffer(s32 meshbuffernr, CXFileReader::SXMesh& mesh, s32 matnr, CXFileReader::SXFrame* frame)
{
	scene::SMeshBuffer* buf = (SMeshBuffer*)OriginalMesh.MeshBuffers[meshbuffernr];

	s32 vcnt = mesh.Vertices.size();
	s32 tcnt = mesh.TextureCoords.size();
	s32 ncnt = mesh.Normals.size();
	s32 fcnt = mesh.Indices.size();

	// precompute which joint belongs to which weight array
	core::array< s32 > jointNumberWeightNumberMap;
	for (s32 w=0; w<(s32)mesh.SkinWeights.size(); ++w)
	{
		s32 jnr = getJointNumberFromName(mesh.SkinWeights[w].TransformNodeName);
		if (jnr == -1)
			os::Printer::log("Unknown Joint referenced in x file",
			                 mesh.SkinWeights[w].TransformNodeName.c_str(), ELL_WARNING);
		else
		{
			Joints[jnr].MatrixOffset = mesh.SkinWeights[w].MatrixOffset;
			IsAnimatedSkinnedMesh = true;
		}

		jointNumberWeightNumberMap.push_back(jnr);
	}


	video::S3DVertex v;
	v.Color = 0xFFFFFFFF;

	// add only those with material matnr
	for (s32 i=0; i<(s32)mesh.MaterialList.FaceIndices.size(); ++i)
		if (mesh.MaterialList.FaceIndices[i] == matnr)
		{
			// add face number i
			for (s32 f=0; f<3; ++f)
			{
				s32 idxidx = i*3+f;
				s32 idx = mesh.Indices[idxidx];

				v.Pos = mesh.Vertices[idx];

				if (tcnt) v.TCoords = mesh.TextureCoords[idx];
				if (ncnt) v.Normal = mesh.Normals[mesh.NormalIndices[idxidx]];

				s32 nidx = buf->Vertices.linear_reverse_search(v);
				bool alreadyIn = (nidx != -1);

				if (!alreadyIn)
				{
					nidx = buf->Vertices.size();
					buf->Indices.push_back(nidx);
					buf->Vertices.push_back(v);
					Weights[meshbuffernr].push_back(SVertexWeight());
				}
				else
					buf->Indices.push_back(nidx);

				bool isWeighted = alreadyIn;

				// add weight data of this vertex to all joints
				if (!alreadyIn)
					for (s32 w=0; w<(s32)mesh.SkinWeights.size(); ++w)
					{
						s32 vertexinweights = mesh.SkinWeights[w].Weights.binary_search(CXFileReader::SXWeight(idx));

						if (vertexinweights != -1)
						{
							s32 jnr = jointNumberWeightNumberMap[w];

							if (jnr != -1)
							{
								// weight per joint
								Joints[jnr].Weights.push_back(SWeightData(
								                                      meshbuffernr,
								                                      nidx,
								                                      mesh.SkinWeights[w].Weights[vertexinweights].Weight));

								// weight per vertex
								SVertexWeight& weight = Weights[meshbuffernr].pointer()[nidx];
								weight.add(mesh.SkinWeights[w].Weights[vertexinweights].Weight, jnr);

								isWeighted = true;
							}
						}
					} // end for all weights

				// if this vertex does not have a weight, create a virtual new
				// joint and attach it to this one
				if (!isWeighted)
					addVirtualWeight(meshbuffernr, nidx, mesh, frame);

			} // end for all faces

		} // end for all materials

	// generate missing normals
	if (!ncnt)
	{
		video::S3DVertex* v = &buf->Vertices[0];
		fcnt = buf->Indices.size();
		for (s32 u=0; u<fcnt; u+=3)
		{
			core::plane3d<f32> p(v[buf->Indices[u+0]].Pos,
			                     v[buf->Indices[u+1]].Pos, v[buf->Indices[u+2]].Pos);
			p.Normal.normalize();
			v[buf->Indices[u+0]].Normal = p.Normal;
			v[buf->Indices[u+1]].Normal = p.Normal;
			v[buf->Indices[u+2]].Normal = p.Normal;
		}
	}

	// transform vertices and normals
	/*core::matrix4 mat = frame->LocalMatrix;
	   mat.makeInverse();

	   vcnt = buf->Vertices.size();
	   for (s32 u=0; u<vcnt; ++u)
	   {
	   	mat.transformVect(buf->Vertices[u].Pos);
	   	mat.inverseRotateVect(buf->Vertices[u].Normal);
	   }*/
}


void CXAnimationPlayer::addVirtualWeight(s32 meshbuffernr, s32 vtxidx,
                                         CXFileReader::SXMesh& mesh,
                                         CXFileReader::SXFrame* frame)
{
	// find original joint of vertex

	s32 jnr = getJointNumberFromName(frame->Name);
	if (jnr == -1)
		return;

	// weight per joint
	Joints[jnr].Weights.push_back(SWeightData(
	                                      meshbuffernr,
	                                      vtxidx,
	                                      1.0f));

	// weight per vertex
	SVertexWeight& weight = Weights[meshbuffernr].pointer()[vtxidx];
	weight.add(1.0f, jnr);
}



s32 CXAnimationPlayer::getJointNumberFromName(core::stringc& name)
{
	for (s32 i=0; i<(s32)Joints.size(); ++i)
		if (Joints[i].Name == name)
			return i;

	return -1;
}



core::stringc CXAnimationPlayer::getTextureFileName(core::stringc texture)
{
	s32 idx = -1;
	idx = FileName.findLast('/');

	if (idx == -1)
		idx = FileName.findLast('\\');

	if (idx == -1)
		return core::stringc();

	core::stringc p = FileName.subString(0, idx+1);
	p.append(texture);
	return p;
}



void CXAnimationPlayer::createJointData(CXFileReader::SXFrame* f, s32 JointParent)
{
	// add joint

	s32 index = Joints.size();
	Joints.push_back(SJoint());
	SJoint& j = Joints.getLast();

	j.Parent = JointParent;
	j.GlobalMatrix = f->GlobalMatrix;
	j.LocalMatrix = f->LocalMatrix;
	j.AnimatedMatrix = j.GlobalMatrix;
	j.LocalAnimatedMatrix = j.LocalMatrix;
	j.CombinedAnimationMatrix = j.AnimatedMatrix * j.MatrixOffset;
	j.IsVirtualJoint = false;

	j.Name = f->Name;

	// add all children
	for (s32 i=0; i<(s32)f->ChildFrames.size(); ++i)
		createJointData(&f->ChildFrames[i], index);
}


//! Returns a pointer to a transformation matrix
core::matrix4* CXAnimationPlayer::getMatrixOfJoint(s32 jointNumber, s32 frame)
{
	if (jointNumber < 0 || jointNumber >= (s32)Joints.size())
		return 0;

	return &Joints[jointNumber].AnimatedMatrix;
}


//! Gets joint count.
s32 CXAnimationPlayer::getJointCount() const
{
	return Joints.size();
}


//! Gets the name of a joint.
const c8* CXAnimationPlayer::getJointName(s32 number) const
{
	if (number<0 || number>=(s32)Joints.size())
		return 0;

	return Joints[number].Name.c_str();
}


//! Gets a joint number from its name
s32 CXAnimationPlayer::getJointNumber(const c8* name) const
{
	for (s32 i=0; i<(s32)Joints.size(); ++i)
		if (Joints[i].Name == name)
			return i;

	return -1;
}


//! Returns a pointer to list of points containing the skeleton.
const core::array<core::vector3df>* CXAnimationPlayer::getDrawableSkeleton(s32 frame)
{
	DebugSkeleton.clear();

	f32 k = DebugSkeletonCrossSize;
	f32 p = 0.0f;

	for (s32 i=0; i<(s32)Joints.size(); ++i)
	{
		core::vector3df start(p,p,p);
		core::vector3df end(p,p,p);

		Joints[i].AnimatedMatrix.transformVect(start);

		DebugSkeleton.push_back(start);
		DebugSkeleton.push_back(start + core::vector3df(0,k,0));
		DebugSkeleton.push_back(start);
		DebugSkeleton.push_back(start + core::vector3df(0,-k,0));
		DebugSkeleton.push_back(start);
		DebugSkeleton.push_back(start + core::vector3df(k,0,0));
		DebugSkeleton.push_back(start);
		DebugSkeleton.push_back(start + core::vector3df(-k,0,0));
		DebugSkeleton.push_back(start);
		DebugSkeleton.push_back(start + core::vector3df(0,0,k));
		DebugSkeleton.push_back(start);
		DebugSkeleton.push_back(start + core::vector3df(0,0,-k));

		if (Joints[i].Parent != -1)
		{
			Joints[Joints[i].Parent].AnimatedMatrix.transformVect(end);

			DebugSkeleton.push_back(end);
			DebugSkeleton.push_back(start);
		}
	}

	return &DebugSkeleton;
}


//! animates the skeleton based on the animation data
void CXAnimationPlayer::animateSkeleton()
{
	s32 i;

	if (!AnimationSets.empty())
	{

		// reset joints
		for (s32 jii=0; jii<(s32)Joints.size(); ++jii)
		{
			Joints[jii].LocalAnimatedMatrix.makeIdentity();
			Joints[jii].WasAnimatedThisFrame = false;
		}
		if(useCMBones){
			int cnt = Joints.size();
			for(int n = 0; n < cnt; n++){
				Joints[n].LocalAnimatedMatrix = (*CMBones)[n];
				Joints[n].WasAnimatedThisFrame = true;
			}
		}

		SXAnimationSet& currentSet = AnimationSets[CurrentAnimationSet];

		// go through all animation tracks
		if(!useCMBones)
			for (i=0; i<(s32)currentSet.Animations.size(); ++i)
			{
				SJoint& joint = Joints[currentSet.Animations[i].jointNr];

				// find indexes to interpolate between
				s32 idx1 = -1;
				s32 idx2 = -1;

				for (int t=0; t<(s32)currentSet.Animations[i].Times.size()-1; ++t)
					if (currentSet.Animations[i].Times[t] <= CurrentAnimationTime &&
					    currentSet.Animations[i].Times[t+1] >= CurrentAnimationTime)
					{
						idx1 = t;
						idx2 = (t+1) % currentSet.Animations[i].Times.size();
						break;
					}

				if (idx1 == -1 || idx2 == -1)
					continue;

				// calculate interpolation factor
				f32 factor = (CurrentAnimationTime - currentSet.Animations[i].Times[idx1]) /
				             (currentSet.Animations[i].Times[idx2] - currentSet.Animations[i].Times[idx1]);

				// animate it
				switch(currentSet.Animations[i].keyType)
				{
				case 0: // rotation
				{
					// with this code, rotations are not 100% ok, they are
					// mengineored.
					core::quaternion q;
					q.slerp(currentSet.Animations[i].Quaternions[idx1],
					        currentSet.Animations[i].Quaternions[idx2],
					        factor);

					joint.LocalAnimatedMatrix *= q.getMatrix();
					joint.WasAnimatedThisFrame = true;
				}
					break;
				case 1: // scale
				{
					core::matrix4 mat1, mat2;
					mat1.setScale(currentSet.Animations[i].Vectors[idx1]);
					mat2.setScale(currentSet.Animations[i].Vectors[idx2]);

					joint.LocalAnimatedMatrix *= mat1.interpolate(mat2, factor);
					joint.WasAnimatedThisFrame = true;
				}
					break;
				case 2: // position
				{
					core::matrix4 mat1, mat2;
					mat1.setTranslation(currentSet.Animations[i].Vectors[idx1]);
					mat2.setTranslation(currentSet.Animations[i].Vectors[idx2]);

					joint.LocalAnimatedMatrix *= mat1.interpolate(mat2, factor);
					joint.WasAnimatedThisFrame = true;
				}
					break;
				case 4:
				case 3: // matrix
				{
					joint.LocalAnimatedMatrix =
					        currentSet.Animations[i].Matrices[idx1].interpolate(
					        currentSet.Animations[i].Matrices[idx2], factor);

					joint.WasAnimatedThisFrame = true;
				}
					break;
				}

			}
	}


	// update all joints
	for (s32 ji=0; ji<(s32)Joints.size(); ++ji)
	{
		if (!Joints[ji].WasAnimatedThisFrame)
			Joints[ji].LocalAnimatedMatrix = Joints[ji].LocalMatrix;

		Joints[ji].AnimatedMatrix = Joints[ji].LocalAnimatedMatrix;

		if (Joints[ji].Parent != -1)
			Joints[ji].AnimatedMatrix = Joints[Joints[ji].Parent].AnimatedMatrix * Joints[ji].AnimatedMatrix;

		Joints[ji].CombinedAnimationMatrix = Joints[ji].AnimatedMatrix *
		                                     Joints[ji].MatrixOffset;
	}
}


//! modifies the skin based on the animated skeleton
void CXAnimationPlayer::modifySkin()
{
	// set animated vertices to zero
	for (s32 k=0; k<(s32)Joints.size(); ++k)
	{
		for (s32 w=0; w<(s32)Joints[k].Weights.size(); ++w)
		{
			SWeightData& wd = Joints[k].Weights[w];

#ifdef _XREADER_DEBUG
			if (wd.buffer >= AnimatedMesh->getMeshBufferCount() ||
			    wd.vertex >= AnimatedMesh->getMeshBuffer(wd.buffer)->getVertexCount())
				os::Printer::log("CXAnimationPlayer: Invalid Weights");
#endif

			video::S3DVertex* nv = (video::S3DVertex*)AnimatedMesh->getMeshBuffer(wd.buffer)->getVertices();
			nv[wd.vertex].Pos.set(0,0,0);
		}
	}


#ifdef _XREADER_DEBUG
	bool somethingIsWrong = false;

	// check if all vertices are set to zero
	for (s32 mb=0; mb<AnimatedMesh->getMeshBufferCount(); ++mb)
	{
		video::S3DVertex* v = (video::S3DVertex*)AnimatedMesh->getMeshBuffer(mb)->getVertices();
		s32 c = AnimatedMesh->getMeshBuffer(mb)->getVertexCount();
		for (s32 vt=0; vt<c; ++vt)
			if (v[vt].Pos != core::vector3df(0,0,0))
			{
				char tmp[255];
				sprintf(tmp, "CXAnimationPlayer: Vertex not null. Buf:%d Vtx:%d", mb, vt);
				// this can happen if something is wrong with the vertex weights,
				// not all vertices have a weight attached.
				os::Printer::log(tmp);
				somethingIsWrong = true;
			}
	}

	if (somethingIsWrong)
	{
		// write out mesh informations
		char tmp[255];
		sprintf(tmp, "CXAnimationPlayer: Meshbuffers:%d", AnimatedMesh->getMeshBufferCount());
		os::Printer::log(tmp);

		for (s32 mb=0; mb<AnimatedMesh->getMeshBufferCount(); ++mb)
		{
			sprintf(tmp, "CXAnimationPlayer: Meshbuffer #%d: %d vertices", mb, AnimatedMesh->getMeshBuffer(mb)->getVertexCount());
			os::Printer::log(tmp);
		}
	}
#endif

	// transform vertices
	for (s32 mb=0; mb<AnimatedMesh->getMeshBufferCount(); ++mb)
	{
		video::S3DVertex* av = (video::S3DVertex*)AnimatedMesh->getMeshBuffer(mb)->getVertices();
		video::S3DVertex* ov = (video::S3DVertex*)OriginalMesh.getMeshBuffer(mb)->getVertices();
		s32 c = AnimatedMesh->getMeshBuffer(mb)->getVertexCount();
		for (s32 vt=0; vt<c; ++vt)
		{
			core::vector3df vtmp;
			core::vector3df orig = ov[vt].Pos;
			SVertexWeight& weight = Weights[mb].pointer()[vt];

			av[vt].Pos = core::vector3df(0,0,0);

			/*if (weight.weightCount == 0)
			   {
			   	av[vt].Pos = ov[vt].Pos;
			   }
			   else*/
			for (int w=0; w<weight.weightCount; ++w)
			{
				Joints[weight.joint[w]].CombinedAnimationMatrix.transformVect(
				        orig, vtmp);

				/*core::matrix4 mat;
				   mat = Joints[weight.joint[w]].GlobalMatrix;
				   mat *= Joints[weight.joint[w]].MatrixOffset;
				   mat.transformVect(orig, vtmp);*/

				vtmp *= weight.weight[w];
				av[vt].Pos += vtmp;
			}


		}
	}

	// transform vertices
	/*core::vector3df vtmp;
	   for (s32 k=0; k<(s32)Joints.size(); ++k)
	   {

	   	for (s32 w=0; w<(s32)Joints[k].Weights.size(); ++w)
	   	{
	   		SWeightData& wd = Joints[k].Weights[w];
	   		video::S3DVertex* nv = (video::S3DVertex*)AnimatedMesh->getMeshBuffer(wd.buffer)->getVertices();
	   		video::S3DVertex* ov = (video::S3DVertex*)OriginalMesh.getMeshBuffer(	wd.buffer)->getVertices();

	   		core::matrix4 m = Joints[k].XFrame->GlobalMatrix;
	   		m *= Joints[k].MatrixOffset;

	   		m.transformVect(ov[wd.vertex].Pos, vtmp);

	   		vtmp *= wd.weight;
	   		nv[wd.vertex].Pos += vtmp;

	   	}
	   }*/

}


//! prepares animation data which was read in from the .x file
void CXAnimationPlayer::prepareAnimationData()
{
	s32 animationSetCount = Reader->getAnimationSetCount();
	for (int i=0; i<animationSetCount; ++i)
	{
		AnimationSets.push_back(SXAnimationSet());
		SXAnimationSet& mySet = AnimationSets.getLast();

		CXFileReader::SXAnimationSet& readerSet = Reader->getAnimationSet(i);
		mySet.AnimationName = readerSet.AnimationName;

		// trough all animations
		for (int a=0; a<(s32)readerSet.Animations.size(); ++a)
		{
			// trough all keys
			for (int k=0; k<(s32)readerSet.Animations[a].Keys.size(); ++k)
			{
				// link with joint
				int jntnr = getJointNumberFromName(readerSet.Animations[a].FrameName);

				if (jntnr == -1)
				{
					os::Printer::log(
					        "CXAnimationPlayer: Animationtrack without corresponding joint found",
					        readerSet.Animations[a].FrameName.c_str());
					continue;
				}

				// add new track
				mySet.Animations.push_back(SXAnimationTrack());
				SXAnimationTrack& myTrack = mySet.Animations.getLast();
				myTrack.jointNr = jntnr;
				IsAnimatedSkinnedMesh = true;

				// copy track
				s32 keyCount = (s32)readerSet.Animations[a].Keys[k].numberOfKeys;
				s32 type = readerSet.Animations[a].Keys[k].keyType;
				s32 i;
				myTrack.keyType = type;

		#ifdef _XREADER_DEBUG
				char tmp[255];
				sprintf(tmp, "adding track %s with %d keys, type:%d", readerSet.Animations[a].FrameName.c_str(), keyCount, type);
				os::Printer::log(tmp);
		#endif

				switch(type)
				{
				case 0: // quaternion
					for (i=0; i<keyCount; ++i)
						myTrack.Quaternions.push_back(readerSet.Animations[a].Keys[k].getQuaternion(i));
					break;
				case 1: // scale
				case 2: // position
					for (i=0; i<keyCount; ++i)
						myTrack.Vectors.push_back(readerSet.Animations[a].Keys[k].getVector(i));
					break;
				case 4:
				case 3: // matrix
					for (i=0; i<keyCount; ++i)
						myTrack.Matrices.push_back(readerSet.Animations[a].Keys[k].getMatrix(i));
					break;
				}

				// copy times
				for (i=0; i<keyCount; ++i)
					myTrack.Times.push_back((f32)readerSet.Animations[a].Keys[k].time[i]);

				if (myTrack.Times.getLast() > LastAnimationTime)
					LastAnimationTime = myTrack.Times.getLast();
			}

		}

		// sort animation tracks
		mySet.Animations.sort();
	}
}

void CXAnimationPlayer::updateBoundingBoxFromAnimation()
{
	if (!Joints.size())
		return;

	bool first = true;

	for (s32 i=1; i<(s32)Joints.size(); ++i)
		if (!Joints[i].Weights.empty())
		{
			core::vector3df p(0,0,0);
			Joints[i].AnimatedMatrix.transformVect(p);

			if (first)
				Box.reset(p);
			else
				Box.addInternalPoint(p);

			first = false;
		}

	AnimatedMesh->BoundingBox = Box;

}

//! Returns amount of animations in .X-file.
s32 CXAnimationPlayer::getAnimationCount() const
{
	return AnimationSets.size();
}

//! Returns the name of an animation.
const c8* CXAnimationPlayer::getAnimationName(s32 idx) const
{
	if (idx < 0 || idx >= (s32)AnimationSets.size())
		return 0;

	return AnimationSets[idx].AnimationName.c_str();
}


//! Sets an animation as animation to play back.
void CXAnimationPlayer::setCurrentAnimation(s32 idx)
{
	if (idx < 0 || idx >= (s32)AnimationSets.size())
		return;

	CurrentAnimationSet = idx;
}


//! Sets an animation as animation to play back.
bool CXAnimationPlayer::setCurrentAnimation(const c8* name)
{
	for (s32 i=0; i<(s32)AnimationSets.size(); ++i)
		if (AnimationSets[i].AnimationName == name)
		{
			CurrentAnimationSet = i;
			return true;
		}

	return false;
}


} // end namespace scene
} // end namespace engine

