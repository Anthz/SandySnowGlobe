#include "Fire.h"

Fire::Fire(ID3D11Device *device, const WCHAR *colourTexture, const WCHAR *noiseTexture, const WCHAR *alphaTexture, ParticleSystem fireSys, Shader *objectShader) : GameObject(device, colourTexture, noiseTexture, alphaTexture, objectShader, true)
{
	//fireParticles = fireSys;
	//fireParticles.Init(device, L"fire01.dds");
	scrollSpeeds = DirectX::XMFLOAT3(1.3f, 2.1f, 2.3f);
	scales = DirectX::XMFLOAT3(1.0f, 2.0f, 3.0f);
	distortion1 = DirectX::XMFLOAT2(0.1f, 0.2f);
	distortion2 = DirectX::XMFLOAT2(0.1f, 0.3f);
	distortion3 = DirectX::XMFLOAT2(0.1f, 0.1f);
	distortionScale = 0.8f;
	distortionBias = 0.5f;
	animTime = 0.0f;
	posOffset = 0.75f;
	active = false;
}

Fire::~Fire()
{
}

void Fire::Update(float dt)
{
	if(active)
	{
		animTime += dt;
		if(animTime > 1000.0f)
		{
			animTime = 0.0f;
		}
	}
}

void Fire::Render(ID3D11DeviceContext *devCon, const DirectX::XMFLOAT4X4 *wMatrix, DirectX::XMFLOAT4X4 *vMatrix, DirectX::XMFLOAT4X4 *pMatrix)
{
	if(active)
	{
		DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(wMatrix);
		m = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&scale));
		m = DirectX::XMMatrixMultiply(m, DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll));
		m = DirectX::XMMatrixMultiply(m, DirectX::XMMatrixTranslation(position.x, position.y + posOffset * scale.y, position.z));
		DirectX::XMFLOAT4X4 worldTemp;
		DirectX::XMStoreFloat4x4(&worldTemp, m);

		model->Render(devCon);
		shader->Render(devCon, model->IndexCount(), &worldTemp, vMatrix, pMatrix, model->GetTexture(0), model->GetTexture(1), model->GetTexture(2), animTime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
	}
}

void Fire::Shrink(float dt)
{
	if(scale.x < 1)
	{
		active = false;
	}
	else
	{
		scale.x *= 0.99f * dt;
		scale.y *= 0.99f * dt;
		scale.z *= 0.99f * dt;
	}
}

void Fire::Grow(float dt)
{
	if(scale.x < 4)
	{
		scale.x *= 1.2f * dt;
		scale.y *= 1.2f * dt;
		scale.z *= 1.2f * dt;
	}
}
