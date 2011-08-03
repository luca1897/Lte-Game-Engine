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

#ifndef __C_SCENE_MANAGER_H_INCLUDED__
#define __C_SCENE_MANAGER_H_INCLUDED__

#include "ISceneManager.h"
#include "ISceneNode.h"
#include "ICursorControl.h"
#include "engineString.h"
#include "engineArray.h"
#include "IMeshLoader.h"
#include "CStringParameters.h"

namespace engine
{
namespace scene
{
class CMeshCache;

/*!
   	The Scene Manager manages scene nodes, mesh recources, cameras and all the other stuff.
 */
class CSceneManager : public ISceneManager, ISceneNode
{
public:

//! constructor
CSceneManager(video::IVideoDriver* driver, io::IFileSystem* fs,
              gui::ICursorControl* cursorControl, CMeshCache* cache = 0);

//! destructor
virtual ~CSceneManager();

//! gets an animateable mesh. loads it if needed. returned pointer must not be dropped.
virtual IAnimatedMesh* getMesh(const c8* filename);

//! gets an animateable mesh. loads it if needed. returned pointer must not be dropped.
virtual IAnimatedMesh* getMesh(io::IReadFile *file);

//! Returns an interface to the mesh cache which is shared beween all existing scene managers.
virtual IMeshCache* getMeshCache();

//! returns the video driver
virtual video::IVideoDriver* getVideoDriver();

//! adds a test scene node for test purposes to the scene. It is a simple cube of (1,1,1) size.
//! the returned pointer must not be dropped.
virtual ISceneNode* addTestSceneNode(f32 size=10.0f, ISceneNode* parent=0, s32 id=-1,
                                     const core::vector3df& position = core::vector3df(0,0,0),       const core::vector3df& rotation = core::vector3df(0,0,0),       const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

//! adds a scene node for rendering an animated mesh model
virtual IAnimatedMeshSceneNode* addAnimatedMeshSceneNode(IAnimatedMesh* mesh, ISceneNode* parent=0, s32 id=-1,
                                                         const core::vector3df& position = core::vector3df(0,0,0),       const core::vector3df& rotation = core::vector3df(0,0,0),       const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

//! adds a scene node for rendering a static mesh
//! the returned pointer must not be dropped.
virtual ISceneNode* addMeshSceneNode(IMesh* mesh, ISceneNode* parent=0, s32 id=-1,
                                     const core::vector3df& position = core::vector3df(0,0,0),       const core::vector3df& rotation = core::vector3df(0,0,0),       const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

//! Adds a scene node for rendering a animated water surface mesh.
virtual ISceneNode* addWaterSurfaceSceneNode(IMesh* mesh, f32 waveHeight, f32 waveSpeed, f32 wlenght, ISceneNode* parent=0, s32 id=-1,
                                             const core::vector3df& position = core::vector3df(0,0,0),
                                             const core::vector3df& rotation = core::vector3df(0,0,0),
                                             const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

//! renders the node.
virtual void render();

//! returns the axis aligned bounding box of this node
virtual const core::aabbox3d<f32>& getBoundingBox() const;

//! registers a node for rendering it at a specific time.
virtual void registerNodeForRendering(ISceneNode* node, E_SCENE_NODE_RENDER_PASS = ESNRP_AUTOMATIC);

//! draws all scene nodes
virtual void drawAll();

//! Adds a scene node for rendering using a octtree to the scene graph. This a good method for rendering
//! scenes with lots of geometry. The Octree is built on the fly from the mesh, much
//! faster then a bsp tree.
virtual ISceneNode* addOctTreeSceneNode(IAnimatedMesh* mesh, ISceneNode* parent=0,
                                        s32 id=-1, s32 minimalPolysPerNode=128);

//! Adss a scene node for rendering using a octtree. This a good method for rendering
//! scenes with lots of geometry. The Octree is built on the fly from the mesh, much
//! faster then a bsp tree.
virtual ISceneNode* addOctTreeSceneNode(IMesh* mesh, ISceneNode* parent=0,
                                        s32 id=-1, s32 minimalPolysPerNode=128);
virtual ISceneNode* addBspSceneNode(void *mesh, ISceneNode* parent,
                                                   s32 id, s32 minimalPolysPerNode);

//! Adds a camera scene node to the tree and sets it as active camera.
//! \param position: Position of the space relative to its parent where the camera will be placed.
//! \param lookat: Position where the camera will look at. Also known as target.
//! \param parent: Parent scene node of the camera. Can be null. If the parent moves,
//! the camera will move too.
//! \return Returns pointer to interface to camera
virtual ICameraSceneNode* addCameraSceneNode(ISceneNode* parent = 0,
                                             const core::vector3df& position = core::vector3df(0,0,0),
                                             const core::vector3df& lookat = core::vector3df(0,0,0), s32 id=-1);

//! Adds a camera scene node which is able to be controlle with the mouse similar
//! like in the 3D Software Maya by Alias Wavefront.
//! The returned pointer must not be dropped.
virtual ICameraSceneNode* addCameraSceneNodeMaya(ISceneNode* parent = 0,
                                                 f32 rotateSpeed = -1500.0f, f32 zoomSpeed = 200.0f, f32 translationSpeed = 100.0f, s32 id=-1);

//! Adds a camera scene node which is able to be controled with the mouse and keys
//! like in most first person shooters (FPS):
virtual ICameraSceneNode* addCameraSceneNodeFPS(ISceneNode* parent = 0,
                                                f32 rotateSpeed = 1500.0f, f32 moveSpeed = 200.0f, s32 id=-1,
                                                SKeyMap* keyMapArray=0, s32 keyMapSize=0, bool noVerticalMovement=false, int moveStyle = 0);

//! Adds a dynamic light scene node. The light will cast dynamic light on all
//! other scene nodes in the scene, which have the material flag video::MTF_LIGHTING
//! turned on. (This is the default setting in most scene nodes).
virtual ILightSceneNode* addLightSceneNode(ISceneNode* parent = 0,
                                           const core::vector3df& position = core::vector3df(0,0,0),
                                           video::SColorf color = video::SColorf(1.0f, 1.0f, 1.0f),        f32 range=100.0f, s32 id=-1);

//! Adds a billboard scene node to the scene. A billboard is like a 3d sprite: A 2d element,
//! which always looks to the camera. It is usually used for things like explosions, fire,
//! lensflares and things like that.
virtual IBillboardSceneNode* addBillboardSceneNode(ISceneNode* parent = 0,
                                                   const core::dimension2d<f32>& size = core::dimension2d<f32>(10.0f, 10.0f),
                                                   const core::vector3df& position = core::vector3df(0,0,0), s32 id=-1);

//! Adds a skybox scene node. A skybox is a big cube with 6 textures on it and
//! is drawed around the camera position.
virtual ISceneNode* addSkyBoxSceneNode(video::ITexture* top, video::ITexture* bottom,
                                       video::ITexture* left, video::ITexture* right, video::ITexture* front,
                                       video::ITexture* back, ISceneNode* parent = 0, s32 id=-1);

//! Adds a text scene node, which is able to display
//! 2d text at a position in three dimensional space
virtual ITextSceneNode* addTextSceneNode(gui::IGUIFont* font, const wchar_t* text,
                                         video::SColor color=video::SColor(100,255,255,255),
                                         ISceneNode* parent = 0, const core::vector3df& position = core::vector3df(0,0,0),
                                         s32 id=-1);

//! Adds a Hill Plane mesh to the mesh pool. The mesh is generated on the fly
//! and looks like a plane with some hills on it. It is uses mostly for quick
//! tests of the engine only. You can specify how many hills there should be
//! on the plane and how high they should be. Also you must specify a name for
//! the mesh, because the mesh is added to the mesh pool, and can be retieved
//! again using ISceneManager::getMesh with the name as parameter.
virtual IAnimatedMesh* addHillPlaneMesh(const c8* name,
                                        const core::dimension2d<f32>&tileSize, const core::dimension2d<s32>&tileCount,
                                        video::SMaterial* material = 0, f32 hillHeight = 0.0f,
                                        const core::dimension2d<f32>& countHills = core::dimension2d<f32>(0.0f, 0.0f),
                                        const core::dimension2d<f32>& textureRepeatCount = core::dimension2d<f32>(1.0f, 1.0f));

//! Adds a terrain mesh to the mesh pool.
virtual IAnimatedMesh* addTerrainMesh(const c8* meshname,       video::IImage* texture, video::IImage* heightmap,
                                      const core::dimension2d<f32>& stretchSize,
                                      f32 maxHeight, const core::dimension2d<s32>& defaultVertexBlockSize);

//! Adds a particle system scene node.
virtual IParticleSystemSceneNode* addParticleSystemSceneNode(
        bool withDefaultEmitter=true, ISceneNode* parent=0, s32 id=-1,
        const core::vector3df& position = core::vector3df(0,0,0),
        const core::vector3df& rotation = core::vector3df(0,0,0),
        const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

//! Adds a terrain scene node to the scene graph.
virtual ITerrainSceneNode* addTerrainSceneNode(
        const char* heightMapFileName,
        ISceneNode* parent=0, s32 id=-1,
        const core::vector3df& position = core::vector3df(0.0f,0.0f,0.0f),
        const core::vector3df& rotation = core::vector3df(0.0f,0.0f,0.0f),
        const core::vector3df& scale = core::vector3df(1.0f,1.0f,1.0f),
        video::SColor vertexColor = video::SColor(255,255,255,255),
        s32 maxLOD=5, E_TERRAIN_PATCH_SIZE patchSize=ETPS_17);

//! Adds a terrain scene node to the scene graph.
virtual ITerrainSceneNode* addTerrainSceneNode(
        io::IReadFile* heightMap,
        ISceneNode* parent=0, s32 id=-1,
        const core::vector3df& position = core::vector3df(0.0f,0.0f,0.0f),
        const core::vector3df& rotation = core::vector3df(0.0f,0.0f,0.0f),
        const core::vector3df& scale = core::vector3df(1.0f,1.0f,1.0f),
        video::SColor vertexColor = video::SColor(255,255,255,255),
        s32 maxLOD=5, E_TERRAIN_PATCH_SIZE patchSize=ETPS_17);

//! Adds a dummy transformation scene node to the scene graph.
virtual IDummyTransformationSceneNode* addDummyTransformationSceneNode(
        ISceneNode* parent=0, s32 id=-1);

//! Adds an empty scene node.
virtual ISceneNode* addEmptySceneNode(ISceneNode* parent, s32 id=-1);

//! Returns the root scene node. This is the scene node wich is parent
//! of all scene nodes. The root scene node is a special scene node which
//! only exists to manage all scene nodes. It is not rendered and cannot
//! be removed from the scene.
//! \return Returns a pointer to the root scene node.
virtual ISceneNode* getRootSceneNode();

//! Returns the current active camera.
//! \return The active camera is returned. Note that this can be NULL, if there
//! was no camera created yet.
virtual ICameraSceneNode* getActiveCamera();

//! Sets the active camera. The previous active camera will be deactivated.
//! \param camera: The new camera which should be active.
virtual void setActiveCamera(ICameraSceneNode* camera);

//! creates a rotation animator, which rotates the attached scene node around itself.
//! \param rotationPerSecond: Specifies the speed of the animation
//! \return Returns the animator. Attach it to a scene node with ISceneNode::addAnimator()
//! and the animator will animate it.
virtual ISceneNodeAnimator* createRotationAnimator(const core::vector3df& rotationPerSecond);

//! creates a fly circle animator, which lets the attached scene node fly
//! around a center. The center is the position of the scene node.
//! \param rotationSpeed:
//! \return Returns the animator. Attach it to a scene node with ISceneNode::addAnimator()
//! and the animator will animate it.
virtual ISceneNodeAnimator* createFlyCircleAnimator(const core::vector3df& normal, f32 radius, f32 speed);

//! Creates a fly straight animator, which lets the attached scene node
//! fly or move along a line between two points.
virtual ISceneNodeAnimator* createFlyStraightAnimator(const core::vector3df& startPoint,
                                                      const core::vector3df& endPoint, u32 timeForWay, bool loop=false);

//! Creates a texture animator, which switches the textures of the target scene
//! node based on a list of textures.
virtual ISceneNodeAnimator* createTextureAnimator(const core::array<video::ITexture*>& textures,
                                                  s32 timePerFrame, bool loop);



virtual ISceneNode* addSphereSceneNode(f32 radius=5.0f, s32 polyCount=16, ISceneNode* parent=0, s32 id=-1,
                                       const core::vector3df& position = core::vector3df(0,0,0),
                                       const core::vector3df& rotation = core::vector3df(0,0,0),
                                       const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f)) ;


//! Creates a scene node animator, which deletes the scene node after
//! some time automaticly.
virtual ISceneNodeAnimator* createDeleteAnimator(u32 timeMS);


//! Creates a special scene node animator for doing automatic collision detection
//! and response.
virtual ISceneNodeAnimatorCollisionResponse* createCollisionResponseAnimator(
        ITriangleSelector* world, ISceneNode* sceneNode,
        const core::vector3df& ellipsoidRadius = core::vector3df(30,60,30),
        const core::vector3df& gravityPerSecond = core::vector3df(0,-1.0f,0),
        const core::vector3df& ellipsoidTranslation = core::vector3df(0,0,0),
        f32 slidingValue = 0.0005f);

//! Creates a follow spline animator.
virtual ISceneNodeAnimator* createFollowSplineAnimator(s32 startTime,
                                                       const core::array< core::vector3df >& points,
                                                       f32 speed = 1.0f, f32 tightness = 0.5f);


//! Creates a simple ITriangleSelector, based on a mesh.
virtual ITriangleSelector* createTriangleSelector(IMesh* mesh, ISceneNode* node);

//! Creates a simple ITriangleSelector, based on a mesh.
virtual ITriangleSelector* createOctTreeTriangleSelector(IMesh* mesh,
                                                         ISceneNode* node, s32 minimalPolysPerNode);

//! Creates a simple dynamic ITriangleSelector, based on a axis aligned bounding box.
virtual ITriangleSelector* createTriangleSelectorFromBoundingBox(
        ISceneNode* node);

//! Creates a meta triangle selector.
virtual IMetaTriangleSelector* createMetaTriangleSelector();

//! Creates a triangle selector which can select triangles from a terrain scene node
//! \param: Pointer to the created terrain scene node
//! \param: Level of detail, 0 for highest detail.
virtual ITriangleSelector* createTerrainTriangleSelector(
        ITerrainSceneNode* node, s32 LOD=0);

//! Adds an external mesh loader.
virtual void addExternalMeshLoader(IMeshLoader* externalLoader);

//! Returns a pointer to the scene collision manager.
virtual ISceneCollisionManager* getSceneCollisionManager();

//! Returns a pointer to the mesh manipulator.
virtual IMeshManipulator* getMeshManipulator();

//! Sets the color of stencil buffers shadows drawed by the scene manager.
virtual void setShadowColor(video::SColor color);

//! Returns the current color of shadows.
virtual video::SColor getShadowColor() const;

//! Adds a scene node to the deletion queue.
virtual void addToDeletionQueue(ISceneNode* node);

//! Returns the first scene node with the specified id.
virtual ISceneNode* getSceneNodeFromId(s32 id, ISceneNode* start=0);

//! Returns the first scene node with the specified name.
virtual ISceneNode* getSceneNodeFromName(const wchar_t* name, ISceneNode* start=0);

//! Posts an input event to the environment. Usually you do not have to
//! use this method, it is used by the internal engine.
virtual bool postEventFromUser(SEvent event);

//! Clears the whole scene. All scene nodes are removed.
virtual void clear();

//! Removes all children of this scene node
virtual void removeAll();

//! Returns interface to the parameters set in this scene.
virtual IStringParameters* getParameters();

//! Returns current render pass.
virtual E_SCENE_NODE_RENDER_PASS getSceneNodeRenderPass();

//! Creates a new scene manager.
virtual ISceneManager* createNewSceneManager();

//! Returns type of the scene node
virtual ESCENE_NODE_TYPE getType() {
	return ESNT_UNKNOWN;
}

private:

//! returns if node is culled
bool isCulled(ISceneNode* node);

//! clears the deletion list
void clearDeletionList();

struct DefaultNodeEntry
{
	DefaultNodeEntry() {
	};

	DefaultNodeEntry(ISceneNode* n)
	{
		textureValue = 0;

		if (n->getMaterialCount())
			textureValue = (n->getMaterial(0).Texture1);

		node = n;
	}

	ISceneNode* node;
	void* textureValue;

	bool operator < (const DefaultNodeEntry& other) const
	{
		return (textureValue < other.textureValue);
	}
};


struct TransparentNodeEntry
{
	TransparentNodeEntry() {
	};

	TransparentNodeEntry(ISceneNode* n, core::vector3df camera)
	{
		node = n;

		// TODO: this could be optimized, by not using sqrt
		distance = (f32)(node->getAbsoluteTransformation().getTranslation().getDistanceFrom(camera));
	}

	ISceneNode* node;
	f32 distance;

	bool operator < (const TransparentNodeEntry& other) const
	{
		return (distance > other.distance);
	}
};

//! video driver
video::IVideoDriver* Driver;

//! file system
io::IFileSystem* FileSystem;

//! cursor control
gui::ICursorControl* CursorControl;

//! collision manager
ISceneCollisionManager* CollisionManager;

//! mesh manipulator
IMeshManipulator* MeshManipulator;

//! render pass lists
core::array<ISceneNode*> LightAndCameraList;
core::array<ISceneNode*> ShadowNodeList;
core::array<ISceneNode*> CelshadingNodeList;
core::array<ISceneNode*> SkyBoxList;
core::array<DefaultNodeEntry> SolidNodeList;
core::array<TransparentNodeEntry> TransparentNodeList;
core::array<ISceneNode*> ShadowCasterNodeList1;
core::array<ISceneNode*> ShadowCasterNodeList2;
core::array<ISceneNode*> ShadowCasterNodeList3;
core::array<ISceneNode*> ShadowCasterNodeList4;

core::array<ISceneNode*> PlanarShadowNodeList;

core::array<IMeshLoader*> MeshLoaderList;
core::array<ISceneNode*> DeletionList;

//! current active camera
ICameraSceneNode* ActiveCamera;
core::vector3df camTransPos;                 // Position of camera for transparent nodes.

video::SColor ShadowColor;

//! String parameters
CStringParameters Parameters;

//! Mesh cache
CMeshCache* MeshCache;

E_SCENE_NODE_RENDER_PASS CurrentRendertime;

};

} // end namespace video
} // end namespace scene

#endif

