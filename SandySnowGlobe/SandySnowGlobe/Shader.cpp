#include "Shader.h"


Shader::Shader(ShaderType shaderType)
{
	type = shaderType;
	vertexShader = nullptr;
	pixelShader = nullptr;
	inputLayout = nullptr;
	matrixBuffer = nullptr;
	sampleState = nullptr;
	lightBuffer = nullptr;
	cameraBuffer = nullptr;
	timeBuffer = nullptr;
	noiseBuffer = nullptr;
	distortionBuffer = nullptr;
}

Shader::~Shader()
{
	//all comptrs
}

/// <summary>
/// Initialise various shader types
/// </summary>
/// <param name="dev">Standard ID3D11Device</param>
/// <param name="vsFile">Vertex shader filepath</param>
/// <param name="psFile">Pixel shader filepath</param>
/// <returns></returns>
bool Shader::Init(ID3D11Device *dev, const std::wstring &vsFile, const std::wstring &psFile)
{
	switch(type)
	{
		case Shader::COLOUR:
			return InitColourShader(dev, vsFile, psFile);
			break;
		case Shader::TEXTURE:
			return InitTextureShader(dev, vsFile, psFile);
			break;
		case Shader::LIGHTS:
			return InitLightShader(dev, vsFile, psFile);
			break;
		case Shader::NORMAL:	//normal mapping w/specular mapping
			return InitNormalShader(dev, vsFile, psFile);
			break;
		case Shader::SKYDOME:
			return InitSkyDomeShader(dev, vsFile, psFile);
			break;
		case Shader::PARTICLE:
			return InitParticleShader(dev, vsFile, psFile);
			break;
		case Shader::FIRE:
			return InitFireShader(dev, vsFile, psFile);
			break;
	}

	return false;
}

/// <summary>
/// Render method for skydome
/// </summary>
void Shader::Render(ID3D11DeviceContext *devCon, unsigned int indexCount, const DirectX::XMFLOAT4X4 *worldMatrix, const DirectX::XMFLOAT4X4 *viewMatrix, const DirectX::XMFLOAT4X4 *projMatrix, ID3D11ShaderResourceView **textureArray, DirectX::XMFLOAT3 time)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	MatricesBuffer *matricesPtr;
	TimeBuffer *timePtr;
	unsigned int bufferID = 0;

	DirectX::XMMATRIX worldMatrixCopy = DirectX::XMLoadFloat4x4(worldMatrix);
	DirectX::XMMATRIX viewMatrixCopy = DirectX::XMLoadFloat4x4(viewMatrix);
	DirectX::XMMATRIX projectionMatrixCopy = DirectX::XMLoadFloat4x4(projMatrix);

	//transpose copy (editing original caused flickering)
	worldMatrixCopy = XMMatrixTranspose(worldMatrixCopy);
	viewMatrixCopy = XMMatrixTranspose(viewMatrixCopy);
	projectionMatrixCopy = XMMatrixTranspose(projectionMatrixCopy);

	HRESULT result = devCon->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if(result != S_OK)
	{
		return;
	}

	matricesPtr = (MatricesBuffer*)resource.pData;
	DirectX::XMStoreFloat4x4(&(matricesPtr->worldMatrix), worldMatrixCopy);
	DirectX::XMStoreFloat4x4(&(matricesPtr->viewMatrix), viewMatrixCopy);
	DirectX::XMStoreFloat4x4(&(matricesPtr->projMatrix), projectionMatrixCopy);

	devCon->Unmap(matrixBuffer.Get(), 0);
	devCon->VSSetConstantBuffers(bufferID, 1, matrixBuffer.GetAddressOf());
	devCon->PSSetShaderResources(0, 2, textureArray);

	result = devCon->Map(timeBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if(result != S_OK)
	{
		return;
	}

	timePtr = (TimeBuffer*)resource.pData;
	timePtr->padding = 0.0f;
	timePtr->time = time;

	devCon->Unmap(timeBuffer.Get(), 0);
	devCon->PSSetConstantBuffers(bufferID, 1, timeBuffer.GetAddressOf());

	devCon->IASetInputLayout(inputLayout.Get());
	devCon->VSSetShader(vertexShader.Get(), nullptr, 0);
	devCon->PSSetShader(pixelShader.Get(), nullptr, 0);
	devCon->DrawIndexed(indexCount, 0, 0);
}

/// <summary>
/// Render method for simple colour texture
/// </summary>
void Shader::Render(ID3D11DeviceContext *devCon, unsigned int indexCount, const DirectX::XMFLOAT4X4 *worldMatrix, const DirectX::XMFLOAT4X4 *viewMatrix, const DirectX::XMFLOAT4X4 *projMatrix, ID3D11ShaderResourceView *texture)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	unsigned int bufferID = 0;

	if(type == PARTICLE)
	{
		MatricesInvBuffer *matricesPtr;
		DirectX::XMFLOAT4X4 inv;
		DirectX::XMMATRIX worldMatrixCopy = DirectX::XMLoadFloat4x4(worldMatrix);
		DirectX::XMMATRIX viewMatrixCopy = DirectX::XMLoadFloat4x4(viewMatrix);
		DirectX::XMMATRIX projectionMatrixCopy = DirectX::XMLoadFloat4x4(projMatrix);

		DirectX::XMMATRIX viewInv = DirectX::XMMatrixInverse(nullptr, viewMatrixCopy);
		DirectX::XMStoreFloat4x4(&inv, viewInv);
		inv._41 = 0;
		inv._42 = 0;
		inv._43 = 0;
		viewInv = DirectX::XMLoadFloat4x4(&inv);

		worldMatrixCopy = XMMatrixTranspose(worldMatrixCopy);
		viewMatrixCopy = XMMatrixTranspose(viewMatrixCopy);
		projectionMatrixCopy = XMMatrixTranspose(projectionMatrixCopy);
		viewInv = XMMatrixTranspose(viewInv);

		HRESULT result = devCon->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		if(result != S_OK)
		{
			return;
		}

		matricesPtr = (MatricesInvBuffer*)resource.pData;
		DirectX::XMStoreFloat4x4(&(matricesPtr->worldMatrix), worldMatrixCopy);
		DirectX::XMStoreFloat4x4(&(matricesPtr->viewMatrix), viewMatrixCopy);
		DirectX::XMStoreFloat4x4(&(matricesPtr->projMatrix), projectionMatrixCopy);
		DirectX::XMStoreFloat4x4(&(matricesPtr->viewInvMatrix), viewInv);

		devCon->Unmap(matrixBuffer.Get(), 0);
		devCon->VSSetConstantBuffers(bufferID, 1, matrixBuffer.GetAddressOf());
		devCon->PSSetShaderResources(0, 1, &texture);
		devCon->IASetInputLayout(inputLayout.Get());
		devCon->VSSetShader(vertexShader.Get(), nullptr, 0);
		devCon->PSSetShader(pixelShader.Get(), nullptr, 0);
		devCon->PSSetSamplers(0, 1, sampleState.GetAddressOf());
		devCon->DrawInstanced(6, indexCount, 0, 0);
	}
	else
	{
		MatricesBuffer *matricesPtr;

		DirectX::XMMATRIX worldMatrixCopy = DirectX::XMLoadFloat4x4(worldMatrix);
		DirectX::XMMATRIX viewMatrixCopy = DirectX::XMLoadFloat4x4(viewMatrix);
		DirectX::XMMATRIX projectionMatrixCopy = DirectX::XMLoadFloat4x4(projMatrix);

		//transpose copy (editing original caused flickering)
		worldMatrixCopy = XMMatrixTranspose(worldMatrixCopy);
		viewMatrixCopy = XMMatrixTranspose(viewMatrixCopy);
		projectionMatrixCopy = XMMatrixTranspose(projectionMatrixCopy);

		HRESULT result = devCon->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		if(result != S_OK)
		{
			return;
		}

		matricesPtr = (MatricesBuffer*)resource.pData;
		DirectX::XMStoreFloat4x4(&(matricesPtr->worldMatrix), worldMatrixCopy);
		DirectX::XMStoreFloat4x4(&(matricesPtr->viewMatrix), viewMatrixCopy);
		DirectX::XMStoreFloat4x4(&(matricesPtr->projMatrix), projectionMatrixCopy);

		devCon->Unmap(matrixBuffer.Get(), 0);
		devCon->VSSetConstantBuffers(bufferID, 1, matrixBuffer.GetAddressOf());
		devCon->PSSetShaderResources(0, 1, &texture);
		devCon->IASetInputLayout(inputLayout.Get());
		devCon->VSSetShader(vertexShader.Get(), nullptr, 0);
		devCon->PSSetShader(pixelShader.Get(), nullptr, 0);
		devCon->PSSetSamplers(0, 1, sampleState.GetAddressOf());
		devCon->DrawIndexed(indexCount, 0, 0);
	}
}

/// <summary>
/// Render method for single colour texture based lighting
/// </summary>
void Shader::Render(ID3D11DeviceContext *devCon, unsigned int indexCount, const DirectX::XMFLOAT4X4 *worldMatrix, const DirectX::XMFLOAT4X4 *viewMatrix, const DirectX::XMFLOAT4X4 *projMatrix, ID3D11ShaderResourceView *texture,
					DirectX::XMFLOAT3 cameraPosition, DirectX::XMFLOAT4 diffuseColour[], DirectX::XMFLOAT3 lightDirection[], float specularIntensity[], DirectX::XMFLOAT4 specularColour[])
{
	D3D11_MAPPED_SUBRESOURCE resource;
	MatricesBuffer *matricesPtr;
	LightBuffer *lightPtr;
	CameraBuffer *cameraPtr;
	unsigned int bufferID = 0;

	DirectX::XMMATRIX worldMatrixCopy = DirectX::XMLoadFloat4x4(worldMatrix);
	DirectX::XMMATRIX viewMatrixCopy = DirectX::XMLoadFloat4x4(viewMatrix);
	DirectX::XMMATRIX projectionMatrixCopy = DirectX::XMLoadFloat4x4(projMatrix);

	//transpose copy (editing original caused flickering)
	worldMatrixCopy = XMMatrixTranspose(worldMatrixCopy);
	viewMatrixCopy = XMMatrixTranspose(viewMatrixCopy);
	projectionMatrixCopy = XMMatrixTranspose(projectionMatrixCopy);

	HRESULT result = devCon->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if(result != S_OK)
	{
		return;
	}

	matricesPtr = (MatricesBuffer*)resource.pData;
	DirectX::XMStoreFloat4x4(&(matricesPtr->worldMatrix), worldMatrixCopy);
	DirectX::XMStoreFloat4x4(&(matricesPtr->viewMatrix), viewMatrixCopy);
	DirectX::XMStoreFloat4x4(&(matricesPtr->projMatrix), projectionMatrixCopy);

	devCon->Unmap(matrixBuffer.Get(), 0);
	devCon->VSSetConstantBuffers(bufferID, 1, matrixBuffer.GetAddressOf());
	bufferID++;

	result = devCon->Map(cameraBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if(result != S_OK)
	{
		return;
	}

	cameraPtr = (CameraBuffer *)resource.pData;
	cameraPtr->cameraPosition = cameraPosition;

	devCon->Unmap(cameraBuffer.Get(), 0);
	devCon->VSSetConstantBuffers(bufferID, 1, cameraBuffer.GetAddressOf());
	bufferID--;

	result = devCon->Map(lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if(result != S_OK)
	{
		return;
	}

	lightPtr = (LightBuffer *)resource.pData;
	lightPtr->sDiffuseColour = diffuseColour[0];
	lightPtr->sLightDirection = lightDirection[0];
	lightPtr->sSpecularIntensity = specularIntensity[0];
	lightPtr->sSpecularColour = specularColour[0];
	
	lightPtr->mDiffuseColour = diffuseColour[1];
	lightPtr->mLightDirection = lightDirection[1];
	lightPtr->mSpecularIntensity = specularIntensity[1];
	lightPtr->mSpecularColour = specularColour[1];
	

// 	for(int i = 0; i < NUM_LIGHTS; i++)
// 	{
// 		lightPtr->diffuseColour[i] = diffuseColour[i];
// 		lightPtr->lightDirection[i] = lightDirection[i];
// 		lightPtr->specularIntensity[i] = specularIntensity[i];
// 		lightPtr->specularColour[i] = specularColour[i];
// 	}

	devCon->Unmap(lightBuffer.Get(), 0);
	devCon->PSSetConstantBuffers(bufferID, 1, lightBuffer.GetAddressOf());
	devCon->PSSetShaderResources(0, 1, &texture);

	devCon->IASetInputLayout(inputLayout.Get());
	devCon->VSSetShader(vertexShader.Get(), nullptr, 0);
	devCon->PSSetShader(pixelShader.Get(), nullptr, 0);
	devCon->PSSetSamplers(0, 1, sampleState.GetAddressOf());
	devCon->DrawIndexed(indexCount, 0, 0);
}

/// <summary>
/// Render method for multi-texture lighting (colour, norm, spec)
/// </summary>
void Shader::Render(ID3D11DeviceContext *devCon, unsigned int indexCount, const DirectX::XMFLOAT4X4 *worldMatrix, const DirectX::XMFLOAT4X4 *viewMatrix, const DirectX::XMFLOAT4X4 *projMatrix, ID3D11ShaderResourceView **textureArray,
					DirectX::XMFLOAT3 cameraPosition, DirectX::XMFLOAT4 diffuseColour[], DirectX::XMFLOAT3 lightDirection[], float specularIntensity[], DirectX::XMFLOAT4 specularColour[])
{
	D3D11_MAPPED_SUBRESOURCE resource;
	MatricesBuffer *matricesPtr;
	LightBuffer *lightPtr;
	CameraBuffer *cameraPtr;
	unsigned int bufferID = 0;

	DirectX::XMMATRIX worldMatrixCopy = DirectX::XMLoadFloat4x4(worldMatrix);
	DirectX::XMMATRIX viewMatrixCopy = DirectX::XMLoadFloat4x4(viewMatrix);
	DirectX::XMMATRIX projectionMatrixCopy = DirectX::XMLoadFloat4x4(projMatrix);

	//transpose copy (editing original caused flickering)
	worldMatrixCopy = XMMatrixTranspose(worldMatrixCopy);
	viewMatrixCopy = XMMatrixTranspose(viewMatrixCopy);
	projectionMatrixCopy = XMMatrixTranspose(projectionMatrixCopy);

	HRESULT result = devCon->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if(result != S_OK)
	{
		return;
	}

	matricesPtr = (MatricesBuffer*)resource.pData;
	DirectX::XMStoreFloat4x4(&(matricesPtr->worldMatrix), worldMatrixCopy);
	DirectX::XMStoreFloat4x4(&(matricesPtr->viewMatrix), viewMatrixCopy);
	DirectX::XMStoreFloat4x4(&(matricesPtr->projMatrix), projectionMatrixCopy);

	devCon->Unmap(matrixBuffer.Get(), 0);
	devCon->VSSetConstantBuffers(bufferID, 1, matrixBuffer.GetAddressOf());
	
	devCon->PSSetShaderResources(0, 3, textureArray);

	result = devCon->Map(lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if(result != S_OK)
	{
		return;
	}

	lightPtr = (LightBuffer *)resource.pData;
	lightPtr->sDiffuseColour = diffuseColour[0];
	lightPtr->sLightDirection = lightDirection[0];
	lightPtr->sSpecularIntensity = specularIntensity[0];
	lightPtr->sSpecularColour = specularColour[0];

	lightPtr->mDiffuseColour = diffuseColour[1];
	lightPtr->mLightDirection = lightDirection[1];
	lightPtr->mSpecularIntensity = specularIntensity[1];
	lightPtr->mSpecularColour = specularColour[1];

	devCon->Unmap(lightBuffer.Get(), 0);
	devCon->PSSetConstantBuffers(bufferID, 1, lightBuffer.GetAddressOf());

	result = devCon->Map(cameraBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if(result != S_OK)
	{
		return;
	}

	cameraPtr = (CameraBuffer*)resource.pData;
	cameraPtr->cameraPosition = cameraPosition;

	devCon->Unmap(cameraBuffer.Get(), 0);
	bufferID++;
	devCon->VSSetConstantBuffers(bufferID, 1, cameraBuffer.GetAddressOf());
	
	devCon->IASetInputLayout(inputLayout.Get());
	devCon->VSSetShader(vertexShader.Get(), nullptr, 0);
	devCon->PSSetShader(pixelShader.Get(), nullptr, 0);
	devCon->PSSetSamplers(0, 1, sampleState.GetAddressOf());
	devCon->DrawIndexed(indexCount, 0, 0);
}

/// <summary>
/// Render method for fire
/// </summary>
/// <param name="devCon">Standard ID3D11DeviceContext</param>
/// <param name="indexCount">Index count</param>
/// <param name="worldMatrix">World matrix</param>
/// <param name="viewMatrix">View matrix</param>
/// <param name="projMatrix">Projection matrix</param>
/// <param name="texture1">Colour texture</param>
/// <param name="texture2">Noise texture</param>
/// <param name="texture3">Alpha texture</param>
/// <param name="animTime">Timer</param>
/// <param name="scrollSpeeds">Varying scroll speeds for the multisamples noise</param>
/// <param name="scales">Varying scales for multisampling noise</param>
/// <param name="distortion1">Noise 1 distortion x/y values</param>
/// <param name="distortion2">Noise 2 distortion x/y values</param>
/// <param name="distortion3">Noise 3 distortion x/y values</param>
/// <param name="distortionScale">Distortion scales</param>
/// <param name="distortionBias">Distortion bias</param>
void Shader::Render(ID3D11DeviceContext *devCon, unsigned int indexCount, const DirectX::XMFLOAT4X4 *worldMatrix, const DirectX::XMFLOAT4X4 *viewMatrix, const DirectX::XMFLOAT4X4 *projMatrix, ID3D11ShaderResourceView *texture1, ID3D11ShaderResourceView *texture2,
	ID3D11ShaderResourceView *texture3, float animTime, DirectX::XMFLOAT3 scrollSpeeds, DirectX::XMFLOAT3 scales, DirectX::XMFLOAT2 distortion1, DirectX::XMFLOAT2 distortion2, DirectX::XMFLOAT2 distortion3, float distortionScale, float distortionBias)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	unsigned int bufferID = 0;

	MatricesInvBuffer *matricesPtr;
	DirectX::XMFLOAT4X4 inv;
	DirectX::XMMATRIX worldMatrixCopy = DirectX::XMLoadFloat4x4(worldMatrix);
	DirectX::XMMATRIX viewMatrixCopy = DirectX::XMLoadFloat4x4(viewMatrix);
	DirectX::XMMATRIX projectionMatrixCopy = DirectX::XMLoadFloat4x4(projMatrix);

	DirectX::XMMATRIX viewInv = DirectX::XMMatrixInverse(nullptr, viewMatrixCopy);
	DirectX::XMStoreFloat4x4(&inv, viewInv);
	inv._41 = 0;
	inv._42 = 0;
	inv._43 = 0;
	viewInv = DirectX::XMLoadFloat4x4(&inv);

	worldMatrixCopy = XMMatrixTranspose(worldMatrixCopy);
	viewMatrixCopy = XMMatrixTranspose(viewMatrixCopy);
	projectionMatrixCopy = XMMatrixTranspose(projectionMatrixCopy);
	viewInv = XMMatrixTranspose(viewInv);

	HRESULT result = devCon->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if(result != S_OK)
	{
		return;
	}

	matricesPtr = (MatricesInvBuffer*)resource.pData;
	DirectX::XMStoreFloat4x4(&(matricesPtr->worldMatrix), worldMatrixCopy);
	DirectX::XMStoreFloat4x4(&(matricesPtr->viewMatrix), viewMatrixCopy);
	DirectX::XMStoreFloat4x4(&(matricesPtr->projMatrix), projectionMatrixCopy);
	DirectX::XMStoreFloat4x4(&(matricesPtr->viewInvMatrix), viewInv);

	devCon->Unmap(matrixBuffer.Get(), 0);
	devCon->VSSetConstantBuffers(bufferID, 1, matrixBuffer.GetAddressOf());
	bufferID++;

	result = devCon->Map(noiseBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if(result != S_OK)
	{
		return;
	}

	NoiseBuffer *noisePtr = (NoiseBuffer*)resource.pData;

	noisePtr->animTime = animTime;
	noisePtr->scrollSpeeds = scrollSpeeds;
	noisePtr->scales = scales;
	noisePtr->padding = 0.0f;

	devCon->Unmap(noiseBuffer.Get(), 0);
	devCon->VSSetConstantBuffers(bufferID, 1, noiseBuffer.GetAddressOf());

	devCon->PSSetShaderResources(0, 1, &texture1);
	devCon->PSSetShaderResources(1, 1, &texture2);
	devCon->PSSetShaderResources(2, 1, &texture3);

	result = devCon->Map(distortionBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if(result != S_OK)
	{
		return;
	}

	DistortionBuffer *distortionPtr = (DistortionBuffer*)resource.pData;

	distortionPtr->distortion1 = distortion1;
	distortionPtr->distortion2 = distortion2;
	distortionPtr->distortion3 = distortion3;
	distortionPtr->distortionScale = distortionScale;
	distortionPtr->distortionBias = distortionBias;

	devCon->Unmap(distortionBuffer.Get(), 0);
	bufferID = 0; //reset to 0 since switching to pixel shader

	devCon->PSSetConstantBuffers(bufferID, 1, distortionBuffer.GetAddressOf());

	devCon->IASetInputLayout(inputLayout.Get());
	devCon->VSSetShader(vertexShader.Get(), nullptr, 0);
	devCon->PSSetShader(pixelShader.Get(), nullptr, 0);
	devCon->PSSetSamplers(0, 1, sampleState.GetAddressOf());
	devCon->PSSetSamplers(1, 1, sampleState2.GetAddressOf());
	devCon->DrawIndexed(indexCount, 0, 0);
}

bool Shader::InitColourShader(ID3D11Device *dev, const std::wstring &vsFile, const std::wstring &psFile)
{
	Microsoft::WRL::ComPtr<ID3D10Blob> vShaderBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> errorMSG = nullptr;
	char *msg = nullptr;
	long bufferSize = 0;
	std::string strMSG = "";
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int layoutCount = 0;

	HRESULT result = D3DCompileFromFile(vsFile.c_str(), nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vShaderBuffer, errorMSG.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Compile vertex shader"))
	{
		msg = (char*)(errorMSG.Get()->GetBufferPointer());
		bufferSize = errorMSG.Get()->GetBufferSize();

		for(int i = 0; i < bufferSize; i++)
			strMSG.push_back(msg[i]);

		Logger::Log(strMSG);
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D10Blob> pShaderBuffer = nullptr;

	result = D3DCompileFromFile(psFile.c_str(), nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &pShaderBuffer, errorMSG.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Compile pixel shader"))
	{
		msg = (char*)(errorMSG.Get()->GetBufferPointer());
		bufferSize = errorMSG.Get()->GetBufferSize();

		for(int i = 0; i < bufferSize; i++)
			strMSG.push_back(msg[i]);

		Logger::Log(strMSG);
		return false;
	}

	delete msg;

	result = dev->CreateVertexShader(vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create vertex shader"))
	{
		return false;
	}

	result = dev->CreatePixelShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create pixel shader"))
	{
		return false;
	}

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	layoutCount = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = dev->CreateInputLayout(polygonLayout, layoutCount, vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), inputLayout.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create input layout"))
	{
		return false;
	}

	D3D11_BUFFER_DESC matrixDesc;

	matrixDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixDesc.ByteWidth = sizeof(MatricesBuffer);
	matrixDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixDesc.MiscFlags = 0;
	matrixDesc.StructureByteStride = 0;

	result = dev->CreateBuffer(&matrixDesc, nullptr, matrixBuffer.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create matrix buffer"))
	{
		return false;
	}

	D3D11_SAMPLER_DESC sampleDesc;

	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.MipLODBias = 0.0f;
	sampleDesc.MaxAnisotropy = 1;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampleDesc.BorderColor[0] = 0;
	sampleDesc.BorderColor[1] = 0;
	sampleDesc.BorderColor[2] = 0;
	sampleDesc.BorderColor[3] = 0;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = dev->CreateSamplerState(&sampleDesc, sampleState.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create sampler"))
	{
		return false;
	}

	return true;
}

bool Shader::InitTextureShader(ID3D11Device *dev, const std::wstring &vsFile, const std::wstring &psFile)
{
	Microsoft::WRL::ComPtr<ID3D10Blob> vShaderBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> errorMSG = nullptr;
	char *msg = nullptr;
	long bufferSize = 0;
	std::string strMSG = "";
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int layoutCount = 0;

	HRESULT result = D3DCompileFromFile(vsFile.c_str(), nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vShaderBuffer, errorMSG.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Compile vertex shader"))
	{
		msg = (char*)(errorMSG.Get()->GetBufferPointer());
		bufferSize = errorMSG.Get()->GetBufferSize();

		for(int i = 0; i < bufferSize; i++)
			strMSG.push_back(msg[i]);

		Logger::Log(strMSG);
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D10Blob> pShaderBuffer = nullptr;

	result = D3DCompileFromFile(psFile.c_str(), nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &pShaderBuffer, errorMSG.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Compile pixel shader"))
	{
		msg = (char*)(errorMSG.Get()->GetBufferPointer());
		bufferSize = errorMSG.Get()->GetBufferSize();

		for(int i = 0; i < bufferSize; i++)
			strMSG.push_back(msg[i]);

		Logger::Log(strMSG);
		return false;
	}

	delete msg;

	result = dev->CreateVertexShader(vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create vertex shader"))
	{
		return false;
	}

	result = dev->CreatePixelShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create pixel shader"))
	{
		return false;
	}

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	layoutCount = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = dev->CreateInputLayout(polygonLayout, layoutCount, vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), inputLayout.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create input layout"))
	{
		return false;
	}

	D3D11_BUFFER_DESC matrixDesc;

	matrixDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixDesc.ByteWidth = sizeof(MatricesBuffer);
	matrixDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixDesc.MiscFlags = 0;
	matrixDesc.StructureByteStride = 0;

	result = dev->CreateBuffer(&matrixDesc, nullptr, matrixBuffer.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create matrix buffer"))
	{
		return false;
	}

	D3D11_SAMPLER_DESC sampleDesc;

	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.MipLODBias = 0.0f;
	sampleDesc.MaxAnisotropy = 1;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampleDesc.BorderColor[0] = 0;
	sampleDesc.BorderColor[1] = 0;
	sampleDesc.BorderColor[2] = 0;
	sampleDesc.BorderColor[3] = 0;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = dev->CreateSamplerState(&sampleDesc, sampleState.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create sampler"))
	{
		return false;
	}

	return true;
}

bool Shader::InitLightShader(ID3D11Device *dev, const std::wstring &vsFile, const std::wstring &psFile)
{
	lightBuffer = nullptr;
	cameraBuffer = nullptr;

	Microsoft::WRL::ComPtr<ID3D10Blob> vShaderBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> errorMSG = nullptr;
	char *msg = nullptr;
	long bufferSize = 0;
	std::string strMSG = "";
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int layoutCount = 0;

	HRESULT result = D3DCompileFromFile(vsFile.c_str(), nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vShaderBuffer, errorMSG.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Compile vertex shader"))
	{
		msg = (char*)(errorMSG.Get()->GetBufferPointer());
		bufferSize = errorMSG.Get()->GetBufferSize();

		for(int i = 0; i < bufferSize; i++)
			strMSG.push_back(msg[i]);

		Logger::Log(strMSG);
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D10Blob> pShaderBuffer = nullptr;

	result = D3DCompileFromFile(psFile.c_str(), nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &pShaderBuffer, errorMSG.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Compile pixel shader"))
	{
		msg = (char*)(errorMSG.Get()->GetBufferPointer());
		bufferSize = errorMSG.Get()->GetBufferSize();

		for(int i = 0; i < bufferSize; i++)
			strMSG.push_back(msg[i]);

		Logger::Log(strMSG);
		return false;
	}

	delete msg;

	result = dev->CreateVertexShader(vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create vertex shader"))
	{
		return false;
	}

	result = dev->CreatePixelShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create pixel shader"))
	{
		return false;
	}

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	layoutCount = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = dev->CreateInputLayout(polygonLayout, layoutCount, vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), inputLayout.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create input layout"))
	{
		return false;
	}

	D3D11_BUFFER_DESC matrixDesc;

	matrixDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixDesc.ByteWidth = sizeof(MatricesBuffer);
	matrixDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixDesc.MiscFlags = 0;
	matrixDesc.StructureByteStride = 0;

	result = dev->CreateBuffer(&matrixDesc, nullptr, matrixBuffer.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create matrix buffer"))
	{
		return false;
	}

	D3D11_BUFFER_DESC lightDesc;

	lightDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightDesc.ByteWidth = sizeof(LightBuffer);
	lightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightDesc.MiscFlags = 0;
	lightDesc.StructureByteStride = 0;

	result = dev->CreateBuffer(&lightDesc, nullptr, lightBuffer.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create light buffer"))
	{
		return false;
	}

	D3D11_BUFFER_DESC cameraDesc;

	cameraDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraDesc.ByteWidth = sizeof(CameraBuffer);
	cameraDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraDesc.MiscFlags = 0;
	cameraDesc.StructureByteStride = 0;

	result = dev->CreateBuffer(&cameraDesc, nullptr, cameraBuffer.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create camera buffer"))
	{
		return false;
	}

	D3D11_SAMPLER_DESC sampleDesc;

	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.MipLODBias = 0.0f;
	sampleDesc.MaxAnisotropy = 1;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampleDesc.BorderColor[0] = 0;
	sampleDesc.BorderColor[1] = 0;
	sampleDesc.BorderColor[2] = 0;
	sampleDesc.BorderColor[3] = 0;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = dev->CreateSamplerState(&sampleDesc, sampleState.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create sampler"))
	{
		return false;
	}

	return true;
}

bool Shader::InitNormalShader(ID3D11Device *dev, const std::wstring &vsFile, const std::wstring &psFile)
{
	lightBuffer = nullptr;
	cameraBuffer = nullptr;

	Microsoft::WRL::ComPtr<ID3D10Blob> vShaderBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> errorMSG = nullptr;
	char *msg = nullptr;
	long bufferSize = 0;
	std::string strMSG = "";
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int layoutCount = 0;

	HRESULT result = D3DCompileFromFile(vsFile.c_str(), nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vShaderBuffer, errorMSG.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Compile vertex shader"))
	{
		msg = (char*)(errorMSG.Get()->GetBufferPointer());
		bufferSize = errorMSG.Get()->GetBufferSize();

		for(int i = 0; i < bufferSize; i++)
			strMSG.push_back(msg[i]);

		Logger::Log(strMSG);
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D10Blob> pShaderBuffer = nullptr;

	result = D3DCompileFromFile(psFile.c_str(), nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &pShaderBuffer, errorMSG.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Compile pixel shader"))
	{
		msg = (char*)(errorMSG.Get()->GetBufferPointer());
		bufferSize = errorMSG.Get()->GetBufferSize();

		for(int i = 0; i < bufferSize; i++)
			strMSG.push_back(msg[i]);

		Logger::Log(strMSG);
		return false;
	}

	delete msg;

	result = dev->CreateVertexShader(vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create vertex shader"))
	{
		return false;
	}

	result = dev->CreatePixelShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create pixel shader"))
	{
		return false;
	}

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	layoutCount = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = dev->CreateInputLayout(polygonLayout, layoutCount, vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), inputLayout.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create input layout"))
	{
		return false;
	}

	D3D11_BUFFER_DESC matrixDesc;

	matrixDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixDesc.ByteWidth = sizeof(MatricesBuffer);
	matrixDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixDesc.MiscFlags = 0;
	matrixDesc.StructureByteStride = 0;

	result = dev->CreateBuffer(&matrixDesc, nullptr, matrixBuffer.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create matrix buffer"))
	{
		return false;
	}

	D3D11_BUFFER_DESC lightDesc;

	lightDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightDesc.ByteWidth = sizeof(LightBuffer);
	lightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightDesc.MiscFlags = 0;
	lightDesc.StructureByteStride = 0;

	result = dev->CreateBuffer(&lightDesc, nullptr, lightBuffer.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create light buffer"))
	{
		return false;
	}

	D3D11_BUFFER_DESC cameraDesc;

	cameraDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraDesc.ByteWidth = sizeof(CameraBuffer);
	cameraDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraDesc.MiscFlags = 0;
	cameraDesc.StructureByteStride = 0;

	result = dev->CreateBuffer(&cameraDesc, nullptr, cameraBuffer.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create camera buffer"))
	{
		return false;
	}

	D3D11_SAMPLER_DESC sampleDesc;

	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.MipLODBias = 0.0f;
	sampleDesc.MaxAnisotropy = 1;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampleDesc.BorderColor[0] = 0;
	sampleDesc.BorderColor[1] = 0;
	sampleDesc.BorderColor[2] = 0;
	sampleDesc.BorderColor[3] = 0;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = dev->CreateSamplerState(&sampleDesc, sampleState.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create sampler"))
	{
		return false;
	}

	return true;
}

bool Shader::InitSkyDomeShader(ID3D11Device *dev, const std::wstring &vsFile, const std::wstring &psFile)
{
	Microsoft::WRL::ComPtr<ID3D10Blob> vShaderBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> errorMSG = nullptr;
	char *msg = nullptr;
	long bufferSize = 0;
	std::string strMSG = "";
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int layoutCount = 0;

	HRESULT result = D3DCompileFromFile(vsFile.c_str(), nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vShaderBuffer, errorMSG.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Compile vertex shader"))
	{
		msg = (char*)(errorMSG.Get()->GetBufferPointer());
		bufferSize = errorMSG.Get()->GetBufferSize();

		for(int i = 0; i < bufferSize; i++)
			strMSG.push_back(msg[i]);

		Logger::Log(strMSG);
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D10Blob> pShaderBuffer = nullptr;

	result = D3DCompileFromFile(psFile.c_str(), nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &pShaderBuffer, errorMSG.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Compile pixel shader"))
	{
		msg = (char*)(errorMSG.Get()->GetBufferPointer());
		bufferSize = errorMSG.Get()->GetBufferSize();

		for(int i = 0; i < bufferSize; i++)
			strMSG.push_back(msg[i]);

		Logger::Log(strMSG);
		return false;
	}

	delete msg;

	result = dev->CreateVertexShader(vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create vertex shader"))
	{
		return false;
	}

	result = dev->CreatePixelShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create pixel shader"))
	{
		return false;
	}

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	layoutCount = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = dev->CreateInputLayout(polygonLayout, layoutCount, vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), inputLayout.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create input layout"))
	{
		return false;
	}

	D3D11_BUFFER_DESC matrixDesc;

	matrixDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixDesc.ByteWidth = sizeof(MatricesBuffer);
	matrixDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixDesc.MiscFlags = 0;
	matrixDesc.StructureByteStride = 0;

	result = dev->CreateBuffer(&matrixDesc, nullptr, matrixBuffer.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create matrix buffer"))
	{
		return false;
	}

	D3D11_BUFFER_DESC timeDesc;

	timeDesc.Usage = D3D11_USAGE_DYNAMIC;
	timeDesc.ByteWidth = sizeof(TimeBuffer);
	timeDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timeDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timeDesc.MiscFlags = 0;
	timeDesc.StructureByteStride = 0;

	result = dev->CreateBuffer(&timeDesc, nullptr, timeBuffer.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create time buffer"))
	{
		return false;
	}

	D3D11_SAMPLER_DESC sampleDesc;

	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.MipLODBias = 0.0f;
	sampleDesc.MaxAnisotropy = 1;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampleDesc.BorderColor[0] = 0;
	sampleDesc.BorderColor[1] = 0;
	sampleDesc.BorderColor[2] = 0;
	sampleDesc.BorderColor[3] = 0;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = dev->CreateSamplerState(&sampleDesc, sampleState.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create sampler"))
	{
		return false;
	}

	return true;
}

bool Shader::InitParticleShader(ID3D11Device * dev, const std::wstring & vsFile, const std::wstring & psFile)
{
	Microsoft::WRL::ComPtr<ID3D10Blob> vShaderBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> errorMSG = nullptr;
	char *msg = nullptr;
	long bufferSize = 0;
	std::string strMSG = "";
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int layoutCount = 0;

	HRESULT result = D3DCompileFromFile(vsFile.c_str(), nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vShaderBuffer, errorMSG.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Compile vertex shader"))
	{
		msg = (char*)(errorMSG.Get()->GetBufferPointer());
		bufferSize = errorMSG.Get()->GetBufferSize();

		for(int i = 0; i < bufferSize; i++)
			strMSG.push_back(msg[i]);

		Logger::Log(strMSG);
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D10Blob> pShaderBuffer = nullptr;

	result = D3DCompileFromFile(psFile.c_str(), nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &pShaderBuffer, errorMSG.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Compile pixel shader"))
	{
		msg = (char*)(errorMSG.Get()->GetBufferPointer());
		bufferSize = errorMSG.Get()->GetBufferSize();

		for(int i = 0; i < bufferSize; i++)
			strMSG.push_back(msg[i]);

		Logger::Log(strMSG);
		return false;
	}

	delete msg;

	result = dev->CreateVertexShader(vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create vertex shader"))
	{
		return false;
	}

	result = dev->CreatePixelShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create pixel shader"))
	{
		return false;
	}

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "COLOR";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TEXCOORD";
	polygonLayout[3].SemanticIndex = 1;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 1;
	polygonLayout[3].AlignedByteOffset = 0;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[3].InstanceDataStepRate = 1;

	polygonLayout[4].SemanticName = "TEXCOORD";
	polygonLayout[4].SemanticIndex = 2;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 1;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[4].InstanceDataStepRate = 1;

	layoutCount = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = dev->CreateInputLayout(polygonLayout, layoutCount, vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), inputLayout.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create input layout"))
	{
		return false;
	}

	D3D11_BUFFER_DESC matrixDesc;

	matrixDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixDesc.ByteWidth = sizeof(MatricesInvBuffer);
	matrixDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixDesc.MiscFlags = 0;
	matrixDesc.StructureByteStride = 0;

	result = dev->CreateBuffer(&matrixDesc, nullptr, matrixBuffer.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create matrix buffer"))
	{
		return false;
	}

	D3D11_SAMPLER_DESC sampleDesc;

	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.MipLODBias = 0.0f;
	sampleDesc.MaxAnisotropy = 1;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampleDesc.BorderColor[0] = 0;
	sampleDesc.BorderColor[1] = 0;
	sampleDesc.BorderColor[2] = 0;
	sampleDesc.BorderColor[3] = 0;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = dev->CreateSamplerState(&sampleDesc, sampleState.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create sampler"))
	{
		return false;
	}

	return true;
}

bool Shader::InitFireShader(ID3D11Device *dev, const std::wstring &vsFile, const std::wstring &psFile)
{
	Microsoft::WRL::ComPtr<ID3D10Blob> vShaderBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> errorMSG = nullptr;
	char *msg = nullptr;
	long bufferSize = 0;
	std::string strMSG = "";
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int layoutCount = 0;

	HRESULT result = D3DCompileFromFile(vsFile.c_str(), nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vShaderBuffer, errorMSG.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Compile vertex shader"))
	{
		msg = (char*)(errorMSG.Get()->GetBufferPointer());
		bufferSize = errorMSG.Get()->GetBufferSize();

		for(int i = 0; i < bufferSize; i++)
			strMSG.push_back(msg[i]);

		Logger::Log(strMSG);
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D10Blob> pShaderBuffer = nullptr;

	result = D3DCompileFromFile(psFile.c_str(), nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &pShaderBuffer, errorMSG.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Compile pixel shader"))
	{
		msg = (char*)(errorMSG.Get()->GetBufferPointer());
		bufferSize = errorMSG.Get()->GetBufferSize();

		for(int i = 0; i < bufferSize; i++)
			strMSG.push_back(msg[i]);

		Logger::Log(strMSG);
		return false;
	}

	delete msg;

	result = dev->CreateVertexShader(vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create vertex shader"))
	{
		return false;
	}

	result = dev->CreatePixelShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create pixel shader"))
	{
		return false;
	}

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "COLOR";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	layoutCount = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = dev->CreateInputLayout(polygonLayout, layoutCount, vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), inputLayout.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create input layout"))
	{
		return false;
	}

	D3D11_BUFFER_DESC matrixDesc;

	matrixDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixDesc.ByteWidth = sizeof(MatricesInvBuffer);
	matrixDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixDesc.MiscFlags = 0;
	matrixDesc.StructureByteStride = 0;

	result = dev->CreateBuffer(&matrixDesc, nullptr, matrixBuffer.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create matrix buffer"))
	{
		return false;
	}

	D3D11_BUFFER_DESC noiseDesc;

	noiseDesc.Usage = D3D11_USAGE_DYNAMIC;
	noiseDesc.ByteWidth = sizeof(NoiseBuffer);
	noiseDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	noiseDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	noiseDesc.MiscFlags = 0;
	noiseDesc.StructureByteStride = 0;

	result = dev->CreateBuffer(&noiseDesc, nullptr, noiseBuffer.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create noise buffer"))
	{
		return false;
	}

	D3D11_BUFFER_DESC distortionDesc;

	// Setup the description of the dynamic distortion constant buffer that is in the pixel shader.
	distortionDesc.Usage = D3D11_USAGE_DYNAMIC;
	distortionDesc.ByteWidth = sizeof(DistortionBuffer);
	distortionDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	distortionDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	distortionDesc.MiscFlags = 0;
	distortionDesc.StructureByteStride = 0;

	result = dev->CreateBuffer(&distortionDesc, nullptr, distortionBuffer.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create distortion buffer"))
	{
		return false;
	}

	D3D11_SAMPLER_DESC sampleDesc;

	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.MipLODBias = 0.0f;
	sampleDesc.MaxAnisotropy = 1;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampleDesc.BorderColor[0] = 0;
	sampleDesc.BorderColor[1] = 0;
	sampleDesc.BorderColor[2] = 0;
	sampleDesc.BorderColor[3] = 0;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = dev->CreateSamplerState(&sampleDesc, sampleState.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create sampler"))
	{
		return false;
	}

	D3D11_SAMPLER_DESC sampleDesc2;

	sampleDesc2.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc2.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc2.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc2.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc2.MipLODBias = 0.0f;
	sampleDesc2.MaxAnisotropy = 1;
	sampleDesc2.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampleDesc2.BorderColor[0] = 0;
	sampleDesc2.BorderColor[1] = 0;
	sampleDesc2.BorderColor[2] = 0;
	sampleDesc2.BorderColor[3] = 0;
	sampleDesc2.MinLOD = 0;
	sampleDesc2.MaxLOD = D3D11_FLOAT32_MAX;

	result = dev->CreateSamplerState(&sampleDesc2, sampleState2.GetAddressOf());
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create sampler2"))
	{
		return false;
	}

	return true;
}