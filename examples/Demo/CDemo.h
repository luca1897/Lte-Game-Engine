// This is a Demo of the LTE Game Engine. This is the original demo of the Irrlicht Engine
// This file is not documentated.

#ifndef __C_DEMO_H_INCLUDED__
#define __C_DEMO_H_INCLUDED__


#include <engine.h>

using namespace engine;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace audio;

const int CAMERA_COUNT = 7;

class CDemo : public IEventReceiver 
{
public:

	CDemo();

	~CDemo();

	void run();
	virtual bool OnEvent(SEvent event);


private:

	void loadSceneData();
	void createLoadingScreen();

	void switchToNextScene();
	void shoot();
	void createParticleImpacts();

	struct SParticleImpact
	{
		u32 when;
		core::vector3df pos;
		core::vector3df outVector;
	};

	int currentScene;
	video::SColor backColor;

	gui::IGUIStaticText* statusText;
	gui::IGUIInOutFader* inOutFader;

	scene::IAnimatedMesh* quakeLevelMesh;
	scene::ISceneNode* quakeLevelNode;
	scene::ISceneNode* skyboxNode;
	scene::IAnimatedMeshSceneNode* model1;
	scene::IAnimatedMeshSceneNode* model2;
	scene::IParticleSystemSceneNode* campFire;
  
	scene::IMetaTriangleSelector* metaSelector;
	scene::ITriangleSelector* mapSelector;

	u32 sceneStartTime;
	u32 timeForThisScene;

	core::array<SParticleImpact> Impacts;
  audio::IAudioSound *impactfx;
};

#endif
