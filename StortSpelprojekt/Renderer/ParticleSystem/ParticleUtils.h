#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>
#include <string>
#include <cereal\archives\json.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\vector.hpp>
#include "../RenderUtils.h"

//Determines how it moves
enum ParticleType { SPLASH, SMOKE, ELECTRICITY, FIRE, MUZZLE_FLASH, ICON, STATIC_ICON };

//Determines how it looks
enum ParticleSubType { BLOOD_SUBTYPE, WATER_SUBTYPE, SPARK_SUBTYPE, SMOKE_SUBTYPE, FIRE_SUBTYPE, MUZZLE_FLASH_SUBTYPE, EXCLAMATIONMARK_SUBTYPE, QUESTIONMARK_SUBTYPE, SELECTED_SUBTYPE, PATROL_SUBTYPE, HEALTH_SUBTYPE, WRENCH_SUBTYPE, NOPLACEMENT_SUBTYPE, OCCUPIED_SUBTYPE}; //Icons have to be last
enum ParticleIconType { ICON_EXCLAMATIONMARK, ICON_QUESTIONMARK, ICON_SELECTED, ICON_PATROL, ICON_HEALTH, ICON_WRENCH, ICON_NOPLACEMENT, ICON_OCCUPIED }; //Used for loading and using icon textures

//Has to be set in the BillboardingPS shader as well. The array there has to be of hard-coded length, 
//so "dynamic" number of textures is not possible if more than the hard-coded number
static const int PARTICLE_TEXTURE_COUNT = 4;
static const int ICON_TEXTURE_COUNT = 8;

struct ParticleMessage
{
	enum ParticleMessageType { REQUEST, UPDATE };

	ParticleMessageType _messageType;
	int _ownerID = -1;

	ParticleMessage(ParticleMessageType messageType, int ownerID)
	{
		_messageType = messageType;
		_ownerID = ownerID;
	}
};

struct ParticleRequestMessage : ParticleMessage
{
	ParticleType _type = SPLASH;
	ParticleSubType _subType = BLOOD_SUBTYPE;
	DirectX::XMFLOAT3 _position = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 _direction = DirectX::XMFLOAT3(0, 1, 0);
	DirectX::XMFLOAT3 _target = DirectX::XMFLOAT3(0, 0, 0); //Used for Electricity
	float _timeLimit = 0; //Milliseconds
	int _particleCount = 0;
	bool _isActive = false;
	bool _isTimed = true;
	float _scale = 1.0f;

	ParticleRequestMessage() : ParticleMessage(REQUEST, -1)
	{
	}

	ParticleRequestMessage(ParticleType type, ParticleSubType subType, int ownerID, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, float timeLimit, int particleCount, float scale, bool isActive, bool isTimed = true, const DirectX::XMFLOAT3& target = DirectX::XMFLOAT3(0, 0, 0))
						 : ParticleMessage(REQUEST, ownerID)
	{
		_type = type;
		_subType = subType;
		_position = position;
		_direction = direction;
		_timeLimit = timeLimit;
		_particleCount = particleCount;
		_isActive = isActive;
		_isTimed = isTimed;
		_target = target;
		_scale = scale;
	}
};

struct ParticleUpdateMessage : ParticleMessage
{
	DirectX::XMFLOAT3 _position = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 _direction = DirectX::XMFLOAT3(0, 1, 0);
	bool _isActive = false;

	ParticleUpdateMessage() : ParticleMessage(UPDATE, -1)
	{
	}

	ParticleUpdateMessage(int ownerID, bool isActive, const DirectX::XMFLOAT3& position = DirectX::XMFLOAT3(0, 0, 0), const DirectX::XMFLOAT3& direction = DirectX::XMFLOAT3(0, 0, 0))
		                : ParticleMessage(UPDATE, ownerID)
	{
		_position = position;
		_direction = direction;
		_isActive = isActive;
	}
};

struct ParticleTextures
{
	ID3D11ShaderResourceView* _bloodTextures[PARTICLE_TEXTURE_COUNT];
	ID3D11ShaderResourceView* _waterTextures[PARTICLE_TEXTURE_COUNT];
	ID3D11ShaderResourceView* _smokeTextures[PARTICLE_TEXTURE_COUNT];
	ID3D11ShaderResourceView* _sparkTextures[PARTICLE_TEXTURE_COUNT];
	ID3D11ShaderResourceView* _fireTextures[PARTICLE_TEXTURE_COUNT];
	ID3D11ShaderResourceView* _muzzleFlashTextures[PARTICLE_TEXTURE_COUNT];
	ID3D11ShaderResourceView* _iconTextures[ICON_TEXTURE_COUNT];

	ParticleTextures()
	{
		for (int i = 0; i < PARTICLE_TEXTURE_COUNT; i++)
		{
			_bloodTextures[i] = nullptr;
			_waterTextures[i] = nullptr;
			_smokeTextures[i] = nullptr;
			_sparkTextures[i] = nullptr;
			_fireTextures[i] = nullptr;
			_muzzleFlashTextures[i] = nullptr;
		}

		for (int i = 0; i < ICON_TEXTURE_COUNT; i++)
		{
			_iconTextures[i] = nullptr;
		}
	}
	~ParticleTextures()
	{
		for (int i = 0; i < PARTICLE_TEXTURE_COUNT; i++)
		{
			SAFE_RELEASE(_bloodTextures[i]);
			SAFE_RELEASE(_waterTextures[i]);
			SAFE_RELEASE(_smokeTextures[i]);
			SAFE_RELEASE(_sparkTextures[i]);
			SAFE_RELEASE(_fireTextures[i]);
			SAFE_RELEASE(_muzzleFlashTextures[i]);
		}

		for (int i = 0; i < ICON_TEXTURE_COUNT; i++)
		{
			SAFE_RELEASE(_iconTextures[i]);
		}
	}
};

/* Holds offsets in position and direction for the different types.

_splashPositionOffset holds the +- offset of the x-position of the particles of type Splash. 
Example: If the emitter-position is at (2, 1, 3), the particle position.x can be in the range of 1.85 - 2.15, etc 

The base direction can be rotated by the direction offset values, randomized in the range [0,offsetValue]. 
For example, if the base direction is (0, 1, 0), and the offset is pi/4 (45 degrees), the final direction might be (0, 0.5f, 0.5f)

The repeat times are applicable to smoke and fire, and used as a limit of movement. 
For example, a smoke particle should be reset after _smokeRepeatTime to limit the height. 
*/
struct ParticleModifierOffsets
{
	float _splashPositionOffset = 0.15f;
	float _smokePositionOffset = 0.25f;
	float _firePositionOffset = 0.35f;

	float _splashDirectionOffset = DirectX::XM_PI / 6.0f; //30 degrees
	float _smokeDirectionOffset = DirectX::XM_PI / 12.0f; //15 degrees
	float _fireDirectionOffset = DirectX::XM_PI / 18.0f;  //10 degrees

	DirectX::XMFLOAT2 _splashSpeedRange = DirectX::XMFLOAT2(0.75f, 1.0f);
	DirectX::XMFLOAT2 _smokeSpeedRange = DirectX::XMFLOAT2(0.2f, 0.5f);
	DirectX::XMFLOAT2 _fireSpeedRange = DirectX::XMFLOAT2(0.5f, 0.7f);

	//Milliseconds
	float _smokeRepeatTime = 1000.0f; 
	float _fireRepeatTime = 1500.0f;
	float _lightningRepeatTime = 100.0f;
};

struct ParticleSystemData
{
	float _splashPositionOffset = 0.15f;
	float _smokePositionOffset = 0.25f;
	float _firePositionOffset = 0.35f;

	float _splashDirectionOffset = DirectX::XM_PI / 6.0f;
	float _smokeDirectionOffset = DirectX::XM_PI / 12.0f;
	float _fireDirectionOffset = DirectX::XM_PI / 18.0f;

	DirectX::XMFLOAT2 _splashSpeedRange = DirectX::XMFLOAT2(0.75f, 1.0f);
	DirectX::XMFLOAT2 _smokeSpeedRange = DirectX::XMFLOAT2(0.2f, 0.5f);
	DirectX::XMFLOAT2 _fireSpeedRange = DirectX::XMFLOAT2(0.5f, 0.7f);

	float _smokeRepeatTime = 1000.0f;
	float _fireRepeatTime = 1500.0f;
	float _lightningRepeatTime = 100.0f;

	std::vector<std::vector<std::string>> _subtypeTexturePaths;
	std::vector<std::string> _iconTexturePaths;

	template<class A>
	void serialize(A& a)
	{
		a(
			(CEREAL_NVP(_splashPositionOffset)),
			(CEREAL_NVP(_smokePositionOffset)),
			(CEREAL_NVP(_firePositionOffset)),
			(CEREAL_NVP(_splashDirectionOffset)),
			(CEREAL_NVP(_smokeDirectionOffset)),
			(CEREAL_NVP(_fireDirectionOffset)),
			(CEREAL_NVP(_splashSpeedRange.x)),
			(CEREAL_NVP(_splashSpeedRange.y)),
			(CEREAL_NVP(_smokeSpeedRange.x)),
			(CEREAL_NVP(_smokeSpeedRange.y)),
			(CEREAL_NVP(_fireSpeedRange.x)),
			(CEREAL_NVP(_fireSpeedRange.y)),
			(CEREAL_NVP(_smokeRepeatTime)),
			(CEREAL_NVP(_fireRepeatTime)),
			(CEREAL_NVP(_lightningRepeatTime)),
			(CEREAL_NVP(_subtypeTexturePaths)),
			(CEREAL_NVP(_iconTexturePaths))
		);
	}
};


/*

This code shows how to use the particle system.

int idToFollow = 5; //Should be obj->GetID();
if (_controls->IsFunctionKeyDown("DEBUG:REQUEST_PARTICLE"))
{
	//If the emitters shouldn't move
	XMFLOAT3 pos = XMFLOAT3(11, 1.0f, 2);
	XMFLOAT3 dir = XMFLOAT3(0, 1, 0);
	ParticleRequestMessage* msg = new ParticleRequestMessage(ParticleType::SPLASH, ParticleSubType::BLOOD_SUBTYPE, -1, pos, dir, 300.0f, 20, 0.1f, true);
	_particleHandler->GetParticleEventQueue()->Insert(msg);

	pos = XMFLOAT3(16, 1.0f, 2);
	dir = XMFLOAT3(0, 0, 1);
	msg = new ParticleRequestMessage(ParticleType::MUZZLE_FLASH, ParticleSubType::MUZZLE_FLASH_SUBTYPE, -1, pos, dir, 50.0f, 1, 0.1f, true);
	_particleHandler->GetParticleEventQueue()->Insert(msg);

	pos = XMFLOAT3(14, 1.0f, 2);
	dir = XMFLOAT3(0, 1, 0);
	msg = new ParticleRequestMessage(ParticleType::SMOKE, ParticleSubType::SMOKE_SUBTYPE, -1, pos, dir, 100000.0f, 50, 0.04f, true);
	_particleHandler->GetParticleEventQueue()->Insert(msg);

	pos = XMFLOAT3(7, 1.0f, 2);
	dir = XMFLOAT3(0, 1, 0);
	msg = new ParticleRequestMessage(ParticleType::SPLASH, ParticleSubType::WATER_SUBTYPE, -1, pos, dir, 400.0f, 20, 0.1f, true);
	_particleHandler->GetParticleEventQueue()->Insert(msg);

	pos = XMFLOAT3(14, 1.0f, 2);
	dir = XMFLOAT3(0, 1, 0);
	msg = new ParticleRequestMessage(ParticleType::FIRE, ParticleSubType::FIRE_SUBTYPE, -1, pos, dir, 100000.0f, 50, 0.1f, true);
	_particleHandler->GetParticleEventQueue()->Insert(msg);

	pos = XMFLOAT3(11, 1.0f, 4);
	msg = new ParticleRequestMessage(ParticleType::ICON, ParticleSubType::EXCLAMATIONMARK_SUBTYPE, -1, pos, XMFLOAT3(0, 0, 0), 1000.0f, 1, 0.25f, true);
	_particleHandler->GetParticleEventQueue()->Insert(msg);

	//If the emitters should change, for example move, connect them to an ID of a game object
	pos = XMFLOAT3(13, 1.0f, 4);
	msg = new ParticleRequestMessage(ParticleType::ICON, ParticleSubType::QUESTIONMARK_SUBTYPE, idToFollow, pos, XMFLOAT3(0, 0, 0), 100000.0f, 1, 0.25f, true, false); //Follows owner and is not timed
	_particleHandler->GetParticleEventQueue()->Insert(msg);

	pos = XMFLOAT3(9, 1.0f, 2);
	dir = XMFLOAT3(0, 0, 1);
	msg = new ParticleRequestMessage(ParticleType::FIRE, ParticleSubType::FIRE_SUBTYPE, idToFollow, pos, dir, 10000.0f, 100, 0.04f, true, true); //Follows owner and is timed
	_particleHandler->GetParticleEventQueue()->Insert(msg);

	//Electricity should never move. One bolt of lightning is created each request, and updated the given time
	pos = XMFLOAT3(5, 1.0f, 3);
	msg = new ParticleRequestMessage(ParticleType::ELECTRICITY, ParticleSubType::SPARK_SUBTYPE, -1, pos, XMFLOAT3(0, 0, 0), 1000.0f, 20, 0.1f, true, true, XMFLOAT3(14.0f, 1.0f, 3));
	_particleHandler->GetParticleEventQueue()->Insert(msg);
}

//If the emitter itself should be updated, for example if the icons move or if we have a spinning flamethrower.
//Set "isAlive" parameter to disable all emitters that are connected to the ID of the GameObject. This should be done when, for example a trap is fixed and should stop smoking
//It is possible to update only position
std::vector<std::vector<GameObject*>>* gameObjects = _objectHandler->GetGameObjects();
if (gameObjects->size() > 0)
{
	for (unsigned int i = 0; i < gameObjects->at(GUARD).size(); i++)
	{
		GameObject* g = gameObjects->at(GUARD).at(i);
		if (g)
		{
			XMFLOAT3 pos = g->GetPosition();
			AI::Vec2D dirv2d = ((Unit*)g)->GetDirection();
			XMFLOAT3 dir = XMFLOAT3(dirv2d._x, 0, dirv2d._y);

			pos.y = 2.5f;
			ParticleUpdateMessage* msg = new ParticleUpdateMessage(idToFollow, true, pos, dir);
			_particleHandler->GetParticleEventQueue()->Insert(msg);
		}
	}
}
*/
