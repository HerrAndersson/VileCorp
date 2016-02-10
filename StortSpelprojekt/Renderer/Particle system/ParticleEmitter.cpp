#include "ParticleEmitter.h"
#include "../stdafx.h"
using namespace DirectX;

namespace Renderer
{

	ParticleEmitter::ParticleEmitter(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		_device = device;
		_deviceContext = deviceContext;
	}

	ParticleEmitter::ParticleEmitter(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const ParticleType& type, const XMFLOAT3& position, const XMFLOAT4 color, int particleCount, float timeLimit, bool isActive)
	{
		_type = type;
		_position = position;
		_timeLeft = timeLimit;
		_isActive = isActive;
		_device = device;
		_deviceContext = deviceContext;

		_particles.reserve(particleCount);

		for (int i = 0; i < particleCount; i++)
		{
			Particle particle(position, XMFLOAT3(0, 1, 0), color);
			_particles.push_back(particle);
		}

		BuildVertexBuffer();
	}

	ParticleEmitter::~ParticleEmitter()
	{
		_particles.clear();
		SAFE_RELEASE(_particlePointsBuffer);
		_device = nullptr;
		_deviceContext = nullptr;
	}

	//Used for initializing the vertex buffer that is holding all the positions of the particles
	void ParticleEmitter::BuildVertexBuffer()
	{
		SAFE_RELEASE(_particlePointsBuffer);

		unsigned int particleCount = _particles.size();
		std::vector<ParticleVertex> points;
		points.reserve(particleCount);

		for (Particle& p : _particles)
		{
			ParticleVertex v;
			v._position = p.GetPosition();
			points.push_back(v);
		}

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(ParticleVertex) * particleCount;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = points.data();
		HRESULT result = _device->CreateBuffer(&bufferDesc, &data, &_particlePointsBuffer);
	}

	void ParticleEmitter::UpdateVertexBuffer()
	{
		unsigned int particleCount = _particles.size();
		std::vector<ParticleVertex> points;
		points.reserve(particleCount);

		for (Particle& p : _particles)
		{
			ParticleVertex v;
			v._position = p.GetPosition();
			points.push_back(v);
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		_deviceContext->Map(_particlePointsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, points.data(), sizeof(ParticleVertex) * particleCount);
		_deviceContext->Unmap(_particlePointsBuffer, 0);
	}

	void ParticleEmitter::Reset(const ParticleType& type, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4 color, int particleCount, float timeLimit, bool isActive)
	{
		_type = type;
		_position = position;
		_timeLeft = timeLimit;
		_isActive = isActive;

		_particles.clear();
		
		for (int i = 0; i < particleCount; i++)
		{
			Particle particle(position, XMFLOAT3(0, 1, 0), color);
			_particles.push_back(particle);
		}

		BuildVertexBuffer();
	}

	void ParticleEmitter::Update(double deltaTime)
	{
		if (_timeLeft <= 0)
		{
			Deactivate();
		}
		else
		{
			for (Particle& i : _particles)
			{
				if (i.IsActive())
				{
					i.Update(deltaTime, _type);
				}
			}
		}

		UpdateVertexBuffer();

		_timeLeft -= (float)deltaTime;
	}

	ID3D11Buffer* ParticleEmitter::GetParticleBuffer()
	{
		return _particlePointsBuffer;
	}

	std::vector<Particle>* ParticleEmitter::GetParticles()
	{
		return &_particles;
	}

	XMFLOAT3 ParticleEmitter::GetPosition() const
	{
		return _position;
	}

	ParticleType ParticleEmitter::GetType() const
	{
		return _type;
	}

	int ParticleEmitter::GetParticleCount() const
	{
		return _particles.size();
	}

	void ParticleEmitter::SetPosition(const DirectX::XMFLOAT3 position)
	{
		_position = position;
	}
	void ParticleEmitter::SetType(const ParticleType& type)
	{
		_type = type;
	}

	void ParticleEmitter::SetParticleCount(const int& particleCount)
	{
		int diff = particleCount - (signed)_particles.size();
		if (diff > 0)
		{
			_particles.resize(particleCount);
			for (int i = 0; i < diff; i++)
			{
				Particle particle(_position);
				_particles.push_back(particle);
			}
		}

		BuildVertexBuffer();
	}

	bool ParticleEmitter::HasTimeLeft() const
	{
		return (_timeLeft > 0);
	}

	bool ParticleEmitter::IsActive() const
	{
		return _isActive;
	}

	void ParticleEmitter::Activate()
	{
		_isActive = true;
	}

	void ParticleEmitter::Deactivate()
	{
		_isActive = false;
	}

}
