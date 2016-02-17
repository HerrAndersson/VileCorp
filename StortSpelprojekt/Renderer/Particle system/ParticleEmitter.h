#pragma once
#define RENDERER_EXPORT __declspec(dllexport)
#include <DirectXMath.h>
#include <d3d11.h>
#include "Particle.h"
#include <vector>

//Disable warning about DirectX XMFLOAT3/XMMATRIX etc
#pragma warning( disable: 4251 )

namespace Renderer
{
	class RENDERER_EXPORT ParticleEmitter
	{

	private:

		struct ParticleVertex
		{
			DirectX::XMFLOAT3 _position;
		};

		ParticleType _type;
		ParticleSubType _subType;
		DirectX::XMFLOAT3 _position;
		std::vector<Particle> _particles;
		ParticleModifierOffsets* _modifiers;

		bool _isActive;
		float _timeLeft;
		int _particleCount;

		ID3D11Device* _device;
		ID3D11DeviceContext* _deviceContext;

		ID3D11Buffer* _particlePointsBuffer;
		int _vertexSize;

		void CreateVertexBuffer();
		void UpdateVertexBuffer();
		void CreateParticles(int count, const DirectX::XMFLOAT3& baseDirection, const DirectX::XMFLOAT3& targetPosition);

	public:

		ParticleEmitter(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ParticleModifierOffsets* modifers);
		ParticleEmitter(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const ParticleType& type, const ParticleSubType& subType, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, int particleCount, float timeLimit, bool isActive, ParticleModifierOffsets* modifers, const DirectX::XMFLOAT3& target = DirectX::XMFLOAT3(0, 0, 0));
		virtual ~ParticleEmitter();

		void Reset(const ParticleType& type, const ParticleSubType& subType, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, int particleCount, float timeLimit, bool isActive, const DirectX::XMFLOAT3& target = DirectX::XMFLOAT3(0, 0, 0));

		void Update(double deltaTime);

		ID3D11Buffer* GetParticleBuffer();

		DirectX::XMFLOAT3 GetPosition() const;
		ParticleType GetType() const;
		ParticleSubType GetSubType() const;
		int GetParticleCount() const;
		int GetVertexSize() const;

		void SetPosition(const DirectX::XMFLOAT3 position);

		bool HasTimeLeft() const;

		bool IsActive() const;
		void Activate();
		void Deactivate();

	};
}

