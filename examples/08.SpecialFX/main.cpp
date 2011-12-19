/* This tutorials describes how to do special effects. It shows how to use the particle system, 
   billboards, dynamic light and the water surface scene node.
*/

#include <engine.h>
#define PSP_ENABLE_DEBUG
#include "../common.h"

using namespace engine;

int engineMain(unsigned int argc, void *argv )
{

  setupPSP();


	engineDevice *device =
		createDevice();

	if (device == 0)
		return 1; // could not create selected driver.


	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	/*
	For our environment, we load a .3ds file. It is a small room I modelled
	with Anim8or and exported it into the 3ds format because the engine
	did not support the .an8 format when I wrote this tutorial. I am a very bad
	3d graphic artist, and so the texture mapping is not very nice in this model.
	Luckily I am a better programmer than artist, and so the engine
	is able to create a cool texture mapping for me: Just use the mesh manipulator
	and create a planar texture mapping for the mesh. If you want to see the mapping
	I made with Anim8or, uncomment this line. I also did not figure out how to
	set the material right in Anim8or, it has an emissive light color which I don't really
	like. I'll switch it off too with this code.
	*/

	scene::IAnimatedMesh* mesh = smgr->getMesh(
		"ms0:/media/room.3ds");

	smgr->getMeshManipulator()->makePlanarTextureMapping(
		mesh->getMesh(0), 0.004f);
  
	scene::ISceneNode* node = 0;

	node = smgr->addAnimatedMeshSceneNode(mesh);
	node->setMaterialTexture(0,	driver->getTexture("ms0:/media/wall.jpg"));
	node->getMaterial(0).EmissiveColor.set(0,0,0,0);
	node->setMaterialFlag(video::EMF_CLIPPING, true);

	/*
	Now, for the first special effect: Animated water. It works like this: The
	WaterSurfaceSceneNode takes a mesh as input and makes
	it wave like a water surface. And if we let this scene node use a nice
	material like the EMT_REFLECTION_2_LAYER, it looks really cool. We are
	doing this with the next few lines of code. As input mesh, we create a hill
	plane mesh, without hills. But any other mesh could be used for this, you could
	even use the room.3ds (which would look really strange) if you wanted to.
	*/

	// add animated water

	mesh = smgr->addHillPlaneMesh("myHill",
		core::dimension2d<f32>(20,20),
		core::dimension2d<s32>(40,40), 0, 0,
		core::dimension2d<f32>(0,0),
		core::dimension2d<f32>(10,10));

	node = smgr->addWaterSurfaceSceneNode(mesh->getMesh(0), 3.0f, 300.0f, 30.0f);
	node->setPosition(core::vector3df(0,7,0));

	node->setMaterialTexture(0,	driver->getTexture("ms0:/media/stones.jpg"));
	node->setMaterialTexture(1,	driver->getTexture("ms0:/media/water.jpg"));

	node->setMaterialType(video::EMT_REFLECTION_2_LAYER);

	/*
	The second special effect is very basic, I bet you saw it already in some
	engine demos: A transparent billboard combined with a dynamic light.
	We simply create a light scene node, let it fly around, an to make it look
	more cool, we attach a billboard scene node to it.
	*/

	// create light

	node = smgr->addLightSceneNode(0, core::vector3df(0,0,0),
		video::SColorf(1.0f, 0.6f, 0.7f, 1.0f), 600.0f);
	scene::ISceneNodeAnimator* anim = 0;
	anim = smgr->createFlyCircleAnimator (core::vector3df(0,150,0),250.0f);
	node->addAnimator(anim);
	anim->drop();

	// attach billboard to light

	node = smgr->addBillboardSceneNode(node, core::dimension2d<f32>(50, 50));
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	node->setMaterialTexture(0,	driver->getTexture("ms0:/media/particlewhite.bmp"));
 
	/*
	The next special effect is a lot more interesting: A particle system. The particle
	system in the engine is quit modular and extensible and yet easy to use.
	There is a particle system scene node into which you can put particle emitters, which
	make particles come out of nothing. These emitters are quite flexible and usually have
	lots of parameters like direction, amount and color of the particles they should create.
	There are different emitters, for example a point emitter which lets particles pop out
	at a fixed point. If the particle emitters available in the engine are not enough for
	you, you can easily create your own ones, you'll simply have to create a class derived
	from the IParticleEmitter interface and attach it to the particle system using setEmitter().
	In this example we create a box particle emitter, which creates particles randomly
	inside a box. The parameters define the box, direction of the particles, minimal and
	maximal new particles per second, color and minimal and maximal livetime of the particles.

	Because only with emitters particle system would be a little bit boring,
	there are particle affectors, which modify particles during they fly around. They can
	be added to the particle system, simulating additional effects like gravity or wind.
	The particle affector we use in this example is an affector, which modifies the color
	of the particles: It lets them fade out. Like the particle emitters, additional
	particle affectors can also be implemented by you, simply derive a class from
	IParticleAffector and add it with addAffector().

	After we set a nice material to the particle system, we have a cool looking camp fire.
	By adjusting material, texture, particle emitter and affector parameters, it is also
	easily possible to create smoke, rain, explosions, snow, and so on.
	*/

	// create a particle system

	scene::IParticleSystemSceneNode* ps = 0;
	ps = smgr->addParticleSystemSceneNode(false);
	ps->setPosition(core::vector3df(-70,60,40));
	ps->setScale(core::vector3df(2,2,2));

	ps->setParticleSize(core::dimension2d<f32>(20.0f, 20.0f));

	scene::IParticleEmitter* em = ps->createBoxEmitter(
		core::aabbox3d<f32>(-7,0,-7,7,1,7),
		core::vector3df(0.0f,0.06f,0.0f),
		80,100,
		video::SColor(0,255,255,255), video::SColor(0,255,255,255),
		800,2000);

	ps->setEmitter(em);
	em->drop();

	scene::IParticleAffector* paf =
		ps->createFadeOutParticleAffector();

	ps->addAffector(paf);
	paf->drop();

	ps->setMaterialFlag(video::EMF_LIGHTING, false);
	ps->setMaterialTexture(0, driver->getTexture("ms0:/media/fire.bmp"));
	ps->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);

	
	/*
	Finally we simply have to draw everything, that's all.
	*/
 
	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS();
	camera->setPosition(core::vector3df(-50,50,-150));

	// disable mouse cursor
	device->getCursorControl()->setVisible(false);

	while(device->run())
	{
		driver->beginScene(true, true, 0);

		smgr->drawAll();

		driver->endScene();

	}

	device->drop();

	return 0;
}

