#include "ParticleEmitter.h"
#include "../stdafx.h"
using namespace DirectX;

namespace Renderer
{

	ParticleEmitter::ParticleEmitter(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		_device = device;
		_deviceContext = deviceContext;
		_isActive = false;
		_type = SPLASH;
		_subType = BLOOD;
		_position = XMFLOAT3(0, 0, 0);
		_timeLeft = 0;
		_particlePointsBuffer = nullptr;
		_particles.reserve(20);
		_vertexSize = 0;
	}

	ParticleEmitter::ParticleEmitter(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const ParticleType& type, const ParticleSubType& subType, const XMFLOAT3& position, const XMFLOAT4 color, int particleCount, float timeLimit, bool isActive)
	{
		_type = type;
		_subType = subType;
		_position = position;
		_timeLeft = timeLimit;
		_isActive = isActive;
		_device = device;
		_deviceContext = deviceContext;

		_particles.reserve(particleCount);

		for (int i = 0; i < particleCount; i++)
		{
			//TODO: Set small offsets on particle position
			Particle particle(XMFLOAT3(-20 / 100.0f + (rand() % 20) / 100.0f, 0, -20/100.0f + (rand() % 20) / 100.0f), XMFLOAT3(0, 1, 0), color);
			particle.Activate();
			_particles.push_back(particle);
		}

		CreateVertexBuffer();
	}

	ParticleEmitter::~ParticleEmitter()
	{
		SAFE_RELEASE(_particlePointsBuffer);
		_device = nullptr;
		_deviceContext = nullptr;
		_particles.clear();
	}

	//Used for initializing the vertex buffer that is holding all the positions of the particles
	void ParticleEmitter::CreateVertexBuffer()
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

		_vertexSize = sizeof(ParticleVertex);

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = _vertexSize * particleCount;;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = points.data();
		HRESULT result = _device->CreateBuffer(&bufferDesc, &data, &_particlePointsBuffer);
		if (FAILED(result))
		{
			throw std::runtime_error("ParticleEmitter::CreateVertexBuffer: Failed to create _particlePointsBuffer");
		}
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
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		HRESULT hr = _deviceContext->Map(_particlePointsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			throw std::runtime_error("ParticleEmitter::UpdateVertexBuffer: Failed to Map _particlePointsBuffer");
		}

		memcpy(mappedResource.pData, points.data(), sizeof(ParticleVertex) * particleCount);
		_deviceContext->Unmap(_particlePointsBuffer, 0);
	}

	void ParticleEmitter::Reset(const ParticleType& type, const ParticleSubType& subType, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4 color, int particleCount, float timeLimit, bool isActive)
	{
		_type = type;
		_subType = subType;
		_position = position;
		_timeLeft = timeLimit;
		_isActive = isActive;

		_particles.clear();
		
		for (int i = 0; i < particleCount; i++)
		{
			//TODO: Set small offsets on position?
			Particle particle(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1, 0), color);
			_particles.push_back(particle);
		}

		CreateVertexBuffer();
	}

	void ParticleEmitter::Update(double deltaTime)
	{
		if (_timeLeft <= 0)
		{
			Deactivate();
		}
		else
		{
			for (Particle& p : _particles)
			{
				if (p.IsActive())
				{
					p.Update(deltaTime, _type);
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

	XMFLOAT3 ParticleEmitter::GetPosition() const
	{
		return _position;
	}

	ParticleType ParticleEmitter::GetType() const
	{
		return _type;
	}

	ParticleSubType ParticleEmitter::GetSubType() const
	{
		return _subType;
	}

	int ParticleEmitter::GetParticleCount() const
	{
		return _particles.size();
	}

	int ParticleEmitter::GetVertexSize() const
	{
		return _vertexSize;
	}

	void ParticleEmitter::SetPosition(const DirectX::XMFLOAT3 position)
	{
		_position = position;
	}
	void ParticleEmitter::SetType(const ParticleType& type)
	{
		_type = type;
	}

	void ParticleEmitter::SetSubType(const ParticleSubType& subType)
	{
		_subType = subType;
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

		CreateVertexBuffer();
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
