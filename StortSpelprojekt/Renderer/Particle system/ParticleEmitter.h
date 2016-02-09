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

		ParticleType _type;
		DirectX::XMFLOAT3 _position;
		std::vector<Particle> _particles;

		bool _isActive;
		float _timeLeft;

	public:

		ParticleEmitter();
		ParticleEmitter(const ParticleType& type, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4 color, int particleCount, float timeLimit, bool isActive);
		virtual ~ParticleEmitter();

		void Reset(const ParticleType& type, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4 color, int particleCount, float timeLimit, bool isActive);

		void Update(double deltaTime);

		std::vector<Particle>* GetParticles();
		DirectX::XMFLOAT3 GetPosition() const;
		ParticleType GetType() const;
		int GetParticleCount() const;

		void SetPosition(const DirectX::XMFLOAT3 position);
		void SetType(const ParticleType& type);
		void SetParticleCount(const int& particleCount);

		bool HasTimeLeft() const;

		bool IsActive() const;
		void Activate();
		void Deactivate();

	};
}

