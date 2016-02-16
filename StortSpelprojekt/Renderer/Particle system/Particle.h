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
		DirectX::XMFLOAT3 _target;

		bool _isActive;

	public:

		Particle();
		Particle(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target = DirectX::XMFLOAT3(0, 0, 0), const DirectX::XMFLOAT3& direction = DirectX::XMFLOAT3(0, 1, 0));
		virtual ~Particle();

		void Reset(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target = DirectX::XMFLOAT3(0, 0, 0), const DirectX::XMFLOAT3& direction = DirectX::XMFLOAT3(0, 1, 0));

		DirectX::XMFLOAT3 GetPosition() const;
		DirectX::XMFLOAT3 GetDirection() const;
		DirectX::XMFLOAT3 GetTargetPosition() const;

		void SetPosition(const DirectX::XMFLOAT3& position);
		void SetDirection(const DirectX::XMFLOAT3& direction);
		void SetTargetPosition(const DirectX::XMFLOAT3& target);

		bool IsActive() const;
		void Activate();
		void Deactivate();

	};
}

