#pragma once

#include <d3d11.h>
#include <DDSTextureLoader.h>
#include <wrl.h>
#include "DXUtil.h"

class Texture
{
public:
	Texture();
	~Texture();

	bool Init(ID3D11Device *dev, const std::wstring &filename);

	ID3D11ShaderResourceView *GetTexture() const { return texture.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
};

