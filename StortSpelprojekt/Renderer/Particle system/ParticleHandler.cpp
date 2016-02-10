#include "ParticleHandler.h"

namespace Renderer
{

	ParticleHandler::ParticleHandler(ID3D11Device* device, ID3D11DeviceContext* deviceContext) 
	{
		_device = device;
		_deviceContext = deviceContext;
		_emitterCount = 5;
		_particleEmitters.reserve(_emitterCount);
		_queue.reserve(_emitterCount);

		_requestQueue = new ParticleRequestQueue(&_queue);
	}

	ParticleHandler::~ParticleHandler()
	{
		delete _requestQueue;
		_queue.clear();
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
				ActivateEmitter(p._type, p._position, p._color, p._particleCount, p._timeLimit, p._isActive);
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

			if (_emitterCount > _particleEmitters.capacity())
			{
				_emitterCount++;
			}
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

	//Only get the vertex buffer from the active emitters. Check result against nullptr before usage
	ID3D11Buffer* ParticleHandler::GetParticleBuffer(int index)
	{
		if (index >= _emitterCount || index < 0)
		{
			throw std::runtime_error("ParticleHandler::GetParticles(int index): Invalid index");
		}

		 ID3D11Buffer* vertexBuffer = nullptr;
		if (_particleEmitters.at(index).IsActive())
		{
			vertexBuffer = _particleEmitters.at(index).GetParticleBuffer();
		}

		return vertexBuffer;
	}

	ParticleRequestQueue* ParticleHandler::GetParticleRequestQueue()
	{
		return _requestQueue;
	}

	int ParticleHandler::GetParticleCount(int index)
	{
		if (index >= _emitterCount || index < 0)
		{
			throw std::runtime_error("ParticleHandler::GetParticles(int index): Invalid index");
		}

		int particleCount = -1;
		if (_particleEmitters.at(index).IsActive())
		{
			particleCount = _particleEmitters.at(index).GetParticleCount();
		}

		return particleCount;
	}
}
