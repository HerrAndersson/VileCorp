#pragma once
#define RENDERER_EXPORT __declspec(dllexport)
#include <DirectXMath.h>
#include <d3d11.h>
#include "ParticleEmitter.h"
#include <vector>
#include "ParticleEventQueue.h"

//Disable warning about DirectX XMFLOAT3/XMMATRIX etc
#pragma warning( disable: 4251 )

namespace Renderer
{
	class RENDERER_EXPORT ParticleHandler
	{

	private:

		ID3D11Device* _device;
		ID3D11DeviceContext* _deviceContext;

		ParticleTextures* _textures;
		ParticleModifierOffsets _modifiers;

		std::vector<ParticleMessage*> _queue;
		ParticleEventQueue* _requestQueue;
		std::vector<ParticleEmitter*> _particleEmitters;

		int _emitterCount;

		void ActivateEmitter(const ParticleType& type, const ParticleSubType& subType, int ownerID, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, int particleCount, float timeLimit, float scale, bool isActive, bool isTimed, const DirectX::XMFLOAT3& target = DirectX::XMFLOAT3(0, 0, 0));

	public:

		ParticleHandler(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ParticleTextures* textures, const ParticleModifierOffsets& modifiers);
		virtual ~ParticleHandler();

		void Update(double deltaTime);

		int GetEmitterCount() const;
		ParticleEmitter* GetEmitter(int index);
		ID3D11ShaderResourceView** GetTextures(int& count, const ParticleSubType& subType);
		ID3D11ShaderResourceView* GetIconTexture(const ParticleSubType& subType);
		
		void DeactivateAllEmitters();

		ParticleEventQueue* GetParticleEventQueue();

	};
}

