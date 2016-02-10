#pragma once
#include <DirectXMath.h>

enum ParticleType { SPLASH, SMOKE, ELECTRICITY };

struct ParticleRequestMessage
{
	ParticleType type = SPLASH;
	DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(0, 0, 0, 0);
	float timeLimit = 0;
	int particleCount = 0;
	bool isActive = false;
};