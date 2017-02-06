#include "SnowGlobe.h"

SnowGlobe::SnowGlobe(const HINSTANCE &hInstance, const int &cmdShow, const std::string &windowName, unsigned int windowWidth, unsigned int windowHeight) : DXBase(hInstance, cmdShow, windowName, windowWidth, windowHeight)
{
	camera = nullptr;
	c1 = nullptr;
	c2 = nullptr;
	c3 = nullptr;
	colourShader = nullptr;
	textureShader = nullptr;
	lightsShader = nullptr;
	normShader = nullptr;
	skyDomeShader = nullptr;
	particleShader = nullptr;
	sun = nullptr;
	moon = nullptr;
	desert = nullptr;
	globe = nullptr;
	globeBase = nullptr;
	cactus1 = nullptr;
	cactus2 = nullptr;
	cactus3 = nullptr;
	cactus4 = nullptr;
	cactus5 = nullptr;
	cactus6 = nullptr;
	cactus7 = nullptr;
	cactus8 = nullptr;
	rain = nullptr;
	snow = nullptr;
	fire = nullptr;
	
	twUsageBar = nullptr;
	fpsCounter = nullptr;
	cpuCounter = nullptr;
	ramCounter = nullptr;
	deltaTime = nullptr;
	fps = 0;
	cpu = 0;
	totalRam = 0;
	usedRam = 0;
	totalUsedRam = 0;
	dt = 0;
	dtMod = 1.0f;
	baseInit = false;
}

SnowGlobe::~SnowGlobe()
{
	try
	{
		Memory::SafeDelete(fpsCounter);
		Memory::SafeDelete(cpuCounter);
		Memory::SafeDelete(ramCounter);
		Memory::SafeDelete(deltaTime);
		Memory::SafeDelete(c1);
		Memory::SafeDelete(c2);
		Memory::SafeDelete(c3);
		Memory::SafeDelete(colourShader);
		Memory::SafeDelete(textureShader);
		Memory::SafeDelete(lightsShader);
		Memory::SafeDelete(normShader);
		Memory::SafeDelete(skyDomeShader);
		Memory::SafeDelete(particleShader);
		Memory::SafeDelete(fireShader);
		Memory::SafeDelete(sun);
		Memory::SafeDelete(moon);
		Memory::SafeDelete(rain);
		Memory::SafeDelete(snow);
		Memory::SafeDelete(fire);


		for each (GameObject* o in colObjectList)
		{
			Memory::SafeDelete(o);
		}

		for each (GameObject* o in texObjectList)
		{
			Memory::SafeDelete(o);
		}

		for each (GameObject* o in litObjectList)
		{
			Memory::SafeDelete(o);
		}

		for each (GameObject* o in normObjectList)
		{
			Memory::SafeDelete(o);
		}

		Memory::SafeDelete(globe);
	}
	catch(int &e)
	{
		
		delete colourShader;
		delete textureShader;
		delete lightsShader;
		delete normShader;
		delete skyDomeShader;
		delete particleShader;
		delete fireShader;
		delete sun;
		delete moon;
		delete fpsCounter;
		delete cpuCounter;
		delete ramCounter;
		delete deltaTime;
		delete twUsageBar;
		delete rain;
		delete snow;
		delete fire;
		delete c1;
		delete c2;
		delete c3;

		for each (GameObject* o in colObjectList)
		{
			delete o;
		}

		for each (GameObject* o in texObjectList)
		{
			delete o;
		}

		for each (GameObject* o in litObjectList)
		{
			delete o;
		}

		for each (GameObject* o in normObjectList)
		{
			delete o;
		}

		delete globe;
	}
}

bool SnowGlobe::Init(bool vsync)
{
	if(!baseInit)
		baseInit = DXBase::Init(vsync);
	if(!baseInit)
		return false;

	fpsCounter = new FPSCounter();
	cpuCounter = new CPUCounter();
	ramCounter = new RAMCounter();
	deltaTime = new Timer();

	totalRam = ramCounter->TotalRAM();

	#pragma region ConfigLoad

	FILE* configFile;
	fopen_s(&configFile, "config.xml", "rb");

	configXML.LoadFile(configFile);

	tinyxml2::XMLNode *pRoot = configXML.FirstChild();
	tinyxml2::XMLElement *pElement = pRoot->FirstChildElement("Objects");

	tinyxml2::XMLElement *pListElement = pElement->FirstChildElement("Position");
	std::vector<DirectX::XMFLOAT3> posList;

	while(pListElement != nullptr)
	{
		float x, y, z;
		tinyxml2::XMLElement *e = pListElement->FirstChildElement("x");
		e->QueryFloatText(&x);
		e = pListElement->FirstChildElement("y");
		e->QueryFloatText(&y);
		e = pListElement->FirstChildElement("z");
		e->QueryFloatText(&z);

		posList.push_back(DirectX::XMFLOAT3(x, y, z));
		pListElement = pListElement->NextSiblingElement("Position");
	}

	DirectX::XMFLOAT3 sunDir, moonDir;
	float x, y, z;

	pElement = pRoot->FirstChildElement("Sun");
	pListElement = pElement->FirstChildElement("Direction");

	tinyxml2::XMLElement *e = pListElement->FirstChildElement("x");
	e->QueryFloatText(&x);
	e = pListElement->FirstChildElement("y");
	e->QueryFloatText(&y);
	e = pListElement->FirstChildElement("z");
	e->QueryFloatText(&z);

	sunDir = DirectX::XMFLOAT3(x, y, z);

	pElement = pRoot->FirstChildElement("Moon");
	pListElement = pElement->FirstChildElement("Direction");

	e = pListElement->FirstChildElement("x");
	e->QueryFloatText(&x);
	e = pListElement->FirstChildElement("y");
	e->QueryFloatText(&y);
	e = pListElement->FirstChildElement("z");
	e->QueryFloatText(&z);

	moonDir = DirectX::XMFLOAT3(x, y, z);

	int seasonLength;

	pElement = pRoot->FirstChildElement("Seasons");
	pListElement = pElement->FirstChildElement("Length");
	pListElement->QueryIntText(&seasonLength);

	fclose(configFile);

	#pragma endregion

	CameraInit();

	colourShader = new Shader(Shader::COLOUR);
	if(!colourShader->Init(dev.Get(), L"Colour.vs", L"Colour.ps"))
		return false;

	textureShader = new Shader(Shader::TEXTURE);
	if(!textureShader->Init(dev.Get(), L"Texture.vs", L"Texture.ps"))
		return false;

	lightsShader = new Shader(Shader::LIGHTS);
	if(!lightsShader->Init(dev.Get(), L"Lights.vs", L"Lights.ps"))
		return false;

	normShader = new Shader(Shader::NORMAL);
	if(!normShader->Init(dev.Get(), L"Normal.vs", L"Normal.ps"))
		return false;

	skyDomeShader = new Shader(Shader::SKYDOME);
	if(!skyDomeShader->Init(dev.Get(), L"SkyCycle.vs", L"SkyCycle.ps"))
		return false;

	particleShader = new Shader(Shader::PARTICLE);
	if(!particleShader->Init(dev.Get(), L"Particle.vs", L"Particle.ps"))
		return false;

	fireShader = new Shader(Shader::FIRE);
	if(!fireShader->Init(dev.Get(), L"Fire.vs", L"Fire.ps"))
		return false;

	rain = new ParticleSystem(ParticleSystem::RAIN, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), particleShader);
	rain->Init(dev.Get(), L"raindrop.dds");

	snow = new ParticleSystem(ParticleSystem::SNOW, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), particleShader);
	snow->Init(dev.Get(), L"snowflake.dds");

	fire = new ParticleSystem(ParticleSystem::FIRE, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), particleShader);

	sun = new Light();
	sun->DiffuseColour(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	sun->StartDirection(sunDir);
	sun->SpecularIntensity(500.0f);
	sun->SpecularColour(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	moon = new Light();
	moon->DiffuseColour(DirectX::XMFLOAT4(0.078f, 0.24f, 0.71f, 1.0f));
	moon->StartDirection(moonDir);
	moon->SpecularColour(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	moon->SpecularIntensity(500.0f);

	desert = new GameObject(dev.Get(), L"desert.obj", L"sand.dds", L"sand_norm.dds", L"sand_spec.dds", normShader);
	desert->Position(posList[0]);
	desert->Scale(DirectX::XMFLOAT3(0.985f, 0.985f, 0.985f));
	normObjectList.push_back(desert);

	globe = new SkyDome(dev.Get(), L"dome.obj", L"sky.dds", L"SkyMapSmooth.dds", skyDomeShader, rain, snow);
	globe->Position(DirectX::XMFLOAT3(0.0f, -10.0f, 0.0f));
	globe->SeasonLength(seasonLength);

	globeBase = new GameObject(dev.Get(), L"snowglobebase.obj", L"wood.dds", textureShader);
	globeBase->Position(posList[1]);
	globeBase->Scale(DirectX::XMFLOAT3(3.75f, 1.8f, 3.75f));
	texObjectList.push_back(globeBase);

	CactusInit(posList);

	TweakInit();
	
	return true;
}

void SnowGlobe::CactusInit(std::vector<DirectX::XMFLOAT3> p)
{
	cactus1 = new Cactus(dev.Get(), L"cactus.obj", L"cactus.dds", L"cactus_norm.dds", L"blank_spec.dds", new Fire(dev.Get(), L"fire01.dds", L"noise01.dds", L"alpha01.dds", *fire, fireShader), normShader);
	cactus1->Position(p[2]);
	cactus1->Raining(globe->GetRaining());
	cactus1->Snowing(globe->GetSnowing());
	cactus1->Sunny(globe->GetSunny());
	cactus1->GetFire()->Position(cactus1->Position());
	normObjectList.push_back(cactus1);

	cactus2 = new Cactus(dev.Get(), L"cactus.obj", L"cactus.dds", L"cactus_norm.dds", L"blank_spec.dds", new Fire(dev.Get(), L"fire01.dds", L"noise01.dds", L"alpha01.dds", *fire, fireShader), normShader);
	cactus2->Position(p[3]);
	cactus2->Raining(globe->GetRaining());
	cactus2->Snowing(globe->GetSnowing());
	cactus2->Sunny(globe->GetSunny());

	cactus2->GetFire()->Position(cactus2->Position());
	normObjectList.push_back(cactus2);

	cactus3 = new Cactus(dev.Get(), L"cactus.obj", L"cactus.dds", L"cactus_norm.dds", L"blank_spec.dds", new Fire(dev.Get(), L"fire01.dds", L"noise01.dds", L"alpha01.dds", *fire, fireShader), normShader);
	cactus3->Position(p[4]);
	cactus3->Raining(globe->GetRaining());
	cactus3->Snowing(globe->GetSnowing());
	cactus3->Sunny(globe->GetSunny());
	cactus3->GetFire()->Position(cactus3->Position());
	normObjectList.push_back(cactus3);

	cactus4 = new Cactus(dev.Get(), L"cactus.obj", L"cactus.dds", L"cactus_norm.dds", L"blank_spec.dds", new Fire(dev.Get(), L"fire01.dds", L"noise01.dds", L"alpha01.dds", *fire, fireShader), normShader);
	cactus4->Position(p[5]);
	cactus4->Raining(globe->GetRaining());
	cactus4->Snowing(globe->GetSnowing());
	cactus4->Sunny(globe->GetSunny());
	cactus4->GetFire()->Position(cactus4->Position());
	normObjectList.push_back(cactus4);

	cactus5 = new Cactus(dev.Get(), L"cactus.obj", L"cactus.dds", L"cactus_norm.dds", L"blank_spec.dds", new Fire(dev.Get(), L"fire01.dds", L"noise01.dds", L"alpha01.dds", *fire, fireShader), normShader);
	cactus5->Position(p[6]);
	cactus5->Raining(globe->GetRaining());
	cactus5->Snowing(globe->GetSnowing());
	cactus5->Sunny(globe->GetSunny());
	cactus5->GetFire()->Position(cactus5->Position());
	normObjectList.push_back(cactus5);

	cactus6 = new Cactus(dev.Get(), L"cactus.obj", L"cactus.dds", L"cactus_norm.dds", L"blank_spec.dds", new Fire(dev.Get(), L"fire01.dds", L"noise01.dds", L"alpha01.dds", *fire, fireShader), normShader);
	cactus6->Position(p[7]);
	cactus6->Raining(globe->GetRaining());
	cactus6->Snowing(globe->GetSnowing());
	cactus6->Sunny(globe->GetSunny());
	cactus6->GetFire()->Position(cactus6->Position());
	normObjectList.push_back(cactus6);

	cactus7 = new Cactus(dev.Get(), L"cactus.obj", L"cactus.dds", L"cactus_norm.dds", L"blank_spec.dds", new Fire(dev.Get(), L"fire01.dds", L"noise01.dds", L"alpha01.dds", *fire, fireShader), normShader);
	cactus7->Position(p[8]);
	cactus7->Raining(globe->GetRaining());
	cactus7->Snowing(globe->GetSnowing());
	cactus7->Sunny(globe->GetSunny());
	cactus7->GetFire()->Position(cactus7->Position());
	normObjectList.push_back(cactus7);

	cactus8 = new Cactus(dev.Get(), L"cactus.obj", L"cactus.dds", L"cactus_norm.dds", L"blank_spec.dds", new Fire(dev.Get(), L"fire01.dds", L"noise01.dds", L"alpha01.dds", *fire, fireShader), normShader);
	cactus8->Position(p[9]);
	cactus8->Raining(globe->GetRaining());
	cactus8->Snowing(globe->GetSnowing());
	cactus8->Sunny(globe->GetSunny());
	cactus8->GetFire()->Position(cactus8->Position());
	normObjectList.push_back(cactus8);
}

bool SnowGlobe::CameraInit()
{
	camera = new Camera();

	c1 = new Camera();

	c1->Position(DirectX::XMFLOAT3(0.0f, 20.0f, -200.0f));

	c2 = new Camera();

	c2->Position(DirectX::XMFLOAT3(0.0f, 30.0f, -50.0f));
	c2->Rotation(DirectX::XMFLOAT3(15.0f, 0.0f, 0.0f));

	c3 = new Camera();

	c3->Position(DirectX::XMFLOAT3(0.0f, 0.0f, -20.0f));

	camera = c1;

	return true;
}

void SnowGlobe::Reset()
{
	#pragma region ConfigLoad

	FILE* configFile;
	fopen_s(&configFile, "config.xml", "rb");

	configXML.LoadFile(configFile);

	tinyxml2::XMLNode *pRoot = configXML.FirstChild();
	tinyxml2::XMLElement *pElement = pRoot->FirstChildElement("Objects");

	tinyxml2::XMLElement *pListElement = pElement->FirstChildElement("Position");
	std::vector<DirectX::XMFLOAT3> posList;

	while(pListElement != nullptr)
	{
		float x, y, z;
		tinyxml2::XMLElement *e = pListElement->FirstChildElement("x");
		e->QueryFloatText(&x);
		e = pListElement->FirstChildElement("y");
		e->QueryFloatText(&y);
		e = pListElement->FirstChildElement("z");
		e->QueryFloatText(&z);

		posList.push_back(DirectX::XMFLOAT3(x, y, z));
		pListElement = pListElement->NextSiblingElement("Position");
	}

	DirectX::XMFLOAT3 sunDir, moonDir;
	float x, y, z;

	pElement = pRoot->FirstChildElement("Sun");
	pListElement = pElement->FirstChildElement("Direction");

	tinyxml2::XMLElement *e = pListElement->FirstChildElement("x");
	e->QueryFloatText(&x);
	e = pListElement->FirstChildElement("y");
	e->QueryFloatText(&y);
	e = pListElement->FirstChildElement("z");
	e->QueryFloatText(&z);

	sunDir = DirectX::XMFLOAT3(x, y, z);

	pElement = pRoot->FirstChildElement("Moon");
	pListElement = pElement->FirstChildElement("Direction");

	e = pListElement->FirstChildElement("x");
	e->QueryFloatText(&x);
	e = pListElement->FirstChildElement("y");
	e->QueryFloatText(&y);
	e = pListElement->FirstChildElement("z");
	e->QueryFloatText(&z);

	moonDir = DirectX::XMFLOAT3(x, y, z);

	int seasonLength;

	pElement = pRoot->FirstChildElement("Seasons");
	pListElement = pElement->FirstChildElement("Length");
	pListElement->QueryIntText(&seasonLength);

	fclose(configFile);

	#pragma endregion

	sun->LightDirection(sunDir);
	moon->LightDirection(moonDir);

	desert->Position(posList[0]);

	globe->Position(DirectX::XMFLOAT3(0.0f, -10.0f, 0.0f));
	globe->Reset();

	globeBase->Position(posList[1]);

	CactusInit(posList);
}

void SnowGlobe::ToggleVsync()
{
	vsyncEnabled = !vsyncEnabled;
}

void TW_CALL SetVsyncCB(void *clientData)
{
	static_cast<SnowGlobe *>(clientData)->ToggleVsync();
}

bool SnowGlobe::TweakInit()
{
	TwInit(TW_DIRECT3D11, dev.Get());
	TwWindowSize(wndWidth, wndHeight);

	twUsageBar = TwNewBar("UsageStats");
	TwAddSeparator(twUsageBar, "", " group='Graphics Stats' ");
	TwAddButton(twUsageBar, "Vsync", SetVsyncCB, this, " label='Vsync' group= 'Graphics Stats'");
	TwAddVarRO(twUsageBar, "FPS", TW_TYPE_UINT32, &fps, " label='FPS' group='Graphics Stats'");
	TwAddVarRO(twUsageBar, "CPU", TW_TYPE_DOUBLE, &cpu, " label='CPU (%)' group='Graphics Stats'");
	TwAddVarRO(twUsageBar, "UsedRAM", TW_TYPE_FLOAT, &usedRam, " label='RAM Used (MB)' group='Graphics Stats'");
	TwAddVarRO(twUsageBar, "TotalUsedRAM", TW_TYPE_STDSTRING, &ram, " label='Total RAM (MB)' group='Graphics Stats'");
	TwAddSeparator(twUsageBar, "", " group= 'Simulation Stats' ");
	TwAddVarRO(twUsageBar, "Time", TW_TYPE_UINT32, globe->GetHours(), " label='Time (hours)' group= 'Simulation Stats'");
	TwAddVarRW(twUsageBar, "TimePercent", TW_TYPE_FLOAT, globe->GetTime(), " label='Time (%)' group= 'Simulation Stats'");
	TwAddVarRO(twUsageBar, "Time Mod", TW_TYPE_FLOAT, &dtMod, " label='Time Modifier' group= 'Simulation Stats'");
	TwAddVarRO(twUsageBar, "Season", TW_TYPE_STDSTRING, globe->GetSeasonString(), " label='Season' group='Simulation Stats'");

	TwDefine(" UsageStats label='Usage Stats' size='250 350' valueswidth=75 ");

	return true;
}

void SnowGlobe::Update()
{
	#pragma region Timers
	fpsCounter->Update();
	cpuCounter->Update();
	ramCounter->Update();
	deltaTime->Update();
	
	fps = fpsCounter->FPS();
	cpu = cpuCounter->CPUUsage();
	usedRam = ramCounter->UsedRAM();
	totalUsedRam = ramCounter->TotalUsedRAM();
	ram = std::to_string((int)totalUsedRam) + "/" + std::to_string((int)totalRam);
	dt = (deltaTime->Time());
	#pragma endregion

	#pragma region InputHandler
	if(inputHandler.IsKeyDown(VK_ESCAPE))
		PostQuitMessage(0);
	if(inputHandler.IsKeyPressed(VK_SPACE))
		ResizeWindow(1680, 1050);
	if(inputHandler.IsKeyDown(VK_CONTROL))
		camera->RotateLock(false);
	if(inputHandler.IsKeyUp(VK_CONTROL))
		camera->RotateLock(true);

	if(inputHandler.IsKeyPressed('T'))
	{
		if(inputHandler.IsKeyDown(VK_SHIFT))
			dtMod += 0.2f;
		else
			dtMod -= 0.2f;
	}		
	
	if(inputHandler.IsKeyPressed(VK_F1))
		camera = c1;
	if(inputHandler.IsKeyPressed(VK_F2))
		camera = c2;
	if(inputHandler.IsKeyPressed(VK_F3))
		camera = c3;
	if(inputHandler.IsKeyPressed(VK_F4))
	{
		cactus1->GetFire()->Active(true);
	}

	if(inputHandler.IsKeyPressed('R'))
		Reset();

	if(inputHandler.IsKeyDown(VK_LEFT))
	{
		if(camera->RotateLock())
		{
			camera->Rotate(Camera::LEFT, dt);
		}
		else
		{
			camera->Move(Camera::LEFT, dt);
		}
	}
	if(inputHandler.IsKeyDown(VK_RIGHT))
	{
		if(camera->RotateLock())
		{
			camera->Rotate(Camera::RIGHT, dt);
		}
		else
		{
			camera->Move(Camera::RIGHT, dt);
		}
	}
	if(inputHandler.IsKeyDown(VK_UP))
	{
		if(camera->RotateLock())
		{
			camera->Rotate(Camera::FORWARD, dt);
		}
		else
		{
			camera->Move(Camera::FORWARD, dt);
		}
	}
	if(inputHandler.IsKeyDown(VK_DOWN))
	{
		if(camera->RotateLock())
		{
			camera->Rotate(Camera::BACKWARD, dt);
		}
		else
		{
			camera->Move(Camera::BACKWARD, dt);
		}
	}
	if(inputHandler.IsKeyDown(VK_PRIOR))	//PgUp
	{
		camera->Move(Camera::UP, dt);
	}
	if(inputHandler.IsKeyDown(VK_NEXT))	//PgDn
	{
		camera->Move(Camera::DOWN, dt);
	}

	if(inputHandler.IsKeyDown(0x57))	//W
	{
		if(camera->RotateLock())
		{
			camera->Rotate(Camera::FORWARD, dt);
		}
		else
		{
			camera->Move(Camera::FORWARD, dt);
		}
	}
	if(inputHandler.IsKeyDown(0x41))	//A
	{
		if(camera->RotateLock())
		{
			camera->Rotate(Camera::LEFT, dt);
		}
		else
		{
			camera->Move(Camera::LEFT, dt);
		}
	}
	if(inputHandler.IsKeyDown(0x53))	//S
	{
		if(camera->RotateLock())
		{
			camera->Rotate(Camera::BACKWARD, dt);
		}
		else
		{
			camera->Move(Camera::BACKWARD, dt);
		}
	}
	if(inputHandler.IsKeyDown(0x44))	//E
	{
		if(camera->RotateLock())
		{
			camera->Rotate(Camera::RIGHT, dt);
		}
		else
		{
			camera->Move(Camera::RIGHT, dt);
		}
	}
	if(inputHandler.IsKeyDown(0x51))	//Q
	{
		camera->Move(Camera::UP, dt);
	}
	if(inputHandler.IsKeyDown(0x45))	//E
	{
		camera->Move(Camera::DOWN, dt);
	}
	#pragma endregion

	dt *= dtMod;

	camera->Update();

	sun->Update(*globe->GetTime());
	moon->Update(*globe->GetTime());

	globe->Update(dt);

	rain->Update(devCon.Get(), dt);
	snow->Update(devCon.Get(), dt);
	//fireBase->Update(dt);

	for each (GameObject* o in colObjectList)
	{
		o->Update(dt);
	}

	for each (GameObject* o in texObjectList)
	{
		o->Update(dt);
	}

	for each (GameObject* o in litObjectList)
	{
		o->Update(dt);
	}

	for each (GameObject* o in normObjectList)
	{
		o->Update(dt);
	}

	for each (GameObject* o in normObjectList)
	{
		if(Cactus *c = dynamic_cast <Cactus*>(o))
		{
			c->Update(devCon.Get(), dt, globe->PrevSunny());
		}
		else
		{
			o->Update(dt);
		}
	}
}

void SnowGlobe::Render()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	DirectX::XMFLOAT4 diffuseColour[NUM_LIGHTS];
	DirectX::XMFLOAT3 lightDirection[NUM_LIGHTS];
	DirectX::XMFLOAT4 specularColour[NUM_LIGHTS];
	float specularIntensity[NUM_LIGHTS];

	diffuseColour[0] = sun->DiffuseColour();
	diffuseColour[1] = moon->DiffuseColour();
	lightDirection[0] = sun->LightDirection();
	lightDirection[1] = moon->LightDirection();
	specularColour[0] = sun->SpecularColour();
	specularColour[1] = moon->SpecularColour();
	specularIntensity[0] = sun->SpecularIntensity();
	specularIntensity[1] = moon->SpecularIntensity();

	BeginDraw();

	for each (GameObject* o in colObjectList)
	{
		o->Render(devCon.Get(), worldMatrix, &camera->ViewMatrix(), projMatrix);
	}

	for each (GameObject* o in texObjectList)
	{
		o->Render(devCon.Get(), worldMatrix, &camera->ViewMatrix(), projMatrix);
	}

	for each (GameObject* o in litObjectList)
	{
		o->Render(devCon.Get(), worldMatrix, &camera->ViewMatrix(), projMatrix, camera->Position(), 
				  diffuseColour, lightDirection, specularIntensity, specularColour);
	}

	for each (GameObject* o in normObjectList)
	{
		if(Cactus *c = dynamic_cast <Cactus*>(o))
		{
			c->Render(devCon.Get(), worldMatrix, &camera->ViewMatrix(), projMatrix, camera->Position(),
				diffuseColour, lightDirection, specularIntensity, specularColour);
		}
		else
		{
			o->Render(devCon.Get(), worldMatrix, &camera->ViewMatrix(), projMatrix, camera->Position(),
				diffuseColour, lightDirection, specularIntensity, specularColour);
		}
	}

	devCon->RSSetState(rasterStateFCull.Get());
	globe->Render(devCon.Get(), worldMatrix, &camera->ViewMatrix(), projMatrix);
	devCon->RSSetState(rasterStateBCull.Get());

	devCon->OMSetBlendState(particleBlendState.Get(), blendFactor, 0xffffffff);
	rain->Render(devCon.Get(), worldMatrix, &camera->ViewMatrix(), projMatrix);
	snow->Render(devCon.Get(), worldMatrix, &camera->ViewMatrix(), projMatrix);

	devCon->OMSetDepthStencilState(depthDisabledState.Get(), 0);
	devCon->OMSetBlendState(alphaBlendState.Get(), blendFactor, 0xffffffff);

	for each (GameObject* o in normObjectList)
	{
		if(Cactus *c = dynamic_cast <Cactus*>(o))
		{
			c->GetFire()->Render(devCon.Get(), worldMatrix, &camera->ViewMatrix(), projMatrix);
		}
	}

	//fireBase->Render(devCon.Get(), worldMatrix, &camera->ViewMatrix(), projMatrix);
	devCon->OMSetDepthStencilState(depthEnabledState.Get(), 0);

	
	globe->Render(devCon.Get(), worldMatrix, &camera->ViewMatrix(), projMatrix);
	devCon->OMSetBlendState(nAlphaBlendState.Get(), blendFactor, 0xffffffff);



	TwDraw();
	
	EndDraw();
}
