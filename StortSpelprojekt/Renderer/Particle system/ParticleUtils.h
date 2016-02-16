#pragma once
#include <DirectXMath.h>
#include <d3d11.h>

//Determines how it moves
enum ParticleType { SPLASH, SMOKE, ELECTRICITY, FIRE };

//Determines how it looks
enum ParticleSubType {BLOOD, WATER, SPARK, SMOKE_SUBTYPE, FIRE_SUBTYPE}; 

struct ParticleRequestMessage
{
	ParticleType _type = SPLASH;
	ParticleSubType _subType = BLOOD;
	DirectX::XMFLOAT3 _position = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 _direction = DirectX::XMFLOAT3(0, 1, 0);
	DirectX::XMFLOAT3 _target = DirectX::XMFLOAT3(0, 0, 0); //Used for Electricity
	float _timeLimit = 0; //Milliseconds
	int _particleCount = 0;
	bool _isActive = false;

	ParticleRequestMessage()
	{
	}

	ParticleRequestMessage(ParticleType type, ParticleSubType subType, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, float timeLimit, int particleCount, bool isActive, const DirectX::XMFLOAT3& target = DirectX::XMFLOAT3(0, 0, 0))
	{
		_type = type;
		_subType = _subType;
		_position = position;
		_direction = direction;
		_timeLimit = timeLimit;
		_particleCount = particleCount;
		_isActive = isActive;
		_target = target;
	}
};

//Has to be set in the BillboardingPS shader aswell. The array there has to be of hard-coded length, 
//so "dynamic" number of textures is not possible if more than the hard-coded number
static const int PARTICLE_TEXTURE_COUNT = 4;

struct ParticleTextures
{
	ID3D11ShaderResourceView* _bloodTextures[PARTICLE_TEXTURE_COUNT];
	ID3D11ShaderResourceView* _waterTextures[PARTICLE_TEXTURE_COUNT];
	ID3D11ShaderResourceView* _smokeTextures[PARTICLE_TEXTURE_COUNT];
	ID3D11ShaderResourceView* _sparkTextures[PARTICLE_TEXTURE_COUNT];
	ID3D11ShaderResourceView* _fireTextures[PARTICLE_TEXTURE_COUNT];

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
	}
};