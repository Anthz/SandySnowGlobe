#pragma once

#include "GameObject.h"
#include "ParticleSystem.h"
#include "Fire.h"

class Cactus : public GameObject
{
public:
	Cactus(ID3D11Device *device, const WCHAR *filename, const WCHAR *colourTexture, const WCHAR *normalTexture, const WCHAR *specularTexture, Fire *fireSys, Shader *objectShader);
	virtual ~Cactus();

	void Update(ID3D11DeviceContext *devCon, float dt, bool prevSun);

	bool *Snowing() const { return snowing; }
	void Snowing(bool *val) { snowing = val; }
	bool *Raining() const { return raining; }
	void Raining(bool *val) { raining = val; }
	const bool *Sunny() { return sunny; }
	void Sunny(bool *val) { sunny = val; }
	Fire *GetFire() { return fire; }

	void Render(ID3D11DeviceContext *devCon, const DirectX::XMFLOAT4X4 *wMatrix, DirectX::XMFLOAT4X4 *vMatrix, DirectX::XMFLOAT4X4 *pMatrix, DirectX::XMFLOAT3 cameraPosition, DirectX::XMFLOAT4 diffuseColour[], DirectX::XMFLOAT3 lightDirection[], float specularIntensity[], DirectX::XMFLOAT4 specularColour[]);

private:
	Fire *fire;

	float maxScale;
	bool *raining, *snowing, *sunny, prev, prevSunny, grow;
	unsigned int growCount;
};

