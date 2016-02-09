#include "ParticleHandler.h"

namespace Renderer
{

	ParticleHandler::ParticleHandler()
	{
		_emitterCount = 5;
		_particleEmitters.reserve(_emitterCount);
	}

	ParticleHandler::~ParticleHandler()
	{
		_particleEmitters.clear();
	}

	void ParticleHandler::Update(double deltaTime)
	{
		for (ParticleEmitter& p : _particleEmitters)
		{
			if (p.IsActive())
			{
				p.Update(deltaTime);
			}
		}
	}

	void ParticleHandler::ActivateEmitter(ParticleType type, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, int particleCount, float timeLimit, bool isActive)
	{
		bool found = false;
		for (ParticleEmitter& p : _particleEmitters)
		{
			if (!p.IsActive())
			{
				p.Reset(type, position, color, particleCount, timeLimit, isActive);
				found = true;
				break;
			}
		}

		if (!found)
		{
			ParticleEmitter particleEmitter;
			particleEmitter.Reset(type, position, color, particleCount, timeLimit, isActive);
			_particleEmitters.push_back(particleEmitter);
			_emitterCount++;
		}
	}

	int ParticleHandler::GetEmitterCount() const
	{
		return _emitterCount;
	}

	std::vector<Particle>* ParticleHandler::GetParticles(int index)
	{
		if (index >= _emitterCount)
		{
			throw std::runtime_error("ParticleHandler::GetParticles(int index): Invalid index");
		}

		return _particleEmitters.at(index).GetParticles();
	}

}
