#pragma once
#define RENDERER_EXPORT __declspec(dllexport)
#include <DirectXMath.h>
#include <d3d11.h>
#include "ParticleUtils.h"

//Disable warning about DirectX XMFLOAT3/XMMATRIX etc
#pragma warning( disable: 4251 )

namespace Renderer
{
	class RENDERER_EXPORT Particle
	{

	private:

		DirectX::XMFLOAT3 _position;
		DirectX::XMFLOAT3 _direction;

		float _speed;
		float _timeLeft;

		bool _isActive;

	public:

		Particle();
		Particle(const DirectX::XMFLOAT3& position, float speed, float timeLeft, const DirectX::XMFLOAT3& direction = DirectX::XMFLOAT3(0, 1, 0));
		virtual ~Particle();

		DirectX::XMFLOAT3 GetPosition() const;
		DirectX::XMFLOAT3 GetDirection() const;
		float GetSpeed() const;
		float GetTimeLeft() const;

		void SetPosition(const DirectX::XMFLOAT3& position);
		void SetDirection(const DirectX::XMFLOAT3& direction);
		void SetSpeed(float speed);

		void ResetTimeLeft(float time);
		void DecreaseTimeLeft(float deltaTime);

		bool IsActive() const;
		void Activate();
		void Deactivate();

	};
}

