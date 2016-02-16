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
		_particles.clear();
		_vertexSize = 0;
		_particleCount = 0;
	}

	ParticleEmitter::ParticleEmitter(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const ParticleType& type, const ParticleSubType& subType, const XMFLOAT3& position, const XMFLOAT3& direction, int particleCount, float timeLimit, bool isActive, const XMFLOAT3& target)
	{
		_type = type;
		_subType = subType;
		_position = position;
		_timeLeft = timeLimit;
		_isActive = isActive;
		_device = device;
		_deviceContext = deviceContext;
		_particleCount = particleCount;
		_particles.clear();

		CreateParticles(particleCount, direction, target);

		CreateVertexBuffer();
	}

	ParticleEmitter::~ParticleEmitter()
	{
		SAFE_RELEASE(_particlePointsBuffer);
		_device = nullptr;
		_deviceContext = nullptr;
		_particles.clear();
	}

	void ParticleEmitter::Reset(const ParticleType& type, const ParticleSubType& subType, const XMFLOAT3& position, const XMFLOAT3& direction, int particleCount, float timeLimit, bool isActive, const XMFLOAT3& target)
	{
		_type = type;
		_subType = subType;
		_position = position;
		_timeLeft = timeLimit;
		_isActive = isActive;
		_particleCount = particleCount;

		CreateParticles(particleCount, direction, target);
		CreateVertexBuffer();
	}

	void ParticleEmitter::CreateParticles(int count, const XMFLOAT3& baseDirection, const XMFLOAT3& targetPosition)
	{
		int diff = count - (signed)_particles.size();

		//The vector of particles needs to be created/recreated/enlarged
		if (diff > 0)
		{
			_particles.clear();
			for (int i = 0; i < diff; i++)
			{
				//TODO: Set offset on the direction and position /Jonas
				XMFLOAT3 dir = baseDirection;

				Particle particle(XMFLOAT3(-20 / 100.0f + (rand() % 20) / 100.0f, 0, -20 / 100.0f + (rand() % 20) / 100.0f), dir);
				particle.Activate();
				_particles.push_back(particle);
			}
		}
		//The vector is big enough to reuse
		else
		{
			for (int i = 0; i < count; i++)
			{
				//TODO: Set offset on the direction and position /Jonas
				XMFLOAT3 dir = baseDirection;

				_particles.at(i).Reset(XMFLOAT3(-20 / 100.0f + (rand() % 20) / 100.0f, 0, -20 / 100.0f + (rand() % 20) / 100.0f), dir);
				_particles.at(i).Activate();
			}
		}
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
					float velocity = 0;
					XMFLOAT3 position = p.GetPosition();
					XMFLOAT3 direction = p.GetDirection();
					switch (_type)
					{
						case SPLASH:
						{
							velocity = 1.0f;
							position.y += direction.y*velocity;
							position.x += direction.x*velocity;
							position.z += direction.z*velocity;

							p.SetPosition(position);

							break;
						}
						case SMOKE:
						{
							//TODO: Move in smoke pattern
							velocity = 0.25f;

							_position.y += (rand() % 100) / 100.0f;
							_position.z += ((rand() % 5) / 50.0f) - 2.5f / 10.0f;
							_position.x += ((rand() % 5) / 50.0f) - 2.5f / 10.0f;

							if (_position.y > 5)
							{
								_position.y = 0;
							}

							break;
						}
						case ELECTRICITY:
						{
							//TODO: generate a bolt of pixels towards target
							//http://gamedevelopment.tutsplus.com/tutorials/how-to-generate-shockingly-good-2d-lightning-effects--gamedev-2681
							break;
						}
						case FIRE:
						{
							velocity = 0.5f;
							//TODO: move particles in fire pattern. Upwards?
							break;
						}
						default:
						{
							throw std::runtime_error("Particle::Update: Invalid particle type");
							break;
						}
					}
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
		return _particleCount;
	}

	int ParticleEmitter::GetVertexSize() const
	{
		return _vertexSize;
	}

	void ParticleEmitter::SetPosition(const XMFLOAT3 position)
	{
		_position = position;
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

		for (Particle& p :_particles)
		{
			p.Deactivate();
		}
	}

}
