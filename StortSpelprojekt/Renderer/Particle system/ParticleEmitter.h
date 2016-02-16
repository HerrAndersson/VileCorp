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

		//Holds offsets in position and direction for the different types.
		//For example, bloodPositionOffsets.x holds the +- offset of the x-position. If the emitter-position is at (2,1,3), the particle position.x can be in the range of 1.85 - 2.15
		struct Offsets
		{
			DirectX::XMFLOAT3 _bloodPositionOffsets = DirectX::XMFLOAT3(0.15f, 0.0f, 0.15f);
			DirectX::XMFLOAT3 _waterPositionOffsets = DirectX::XMFLOAT3(0.25f, 0.0f, 0.25f);
			DirectX::XMFLOAT3 _smokePositionOffsets = DirectX::XMFLOAT3(0.35f, 0.2f, 0.35f);


		};

		struct ParticleVertex
		{
			DirectX::XMFLOAT3 _position;
		};

		ParticleType _type;
		ParticleSubType _subType;
		DirectX::XMFLOAT3 _position;
		std::vector<Particle> _particles;

		bool _isActive;
		float _timeLeft;

		ID3D11Device* _device;
		ID3D11DeviceContext* _deviceContext;
		ID3D11Buffer* _particlePointsBuffer;
		int _vertexSize;

		void CreateVertexBuffer();
		void UpdateVertexBuffer();

	public:

		ParticleEmitter(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		ParticleEmitter(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const ParticleType& type, const ParticleSubType& subType, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4 color, int particleCount, float timeLimit, bool isActive);
		virtual ~ParticleEmitter();

		void Reset(const ParticleType& type, const ParticleSubType& subType, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4 color, int particleCount, float timeLimit, bool isActive);

		void Update(double deltaTime);

		ID3D11Buffer* GetParticleBuffer();

		DirectX::XMFLOAT3 GetPosition() const;
		ParticleType GetType() const;
		ParticleSubType GetSubType() const;
		int GetParticleCount() const;
		int GetVertexSize() const;

		void SetPosition(const DirectX::XMFLOAT3 position);
		void SetType(const ParticleType& type);
		void SetSubType(const ParticleSubType& subType);
		void SetParticleCount(const int& particleCount);

		bool HasTimeLeft() const;

		bool IsActive() const;
		void Activate();
		void Deactivate();

	};
}

