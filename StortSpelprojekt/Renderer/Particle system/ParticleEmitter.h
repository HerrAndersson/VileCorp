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
		DirectX::XMFLOAT3 _position;
		std::vector<Particle> _particles;

		bool _isActive;
		float _timeLeft;

		ID3D11Device* _device;
		ID3D11DeviceContext* _deviceContext;
		ID3D11Buffer* _particlePointsBuffer;
		int _bufferSize;

		void CreateVertexBuffer();
		void UpdateVertexBuffer();

	public:

		ParticleEmitter(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		ParticleEmitter(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const ParticleType& type, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4 color, int particleCount, float timeLimit, bool isActive);
		virtual ~ParticleEmitter();

		void Reset(const ParticleType& type, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4 color, int particleCount, float timeLimit, bool isActive);

		void Update(double deltaTime);

		std::vector<Particle>* GetParticles();
		ID3D11Buffer* GetParticleBuffer();

		DirectX::XMFLOAT3 GetPosition() const;
		ParticleType GetType() const;
		int GetParticleCount() const;
		int GetBufferSize() const;

		void SetPosition(const DirectX::XMFLOAT3 position);
		void SetType(const ParticleType& type);
		void SetParticleCount(const int& particleCount);

		bool HasTimeLeft() const;

		bool IsActive() const;
		void Activate();
		void Deactivate();

	};
}

