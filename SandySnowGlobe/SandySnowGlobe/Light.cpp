#include "Light.h"

Light::Light()
{
	diffuseColour = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	lightDirection = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
	startDirection = lightDirection;
	specularIntensity = 100.0f;
	specularColour = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

Light::~Light()
{
}

void Light::Update(float dayPercent)
{
	DirectX::XMVECTOR orbitQuaternion = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), dayPercent * DirectX::XM_2PI);
	DirectX::XMVECTOR v = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&startDirection) , orbitQuaternion);
	DirectX::XMStoreFloat3(&lightDirection, v);
}
