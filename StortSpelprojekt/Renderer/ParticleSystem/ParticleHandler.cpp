#include "ParticleHandler.h"
#include <../stdafx.h>

namespace Renderer
{

	ParticleHandler::ParticleHandler(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ParticleTextures* textures, const ParticleModifierOffsets& modifiers)
	{
		_device = device;
		_deviceContext = deviceContext;
		_emitterCount = 0;
		_textures = textures;
		_modifiers = modifiers;
		_requestQueue = new ParticleEventQueue(&_queue);
	}

	ParticleHandler::~ParticleHandler()
	{
		delete _requestQueue;
		_requestQueue = nullptr;

		for (ParticleMessage* p : _queue)
		{
			delete p;
		}

		_device = nullptr;
		_deviceContext = nullptr;

		for (ParticleEmitter* p : _particleEmitters)
		{
			delete p;
		}
		delete _textures;
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
				textures = _textures->_bloodTextures;
				break;
			}
			case ParticleSubType::WATER_SUBTYPE:
			{
				count = PARTICLE_TEXTURE_COUNT;
				textures = _textures->_waterTextures;
				break;
			}
			case ParticleSubType::SPARK_SUBTYPE:
			{
				count = PARTICLE_TEXTURE_COUNT;
				textures = _textures->_sparkTextures;
				break;
			}
			case ParticleSubType::SMOKE_SUBTYPE:
			{
				count = PARTICLE_TEXTURE_COUNT;
				textures = _textures->_smokeTextures;
				break;
			}
			case ParticleSubType::FIRE_SUBTYPE:
			{
				count = PARTICLE_TEXTURE_COUNT;
				textures = _textures->_fireTextures;
				break;
			}
			case ParticleSubType::MUZZLE_FLASH_SUBTYPE:
			{
				count = PARTICLE_TEXTURE_COUNT;
				textures = _textures->_muzzleFlashTextures;
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
				icon = _textures->_iconTextures[ICON_EXCLAMATIONMARK];
				break;
			}

			case ParticleSubType::QUESTIONMARK_SUBTYPE:
			{
				icon = _textures->_iconTextures[ICON_QUESTIONMARK];
				break;
			}
			case ParticleSubType::SELECTED_SUBTYPE:
			{
				icon = _textures->_iconTextures[ICON_SELECTED];
				break;
			}

			case ParticleSubType::PATROL_SUBTYPE:
			{
				icon = _textures->_iconTextures[ICON_PATROL];
				break;
			}

			case ParticleSubType::HEALTH_SUBTYPE:
			{
				icon = _textures->_iconTextures[ICON_HEALTH];
				break;
			}

			case ParticleSubType::WRENCH_SUBTYPE:
			{
				icon = _textures->_iconTextures[ICON_WRENCH];
				break;
			}

			case ParticleSubType::NOPLACEMENT_SUBTYPE:
			{
				icon = _textures->_iconTextures[ICON_NOPLACEMENT];
				break;
			}

			case ParticleSubType::OCCUPIED_SUBTYPE:
			{
				icon = _textures->_iconTextures[ICON_OCCUPIED];
				break;
			}

			case ParticleSubType::LOOT_SUBTYPE:
			{
				icon = _textures->_iconTextures[ICON_LOOT];
				break;
			}
			case ParticleSubType::SPAWN_SUBTYPE:
			{
				icon = _textures->_iconTextures[ICON_SPAWN];
				break;
			}
			case ParticleSubType::AOE_RED_SUBTYPE:
			{
				icon = _textures->_iconTextures[ICON_AOE_RED];
				break;
			}
			case ParticleSubType::AOE_YELLOW_SUBTYPE:
			{
				icon = _textures->_iconTextures[ICON_AOE_YELLOW];
				break;
			}
			case ParticleSubType::AOE_GREEN_SUBTYPE:
			{
				icon = _textures->_iconTextures[ICON_AOE_GREEN];
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
