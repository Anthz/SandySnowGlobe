#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ParticleType particleType, DirectX::XMFLOAT3 position, Shader *particleShader)
{
	shader = particleShader;
	vertexBuffer = nullptr;
	instanceBuffer = nullptr;
	type = particleType;
	active = false;
	texture = nullptr;
	vertexCount = 0;
	instanceCount = 0;
	maxParticles = 0;
	particleCount = 0;
	particleSize = 0.0f;
	particleFreq = 0.0f;
	systemPosition = position;
	particleVelocity = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	particleVelDiff = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	particleDispDiff = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	particleColour = DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f };
	particles = nullptr;
	instances = nullptr;
	accumulatedTime = 0.0f;
}

ParticleSystem::ParticleSystem()
{
	shader = nullptr;
	vertexBuffer = nullptr;
	instanceBuffer = nullptr;
	type = RAIN;
	active = false;
	texture = nullptr;
	vertexCount = 0;
	instanceCount = 0;
	maxParticles = 0;
	particleCount = 0;
	particleSize = 0.0f;
	particleFreq = 0.0f;
	systemPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);;
	particleVelocity = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	particleVelDiff = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	particleDispDiff = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	particleColour = DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f };
	particles = nullptr;
	instances = nullptr;
	accumulatedTime = 0.0f;
}

ParticleSystem::~ParticleSystem()
{
	delete[] particles;
	delete[] instances;
	delete texture;
}

/// <summary>
/// Initialise texture, particle specific variables and buffers
/// </summary>
/// <param name="dev">Standard ID3D11Device</param>
/// <param name="textureName">Colour texture filepath</param>
/// <returns></returns>
bool ParticleSystem::Init(ID3D11Device *dev, const WCHAR *textureName)
{
	if(!LoadTexture(dev, textureName))
	{
		return false;
	}

	if(!InitParticles())
	{
		return false;
	}

	if(!InitBuffers(dev))
	{
		return false;
	}

	return true;
}

/// <summary>
/// Initialise particle specific variables dependent on type
/// </summary>
/// <returns></returns>
bool ParticleSystem::InitParticles()
{
	switch(type)
	{
		case ParticleSystem::SNOW:
			maxParticles = 50000;
			particleSize = 0.3f;
			particleFreq = 5000.0f;
			particleVelocity = DirectX::XMFLOAT3{ 0.0f, -8.0f, 0.0f };
			particleVelDiff = DirectX::XMFLOAT3{ 0.0f, 4.0f, 0.0f };
			particleDispDiff = DirectX::XMFLOAT3{ 75.0f, 75.0f, 75.0f };
			break;
		case ParticleSystem::FIRE:
			maxParticles = 50;
			particleSize = 0.2f;
			particleFreq = 10.0f;
			particleVelocity = DirectX::XMFLOAT3{ 0.0f, 1.5f, 0.0f };
			particleVelDiff = DirectX::XMFLOAT3{ 0.75f, 0.5f, 0.75f };
			particleDispDiff = DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f };
			particleColour = DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f };
			break;
		case ParticleSystem::DUST:
			break;
		case ParticleSystem::RAIN:
			maxParticles = 50000;
			particleSize = 0.1f;
			particleFreq = 10000.0f;
			particleVelocity = DirectX::XMFLOAT3{ 0.0f, -50.0f, 0.0f };
			particleVelDiff = DirectX::XMFLOAT3{ 0.0f, 10.0f, 0.0f };
			particleDispDiff = DirectX::XMFLOAT3{ 75.0f, 75.0f, 75.0f };
			break;
	}

	particles = new Particle[maxParticles];

	particleCount = 0;
	accumulatedTime = 0.0f;

	return true;
}

/// <summary>
/// Initialise vertex/instance buffers with billboarded quad
/// </summary>
/// <param name="dev">Standard ID3D11Device</param>
/// <returns></returns>
bool ParticleSystem::InitBuffers(ID3D11Device *dev)
{
	vertexCount = 6;
	instanceCount = maxParticles;

	ParticleVertex *vertices = new ParticleVertex[vertexCount];

	memset(vertices, 0, sizeof(ParticleVertex) * vertexCount);
	
	//create simple textured quad
	vertices[0].position = DirectX::XMFLOAT3(-particleSize, -particleSize, 0.0f);
	vertices[0].texCoord = DirectX::XMFLOAT2(0.0f, 1.0f);
	vertices[0].colour = particleColour;
	vertices[1].position = DirectX::XMFLOAT3(-particleSize, particleSize, 0.0f);
	vertices[1].texCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
	vertices[1].colour = particleColour;
	vertices[2].position = DirectX::XMFLOAT3(particleSize, -particleSize, 0.0f);
	vertices[2].texCoord = DirectX::XMFLOAT2(1.0f, 1.0f);
	vertices[2].colour = particleColour;
	vertices[3].position = DirectX::XMFLOAT3(particleSize, -particleSize, 0.0f);
	vertices[3].texCoord = DirectX::XMFLOAT2(1.0f, 1.0f);
	vertices[3].colour = particleColour;
	vertices[4].position = DirectX::XMFLOAT3(-particleSize, particleSize, 0.0f);
	vertices[4].texCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
	vertices[4].colour = particleColour;
	vertices[5].position = DirectX::XMFLOAT3(particleSize, particleSize, 0.0f);
	vertices[5].texCoord = DirectX::XMFLOAT2(1.0f, 0.0f);
	vertices[5].colour = particleColour;

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

	HRESULT result = dev->CreateBuffer(&vertexDesc, &vertexData, vertexBuffer.GetAddressOf());

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create vertex buffer"))
	{
		return false;
	}

	instances = new ParticleInstance[maxParticles];

	memset(instances, 0, sizeof(ParticleInstance) * maxParticles);

	D3D11_BUFFER_DESC instanceDesc;
	D3D11_SUBRESOURCE_DATA instanceData;

	instanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceDesc.ByteWidth = sizeof(ParticleInstance) * maxParticles;
	instanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceDesc.MiscFlags = 0;
	instanceDesc.StructureByteStride = 0;
	instanceDesc.Usage = D3D11_USAGE_DYNAMIC;

	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	result = dev->CreateBuffer(&instanceDesc, &instanceData, instanceBuffer.GetAddressOf());

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create index buffer"))
	{
		return false;
	}

	Memory::SafeDelete(vertices);

	return true;
}

/// <summary>
/// Remove "dead particles", emit new ones and update position of those alive
/// </summary>
/// <param name="devCon">Standard ID3D11DeviceContext</param>
/// <param name="dt">Delta time</param>
void ParticleSystem::Update(ID3D11DeviceContext *devCon, float dt)
{
	Kill();

	if(active)
	{
		Emit(dt);
	}

	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position.x = particles[i].position.x + (particles[i].velocity.x * dt);
		particles[i].position.y = particles[i].position.y + (particles[i].velocity.y * dt);
		particles[i].position.z = particles[i].position.z + (particles[i].velocity.z * dt);
	}

	UpdateVertices(devCon);
}

void ParticleSystem::Render(ID3D11DeviceContext *devCon, const DirectX::XMFLOAT4X4 *wMatrix, DirectX::XMFLOAT4X4 *vMatrix, DirectX::XMFLOAT4X4 *pMatrix)
{
	DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(wMatrix);
	m = DirectX::XMMatrixMultiply(m, DirectX::XMMatrixTranslation(systemPosition.x, systemPosition.y, systemPosition.z));
	DirectX::XMFLOAT4X4 worldTemp;
	DirectX::XMStoreFloat4x4(&worldTemp, m);

	unsigned int strides[2];
	strides[0] = sizeof(ParticleVertex);
	strides[1] = sizeof(ParticleInstance);

	unsigned int offsets[2];
	offsets[0] = 0;
	offsets[1] = 0;

	ID3D11Buffer *buffers[2];
	buffers[0] = vertexBuffer.Get();
	buffers[1] = instanceBuffer.Get();

	devCon->IASetVertexBuffers(0, 2, buffers, strides, offsets);
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Render(devCon, particleCount, &worldTemp, vMatrix, pMatrix, texture->GetTexture());
}

/// <summary>
/// Update instance buffer with new positions
/// </summary>
/// <param name="devCon">Standard ID3D11DeviceContext</param>
/// <returns></returns>
bool ParticleSystem::UpdateVertices(ID3D11DeviceContext *devCon)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	ParticleInstance *instancePtr;

	memset(instances, 0, sizeof(ParticleInstance) * particleCount);

	for(int i = 0; i < particleCount; i++)
	{
		instances[i].position = particles[i].position;
	}

	HRESULT result = devCon->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	if(result != S_OK)
	{
		return false;
	}

	instancePtr = (ParticleInstance *)resource.pData;
	memcpy(instancePtr, (void*)instances, sizeof(ParticleInstance) * particleCount);
	devCon->Unmap(instanceBuffer.Get(), 0);

	return true;
}

/// <summary>
/// Emits new particles if time req is met and count < max 
/// </summary>
/// <param name="dt">Delta time</param>
void ParticleSystem::Emit(float dt)
{
	bool emit = false;
	DirectX::XMFLOAT3 pos, vel;
	float dtReq = (1000.0f / particleFreq);
	int loopCounter;

	accumulatedTime += dt * 1000.0f;

	if(accumulatedTime > dtReq)
	{
		accumulatedTime = 0.0f;
		emit = true;
	}

	if(emit && particleCount < (maxParticles - 1))
	{
		particleCount++;

		pos.x = (((float)(rand()) - (float)(rand())) / (float)(RAND_MAX)* particleDispDiff.x) + systemPosition.x;
		pos.y = (((float)(rand()) / (float)(RAND_MAX)) * particleDispDiff.y) + (systemPosition.y / 2);
		pos.z = (((float)(rand()) - (float)(rand())) / (float)(RAND_MAX)* particleDispDiff.z) + systemPosition.z;

		vel.x = particleVelocity.x + ((float)(rand()) - (float)(rand())) / (float)(RAND_MAX)* particleVelDiff.x;
		vel.y = particleVelocity.y + ((float)(rand()) - (float)(rand())) / (float)(RAND_MAX)* particleVelDiff.y;
		vel.z = particleVelocity.z + ((float)(rand()) - (float)(rand())) / (float)(RAND_MAX)* particleVelDiff.z;

		unsigned int i = 0;
		bool placed = false;

		while(!placed)
		{
			if((particles[i].alive == false) || particles[i].position.z < pos.z)
			{
				placed = true;
			}
			else
			{
				i++;
			}
		}

		int j = particleCount;
		int k = j - 1;

		while(j != i && j > 0)
		{
			particles[j].position = particles[k].position;
			particles[j].velocity = particles[k].velocity;
			particles[j].colour = particles[k].colour;
			particles[j].alive = particles[k].alive;
			j--;
			k--;
		}

		particles[i].position = pos;
		particles[i].velocity = vel;
		particles[i].colour = particleColour;
		particles[i].alive = true;
	}
}

/// <summary>
/// Remove particles if they reach their "cutoff point"
/// </summary>
void ParticleSystem::Kill()
{
	for(int i = 0; i < maxParticles; i++)
	{
		if(type != FIRE)
		{
			if(particles[i].alive && particles[i].position.y < -10.0f)
			{
				particles[i].alive = false;
				particleCount--;

				for(int j = i; j < maxParticles - 1; j++)
				{
					particles[j].position = particles[j + 1].position;
					particles[j].velocity = particles[j + 1].velocity;
					particles[j].colour = particles[j + 1].colour;
					particles[j].alive = particles[j + 1].alive;
				}
			}
		}
		else
		{
			if(particles[i].alive && particles[i].position.y >= systemPosition.y + 15.0f)
			{
				particles[i].alive = false;
				particleCount--;

				for(int j = i; j < maxParticles - 1; j++)
				{
					particles[j].position = particles[j + 1].position;
					particles[j].velocity = particles[j + 1].velocity;
					particles[j].colour = particles[j + 1].colour;
					particles[j].alive = particles[j + 1].alive;
				}
			}
		}
	}
}

bool ParticleSystem::LoadTexture(ID3D11Device *dev, const WCHAR *textureName)
{
	texture = new Texture();
	HRESULT result = texture->Init(dev, textureName);

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Initialise particle texture"))
	{
		return false;
	}

	return true;
}

ParticleSystem& ParticleSystem::operator=(const ParticleSystem& p)
{
	shader = p.shader;
	vertexBuffer = nullptr;
	instanceBuffer = nullptr;
	type = p.type;
	active = p.active;
	texture = nullptr;
	vertexCount = p.vertexCount;
	instanceCount = p.instanceCount;
	maxParticles = p.maxParticles;
	particleCount = p.particleCount;
	particleSize = p.particleSize;
	particleFreq = p.particleFreq;
	systemPosition = p.systemPosition;
	particleVelocity = p.particleVelocity;
	particleVelDiff = p.particleVelDiff;
	particleDispDiff = p.particleDispDiff;
	particleColour = p.particleColour;
	particles = nullptr;
	instances = nullptr;
	accumulatedTime = 0.0f;

	return *this;
}

ParticleSystem::ParticleSystem(const ParticleSystem& p)
{
	shader = p.shader;
	vertexBuffer = nullptr;
	instanceBuffer = nullptr;
	type = p.type;
	active = p.active;
	texture = nullptr;
	vertexCount = p.vertexCount;
	instanceCount = p.instanceCount;
	maxParticles = p.maxParticles;
	particleCount = p.particleCount;
	particleSize = p.particleSize;
	particleFreq = p.particleFreq;
	systemPosition = p.systemPosition;
	particleVelocity = p.particleVelocity;
	particleVelDiff = p.particleVelDiff;
	particleDispDiff = p.particleDispDiff;
	particleColour = p.particleColour;
	particles = nullptr;
	instances = nullptr;
	accumulatedTime = 0.0f;
}


