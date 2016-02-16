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
	DirectX::XMFLOAT4 _color = DirectX::XMFLOAT4(0, 0, 0, 0);
	float _timeLimit = 0; //Milliseconds
	int _particleCount = 0;
	bool _isActive = false;

	ParticleRequestMessage()
	{
	}

	ParticleRequestMessage(ParticleType type, ParticleSubType subType, DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 color, float timeLimit, int particleCount, bool isActive)
	{
		_type = type;
		_subType = _subType;
		_position = position;
		_color = color;
		_timeLimit = timeLimit;
		_particleCount = particleCount;
		_isActive = isActive;
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