#include "ParticleHandler.h"

namespace Renderer
{

	ParticleHandler::ParticleHandler()
	{
		_particleEmitters.reserve(10);
	}

	ParticleHandler::~ParticleHandler()
	{
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

	void ParticleHandler::ActivateEmitter(ParticleType type, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, float particleCount, int timeLimit, bool isActive)
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
		}
	}

}
