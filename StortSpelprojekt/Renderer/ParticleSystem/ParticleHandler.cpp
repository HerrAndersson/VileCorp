#include "ParticleHandler.h"
#include <../stdafx.h>

namespace Renderer
{

	ParticleHandler::ParticleHandler(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const ParticleTextures& textures, const ParticleModifierOffsets& modifiers)
	{
		_device = device;
		_deviceContext = deviceContext;
		_emitterCount = 0;
		_textures = textures;
		_modifiers = modifiers;

		for (int i = 0; i < _emitterCount; i++)
		{
			_particleEmitters.push_back(new ParticleEmitter(device, deviceContext, &_modifiers));
		}

		_requestQueue = new ParticleEventQueue(&_queue);
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
			if (p->IsActive() && p->GetOwnerID() == -1)
			{
				p->Update(deltaTime);
			}
		}

		//Check the queue for messages. Activate the new requests and update the emitters with an owner
		if (_queue.size() > 0)
		{
			for (auto& msg : _queue)
			{
				if (msg->_messageType == msg->REQUEST)
				{
					int ownerID = msg->_ownerID;
					ParticleRequestMessage* message = (ParticleRequestMessage*)msg;
					ActivateEmitter(message->_type, message->_subType, ownerID, message->_position, message->_direction, message->_particleCount, message->_timeLimit, message->_scale, message->_isActive, message->_isTimed, message->_target);
				}
				else if (msg->_messageType == msg->UPDATE)
				{
					for (ParticleEmitter* emitter : _particleEmitters)
					{
						if (emitter->IsActive() && emitter->GetOwnerID() == msg->_ownerID)
						{
							ParticleUpdateMessage* message = (ParticleUpdateMessage*)msg;
							if (message->_isActive)
							{
								emitter->SetPosition(message->_position);
								emitter->SetDirection(message->_direction);
								emitter->Update(deltaTime);
							}
							else
							{
								emitter->Deactivate();
							}
						}
					}
				}

				delete msg;
			}
			_queue.clear();
		}
	}

	void ParticleHandler::ActivateEmitter(const ParticleType& type, const ParticleSubType& subType, int ownerID, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, int particleCount, float timeLimit, float scale, bool isActive, bool isTimed, const DirectX::XMFLOAT3& target)
	{
		bool found = false;

		for (ParticleEmitter* p : _particleEmitters)
		{
			if (!p->IsActive())
			{
				p->Reset(type, subType, ownerID, position, direction, particleCount, timeLimit, scale, isActive, isTimed, target);
				found = true;
				break;
			}
		}

		if (!found)
		{
			ParticleEmitter* particleEmitter = new ParticleEmitter(_device, _deviceContext, type, subType, ownerID, position, direction, particleCount, timeLimit, scale, isActive, isTimed, &_modifiers, target);
			_particleEmitters.push_back(particleEmitter);
			_emitterCount++;
		}
	}

	int ParticleHandler::GetEmitterCount() const
	{
		return _emitterCount;
	}

	ParticleEventQueue* ParticleHandler::GetParticleEventQueue()
	{
		return _requestQueue;
	}

	ID3D11ShaderResourceView** ParticleHandler::GetTextures(int& count, const ParticleSubType& subType)
	{
		ID3D11ShaderResourceView** textures = nullptr;
		count = -1;

		switch (subType)
		{
			case ParticleSubType::BLOOD_SUBTYPE:
			{
				count = PARTICLE_TEXTURE_COUNT;
				textures = _textures._bloodTextures;
				break;
			}
			case ParticleSubType::WATER_SUBTYPE:
			{
				count = PARTICLE_TEXTURE_COUNT;
				textures = _textures._waterTextures;
				break;
			}
			case ParticleSubType::SPARK_SUBTYPE:
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
			case ParticleSubType::MUZZLE_FLASH_SUBTYPE:
			{
				count = PARTICLE_TEXTURE_COUNT;
				textures = _textures._muzzleFlashTextures;
				break;
			}
			default:
			{
				break;
			}
		}

		return textures;
	}

	ID3D11ShaderResourceView* ParticleHandler::GetIconTexture(const ParticleSubType& subType)
	{
		ID3D11ShaderResourceView* icon = nullptr;
		switch (subType)
		{
			case ParticleSubType::EXCLAMATIONMARK_SUBTYPE:
			{
				icon = _textures._iconTextures[ICON_EXCLAMATIONMARK];
				break;
			}

			case ParticleSubType::QUESTIONMARK_SUBTYPE:
			{
				icon = _textures._iconTextures[ICON_QUESTIONMARK];
				break;
			}
			case ParticleSubType::WRENCH_SUBTYPE:
			{
				icon = _textures._iconTextures[ICON_WRENCH];
				break;
			}
			default:
			{
				break;
			}
		}
		return icon;
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

	void ParticleHandler::DeactivateAllEmitters()
	{
		for (ParticleEmitter* p : _particleEmitters)
		{
			p->Deactivate();
		}
	}
}
