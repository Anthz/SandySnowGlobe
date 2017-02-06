#include "Texture.h"

Texture::Texture()
{
	texture = nullptr;
}


Texture::~Texture()
{
}

bool Texture::Init(ID3D11Device *dev, const std::wstring &filename)
{
	HRESULT result = DirectX::CreateDDSTextureFromFile(dev, filename.c_str(), nullptr, texture.GetAddressOf());

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create DDS texture from file"))
	{
		return false;
	}

	return true;
}