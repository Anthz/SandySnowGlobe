#pragma once

#include "DirectXMath.h"

class Light
{
public:
	Light();
	~Light();

	void Update(float dayPercent);

	DirectX::XMFLOAT4 DiffuseColour() const { return diffuseColour; }
	void DiffuseColour(DirectX::XMFLOAT4 val) { diffuseColour = val; }

	DirectX::XMFLOAT3 LightDirection() const { return lightDirection; }
	void LightDirection(DirectX::XMFLOAT3 val) { lightDirection = val; }

	DirectX::XMFLOAT3 StartDirection() const { return startDirection; }
	void StartDirection(DirectX::XMFLOAT3 val) { startDirection = val; }

	float SpecularIntensity() const { return specularIntensity; }
	void SpecularIntensity(float val) { specularIntensity = val; }

	DirectX::XMFLOAT4 SpecularColour() const { return specularColour; }
	void SpecularColour(DirectX::XMFLOAT4 val) { specularColour = val; }

private:
	DirectX::XMFLOAT4 diffuseColour;
	DirectX::XMFLOAT3 lightDirection;
	DirectX::XMFLOAT3 startDirection;
	DirectX::XMFLOAT4 specularColour;
	float specularIntensity;
};

