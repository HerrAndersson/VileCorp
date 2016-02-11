#pragma once
#define RENDERER_EXPORT __declspec(dllexport)
#include <DirectXMath.h>
#include <d3d11.h>
#include "ParticleEmitter.h"
#include <vector>
#include "ParticleRequestQueue.h"

//Disable warning about DirectX XMFLOAT3/XMMATRIX etc
#pragma warning( disable: 4251 )

namespace Renderer
{
	class RENDERER_EXPORT ParticleHandler
	{

	private:

		ID3D11Device* _device;
		ID3D11DeviceContext* _deviceContext;

		std::vector<ParticleRequestMessage> _queue;
		ParticleRequestQueue* _requestQueue;
		std::vector<ParticleEmitter> _particleEmitters;

		int _emitterCount;

	public:

		ParticleHandler(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		virtual ~ParticleHandler();

		void Update(double deltaTime);

		void ActivateEmitter(ParticleType type, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, int particleCount, float timeLimit, bool isActive);

		int GetEmitterCount() const;
		ParticleEmitter* GetEmitter(int index);

		ParticleRequestQueue* GetParticleRequestQueue();

	};
}

