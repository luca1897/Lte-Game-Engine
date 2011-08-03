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

#ifndef __C_MESH_MANIPULATOR_H_INCLUDED__
#define __C_MESH_MANIPULATOR_H_INCLUDED__

#include "IMeshManipulator.h"

namespace engine
{
namespace scene
{

//! An interface for easily manipulate meshes.
/** Scale, set alpha value, flip surfaces, and so on. This exists for fixing problems
   with wrong imported or exported meshes quickly after loading. It is not intended for doing mesh
   modifications and/or animations during runtime.
 */
class CMeshManipulator : public IMeshManipulator
{
public:

//! Constructor
CMeshManipulator();

//! destructor
virtual ~CMeshManipulator();

//! Flips the direction of surfaces. Changes backfacing triangles to frontfacing
//! triangles and vice versa.
//! \param mesh: Mesh on which the operation is performed.
virtual void flipSurfaces(scene::IMesh* mesh) const;

//! Sets the alpha vertex color value of the whole mesh to a new value
//! \param mesh: Mesh on which the operation is performed.
virtual void setVertexColorAlpha(scene::IMesh* mesh, s32 alpha) const;

//! Sets the colors of all vertices to one color
virtual void setVertexColors(IMesh* mesh, video::SColor color) const;

virtual void subdivideTriangles(scene::IMesh* mesh, s32 maxlen = 64) const;

//! Recalculates all normals of the mesh.
/** \param mesh: Mesh on which the operation is performed. */
virtual void recalculateNormals(scene::IMesh* mesh, bool smooth = false) const;

//! Recalculates all normals of the mesh buffer.
/** \param buffer: Mesh buffer on which the operation is performed. */
virtual void recalculateNormals(IMeshBuffer* buffer, bool smooth = false) const;

//! Scales the whole mesh.
//! \param mesh: Mesh on which the operation is performed.
virtual void scaleMesh(scene::IMesh* mesh, const core::vector3df& scale) const;

//! Clones a static IMesh into a modifyable SMesh.
virtual SMesh* createMeshCopy(scene::IMesh* mesh) const;

//! Generate a shadow volume for the given mesh
//! \return Returns a Mesh that can be used as a prebuilt shadow volume
//! for IAnimatedMeshSceneNode::addShadowVolumeSceneNode, note you must specify
//! ESV_PREBUILT as type
virtual SMesh* buildShadowVolume(scene::IMesh* mesh);

//! Creates a planar texture mapping on the mesh
//! \param mesh: Mesh on which the operation is performed.
//! \param resolution: resolution of the planar mapping. This is the value
//! specifying which is the releation between world space and
//! texture coordinate space.
virtual void makePlanarTextureMapping(scene::IMesh* mesh, f32 resolution) const;

//! Creates a copy of the mesh, which will only consist of S3DVertexTangents vertices.
//! This is useful if you want to draw tangent space normal mapped geometry because
//! it calculates the tangent and binormal data which is needed there.
//! \param mesh: Input mesh
//! \return Mesh consiting only of S3DVertexNormalMapped vertices.
//! If you no longer need the cloned mesh, you should call IMesh::drop().
//! See IUnknown::drop() for more information.
virtual IMesh* createMeshWithTangents(IMesh* mesh) const;

virtual IMesh* createMeshUniquePrimitives(IMesh* mesh) const;

//! Recalculates the bounding box for a meshbuffer
virtual void recalculateBoundingBox(scene::IMeshBuffer* buffer) const;

//! Returns amount of polygons in mesh.
virtual s32 getPolyCount(scene::IMesh* mesh) const;

//! Returns amount of polygons in mesh.
virtual s32 getPolyCount(scene::IAnimatedMesh* mesh) const;

private:

static void calculateTangents(core::vector3df& normal,
                              core::vector3df& tangent,
                              core::vector3df& binormal,
                              core::vector3df& vt1, core::vector3df& vt2, core::vector3df& vt3,
                              core::vector2df& tc1, core::vector2df& tc2, core::vector2df& tc3);
};

} // end namespace scene
} // end namespace engine


#endif

