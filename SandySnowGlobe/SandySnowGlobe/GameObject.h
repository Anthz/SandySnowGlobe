#pragma once
#include <d3d11.h>
#include "DirectXMath.h"
#include "Shader.h"
#include "Model.h"
#include "DXUtil.h"

class GameObject
{
public:
	GameObject(ID3D11Device *device, const WCHAR *filename, Shader *objectShader);
	GameObject(ID3D11Device *device, const WCHAR *filename, const WCHAR *textureName, Shader *objectShader);
	GameObject(ID3D11Device *device, const WCHAR *filename, const WCHAR *skyTexture, const WCHAR *gradientTexture, Shader *objectShader);
	GameObject(ID3D11Device *device, const WCHAR *filename, const WCHAR *colourTexture, const WCHAR *normalTexture, const WCHAR *specularTexture, Shader *objectShader);
	GameObject(ID3D11Device *device, const WCHAR *colourTexture, const WCHAR *noiseTexture, const WCHAR *alphaTexture, Shader *objectShader, bool billboarded);
	virtual ~GameObject();

	virtual void Update(float dt);
	virtual void Render(ID3D11DeviceContext *devCon, const DirectX::XMFLOAT4X4 *wMatrix, DirectX::XMFLOAT4X4 *vMatrix, DirectX::XMFLOAT4X4 *pMatrix);
	virtual void Render(ID3D11DeviceContext *devCon, const DirectX::XMFLOAT4X4 *wMatrix, DirectX::XMFLOAT4X4 *vMatrix, DirectX::XMFLOAT4X4 *pMatrix, DirectX::XMFLOAT3 cameraPosition,
				DirectX::XMFLOAT4 diffuseColour[], DirectX::XMFLOAT3 lightDirection[], float specularIntensity[], DirectX::XMFLOAT4 specularColour[]);

	DirectX::XMFLOAT3 Position() const { return position; }
	void Position(const DirectX::XMFLOAT3 &val) { position = val; }

	DirectX::XMFLOAT3 Rotation() const { return rotation; }
	void Rotation(const DirectX::XMFLOAT3 &val) { rotation = val; }

	DirectX::XMFLOAT3 ConstantRotation() const { return constantRotation; }
	void ConstantRotation(const DirectX::XMFLOAT3 &val) { constantRotation = val; }

	DirectX::XMFLOAT3 Scale() const { return scale; }
	void Scale(const DirectX::XMFLOAT3 &val) { scale = val; }

	bool FreeRotate() const { return freeRotate; }
	void FreeRotate(const bool &val) { freeRotate = val; }

protected:
	Shader *shader;
	Model *model;

	DirectX::XMFLOAT3 position, rotation, constantRotation, scale;
	float pitch, yaw, roll;
	bool freeRotate;

private:
	GameObject& operator= (const GameObject&);
	GameObject(const GameObject&);

};

