#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>
#include <string>
#include <cereal\archives\json.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\vector.hpp>

//Determines how it moves
enum ParticleType { SPLASH, SMOKE, ELECTRICITY, FIRE, MUZZLE_FLASH, ICON };

//Determines how it looks
enum ParticleSubType { BLOOD_SUBTYPE, WATER_SUBTYPE, SPARK_SUBTYPE, SMOKE_SUBTYPE, FIRE_SUBTYPE, MUZZLE_FLASH_SUBTYPE, EXCLAMATIONMARK_SUBTYPE, QUESTIONMARK_SUBTYPE }; //Icons have to be last
enum ParticleIconType { ICON_EXCLAMATIONMARK, ICON_QUESTIONMARK }; //Used for loading and using icon textures

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
	float _scale = 1.0f;

	ParticleRequestMessage() : ParticleMessage(REQUEST, -1)
	{
	}

	ParticleRequestMessage(ParticleType type, ParticleSubType subType, int ownerID, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, float timeLimit, int particleCount, float scale, bool isActive, const DirectX::XMFLOAT3& target = DirectX::XMFLOAT3(0, 0, 0))
						 : ParticleMessage(REQUEST, ownerID)
	{
		_type = type;
		_subType = subType;
		_position = position;
		_direction = direction;
		_timeLimit = timeLimit;
		_particleCount = particleCount;
		_isActive = isActive;
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

//Has to be set in the BillboardingPS shader aswell. The array there has to be of hard-coded length, 
//so "dynamic" number of textures is not possible if more than the hard-coded number
static const int PARTICLE_TEXTURE_COUNT = 4;
static const int ICON_TEXTURE_COUNT = 2;

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
		}

		for (int i = 0; i < ICON_TEXTURE_COUNT; i++)
		{
			_iconTextures[i] = nullptr;
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