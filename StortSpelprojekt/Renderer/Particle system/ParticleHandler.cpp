#include "ParticleHandler.h"
#include <../stdafx.h>

namespace Renderer
{

	ParticleHandler::ParticleHandler(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ParticleTextures textures)
	{
		_device = device;
		_deviceContext = deviceContext;
		_emitterCount = 0;
		_textures = textures;

		_queue.reserve(5);
		_particleEmitters.reserve(5);
		for (int i = 0; i < _emitterCount; i++)
		{
			_particleEmitters.push_back(new ParticleEmitter(device, deviceContext));
		}

		_requestQueue = new ParticleRequestQueue(&_queue);
	}

	ParticleHandler::~ParticleHandler()
	{
		SAFE_DELETE(_requestQueue);
		_queue.clear();
		_device = nullptr;
		_deviceContext = nullptr;

		for (ParticleEmitter* p : _particleEmitters)
		{
			SAFE_DELETE(p);
		}
		_particleEmitters.clear();
	}

	void ParticleHandler::Update(double deltaTime)
	{
		//Update the active emitters and particles
		for (ParticleEmitter* p : _particleEmitters)
		{
			if (p->IsActive())
			{
				p->Update(deltaTime);
			}
		}

		//Check the queue for messages and activate the new requests
		if (_queue.size() > 0)
		{
			for (ParticleRequestMessage& p : _queue)
			{
				ActivateEmitter(p._type, p._subType, p._position, p._color, p._particleCount, p._timeLimit, p._isActive);
			}
			_queue.clear();
		}
	}

	void ParticleHandler::ActivateEmitter(ParticleType type, ParticleSubType subType, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, int particleCount, float timeLimit, bool isActive)
	{
		bool found = false;

		for (ParticleEmitter* p : _particleEmitters)
		{
			if (!p->IsActive())
			{
				p->Reset(type, subType, position, color, particleCount, timeLimit, isActive);
				found = true;
				break;
			}
		}

		if (!found)
		{
			ParticleEmitter* particleEmitter = new ParticleEmitter(_device, _deviceContext, type, subType, position, color, particleCount, timeLimit, isActive);
			_particleEmitters.push_back(particleEmitter);
			_emitterCount++;
		}
	}

	int ParticleHandler::GetEmitterCount() const
	{
		return _emitterCount;
	}

	ParticleRequestQueue* ParticleHandler::GetParticleRequestQueue()
	{
		return _requestQueue;
	}

	ID3D11ShaderResourceView** ParticleHandler::GetTextures(int& count, const ParticleSubType& subType)
	{
		ID3D11ShaderResourceView** textures = nullptr;
		count = -1;

		switch (subType)
		{
			case ParticleSubType::BLOOD:
			{
				count = PARTICLE_TEXTURE_COUNT;
				textures = _textures._bloodTextures;
				break;
			}
			case ParticleSubType::WATER:
			{
				count = PARTICLE_TEXTURE_COUNT;
				textures = _textures._waterTextures;
				break;
			}
			case ParticleSubType::SPARK:
			{
				count = PARTICLE_TEXTURE_COUNT;
				textures = _textures._sparkTextures;
				break;
			}
			case ParticleSubType::SMOKE_SUBTYPE:
			{
				count = PARTICLE_TEXTURE_COUNT;
				textures = _textures._smokeTextures;
				break;
			}
			case ParticleSubType::FIRE_SUBTYPE:
			{
				count = PARTICLE_TEXTURE_COUNT;
				textures = _textures._fireTextures;
				break;
			}
			default:
			{
				break;
			}
		}

		return textures;
	}

	//Only return if the emitter is both valid and active
	ParticleEmitter* ParticleHandler::GetEmitter(int index)
	{
		if (index >= _emitterCount || index < 0)
		{
			return nullptr;
		}

		ParticleEmitter* emitter = _particleEmitters.at(index);
		if (!emitter->IsActive())
		{
			emitter = nullptr;
		}

		return emitter;
	}
}
