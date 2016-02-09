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
			for (Particle& i : _particles)
			{
				if (i.IsActive())
				{
					i.Update()
				}
			}
		}

		_timeLeft -= (float)deltaTime;
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
			_particles.resize(particleCount);
			for (int i = 0; i < diff; i++)
			{
				Particle particle(_position);
				_particles.push_back(particle);
			}
		}
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
