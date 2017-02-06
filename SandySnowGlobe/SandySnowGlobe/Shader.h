#pragma once

#include <d3d11.h>
#include "DirectXMath.h"
#include <fstream>
#include <wrl.h>
#include <d3dcompiler.h>
#include "DXUtil.h"

const int NUM_LIGHTS = 2;

class Shader
{
public:
	struct MatricesBuffer
	{
		DirectX::XMFLOAT4X4 worldMatrix;
		DirectX::XMFLOAT4X4 viewMatrix;
		DirectX::XMFLOAT4X4 projMatrix;
	};

	struct MatricesInvBuffer
	{
		DirectX::XMFLOAT4X4 worldMatrix;
		DirectX::XMFLOAT4X4 viewMatrix;
		DirectX::XMFLOAT4X4 viewInvMatrix;
		DirectX::XMFLOAT4X4 projMatrix;
	};

	struct LightBuffer
	{
		DirectX::XMFLOAT4 sDiffuseColour;
		DirectX::XMFLOAT3 sLightDirection;
		float sSpecularIntensity;
		DirectX::XMFLOAT4 sSpecularColour;
		DirectX::XMFLOAT4 mDiffuseColour;
		DirectX::XMFLOAT3 mLightDirection;
		float mSpecularIntensity;
		DirectX::XMFLOAT4 mSpecularColour;
	};

	struct CameraBuffer
	{
		DirectX::XMFLOAT3 cameraPosition;
		float padding;
	};

	struct TimeBuffer
	{
		DirectX::XMFLOAT3 time;
		float padding;
	};

	struct NoiseBuffer
	{
		float animTime;
		DirectX::XMFLOAT3 scrollSpeeds;
		DirectX::XMFLOAT3 scales;
		float padding;
	};

	struct DistortionBuffer
	{
		DirectX::XMFLOAT2 distortion1;
		DirectX::XMFLOAT2 distortion2;
		DirectX::XMFLOAT2 distortion3;
		float distortionScale;
		float distortionBias;
	};

	enum ShaderType
	{
		COLOUR,
		TEXTURE,
		LIGHTS,
		NORMAL,
		SKYDOME,
		PARTICLE,
		FIRE
	};

	explicit Shader(ShaderType shaderType);
	~Shader();

	bool Init(ID3D11Device *dev, const std::wstring &vsFile, const std::wstring &psFile);

	void Render(ID3D11DeviceContext *devCon, unsigned int indexCount, const DirectX::XMFLOAT4X4 *worldMatrix, const DirectX::XMFLOAT4X4 *viewMatrix,
		const DirectX::XMFLOAT4X4 *projMatrix, ID3D11ShaderResourceView **textureArray, DirectX::XMFLOAT3 time);

	void Render(ID3D11DeviceContext *devCon, unsigned int indexCount, const DirectX::XMFLOAT4X4 *worldMatrix,
		const DirectX::XMFLOAT4X4 *viewMatrix, const DirectX::XMFLOAT4X4 *projMatrix, ID3D11ShaderResourceView *texture);

	void Render(ID3D11DeviceContext *devCon, unsigned int indexCount, const DirectX::XMFLOAT4X4 *worldMatrix,
		const DirectX::XMFLOAT4X4 *viewMatrix, const DirectX::XMFLOAT4X4 *projMatrix, ID3D11ShaderResourceView *texture, DirectX::XMFLOAT3 cameraPosition,
		DirectX::XMFLOAT4 diffuseColour[], DirectX::XMFLOAT3 lightDirection[], float specularIntensity[], DirectX::XMFLOAT4 specularColour[]);

	void Render(ID3D11DeviceContext *devCon, unsigned int indexCount, const DirectX::XMFLOAT4X4 *worldMatrix,
		const DirectX::XMFLOAT4X4 *viewMatrix, const DirectX::XMFLOAT4X4 *projMatrix, ID3D11ShaderResourceView **texture, DirectX::XMFLOAT3 cameraPosition,
		DirectX::XMFLOAT4 diffuseColour[], DirectX::XMFLOAT3 lightDirection[], float specularIntensity[], DirectX::XMFLOAT4 specularColour[]);

	void Render(ID3D11DeviceContext *devCon, unsigned int indexCount, const DirectX::XMFLOAT4X4 *worldMatrix, const DirectX::XMFLOAT4X4 *viewMatrix, const DirectX::XMFLOAT4X4 *projMatrix,
		ID3D11ShaderResourceView *texture1, ID3D11ShaderResourceView *texture2, ID3D11ShaderResourceView *texture3, float animTime, DirectX::XMFLOAT3 scrollSpeeds, DirectX::XMFLOAT3 scales,
		DirectX::XMFLOAT2 distortion1, DirectX::XMFLOAT2 distortion2, DirectX::XMFLOAT2 distortion3, float distortionScale, float distortionBias);

private:
	ShaderType type;

	bool InitColourShader(ID3D11Device *dev, const std::wstring &vsFile, const std::wstring &psFile);
	bool InitTextureShader(ID3D11Device *dev, const std::wstring &vsFile, const std::wstring &psFile);
	bool InitLightShader(ID3D11Device *dev, const std::wstring &vsFile, const std::wstring &psFile);
	bool InitNormalShader(ID3D11Device * dev, const std::wstring &vsFile, const std::wstring &psFile);
	bool InitSkyDomeShader(ID3D11Device *dev, const std::wstring &vsFile, const std::wstring &psFile);
	bool InitParticleShader(ID3D11Device * dev, const std::wstring & vsFile, const std::wstring & psFile);
	bool InitFireShader(ID3D11Device *dev, const std::wstring &vsFile, const std::wstring &psFile);
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> matrixBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> lightBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cameraBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> timeBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> noiseBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> distortionBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampleState;		//wrap
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampleState2;	//clamp
};

