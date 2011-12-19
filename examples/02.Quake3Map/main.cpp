/*
This Tutorial shows how to load a Quake 3 map into the
engine, create a SceneNode for optimizing the speed of
rendering and how to create a user controlled camera.

Lets start like the HelloWorld example: We include
the engine header file.
*/
#include <engine.h>
#include "../common.h"



/*
Use the main namespace engine
*/
using namespace engine;

// use the core namespace, this contain
// all basic classes like vectors, planes, rects ...
using namespace core;

// use the GUI namespace, this contain
// all the functions of the GUI system
using namespace gui;

engineDevice *device = 0;

int engineMain(unsigned int argc, void *argv )
{ 
  // setup psp callbacks
  setupPSP();

	device =
		createDevice();

  


	/*
	Get a pointer to the video driver and the SceneManager so that
	we do not always have to write device->getVideoDriver() and
	device->getSceneManager().
	*/
	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
  IGUIEnvironment* guienv = device->getGUIEnvironment();



	/*
	We add a static label to let you know the FPS count.
	*/
	gui::IGUIStaticText* statusText = guienv->addStaticText(L"Loading ...",
		rect<int>(10,10,260,22), true);

	/*
	To display the Quake 3 map, we first need to load it. Quake 3 maps
	are packed into .pk3 files wich are nothing other than .zip files.
	So we add the .pk3 file to our FileSystem. After it was added,
	we are able to read from the files in that archive as they would
	directly be stored on disk.
	*/
	
	device->getFileSystem()->addZipFileArchive("ms0:/media/map-20kdm2.pk3");
	
	/* 
	Now we can load the mesh by calling getMesh(). We get a pointer returned
	to a IAnimatedMesh. As you know, Quake 3 maps are not really animated,
	they are only a huge chunk of static geometry with some materials
	attached. Hence the IAnimated mesh consists of only one frame,
	so we get the "first frame" of the "animation", which is our quake level
	and create an OctTree scene node with it, using addOctTreeSceneNode().
	The OctTree optimizes the scene a little bit, trying to draw only geometry
	which is currently visible. An alternative to the OctTree would be a 
	AnimatedMeshSceneNode, which would draw always the complete geometry of 
	the mesh, without optimization. Try it out: Write addAnimatedMeshSceneNode
	instead of addOctTreeSceneNode and compare the primitives drawed by the
	video driver. (There is a getPrimitiveCountDrawed() method in the 
	IVideoDriver class). Note that this optimization with the Octree is only
	useful when drawing huge meshes consiting of lots of geometry.
	*/
 
	scene::IAnimatedMesh* mesh = smgr->getMesh("20kdm2.bsp");
	scene::ISceneNode* node = 0;
	
	if (mesh)
		node = smgr->addOctTreeSceneNode(mesh->getMesh(0));

	/*
	Because the level was modelled not around the origin (0,0,0), we translate
	the whole level a little bit.
	
	*/
	if (node)
	{
		node->setPosition(core::vector3df(-1300,-144,-1249));
				
		// Mesh need to be clipped
		node->setMaterialFlag(video::EMF_CLIPPING, true);
			
  }
	
	
	/*
	Now we only need a Camera to look at the Quake 3 map.
	And we want to create a user controlled camera. There are some
	different cameras available in the Irrlicht engine. For example the 
	Maya Camera which can be controlled compareable to the camera in Maya:
	Rotate with left mouse button pressed, Zoom with both buttons pressed,
	translate with right mouse button pressed. This could be created with
	addCameraSceneNodeMaya(). But for this example, we want to create a 
	camera which behaves like the ones in first person shooter games (FPS).
	*/
	smgr->addCameraSceneNodeFPS();
	
	/*
	The mouse cursor needs not to be visible, so we make it invisible.
	*/

  device->getCursorControl()->setVisible(false);

	/*
	We have done everything, so lets draw it.
	*/
	while(device->run())
	{

		driver->beginScene(true, true, video::SColor(0,200,200,200));
		smgr->drawAll();
    guienv->drawAll();
		driver->endScene();
   
		int fps = driver->getFPS();
		core::stringw str = L"FPS: ";
		str += fps; 
    statusText->setText(str.c_str());
	}
  
  sceKernelExitGame();
	
	return 0;
}

