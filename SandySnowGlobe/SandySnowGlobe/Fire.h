#pragma once
#include "GameObject.h"
#include "ParticleSystem.h"


class Fire : public GameObject
{
public:
	Fire(ID3D11Device *device, const WCHAR *colourTexture, const WCHAR *noiseTexture, const WCHAR *alphaTexture, ParticleSystem fireSys, Shader *objectShader);
	~Fire();

	void Update(float dt);
	void Render(ID3D11DeviceContext *devCon, const DirectX::XMFLOAT4X4 *wMatrix, DirectX::XMFLOAT4X4 *vMatrix, DirectX::XMFLOAT4X4 *pMatrix) override;
	void Shrink(float dt);
	void Grow(float dt);
	bool Active() const { return active; }
	void Active(bool val) { active = val; }
	
private:
	ParticleSystem fireParticles;

	DirectX::XMFLOAT3 scrollSpeeds, scales;
	DirectX::XMFLOAT2 distortion1, distortion2, distortion3;
	float distortionScale, distortionBias, animTime, posOffset;
	bool active;
};

