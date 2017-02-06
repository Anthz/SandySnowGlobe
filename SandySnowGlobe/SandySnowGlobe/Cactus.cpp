#include "Cactus.h"

Cactus::Cactus(ID3D11Device *device, const WCHAR *filename, const WCHAR *colourTexture, const WCHAR *normalTexture, const WCHAR *specularTexture, Fire *fireSys, Shader *objectShader) : GameObject(device, filename, colourTexture, normalTexture, specularTexture, objectShader)
{
	fire = fireSys;
	maxScale = 1.5f;
	prev = false;
	grow = false;
	prevSunny = false;
	growCount = 0;
	raining = nullptr;
	snowing = nullptr;
	sunny = nullptr;
}

Cactus::~Cactus()
{
	delete fire;
}

void Cactus::Update(ID3D11DeviceContext *devCon, float dt, bool prevSun)
{
	GameObject::Update(dt);

	//checks rain/snow pointer to bool
	if(!*raining && !*snowing)
	{
		if(prev && *sunny)	//if prev watered (rain or snow) && sunny -> grow
		{
			grow = true;
			prev = false;
		}

		if(prevSun && *sunny)	//if prev sunny && sunny, chance for ignition
		{
			if(((float)(rand()) / (float)(RAND_MAX)) <= 0.001f)
			{
				fire->Active(true);
			}
			if(fire->Active())
			{
				if(((float)(rand()) / (float)(RAND_MAX)) <= 0.005f)
				{
					fire->Grow(dt);	//if already on fire, chance for fire to grow
				}
			}
		}
	}

	if(*raining || *snowing)
	{
		fire->Shrink(dt);	//if fire active, shrink whilst raining/snowing until min scale
		if(prev == false)
		{
			prev = true;
		}
	}

	//if grow -> scale cactus over time
	if(grow)
	{
		if(!(scale.x > maxScale))
		{
			scale = DirectX::XMFLOAT3(scale.x + (0.05 * dt), scale.y + (0.1 * dt), scale.z + (0.05 * dt));
		}

		growCount++;

		if(growCount >= 100)
		{
			grow = false;
			growCount = 0;
		}
	}

	fire->Update(dt);
}

void Cactus::Render(ID3D11DeviceContext *devCon, const DirectX::XMFLOAT4X4 *wMatrix, DirectX::XMFLOAT4X4 *vMatrix, DirectX::XMFLOAT4X4 *pMatrix, DirectX::XMFLOAT3 cameraPosition, DirectX::XMFLOAT4 diffuseColour[], DirectX::XMFLOAT3 lightDirection[], float specularIntensity[], DirectX::XMFLOAT4 specularColour[])
{
	GameObject::Render(devCon, wMatrix, vMatrix, pMatrix, cameraPosition, diffuseColour, lightDirection, specularIntensity, specularColour);
}