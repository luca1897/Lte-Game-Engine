/*
This tutorial will briefly show how to use the terrain renderer of the engine. It will also
show the terrain renderer triangle selector to be able to do collision detection with
terrain.

Note that the Terrain Renderer in this engine is based on Spintz' GeoMipMapSceneNode, lots 
of thanks go to him.
In the beginning there is nothing special. We include the needed header files.
*/
#include <engine.h>
#include "../common.h"

using namespace engine;

int engineMain(unsigned int argc, void *argv )
{
	// setup psp
  setupPSP();

  pspDebugScreenInit();
	engineDevice* device = createDevice();

	if (device == 0)
		return 1; // could not create selected driver.

	

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	// add camera
	scene::ICameraSceneNode* camera = 
		smgr->addCameraSceneNodeFPS(0,100.0f,1200.0f);

	camera->setPosition(core::vector3df(1900*2,255*2,3700*2));
	camera->setTarget(core::vector3df(2397*2,343*2,2700*2));
	camera->setFarValue(12000.0f);


	gui::IGUIStaticText* statusText = env->addStaticText(L"Terrain Rendering",
		core::rect<int>(10,10,260,22), true);

	// disable mouse cursor
	device->getCursorControl()->setVisible(false);

	/*
	Here comes the terrain renderer scene node: We add it just like any 
	other scene node to the scene using ISceneManager::addTerrainSceneNode(). 
	The only parameter we use is a file name to the heightmap we use. A heightmap
	is simply a gray scale texture. The terrain renderer loads it and creates 
	the 3D terrain from it.
	To make the terrain look more big, we change the scale factor of it to (40, 4.4, 40).
	Because we don't have any dynamic lights in the scene, we switch off the lighting,
	and we set the file terrain-texture.jpg as texture for the terrain and 
	detailmap3.jpg as second texture, called detail map. At last, we set
	the scale values for the texture: The first texture will be repeated only one time over 
	the whole terrain, and the second one (detail map) 20 times. 
	*/

	// add terrain scene node
	scene::ITerrainSceneNode* terrain = smgr->addTerrainSceneNode( 
		"ms0:/media/terrain-heightmap.bmp");

	terrain->setScale(core::vector3df(40, 4.4f, 40));
	terrain->setMaterialFlag(video::EMF_LIGHTING, false);

	terrain->setMaterialTexture(0, driver->getTexture("ms0:/media/terrain-texture.jpg"));
	//terrain->setMaterialType(video::EMT_DETAIL_MAP);

	terrain->scaleTexture(1.0f, 20.0f);

	/*
	To be able to do collision with the terrain, we create a triangle selector.
	If you want to know what triangle selectors do, just take a look into the 
	collision tutorial. The terrain triangle selector works together with the
	terrain. To demonstrate this, we create a collision response animator 
	and attach it to the camera, so that the camera will not be able to fly 
	through the terrain.
	*/

	// create triangle selector for the terrain	
	scene::ITriangleSelector* selector
		= smgr->createTerrainTriangleSelector(terrain, 0);
	terrain->setTriangleSelector(selector);
	selector->drop();

	// create collision response animator and attach it to the camera
	scene::ISceneNodeAnimator* anim = smgr->createCollisionResponseAnimator(
		selector, camera, core::vector3df(60,100,60),
		core::vector3df(0,0,0), 
		core::vector3df(0,50,0));
	camera->addAnimator(anim);
	anim->drop(); 

	/*
	 We add the skybox which we already used in lots of engine examples.
	*/

   	// create skybox
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

	smgr->addSkyBoxSceneNode(
		driver->getTexture("ms0:/media/skybox_up.jpg"),
		driver->getTexture("ms0:/media/skybox_dn.jpg"),
		driver->getTexture("ms0:/media/skybox_lf.jpg"),
		driver->getTexture("ms0:/media/skybox_rt.jpg"),
		driver->getTexture("ms0:/media/skybox_ft.jpg"),
		driver->getTexture("ms0:/media/skybox_bk.jpg"))->setMaterialFlag(video::EMF_CLIPPING,true);

	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	/*
	That's it, draw everything. Now you know how to use terrain in LTE Game Engine.
	*/
	while(device->run())
	{
		driver->beginScene(true, true, 0 );

		smgr->drawAll();
		env->drawAll();

	  core::stringw status = L"Terrain Rendering FPS = ";
	  status += driver->getFPS();
   
		statusText->setText(status.c_str());



		driver->endScene();
	}

	device->drop();
	
	return 0;
}

