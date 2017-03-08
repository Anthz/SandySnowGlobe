#include "SkyDome.h"


SkyDome::SkyDome(ID3D11Device *device, const WCHAR *filename, const WCHAR *skyTexture, const WCHAR *gradientTexture, Shader *objectShader, ParticleSystem *rainSys, ParticleSystem *snowSys) : GameObject(device, filename, skyTexture, gradientTexture, objectShader)
{
	season = Season(Season::SPRING);
	currentTime = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	stepCounter = 0.0f;
	stepAmount = 0.025f;	//0.0005f
	hours = 0;
	days = 0;
	seasonLength = 0;
	rain = rainSys;
	snow = snowSys;
	particleActive = new bool(false);
	sunny = new bool(false);
	prevSunny = false;
}

SkyDome::~SkyDome()
{
	try
	{
		Memory::SafeDelete(sunny);
	}
	catch(int &e)
	{
		delete sunny;
	}
}

void SkyDome::Reset()
{
	season = Season(Season::SPRING);
	currentTime = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	stepCounter = 0.0f;
	stepAmount = 0.0005f;
	hours = 0;
	days = 0;
	seasonLength = 0;
	*particleActive = false;
	*sunny = false;
	prevSunny = false;
}

void SkyDome::SetTime(float t)
{
	currentTime.x = t;
}

unsigned int *SkyDome::GetHours()
{
	return &hours;
}

/// <summary>
/// Handles day-night cycles and weather effects
/// </summary>
/// <param name="dt">Delta time</param>
void SkyDome::Update(float dt)
{
	currentTime.x += stepAmount * dt;
	currentTime.y = ((int)(currentTime.x / 0.03125f) * 0.03125) + 0.03125f;

	hours = currentTime.x * 24.0f;

	if((hours >= 8 && hours <= 16) && !*particleActive)
	{
		*sunny = true;
	}
	else
	{
		*sunny = false;
	}

	if(hours >= 24)
	{
		if(*particleActive)
		{
			prevSunny = false;
		}
		else
		{
			prevSunny = true;
		}

		currentTime.x -= 1.0f;
		hours = std::fmin(currentTime.x * 24.0f, 24.0f);

		rain->Active(false);
		snow->Active(false);

		if(((float)(rand()) / (float)(RAND_MAX)) <= GetRainChance() && !(*particleActive))
		{
			particleActive = rain->Active();
			rain->Active(true);
		}

		if(((float)(rand()) / (float)(RAND_MAX)) <= GetSnowChance() && !(*particleActive))
		{
			particleActive = snow->Active();
			snow->Active(true);
		}

		days++;

		if(days != 0 && days % seasonLength == 0)
		{
			season++;
		}
	}

	if(currentTime.y > 1.0f)
	{
		currentTime.y = 0.0f;
	}

	if(currentTime.y <= 0.0000001f)
	{
		currentTime.z = currentTime.x / (currentTime.y + 1.0f);
	}
	else
	{
		//% for lerp
		currentTime.z = (currentTime.x - ((int)(currentTime.x / 0.03125f) * 0.03125)) / (currentTime.y - ((int)(currentTime.x / 0.03125f) * 0.03125));
	}
}

void SkyDome::Render(ID3D11DeviceContext *devCon, const DirectX::XMFLOAT4X4 *wMatrix, DirectX::XMFLOAT4X4 *vMatrix, DirectX::XMFLOAT4X4 *pMatrix)
{
	DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(wMatrix);
	m = DirectX::XMMatrixMultiply(m, DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll));
	m = DirectX::XMMatrixMultiply(m, DirectX::XMMatrixTranslation(position.x, position.y, position.z));
	DirectX::XMFLOAT4X4 worldTemp;
	DirectX::XMStoreFloat4x4(&worldTemp, m);

	model->Render(devCon);
	ID3D11ShaderResourceView **textureArray = nullptr;
	shader->Render(devCon, model->IndexCount(), &worldTemp, vMatrix, pMatrix, model->GetTextureArray(textureArray), currentTime);
	delete textureArray;
}

float SkyDome::GetRainChance()
{
	return season.GetRainChance(*season.CurrentSeason());
}

float SkyDome::GetSnowChance()
{
	return season.GetSnowChance(*season.CurrentSeason());
}

