#pragma once
#define RENDERER_EXPORT __declspec(dllexport)
#include <DirectXMath.h>
#include <d3d11.h>
#include "ParticleEmitter.h"
#include <vector>

//Disable warning about DirectX XMFLOAT3/XMMATRIX etc
#pragma warning( disable: 4251 )

namespace Renderer
{
	class RENDERER_EXPORT ParticleHandler
	{

	private:

		std::vector<ParticleEmitter> _particleEmitters;

	public:

		ParticleHandler();
		virtual ~ParticleHandler();

		void Update(double deltaTime);

		void ActivateEmitter(ParticleType type, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, float particleCount, int timeLimit, bool isActive);

	};
}

