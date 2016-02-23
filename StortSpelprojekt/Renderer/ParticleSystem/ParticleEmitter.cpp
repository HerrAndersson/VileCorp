#include "ParticleEmitter.h"
#include "../stdafx.h"
using namespace DirectX;

namespace Renderer
{

	ParticleEmitter::ParticleEmitter(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ParticleModifierOffsets* modifers)
	{
		_device = device;
		_deviceContext = deviceContext;
		_isActive = false;
		_type = SPLASH;
		_subType = BLOOD_SUBTYPE;
		_position = XMFLOAT3(0, 0, 0);
		_timeLeft = 0;
		_particlePointsBuffer = nullptr;
		_particles.clear();
		_vertexSize = 0;
		_particleCount = 0;
		_modifiers = modifers;
		_particleScale = 1.0f;
		_targetPosition = XMFLOAT3(0, 0, 0);
		_ownerID = -1;
		_isTimed = true;
		_shaderData.clear();
	}

	ParticleEmitter::ParticleEmitter(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const ParticleType& type, const ParticleSubType& subType, int ownerID, const XMFLOAT3& position, const XMFLOAT3& direction, int particleCount, float timeLimit, float scale, bool isActive, bool isTimed, ParticleModifierOffsets* modifers, const XMFLOAT3& target)
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
		_modifiers = modifers;
		_particleScale = scale;
		_targetPosition = target;
		_ownerID = ownerID;
		_baseDirection = direction;
		_isTimed = isTimed;

		_shaderData.resize(_particleCount);
		_particles.reserve(_particleCount);

		CreateAllParticles(particleCount, _targetPosition);

		CreateVertexBuffer();
	}

	ParticleEmitter::~ParticleEmitter()
	{
		SAFE_RELEASE(_particlePointsBuffer);
		_device = nullptr;
		_deviceContext = nullptr;
		_particles.clear();
	}

	void ParticleEmitter::Reset(const ParticleType& type, const ParticleSubType& subType, int ownerID, const XMFLOAT3& position, const XMFLOAT3& direction, int particleCount, float timeLimit, float scale, bool isActive, bool isTimed, const XMFLOAT3& target)
	{
		_type = type;
		_subType = subType;
		_position = position;
		_timeLeft = timeLimit;
		_isActive = isActive;
		_particleCount = particleCount;
		_particleScale = scale;
		_targetPosition = target;
		_ownerID = ownerID;
		_baseDirection = direction;
		_isTimed = isTimed;

		_shaderData.resize(_particleCount);
		_particles.reserve(_particleCount);

		CreateAllParticles(particleCount, target);
		CreateVertexBuffer();
	}

	void ParticleEmitter::CreateElectricityPattern(int count, const DirectX::XMFLOAT3& targetPosition)
	{
		_particles.resize(count);

		XMVECTOR basePos = XMLoadFloat3(&_position);
		XMVECTOR target = XMLoadFloat3(&targetPosition);
		XMVECTOR posToTarget = target - basePos;

		float length = XMVectorGetX(XMVector3Length(posToTarget));
		posToTarget = XMVector3Normalize(posToTarget);

		for (int i = 0; i < count; i++)
		{
			XMVECTOR posV = posToTarget * (length / (float)count) * (float)i;
			XMFLOAT3 pos;
			XMStoreFloat3(&pos, posV);

			_particles[i] = Particle(pos, 0.0f, _modifiers->_lightningRepeatTime, 0);
		}

		ComputeLightning(0, count - 1, length);
	}

	void ParticleEmitter::ComputeLightning(int startIndex, int endIndex, float totalLength)
	{
		if (endIndex - startIndex <= 1)
		{
			return;
		}

		int midpoint = (startIndex + endIndex) / 2;

		ComputeLightning(startIndex, midpoint, totalLength);
		ComputeLightning(midpoint, endIndex, totalLength);

		XMVECTOR basePos = XMLoadFloat3(&_particles[startIndex].GetPosition());
		XMVECTOR target = XMLoadFloat3(&_particles[endIndex].GetPosition());
		XMVECTOR posToTarget = target - basePos;

		float length = XMVectorGetX(XMVector3Length(posToTarget));

		//Breaks if posToTarget is (0,1,0,0)
		XMVECTOR perpendicular1 = XMVector3Cross(posToTarget, XMVectorSet(0, 1, 0, 0));
		perpendicular1 = XMVector3Normalize(perpendicular1);
		XMVECTOR perpendicular2 = XMVector3Cross(posToTarget, perpendicular1);
		perpendicular2 = XMVector3Normalize(perpendicular2);

		XMVECTOR midpointPos = XMLoadFloat3(&_particles[midpoint].GetPosition());

		midpointPos += perpendicular1 * GetRandomOffset(totalLength/_particleCount, true) + perpendicular2 * GetRandomOffset(totalLength/_particleCount, true);

		XMFLOAT3 pos;
		XMStoreFloat3(&pos, midpointPos);
		_particles[midpoint].SetPosition(pos);
	}

	void ParticleEmitter::CreateAllParticles(int count, const XMFLOAT3& targetPosition)
	{

		if (_type == ParticleType::ELECTRICITY)
		{
			CreateElectricityPattern(count, targetPosition);
		}
		else
		{
			int diff = count - (signed)_particles.size();

			//The vector of particles needs to be created/recreated/enlarged
			if (diff > 0)
			{
				_particles.clear();
				for (int i = 0; i < count; i++)
				{
					if (_type != ParticleType::ELECTRICITY)
					{
						//Add the particle to the vector of particles
						Particle particle = CreateSingleParticle();
						particle.Activate();
						_particles.push_back(particle);

						//Add relevant data to the vector holding shader information
						XMFLOAT3 pos = particle.GetPosition();
						_shaderData[i]._position = XMFLOAT4(pos.x, pos.y, pos.z, particle.GetTextureNumber());
					}
				}
			}
			//The vector is big enough to reuse
			else
			{
				for (int i = 0; i < count; i++)
				{
					if (_type != ParticleType::ELECTRICITY)
					{
						Particle particle = CreateSingleParticle();
						_particles.at(i) = particle;
						_particles.at(i).Activate();

						//Add relevant data to the vector holding shader information
						XMFLOAT3 pos = particle.GetPosition();
						_shaderData[i]._position = XMFLOAT4(pos.x, pos.y, pos.z, particle.GetTextureNumber());
					}
				}
			}
		}
	}

	float ParticleEmitter::GetRandomOffset(float maxOffset, bool includeNegative)
	{
		float offset = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxOffset + std::numeric_limits<float>::epsilon())));

		if (includeNegative)
		{
			int sign = rand() % 2;

			if (sign > 0)
			{
				offset *= -1;
			}
		}

		return offset;
	}

	float ParticleEmitter::GetRandomOffsetInRange(DirectX::XMFLOAT2 offsets)
	{
		return offsets.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (offsets.y - offsets.x + std::numeric_limits<float>::epsilon())));
	}

	DirectX::XMFLOAT3 ParticleEmitter::Normalize(DirectX::XMFLOAT3 dir)
	{
		float length = abs(dir.x) + abs(dir.y) + abs(dir.z);

		//Eliminates division by zero
		if (length <= 0)
		{
			return XMFLOAT3(0, 0, 0);
		}

		return XMFLOAT3(dir.x / length, dir.y / length, dir.z / length);
	}

	Particle ParticleEmitter::CreateSingleParticle()
	{
		Particle p;
		XMFLOAT3 pos;
		XMFLOAT3 dir;

		float positionOffset;
		float directionOffset;
		float repeatTime;
		XMFLOAT2 speedRange;
		float speed;
		bool hasModifiers = false;
		float randomTexture = GetRandomOffset(PARTICLE_TEXTURE_COUNT, false);

		//Set modifier values depending on the type
		switch (_type)
		{
			case SPLASH:
			{
				positionOffset = _modifiers->_splashPositionOffset;
				directionOffset = _modifiers->_splashDirectionOffset;
				speedRange = _modifiers->_splashSpeedRange;
				repeatTime = FLT_MAX;
				hasModifiers = true;
				break;
			}
			case SMOKE:
			{
				positionOffset = _modifiers->_smokePositionOffset;
				directionOffset = _modifiers->_smokeDirectionOffset;
				speedRange = _modifiers->_smokeSpeedRange;
				repeatTime = _modifiers->_smokeRepeatTime;
				hasModifiers = true;
				break;
			}
			case FIRE:
			{
				positionOffset = _modifiers->_firePositionOffset;
				directionOffset = _modifiers->_fireDirectionOffset;
				speedRange = _modifiers->_fireSpeedRange;
				repeatTime = _modifiers->_fireRepeatTime;
				hasModifiers = true;

				break;
			}
			default:
			{
				break;
			}
		}

		//Set all common variables
		if (hasModifiers)
		{
			pos.x = GetRandomOffset(positionOffset, true);
			pos.y = GetRandomOffset(positionOffset, true);
			pos.z = GetRandomOffset(positionOffset, true);

			dir.x = _baseDirection.x + GetRandomOffset(directionOffset, true);
			dir.y = _baseDirection.y + GetRandomOffset(directionOffset, true);
			dir.z = _baseDirection.z + GetRandomOffset(directionOffset, true);
			dir = Normalize(dir);

			speed = GetRandomOffsetInRange(speedRange);
		}

		//Set variables that are not shared for all types, and create the particle
		switch (_type)
		{
			case SPLASH:
			{
				p = Particle(pos, speed, repeatTime, randomTexture, dir);

				break;
			}
			case FIRE:
			case SMOKE:
			{
				XMFLOAT2 offsets(repeatTime /2, repeatTime);
				float time = GetRandomOffsetInRange(offsets);

				p = Particle(pos, speed, time, randomTexture, dir);

				break;
			}
			case MUZZLE_FLASH:
			{
				pos = XMFLOAT3(GetRandomOffset(0.01f, true), GetRandomOffset(0.01f, true), GetRandomOffset(0.01f, true));
				p = Particle(pos, 0, 100, randomTexture);
				break;
			}
			case ICON:
			{
				//Do not set random texture here. The desired icon texture will always be at register 0
				p = Particle(XMFLOAT3(0, 0, 0), 0, 100, 0);
				break;
			}
			default:
			{
				throw std::runtime_error("Particle::CreateSingleParticle: Invalid particle type");
				break;
			}
		}

		return p;
	}

	//Used for initializing the vertex buffer that is holding all the positions of the particles
	void ParticleEmitter::CreateVertexBuffer()
	{
		SAFE_RELEASE(_particlePointsBuffer);
		unsigned int particleCount = _particles.size();

		for (unsigned int i = 0; i < particleCount; i++)
		{
			ParticleVertex v;
			XMFLOAT3 pos = _particles[i].GetPosition();
			v._position = XMFLOAT4(pos.x, pos.y, pos.z, _particles[i].GetTextureNumber());
			_shaderData[i] = v;
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
		data.pSysMem = _shaderData.data();
		HRESULT result = _device->CreateBuffer(&bufferDesc, &data, &_particlePointsBuffer);
		if (FAILED(result))
		{
			throw std::runtime_error("ParticleEmitter::CreateVertexBuffer: Failed to create _particlePointsBuffer");
		}
	}

	void ParticleEmitter::UpdateVertexBuffer()
	{
		unsigned int particleCount = _particles.size();
		for (unsigned int i = 0; i < particleCount; i++)
		{
			ParticleVertex v;
			XMFLOAT3 pos = _particles[i].GetPosition();
			v._position = XMFLOAT4(pos.x, pos.y, pos.z, _particles[i].GetTextureNumber());
			_shaderData[i] = v;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		HRESULT hr = _deviceContext->Map(_particlePointsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			throw std::runtime_error("ParticleEmitter::UpdateVertexBuffer: Failed to Map _particlePointsBuffer");
		}

		memcpy(mappedResource.pData, _shaderData.data(), sizeof(ParticleVertex) * particleCount);
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
					XMFLOAT3 position = p.GetPosition();
					XMFLOAT3 direction = p.GetDirection();
					float speed = p.GetSpeed();
					float deltatimeSeconds = (float)deltaTime / 1000.0f;

					switch (_type)
					{
						case SPLASH:
						{
							position.y += direction.y * speed * deltatimeSeconds;
							position.x += direction.x * speed * deltatimeSeconds;
							position.z += direction.z * speed * deltatimeSeconds;

							p.SetPosition(position);

							break;
						}
						case ELECTRICITY:
						{
							if (_particles.at(0).GetTimeLeft() <= 0)
							{
								CreateElectricityPattern(_particleCount, _targetPosition);
							}

							_particles.at(0).DecreaseTimeLeft((float)deltaTime);

							break;
						}
						case SMOKE:
						case FIRE:
						{
							position.y += direction.y * speed * deltatimeSeconds;
							position.x += direction.x * speed * deltatimeSeconds;
							position.z += direction.z * speed * deltatimeSeconds;

							if (p.GetTimeLeft() < 0)
							{
								p = CreateSingleParticle();
							}
							else
							{
								p.SetPosition(position);
								p.DecreaseTimeLeft((float)deltaTime);
							}

							break;
						}
						case MUZZLE_FLASH:
						case ICON:
						{
							p.DecreaseTimeLeft((float)deltaTime);
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

		//The emitter has no owner and should be removed when the time is out. For example Splash.
		//If the emitter has an owner this can be set to false, but then the emitter has to be deactivated from there through an UPDATE message in the queue
		if (_isTimed)
		{
			_timeLeft -= (float)deltaTime;
		}
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

	float ParticleEmitter::GetParticleScale() const
	{
		return _particleScale;
	}

	int ParticleEmitter::GetOwnerID() const
	{
		return _ownerID;
	}

	void ParticleEmitter::SetPosition(const XMFLOAT3& position)
	{
		_position = position;
	}

	void ParticleEmitter::SetDirection(const XMFLOAT3& direction)
	{
		_baseDirection = direction;
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
