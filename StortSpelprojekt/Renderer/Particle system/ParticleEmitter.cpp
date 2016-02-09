#include "ParticleEmitter.h"
using namespace DirectX;

namespace Renderer
{

	ParticleEmitter::ParticleEmitter()
	{
	}

	ParticleEmitter::ParticleEmitter(const ParticleType& type, const XMFLOAT3& position, const XMFLOAT4 color, int particleCount, float timeLimit, bool isActive)
	{
		_type = type;
		_position = position;
		_timeLeft = timeLimit;
		_isActive = isActive;

		_particles.reserve(particleCount);

		for (int i = 0; i < particleCount; i++)
		{
			Particle particle(position, XMFLOAT3(0, 1, 0), color);
			_particles.push_back(particle);
		}
	}

	ParticleEmitter::~ParticleEmitter()
	{
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
	}

	void ParticleEmitter::Update(double deltaTime)
	{
		if (_timeLeft <= 0)
		{
			Deactivate();
		}
		else
		{
			switch (_type)
			{
				case SPLASH:
				{
					for (Particle& i : _particles)
					{
						if (i.IsActive())
						{
							//TODO: Update particle positions in a splash pattern. /Jonas
						}
					}
					break;
				}
				case SMOKE:
				{
					for (Particle& i : _particles)
					{
						if (i.IsActive())
						{
							//TODO: Update particle positions in a smoke pattern. /Jonas
						}
					}
					break;
				}
				case ELECTRICITY:
				{
					for (Particle& i : _particles)
					{
						if (i.IsActive())
						{
							//TODO: Update particle positions in a pattern resembling flashes/elctricity. /Jonas
						}
					}
					break;
				}
				default:
				{
					throw std::runtime_error("ParticleEmitter::Update: Invalid particle type");
					break;
				}
			}

			_timeLeft -= (float)deltaTime;
		}
	}

	DirectX::XMFLOAT3 ParticleEmitter::GetPosition() const
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
			for (int i = 0; i < diff; i++)
			{
				Particle particle(_position);
				_particles.push_back(particle);
			}
		}
	}

	bool ParticleEmitter::HasTimeLeft() const
	{
		if(_timeLeft > 0)
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
