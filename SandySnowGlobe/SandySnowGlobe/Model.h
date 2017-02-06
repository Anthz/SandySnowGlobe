#pragma once
#include <d3d11.h>
#include "DirectXMath.h"
#include <wrl.h>
#include <string>
#include <iostream>
#include <fstream>
#include "DXUtil.h"
#include "Texture.h"

class Model
{
public:
	struct Vertex
	{
		Vertex(){};
		Vertex(float x, float y, float z,
			   float u, float v,
			   float n1, float n2, float n3) : position(x, y, z), 
											   texCoord(u, v), 
											   normal(n1, n2, n3){}
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texCoord;
		DirectX::XMFLOAT3 normal;
	};

	struct ParticleVertex
	{
		ParticleVertex(){};
		ParticleVertex(float x, float y, float z,
			float u, float v,
			float r, float g, float b, float a) : position(x, y, z),
			texCoord(u, v),
			colour(r, g, b, a){}
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texCoord;
		DirectX::XMFLOAT4 colour;
	};

	struct BumpVertex
	{
		BumpVertex(){};
		BumpVertex(float x, float y, float z,
			float u, float v,
			float n1, float n2, float n3,
			float tx, float ty, float tz,
			float bx, float by, float bz) : position(x, y, z),
			texCoord(u, v),
			normal(n1, n2, n3),
			tangent(tx, ty, tz),
			binormal(bx, by, bz){}
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texCoord;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
		DirectX::XMFLOAT3 binormal;
	};

	struct SkyDomeVertex
	{
		SkyDomeVertex(){ domeRadius = 100.0f; }
		SkyDomeVertex(float x, float y, float z,
			float u, float v,
			float n1, float n2, float n3,
			float r) : position(x, y, z),
			texCoord(u, v),
			normal(n1, n2, n3),
			domeRadius(r){}
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texCoord;
		DirectX::XMFLOAT3 normal;
		float domeRadius;
	};

	struct Face
	{
		unsigned int vert1, vert2, vert3,
					 tex1, tex2, tex3,
					 norm1, norm2, norm3;
	};

	Model();
	~Model();

	bool Init(ID3D11Device *device, const WCHAR *filename);
	bool Init(ID3D11Device *device, const WCHAR *filename, const WCHAR *textureName);
	bool Init(ID3D11Device *device, const WCHAR *filename, const WCHAR *skyTexture, const WCHAR *gradientTexture);
	bool Init(ID3D11Device *device, const WCHAR *filename, const WCHAR *colourTexture, const WCHAR *normalTexture, const WCHAR *specularTexture);
	bool Init(ID3D11Device *device, Vertex *vert);

	bool InitBump(ID3D11Device *device, BumpVertex *vertices);
	bool InitBillboared(ID3D11Device *device, const WCHAR *texture1, const WCHAR *texture2, const WCHAR *texture3);
	void Render(ID3D11DeviceContext *devContext);

	unsigned int VertexCount() const { return vertexCount; }
	unsigned int IndexCount() const { return indexCount; }
	ID3D11ShaderResourceView *GetTexture(unsigned int id) const { return texture[id]->GetTexture(); }
	ID3D11ShaderResourceView **GetTextureArray(ID3D11ShaderResourceView **textureArray) const;
	unsigned int TextureCount() const { return textureCount; }
	
private:
	Model& operator= (const Model&);
	Model(const Model&);

	bool LoadModel(const WCHAR *filename, Vertex *vert);
	bool LoadBumpModel(const WCHAR *filename, BumpVertex *vert);
	bool CheckModelCounts(const WCHAR *filename, unsigned int &vCount, unsigned int &tCount, unsigned int &nCount, unsigned int &iCount) const;
	bool LoadTexture(ID3D11Device *dev, const WCHAR *filename);
	bool LoadTextures(ID3D11Device *dev, const WCHAR *skyTexture, const WCHAR *gradientTexture);
	bool LoadTextures(ID3D11Device *device, const WCHAR *colourTexture, const WCHAR *normalTexture, const WCHAR *specularTexture);
	void CalculateTangBinorm(BumpVertex &vert1, BumpVertex &vert2, BumpVertex &vert3, DirectX::XMFLOAT3 *tangent, DirectX::XMFLOAT3 *binormal) const;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;
	unsigned int vertexCount, texCoCount, normCount, faceCount, indexCount, textureCount;
	Texture **texture;
};

