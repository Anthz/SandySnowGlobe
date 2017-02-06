#pragma once

#include <d3d11.h>
#include "DirectXMath.h"
#include "Texture.h"
#include "Shader.h"

class ParticleSystem
{
public:
	struct Particle
	{
		Particle() : alive(false){};
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 velocity;
		DirectX::XMFLOAT4 colour;
		bool alive;
	};

	struct ParticleVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texCoord;
		DirectX::XMFLOAT4 colour;
	};

	struct ParticleInstance
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotation;
	};

	enum ParticleType
	{
		SNOW,
		FIRE,
		DUST,
		RAIN
	};

	ParticleSystem();
	ParticleSystem(ParticleType type, DirectX::XMFLOAT3 position, Shader *particleShader);
	~ParticleSystem();

	ParticleSystem& operator= (const ParticleSystem& p);
	ParticleSystem(const ParticleSystem& p);

	bool Init(ID3D11Device *dev, const WCHAR *textureName);
	bool Init(ID3D11Device *dev, const WCHAR *tex1, const WCHAR *tex2, const WCHAR *tex3);
	void Update(ID3D11DeviceContext *devCon, float dt);
	void Render(ID3D11DeviceContext *devCon, const DirectX::XMFLOAT4X4 *wMatrix, DirectX::XMFLOAT4X4 *vMatrix, DirectX::XMFLOAT4X4 *pMatrix);

	bool *Active() { return &active; }
	void Active(bool val) { active = val; }

	DirectX::XMFLOAT3 SystemPosition() const { return systemPosition; }
	void SystemPosition(DirectX::XMFLOAT3 val) { systemPosition = val; }

private:
	bool LoadTexture(ID3D11Device *dev, const WCHAR *textureName);
	bool InitParticles();
	bool InitBuffers(ID3D11Device *dev);
	bool UpdateVertices(ID3D11DeviceContext *devCon);
	void Emit(float dt);
	void Kill();

	Particle *particles;
	ParticleInstance *instances;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, instanceBuffer;
	ParticleType type;
	Texture *texture;
	Shader *shader;
	unsigned int maxParticles, particleCount, vertexCount, instanceCount;
	float particleSize, particleFreq, accumulatedTime;
	DirectX::XMFLOAT3 systemPosition, particleVelocity, particleVelDiff, particleDispDiff;

	DirectX::XMFLOAT4 particleColour;
	bool active;
};

