#pragma once
#define RENDERER_EXPORT __declspec(dllexport)
#include <DirectXMath.h>
#include <d3d11.h>

//Disable warning about DirectX XMFLOAT3/XMMATRIX etc
#pragma warning( disable: 4251 )

namespace Renderer
{
	class RENDERER_EXPORT Particle
	{

	private:

		DirectX::XMFLOAT3 _position;
		DirectX::XMFLOAT3 _direction;
		DirectX::XMFLOAT4 _color;

		bool _isActive;

	public:

		Particle();
		Particle(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction = DirectX::XMFLOAT3(0,1,0), const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f));
		virtual ~Particle();

		void Reset(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT4& color);

		DirectX::XMFLOAT3 GetPosition() const;
		DirectX::XMFLOAT3 GetDirection() const;
		DirectX::XMFLOAT4 GetColor() const;

		void SetPosition(const DirectX::XMFLOAT3& position);
		void SetDirection(const DirectX::XMFLOAT3& direction);
		void SetColor(const DirectX::XMFLOAT4& color);

		bool IsActive() const;
		void Activate();
		void Deactivate();

	};
}

