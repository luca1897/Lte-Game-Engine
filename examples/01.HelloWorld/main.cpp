
/*
This tutorial show how to make you first LTE Game Engine homebrew,
the first thing to do to use the engine is to include the file
<engine.h>, all is stored there.

*/
#include <engine.h>

#include "../common.h"

/*
In the LTE Game Engine all things are stored into the namespace
'engine', doing using name space engine; will save us to write
engine:: to access each member of the engine.
*/
using namespace engine;

/* 
There are 6 sub namespaces in the LTE Game Engine. You can read
their description in the offline or online documentation. 
Online documentation can be found here: http://www.ltestudios.com/3d/docs
*/
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace audio;

/*
  There is an additional namespace audio where are stored
  all functions to play audio files.
*/
// using namespace audio;

int engineMain( unsigned int argc, void *argv )
{

  /* setups the PSP callbacks */
  setupPSP();


	/*
	To create an LTE Game Engine instance you have to call the function
   createDevice(), it has two optional parameters:
 
   createDevices( IEventReceiver *receiver = 0,
                  bool showLogo = true );

   receiver can be used to set the user defined event receiver,
   showLogo tells if the LTE Game Engine logo is shown after creating  
   the device. You can disable the logo but the engine will stop 
   to run after ten minutes. You can disable the logo to try your
   homebrew when prorgamming.
 
	*/

	engineDevice *device =
		createDevice();

	/*
	Get a pointer to the video driver, the SceneManager and the
	graphical user interface environment, so that
	we do not always have to write device->getVideoDriver(),
	device->getSceneManager() and device->getGUIEnvironment().
	*/
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

	/*
	We add a hello world label to the window, using the GUI environment,
  LTE Game Engine uses widechar and you have to type L before the string
  to convert it into widechar.
	*/
	gui::IGUIStaticText* statusText = guienv->addStaticText(L"Hello World!",
		rect<int>(10,10,260,22), true);


	/*
	To display something interesting, we load a Quake 2 model 
	and display it. We only have to get the Mesh from the Scene 
	Manager (getMesh()) and add a SceneNode to display the mesh.
	(addAnimatedMeshSceneNode()). Instead of writing the filename
	sydney.md2, it would also be possible to load a Maya object file
	(.obj), a complete Quake3 map (.bsp) or a Milshape file (.ms3d).
	By the way, that cool Quake 2 model called sydney was modelled
	by Brian Collins.
	*/
	IAnimatedMesh* mesh = smgr->getMesh("ms0:/media/sydney.md2");
	IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );

	/*
	To let the mesh look a little bit nicer, we change its material a
	little bit: We disable lighting because we do not have a dynamic light
	in here, and the mesh would be totally black. Then we set the frame
	loop, so that the animation is looped between the frames 0 and 310.
	And at last, we apply a texture to the mesh. Without it the mesh
	would be drawn using only a color.
	*/
	if (node)
	{
		node->setMaterialFlag(EMF_LIGHTING, false);
		node->setMD2Animation ( scene::EMAT_RUN );
		node->setMaterialTexture( 0, driver->getTexture("ms0:/media/sydney.bmp") );
    node->addCelshadingSceneNode();

    // Scale the mesh to make it smaller
    node->setScale(core::vector3df(0.7f, 0.7f,0.7f));
	}

	/*
	To look at the mesh, we place a camera into 3d space at the position
	(0, 30, -40). The camera looks from there to (0,5,0).
	*/
	smgr->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));

	/*
	Ok, now we have set up the scene, lets draw everything:
	We run the device in a while() loop. The device->run() function
  is very important, it is used to refresh the timer and to detect
  events.
	*/
	while(device->run())
	{
		/*
		Anything can be drawn between a beginScene() and an endScene()
		call. The beginScene clears the screen with a color and also the
		depth buffer if wanted. Then we let the Scene Manager and the
		GUI Environment draw their content. With the endScene() call
		everything is presented on the screen.
		*/
		driver->beginScene(true, true, SColor(255,100,101,140));

		smgr->drawAll();
		guienv->drawAll();

		driver->endScene();


	  core::stringw status = L"Hello World! FPS = ";
	  status += driver->getFPS();		
		statusText->setText(status.c_str());

	}

  // sleep thread
  sceKernelSleepThreadCB();  
	return 0;
}

