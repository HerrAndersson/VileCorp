#pragma once
#define RENDERER_EXPORT __declspec(dllexport)
#include <DirectXMath.h>
#include <d3d11.h>
#include "Particle.h"
#include <vector>
#include <limits>

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
		float _particleScale; //Used in the geometry shader to generate particles of given scale

		ID3D11Device* _device;
		ID3D11DeviceContext* _deviceContext;

		ID3D11Buffer* _particlePointsBuffer;
		int _vertexSize;

		void CreateVertexBuffer();
		void UpdateVertexBuffer();
		void CreateAllParticles(int count, const DirectX::XMFLOAT3& baseDirection, const DirectX::XMFLOAT3& targetPosition);

		Particle CreateSingleParticle(const DirectX::XMFLOAT3& baseDirection);
		void CreateElectricityPattern(int count, const DirectX::XMFLOAT3& targetPosition);

		//If includeNegative is true, the return will be in the range [-max, max]
		float GetRandomOffset(float maxOffset, bool includeNegative);

		//Generates a random number in the range [offsets.x, offsets.y]
		float GetRandomOffsetInRange(DirectX::XMFLOAT2 offsets);

		DirectX::XMFLOAT3 NormalizeDirection(DirectX::XMFLOAT3 dir);

	public:

		ParticleEmitter(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ParticleModifierOffsets* modifers);
		ParticleEmitter(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const ParticleType& type, const ParticleSubType& subType, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, int particleCount, float timeLimit, float scale, bool isActive, ParticleModifierOffsets* modifers, const DirectX::XMFLOAT3& target = DirectX::XMFLOAT3(0, 0, 0));
		virtual ~ParticleEmitter();

		void Reset(const ParticleType& type, const ParticleSubType& subType, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, int particleCount, float timeLimit, float scale, bool isActive, const DirectX::XMFLOAT3& target = DirectX::XMFLOAT3(0, 0, 0));

		void Update(double deltaTime);

		ID3D11Buffer* GetParticleBuffer();

		DirectX::XMFLOAT3 GetPosition() const;
		ParticleType GetType() const;
		ParticleSubType GetSubType() const;
		int GetParticleCount() const;
		int GetVertexSize() const;
		float GetParticleScale() const;

		void SetPosition(const DirectX::XMFLOAT3 position);

		bool HasTimeLeft() const;

		bool IsActive() const;
		void Activate();
		void Deactivate();

	};
}

