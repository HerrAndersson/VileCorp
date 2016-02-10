#include "ParticleHandler.h"

namespace Renderer
{

	ParticleHandler::ParticleHandler(ID3D11Device* device, ID3D11DeviceContext* deviceContext) : _requestQueue(&_queue)
	{
		_device = device;
		_deviceContext = deviceContext;
		_emitterCount = 5;
		_particleEmitters.reserve(_emitterCount);
	}

	ParticleHandler::~ParticleHandler()
	{
		_particleEmitters.clear();
		_device = nullptr;
		_deviceContext = nullptr;
	}

	void ParticleHandler::Update(double deltaTime)
	{
		//Update the active emitters and particles
		for (ParticleEmitter& p : _particleEmitters)
		{
			if (p.IsActive())
			{
				p.Update(deltaTime);
			}
		}

		//Check the queue for messages and activate the new requests
		if (_queue.size() > 0)
		{
			for (ParticleRequestMessage& p : _queue)
			{
				ActivateEmitter(p.type, p.position, p.color, p.particleCount, p.timeLimit, p.isActive);
			}
			_queue.clear();
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
			ParticleEmitter particleEmitter(_device, _deviceContext,type, position, color, particleCount, timeLimit, isActive);
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
		if (index >= _emitterCount || index < 0)
		{
			throw std::runtime_error("ParticleHandler::GetParticles(int index): Invalid index");
		}

		return _particleEmitters.at(index).GetParticles();
	}

	ID3D11Buffer* ParticleHandler::GetParticleBuffer(int index)
	{
		if (index >= _emitterCount || index < 0)
		{
			throw std::runtime_error("ParticleHandler::GetParticles(int index): Invalid index");
		}

		return _particleEmitters.at(index).GetParticleBuffer();
	}

	ParticleRequestQueue* ParticleHandler::GetParticleRequestQueue()
	{
		return &_requestQueue;
	}

}
