#pragma once
#include <DirectXMath.h>

enum ParticleType { SPLASH, SMOKE, ELECTRICITY };

struct ParticleRequestMessage
{
	ParticleType _type = SPLASH;
	DirectX::XMFLOAT3 _position = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT4 _color = DirectX::XMFLOAT4(0, 0, 0, 0);
	float _timeLimit = 0; //Milliseconds
	int _particleCount = 0;
	bool _isActive = false;

	ParticleRequestMessage()
	{
	}

	ParticleRequestMessage(ParticleType type, DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 color, float timeLimit, int particleCount, bool isActive)
	{
		_type = type;
		_position = position;
		_color = color;
		_timeLimit = timeLimit;
		_particleCount = particleCount;
		_isActive = isActive;
	}
};