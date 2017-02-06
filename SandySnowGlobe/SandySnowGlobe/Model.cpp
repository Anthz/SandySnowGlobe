#include "Model.h"

Model::Model()
{
	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	texture = nullptr;
	vertexCount = 0;
	texCoCount = 0;
	normCount = 0;
	faceCount = 0;
	indexCount = 0;
	textureCount = 0;
}

Model::~Model()
{
	try
	{
		Memory::SafeDelete(texture);	//delete pointer to array of comptrs
	}
	catch(int &e)
	{
		delete texture;
	}
}

/// <summary>
/// Initialise textureless model
/// Load in .obj and setup buffers
/// </summary>
/// <param name="device">Standard ID3D11Device</param>
/// <param name="filename">Model filepath</param>
/// <returns></returns>
bool Model::Init(ID3D11Device *device, const WCHAR *filename)
{
	if(!CheckModelCounts(filename, vertexCount, texCoCount, normCount, faceCount))
	{
		return false;
	}

	Vertex *vertices = new Vertex[faceCount * 3];

	if(!LoadModel(filename, vertices))
	{
		return false;
	}

	if(!Init(device, vertices))
	{
		return false;
	}

	return true;
}

/// <summary>
/// Initialise simple textured model
/// Load in .obj, create texture and setup buffers
/// </summary>
/// <param name="device">Standard ID3D11Device</param>
/// <param name="filename">Model filepath</param>
/// <param name="textureName">Colour texture filepath</param>
/// <returns></returns>
bool Model::Init(ID3D11Device *device, const WCHAR *filename, const WCHAR *textureName)
{
	if(!LoadTexture(device, textureName))
	{
		return false;
	}

	if(!CheckModelCounts(filename, vertexCount, texCoCount, normCount, faceCount))
	{
		return false;
	}

	Vertex *vertices = new Vertex[faceCount * 3];

	if(!LoadModel(filename, vertices))
	{
		return false;
	}

	if(!Init(device, vertices))
	{
		return false;
	}

	return true;
}

/// <summary>
/// Initialise skybox type model
/// Load in .obj, create textures and setup buffers
/// </summary>
/// <param name="device">Standard ID3D11Device</param>
/// <param name="filename">Model filepath</param>
/// <param name="skyTexture">Colour texture filepath</param>
/// <param name="gradientTexture">Gradient texture for day/night cycle</param>
/// <returns></returns>
bool Model::Init(ID3D11Device *device, const WCHAR *filename, const WCHAR *skyTexture, const WCHAR *gradientTexture)
{
	if(!LoadTextures(device, skyTexture, gradientTexture))
	{
		return false;
	}

	if(!CheckModelCounts(filename, vertexCount, texCoCount, normCount, faceCount))
	{
		return false;
	}

	Vertex *vertices = new Vertex[faceCount * 3];
	if(!LoadModel(filename, vertices))
	{
		return false;
	}

	if(!Init(device, vertices))
	{
		return false;
	}
}

/// <summary>
/// Initialise lighting enabled model
/// Load in .obj, create textures and setup buffers
/// </summary>
/// <param name="device">Standard ID3D11Device</param>
/// <param name="filename">Model filepath</param>
/// <param name="colourTexture">Colour texture filepath</param>
/// <param name="normalTexture">Normal map texture filepath</param>
/// <param name="specularTexture">Specular map texture filepath</param>
/// <returns></returns>
bool Model::Init(ID3D11Device *device, const WCHAR *filename, const WCHAR *colourTexture, const WCHAR *normalTexture, const WCHAR *specularTexture)
{
	if(!LoadTextures(device, colourTexture, normalTexture, specularTexture))
	{
		return false;
	}

	if(!CheckModelCounts(filename, vertexCount, texCoCount, normCount, faceCount))
	{
		return false;
	}

	BumpVertex *bVertices = new BumpVertex[faceCount * 3];
	if(!LoadBumpModel(filename, bVertices))
	{
		return false;
	}

	if(!InitBump(device, bVertices))
	{
		return false;
	}
}

/// <summary>
/// Initialise buffers with preset vertices (pos/tex/norm)
/// </summary>
/// <param name="device">Standard ID3D11Device</param>
/// <param name="vertices">Preloaded vertices (pos/tex/norm)</param>
/// <returns></returns>
bool Model::Init(ID3D11Device *device, Vertex *vertices)
{
	unsigned long *indices = new unsigned long[indexCount];

	for(int i = 0; i < vertexCount; i++)
	{
		indices[i] = i;
	}

	D3D11_BUFFER_DESC vertexDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = sizeof(Vertex) * vertexCount;
	vertexDesc.CPUAccessFlags = 0;
	vertexDesc.MiscFlags = 0;
	vertexDesc.StructureByteStride = 0;
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HRESULT result = device->CreateBuffer(&vertexDesc, &vertexData, vertexBuffer.GetAddressOf());

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create vertex buffer"))
	{
		return false;
	}

	D3D11_BUFFER_DESC indexDesc;
	D3D11_SUBRESOURCE_DATA indexData;

	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexDesc.CPUAccessFlags = 0;
	indexDesc.MiscFlags = 0;
	indexDesc.StructureByteStride = 0;
	indexDesc.Usage = D3D11_USAGE_DEFAULT;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexDesc, &indexData, indexBuffer.GetAddressOf());

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create index buffer"))
	{
		return false;
	}

	Memory::SafeDeleteArr(vertices);
	Memory::SafeDeleteArr(indices);

	return true;
}

/// <summary>
/// Initialise buffers with preset vertices (pos/tex/norm/tang/binorm)
/// </summary>
/// <param name="device">Standard ID3D11Device</param>
/// <param name="vertices">Preloaded vertices (pos/tex/norm/tang/binorm)</param>
/// <returns></returns>
bool Model::InitBump(ID3D11Device *device, BumpVertex *vertices)
{
	unsigned long *indices = new unsigned long[indexCount];

	for(int i = 0; i < indexCount; i++)
	{
		indices[i] = i;
	}

	D3D11_BUFFER_DESC vertexDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = sizeof(BumpVertex) * vertexCount;
	vertexDesc.CPUAccessFlags = 0;
	vertexDesc.MiscFlags = 0;
	vertexDesc.StructureByteStride = 0;
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HRESULT result = device->CreateBuffer(&vertexDesc, &vertexData, vertexBuffer.GetAddressOf());

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create vertex buffer"))
	{
		return false;
	}

	D3D11_BUFFER_DESC indexDesc;
	D3D11_SUBRESOURCE_DATA indexData;

	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexDesc.CPUAccessFlags = 0;
	indexDesc.MiscFlags = 0;
	indexDesc.StructureByteStride = 0;
	indexDesc.Usage = D3D11_USAGE_DEFAULT;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexDesc, &indexData, indexBuffer.GetAddressOf());

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create index buffer"))
	{
		return false;
	}

	Memory::SafeDeleteArr(vertices);
	Memory::SafeDeleteArr(indices);

	return true;
}

/// <summary>
/// Initialise model for a default billboarded quad
/// <param name="device">Standard ID3D11Device</param>
/// <param name="texture1">Texture 1 filepath</param>
/// <param name="texture2">Texture 2 filepath</param>
/// <param name="texture3">Texture 3 filepath</param>
/// <returns></returns>
bool Model::InitBillboared(ID3D11Device *device, const WCHAR *texture1, const WCHAR *texture2, const WCHAR *texture3)
{
	if(!LoadTextures(device, texture1, texture2, texture3))
	{
		return false;
	}

	vertexCount = 6;
	indexCount = 6;

	ParticleVertex *vertices = new ParticleVertex[vertexCount];

	unsigned long *indices = new unsigned long[indexCount];

	for(int i = 0; i < indexCount; i++)
	{
		indices[i] = i;
	}

	memset(vertices, 0, sizeof(ParticleVertex) * vertexCount);

	//create simple textured quad
	vertices[0].position = DirectX::XMFLOAT3(-1, -1, 0.0f);
	vertices[0].texCoord = DirectX::XMFLOAT2(0.0f, 1.0f);
	vertices[0].colour = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[1].position = DirectX::XMFLOAT3(-1, 1, 0.0f);
	vertices[1].texCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
	vertices[1].colour = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[2].position = DirectX::XMFLOAT3(1, -1, 0.0f);
	vertices[2].texCoord = DirectX::XMFLOAT2(1.0f, 1.0f);
	vertices[2].colour = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[3].position = DirectX::XMFLOAT3(1, -1, 0.0f);
	vertices[3].texCoord = DirectX::XMFLOAT2(1.0f, 1.0f);
	vertices[3].colour = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[4].position = DirectX::XMFLOAT3(-1, 1, 0.0f);
	vertices[4].texCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
	vertices[4].colour = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[5].position = DirectX::XMFLOAT3(1, 1, 0.0f);
	vertices[5].texCoord = DirectX::XMFLOAT2(1.0f, 0.0f);
	vertices[5].colour = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	D3D11_BUFFER_DESC vertexDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = sizeof(ParticleVertex) * vertexCount;
	vertexDesc.CPUAccessFlags = 0;
	vertexDesc.MiscFlags = 0;
	vertexDesc.StructureByteStride = 0;
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HRESULT result = device->CreateBuffer(&vertexDesc, &vertexData, vertexBuffer.GetAddressOf());

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create vertex buffer"))
	{
		return false;
	}

	D3D11_BUFFER_DESC indexDesc;
	D3D11_SUBRESOURCE_DATA indexData;

	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexDesc.CPUAccessFlags = 0;
	indexDesc.MiscFlags = 0;
	indexDesc.StructureByteStride = 0;
	indexDesc.Usage = D3D11_USAGE_DEFAULT;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexDesc, &indexData, indexBuffer.GetAddressOf());

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create index buffer"))
	{
		return false;
	}

	Memory::SafeDeleteArr(vertices);
	Memory::SafeDeleteArr(indices);

	return true;
}

void Model::Render(ID3D11DeviceContext *devContext)
{
	unsigned int stride;
	stride = sizeof(Vertex);

	if(textureCount == 3)
	{
		if(faceCount == 0)
			stride = sizeof(ParticleVertex);
		else
			stride = sizeof(BumpVertex);
	}

	unsigned int offset = 0;

	devContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	devContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

/// <summary>
/// Read in obj file and return counts of vert/tex/norm/face
/// </summary>
/// <param name="filename">OBJ filepath</param>
/// <param name="vCount">Vertex count</param>
/// <param name="tCount">Texture coord count</param>
/// <param name="nCount">Normal count</param>
/// <param name="fCount">Face count</param>
/// <returns></returns>
bool Model::CheckModelCounts(const WCHAR *filename, unsigned int &vCount, unsigned int &tCount, unsigned int &nCount, unsigned int &fCount) const
{
	std::ifstream input;
	char c;

	input.open(filename);

	if(input.fail())
	{
		return false;
	}

	input.get(c);
	while(!input.eof())
	{
		switch(c)
		{
			case 'v':
				input.get(c);
				if(c == ' ')
					vCount++;
				if(c == 't')
					tCount++;
				if(c == 'n')
					nCount++;
				break;
			case 'f':
				fCount++;
				break;
		}
		
		while(c != '\n')
		{
			input.get(c);
		}

		input.get(c);
	}

	input.close();

	return true;
}

/// <summary>
/// Read in obj file and add to vertex struct
/// </summary>
/// <param name="filename">OBJ filepath</param>
/// <param name="vert">Vertex struct</param>
/// <returns></returns>
bool Model::LoadModel(const WCHAR *filename, Vertex *vert)
{
	DirectX::XMFLOAT3 *v = new DirectX::XMFLOAT3[vertexCount];
	DirectX::XMFLOAT2 *t = new DirectX::XMFLOAT2[texCoCount];
	DirectX::XMFLOAT3 *n = new DirectX::XMFLOAT3[normCount];
	Face *f = new Face[faceCount];

	std::ifstream input;
	char c;

	unsigned int vertIndex = 0;
	unsigned int texIndex = 0;
	unsigned int normIndex = 0;
	unsigned int faceIndex = 0;

	input.open(filename);

	if(input.fail())
	{
		return false;
	}

	input.get(c);
	while(!input.eof())
	{
		switch(c)
		{
		case 'v':
			input.get(c);
			if(c == ' ')
			{
				input >> v[vertIndex].x >> v[vertIndex].y >> v[vertIndex].z;
				v[vertIndex].z *= -1.0f;
				vertIndex++;
				break;
			}

			if(c == 't')
			{
				input >> t[texIndex].x >> t[texIndex].y;
				t[texIndex].y = 1.0f - t[texIndex].y;
				texIndex++;
				break;
			}

			if(c == 'n')
			{
				input >> n[normIndex].x >> n[normIndex].y >> n[normIndex].z;
				n[normIndex].z *= -1.0f;
				normIndex++;
				break;
			}

		case 'f':
			char c2;
			input >> f[faceIndex].vert3 >> c2 >> f[faceIndex].tex3 >> c2 >> f[faceIndex].norm3
				>> f[faceIndex].vert2 >> c2 >> f[faceIndex].tex2 >> c2 >> f[faceIndex].norm2
				>> f[faceIndex].vert1 >> c2 >> f[faceIndex].tex1 >> c2 >> f[faceIndex].norm1;
			faceIndex++;
			break;
		}

		while(c != '\n')
		{
			input.get(c);
		}

		input.get(c);
	}

	input.close();

	vertIndex = 0;
	texIndex = 0;
	normIndex = 0;

	vertexCount = faceCount * 3;
	indexCount = vertexCount;

	unsigned int newIndex = 0;

	//reorder vertices
	for(int i = 0; i < faceCount; i++)
	{
		vertIndex = f[i].vert1 - 1;
		texIndex = f[i].tex1 - 1;
		normIndex = f[i].norm1 - 1;

		vert[newIndex].position.x = v[vertIndex].x;
		vert[newIndex].position.y = v[vertIndex].y;
		vert[newIndex].position.z = v[vertIndex].z;
		vert[newIndex].texCoord.x = t[texIndex].x;
		vert[newIndex].texCoord.y = t[texIndex].y;
		vert[newIndex].normal.x = n[normIndex].x;
		vert[newIndex].normal.y = n[normIndex].y;
		vert[newIndex].normal.z = n[normIndex].z;

		newIndex++;

		vertIndex = f[i].vert2 - 1;
		texIndex = f[i].tex2 - 1;
		normIndex = f[i].norm2 - 1;

		vert[newIndex].position.x = v[vertIndex].x;
		vert[newIndex].position.y = v[vertIndex].y;
		vert[newIndex].position.z = v[vertIndex].z;
		vert[newIndex].texCoord.x = t[texIndex].x;
		vert[newIndex].texCoord.y = t[texIndex].y;
		vert[newIndex].normal.x = n[normIndex].x;
		vert[newIndex].normal.y = n[normIndex].y;
		vert[newIndex].normal.z = n[normIndex].z;

		newIndex++;

		vertIndex = f[i].vert3 - 1;
		texIndex = f[i].tex3 - 1;
		normIndex = f[i].norm3 - 1;

		vert[newIndex].position.x = v[vertIndex].x;
		vert[newIndex].position.y = v[vertIndex].y;
		vert[newIndex].position.z = v[vertIndex].z;
		vert[newIndex].texCoord.x = t[texIndex].x;
		vert[newIndex].texCoord.y = t[texIndex].y;
		vert[newIndex].normal.x = n[normIndex].x;
		vert[newIndex].normal.y = n[normIndex].y;
		vert[newIndex].normal.z = n[normIndex].z;

		newIndex++;
	}

	Memory::SafeDeleteArr(v);
	Memory::SafeDeleteArr(t);
	Memory::SafeDeleteArr(n);
	Memory::SafeDeleteArr(f);

	return true;
}

/// <summary>
/// Read in obj file, compute tangent/binormal and add to BumpVertex struct
/// </summary>
/// <param name="filename">OBJ filepath</param>
/// <param name="vert">Vertex struct</param>
/// <returns></returns>
bool Model::LoadBumpModel(const WCHAR *filename, BumpVertex *vert)	//convert vert/norm/tex to vert
{
	DirectX::XMFLOAT3 *v = new DirectX::XMFLOAT3[vertexCount];
	DirectX::XMFLOAT2 *t = new DirectX::XMFLOAT2[texCoCount];
	DirectX::XMFLOAT3 *n = new DirectX::XMFLOAT3[normCount];
	Face *f = new Face[faceCount];

	std::ifstream input;
	char c;

	unsigned int vertIndex = 0;
	unsigned int texIndex = 0;
	unsigned int normIndex = 0;
	unsigned int faceIndex = 0;

	input.open(filename);

	if(input.fail())
	{
		return false;
	}

	input.get(c);
	while(!input.eof())
	{
		switch(c)
		{
		case 'v':
			input.get(c);
			if(c == ' ')
			{
				input >> v[vertIndex].x >> v[vertIndex].y >> v[vertIndex].z;
				v[vertIndex].z *= -1.0f;
				vertIndex++;
				break;
			}

			if(c == 't')
			{
				input >> t[texIndex].x >> t[texIndex].y;
				t[texIndex].y = 1.0f - t[texIndex].y;
				texIndex++;
				break;
			}

			if(c == 'n')
			{
				input >> n[normIndex].x >> n[normIndex].y >> n[normIndex].z;
				n[normIndex].z *= -1.0f;
				normIndex++;
				break;
			}

		case 'f':
			char c2;
			input >> f[faceIndex].vert3 >> c2 >> f[faceIndex].tex3 >> c2 >> f[faceIndex].norm3
				>> f[faceIndex].vert2 >> c2 >> f[faceIndex].tex2 >> c2 >> f[faceIndex].norm2
				>> f[faceIndex].vert1 >> c2 >> f[faceIndex].tex1 >> c2 >> f[faceIndex].norm1;
			faceIndex++;
			break;
		}

		while(c != '\n')
		{
			input.get(c);
		}

		input.get(c);
	}

	input.close();

	vertIndex = 0;
	texIndex = 0;
	normIndex = 0;

	vertexCount = faceCount * 3;
	indexCount = vertexCount;

	unsigned int newIndex = 0;

	for(int i = 0; i < faceCount; i++)
	{
		vertIndex = f[i].vert1 - 1;
		texIndex = f[i].tex1 - 1;
		normIndex = f[i].norm1 - 1;

		vert[newIndex].position.x = v[vertIndex].x;
		vert[newIndex].position.y = v[vertIndex].y;
		vert[newIndex].position.z = v[vertIndex].z;
		vert[newIndex].texCoord.x = t[texIndex].x;
		vert[newIndex].texCoord.y = t[texIndex].y;
		vert[newIndex].normal.x = n[normIndex].x;
		vert[newIndex].normal.y = n[normIndex].y;
		vert[newIndex].normal.z = n[normIndex].z;

		newIndex++;

		vertIndex = f[i].vert2 - 1;
		texIndex = f[i].tex2 - 1;
		normIndex = f[i].norm2 - 1;

		vert[newIndex].position.x = v[vertIndex].x;
		vert[newIndex].position.y = v[vertIndex].y;
		vert[newIndex].position.z = v[vertIndex].z;
		vert[newIndex].texCoord.x = t[texIndex].x;
		vert[newIndex].texCoord.y = t[texIndex].y;
		vert[newIndex].normal.x = n[normIndex].x;
		vert[newIndex].normal.y = n[normIndex].y;
		vert[newIndex].normal.z = n[normIndex].z;

		newIndex++;

		vertIndex = f[i].vert3 - 1;
		texIndex = f[i].tex3 - 1;
		normIndex = f[i].norm3 - 1;

		vert[newIndex].position.x = v[vertIndex].x;
		vert[newIndex].position.y = v[vertIndex].y;
		vert[newIndex].position.z = v[vertIndex].z;
		vert[newIndex].texCoord.x = t[texIndex].x;
		vert[newIndex].texCoord.y = t[texIndex].y;
		vert[newIndex].normal.x = n[normIndex].x;
		vert[newIndex].normal.y = n[normIndex].y;
		vert[newIndex].normal.z = n[normIndex].z;

		newIndex++;

		DirectX::XMFLOAT3 tangent, binormal, normal;

		CalculateTangBinorm(vert[newIndex - 3], vert[newIndex - 2], vert[newIndex - 1], &tangent, &binormal);

		vert[newIndex - 3].tangent = tangent;
		vert[newIndex - 3].binormal = binormal;
		vert[newIndex - 2].tangent = tangent;
		vert[newIndex - 2].binormal = binormal;
		vert[newIndex - 1].tangent = tangent;
		vert[newIndex - 1].binormal = binormal;
	}

	Memory::SafeDeleteArr(v);
	Memory::SafeDeleteArr(t);
	Memory::SafeDeleteArr(n);
	Memory::SafeDeleteArr(f);

	return true;
}

void Model::CalculateTangBinorm(BumpVertex &vert1, BumpVertex &vert2, BumpVertex &vert3, DirectX::XMFLOAT3 *tangent, DirectX::XMFLOAT3 *binormal) const
{
	DirectX::XMVECTOR v, length;
	DirectX::XMFLOAT3 tempVert1, tempVert2;
	DirectX::XMFLOAT2 tempU, tempV;
	float denominator;

	v = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&vert2.position), DirectX::XMLoadFloat3(&vert1.position));
	DirectX::XMStoreFloat3(&tempVert1, v);
	v = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&vert3.position), DirectX::XMLoadFloat3(&vert1.position));
	DirectX::XMStoreFloat3(&tempVert2, v);

	tempU.x = vert2.texCoord.x - vert1.texCoord.x;
	tempU.y = vert3.texCoord.x - vert1.texCoord.x;
	tempV.x = vert2.texCoord.y - vert1.texCoord.y;
	tempV.y = vert3.texCoord.y - vert1.texCoord.y;

	denominator = 1.0f / (tempU.x * tempV.y - tempU.y * tempV.x);

	tangent->x = (tempV.y * tempVert1.x - tempV.x * tempVert2.x) * denominator;
	tangent->y = (tempV.y * tempVert1.y - tempV.x * tempVert2.y) * denominator;
	tangent->z = (tempV.y * tempVert1.z - tempV.x * tempVert2.z) * denominator;

	v = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(tangent));
	DirectX::XMStoreFloat3(tangent, v);

	binormal->x = (tempU.x * tempVert2.x - tempU.y * tempVert1.x) * denominator;
	binormal->y = (tempU.x * tempVert2.y - tempU.y * tempVert1.y) * denominator;
	binormal->z = (tempU.x * tempVert2.z - tempU.y * tempVert1.z) * denominator;

	v = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(binormal));
	DirectX::XMStoreFloat3(binormal, v);
}

bool Model::LoadTexture(ID3D11Device *dev, const WCHAR *filename)
{
	textureCount = 1;
	texture = new Texture*[textureCount]();
	texture[0] = new Texture();
	HRESULT result = texture[0]->Init(dev, filename);

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Initialise texture"))
	{
		return false;
	}

	return true;
}

bool Model::LoadTextures(ID3D11Device *dev, const WCHAR *skyTexture, const WCHAR *gradientTexture)
{
	textureCount = 2;
	texture = new Texture*[textureCount]();
	for(int i = 0; i < textureCount; i++)
	{
		texture[i] = new Texture();
	}

	HRESULT result = texture[0]->Init(dev, skyTexture);

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Initialise texture"))
	{
		return false;
	}

	result = texture[1]->Init(dev, gradientTexture);

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Initialise texture"))
	{
		return false;
	}

	return true;
}

bool Model::LoadTextures(ID3D11Device *dev, const WCHAR *colourTexture, const WCHAR *normalTexture, const WCHAR *specularTexture)
{
	textureCount = 3;
	texture = new Texture*[textureCount]();
	for(int i = 0; i < textureCount; i++)
	{
		texture[i] = new Texture();
	}

	HRESULT result = texture[0]->Init(dev, colourTexture);

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Initialise texture"))
	{
		return false;
	}

	result = texture[1]->Init(dev, normalTexture);

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Initialise texture"))
	{
		return false;
	}

	result = texture[2]->Init(dev, specularTexture);

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Initialise texture"))
	{
		return false;
	}

	return true;
}

ID3D11ShaderResourceView **Model::GetTextureArray(ID3D11ShaderResourceView **textureArray) const
{
	textureArray = new ID3D11ShaderResourceView*[textureCount];
	for(int i = 0; i < textureCount; i++)
	{
		textureArray[i] = texture[i]->GetTexture();
	}

	return textureArray;
}

