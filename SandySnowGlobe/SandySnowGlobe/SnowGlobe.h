#pragma once
#include <list>
#include <AntTweakBar.h>
#include "DXBase.h"
#include "DXUtil.h"
#include "GameObject.h"
#include "SkyDome.h"
#include "Camera.h"
#include "Shader.h"
#include "Light.h"
#include "FPSCounter.h"
#include "CPUCounter.h"
#include "RAMCounter.h"
#include "Timer.h"
#include "ParticleSystem.h"
#include "Cactus.h"
#include "tinyxml2.h"
#include <vector>
#include "Fire.h"

class SnowGlobe : public DXBase
{
public:
	SnowGlobe(const HINSTANCE &hInstance, const int &cmdShow, const std::string &windowName, unsigned int windowWidth, unsigned int windowHeight);
	~SnowGlobe();

	bool Init(bool vsync) override;
	void Update() override;
	void Render() override;

	bool TweakInit();
	void ToggleVsync();

private:
	SnowGlobe& operator= (const SnowGlobe&);
	SnowGlobe(const SnowGlobe&);

	bool CameraInit();
	void CactusInit(std::vector<DirectX::XMFLOAT3> p);
	void Reset();
	FPSCounter *fpsCounter;
	unsigned int fps;
	CPUCounter *cpuCounter;
	double cpu;
	RAMCounter *ramCounter;
	float totalRam, usedRam, totalUsedRam;
	std::string ram;
	Timer *deltaTime;
	float dt, dtMod;

	TwBar *twUsageBar;

	std::list<GameObject*> colObjectList, texObjectList, litObjectList, normObjectList;
	Camera *camera, *c1, *c2, *c3;
	Shader *colourShader, *textureShader, *lightsShader, *normShader, *skyDomeShader, *particleShader, *fireShader;
	
	Light *sun, *moon;
	GameObject *desert, *globeBase;
	Cactus *cactus1, *cactus2, *cactus3, *cactus4, *cactus5, *cactus6, *cactus7, *cactus8;
	SkyDome *globe;
	ParticleSystem *rain, *snow, *fire;
	tinyxml2::XMLDocument configXML;
	bool baseInit;
};

