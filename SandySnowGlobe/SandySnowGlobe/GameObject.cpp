#include "GameObject.h"

GameObject::GameObject(ID3D11Device *device, const WCHAR *filename, Shader *objectShader)
{
	model = new Model();
	model->Init(device, filename);
	shader = objectShader;

	position = { 0, 0, 0 };
	rotation = { 0, 0, 0 };
	constantRotation = { 0, 0, 0 };
	scale = { 1.0f, 1.0f, 1.0f };
	yaw = 0.0f;
	pitch = 0.0f;
	roll = 0.0f;
	freeRotate = false;
}

GameObject::GameObject(ID3D11Device *device, const WCHAR *filename, const WCHAR *textureName, Shader *objectShader)
{
	model = new Model();
	model->Init(device, filename, textureName);
	shader = objectShader;

	position = { 0, 0, 0 };
	rotation = { 0, 0, 0 };
	constantRotation = { 0, 0, 0 };
	scale = { 1.0f, 1.0f, 1.0f };
	yaw = 0.0f;
	pitch = 0.0f;
	roll = 0.0f;
	freeRotate = false;
}


GameObject::GameObject(ID3D11Device *device, const WCHAR *filename, const WCHAR *skyTexture, const WCHAR *gradientTexture, Shader *objectShader)
{
	model = new Model();
	model->Init(device, filename, skyTexture, gradientTexture);
	shader = objectShader;

	position = { 0, 0, 0 };
	rotation = { 0, 0, 0 };
	constantRotation = { 0, 0, 0 };
	scale = { 1.0f, 1.0f, 1.0f };
	yaw = 0.0f;
	pitch = 0.0f;
	roll = 0.0f;
	freeRotate = false;
}

GameObject::GameObject(ID3D11Device *device, const WCHAR *filename, const WCHAR *colourTexture, const WCHAR *normalTexture, const WCHAR *specularTexture, Shader *objectShader)
{
	model = new Model();
	model->Init(device, filename, colourTexture, normalTexture, specularTexture);
	shader = objectShader;

	position = { 0, 0, 0 };
	rotation = { 0, 0, 0 };
	constantRotation = { 0, 0, 0 };
	scale = { 1.0f, 1.0f, 1.0f };
	yaw = 0.0f;
	pitch = 0.0f;
	roll = 0.0f;
	freeRotate = false;
}

GameObject::GameObject(ID3D11Device *device, const WCHAR *colourTexture, const WCHAR *noiseTexture, const WCHAR *alphaTexture, Shader *objectShader, bool billboard)
{
	model = new Model();
	model->InitBillboared(device, colourTexture, noiseTexture, alphaTexture);
	shader = objectShader;

	position = { 0, 0, 0 };
	rotation = { 0, 0, 0 };
	constantRotation = { 0, 0, 0 };
	scale = { 1.0f, 1.0f, 1.0f };
	yaw = 0.0f;
	pitch = 0.0f;
	roll = 0.0f;
	freeRotate = false;
}

GameObject::~GameObject()
{
	try
	{
		Memory::SafeDelete(model);
	}
	catch(int &e)
	{
		delete model;
	}
}


void GameObject::Update(float dt)
{
	if(freeRotate)
	{
		(rotation.x >= 360) ? rotation.x -= 360.0f : rotation.x += (constantRotation.x * dt);
		(rotation.y >= 360) ? rotation.y -= 360.0f : rotation.y += (constantRotation.y * dt);
		(rotation.z >= 360) ? rotation.z -= 360.0f : rotation.z += (constantRotation.z * dt);
	}

	pitch = rotation.x * DirectX::XM_PI / 180.0f;
	yaw = rotation.y * DirectX::XM_PI / 180.0f;
	roll = rotation.z * DirectX::XM_PI / 180.0f;
}


void GameObject::Render(ID3D11DeviceContext *devCon, const DirectX::XMFLOAT4X4 *wMatrix, DirectX::XMFLOAT4X4 *vMatrix, DirectX::XMFLOAT4X4 *pMatrix)
{
	DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(wMatrix);
	m = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&scale));
	m = DirectX::XMMatrixMultiply(m, DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll));
	m = DirectX::XMMatrixMultiply(m, DirectX::XMMatrixTranslation(position.x, position.y, position.z));
	DirectX::XMFLOAT4X4 worldTemp;
	DirectX::XMStoreFloat4x4(&worldTemp, m);

	model->Render(devCon);
	shader->Render(devCon, model->IndexCount(), &worldTemp, vMatrix, pMatrix, model->GetTexture(0));
}

//Order: Scale -> Rotation -> Translation
void GameObject::Render(ID3D11DeviceContext *devCon, const DirectX::XMFLOAT4X4 *wMatrix, DirectX::XMFLOAT4X4 *vMatrix, DirectX::XMFLOAT4X4 *pMatrix, DirectX::XMFLOAT3 cameraPosition,
	DirectX::XMFLOAT4 diffuseColour[], DirectX::XMFLOAT3 lightDirection[], float specularIntensity[], DirectX::XMFLOAT4 specularColour[])
{
	DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(wMatrix);
	m = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&scale));
	m = DirectX::XMMatrixMultiply(m, DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll));
	m = DirectX::XMMatrixMultiply(m, DirectX::XMMatrixTranslation(position.x, position.y, position.z));
	DirectX::XMFLOAT4X4 worldTemp;
	DirectX::XMStoreFloat4x4(&worldTemp, m);

	model->Render(devCon);
	if(model->TextureCount() == 1)
		shader->Render(devCon, model->IndexCount(), &worldTemp, vMatrix, pMatrix, model->GetTexture(0), cameraPosition, diffuseColour, lightDirection, specularIntensity, specularColour);
	else if(model->TextureCount() == 3)
	{
		ID3D11ShaderResourceView **textureArray = nullptr;
		shader->Render(devCon, model->IndexCount(), &worldTemp, vMatrix, pMatrix, model->GetTextureArray(textureArray), cameraPosition, diffuseColour, lightDirection, specularIntensity, specularColour);
		delete textureArray;
	}
}