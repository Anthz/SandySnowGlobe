#pragma once

#include "GameObject.h"
#include "Season.h"
#include "ParticleSystem.h"
#include "DirectXMath.h"
#include "AntTweakBar.h"

class SkyDome : public GameObject
{
public:
	SkyDome(ID3D11Device *device, const WCHAR *filename, const WCHAR *skyTexture, const WCHAR *gradientTexture, Shader *objectShader, ParticleSystem *rainSys, ParticleSystem *snowSys);
	virtual ~SkyDome();

	void Update(float dt);
	void Render(ID3D11DeviceContext *devCon, const DirectX::XMFLOAT4X4 *wMatrix, DirectX::XMFLOAT4X4 *vMatrix, DirectX::XMFLOAT4X4 *pMatrix);

	void SetTime(float t);
	float *GetTime(){ return &currentTime.x ; }
	unsigned int *GetHours();
	float *GetTimeStep(){ return &stepAmount; }
	std::string *GetSeasonString() { return season.GetSeasonString(); }

	float GetRainChance();
	float GetSnowChance();

	bool *GetRaining() { return rain->Active(); }
	bool *GetSnowing() { return snow->Active(); };
	bool *GetSunny() { return sunny; }
	bool PrevSunny() { return prevSunny; }
	void PrevSunny(const bool val) { prevSunny = val; }

	unsigned int SeasonLength() const { return seasonLength; }
	void SeasonLength(unsigned int val) { seasonLength = val; }

	void Reset();

private:
	Season season;
	unsigned int hours, days, seasonLength;
	float stepCounter, stepAmount;
	DirectX::XMFLOAT3 currentTime;
	ParticleSystem *rain, *snow;
	bool *particleActive, *sunny, prevSunny;
	
};

