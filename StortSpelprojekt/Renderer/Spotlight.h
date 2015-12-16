#pragma once
#include <DirectXMath.h>

#define RENDERER_EXPORT __declspec(dllexport)

//Disable warning about DirectX  FLOAT3/MATRIX
#pragma warning( disable: 4251 )

namespace Renderer
{
	class RENDERER_EXPORT Spotlight
	{
	private:

		DirectX::XMFLOAT3	_position;
		DirectX::XMFLOAT3	_rotation;
		DirectX::XMFLOAT3	_direction;
		DirectX::XMFLOAT3	_up;

		DirectX::XMMATRIX	_viewMatrix;
		DirectX::XMMATRIX	_projectionMatrix;

		float				_angle;
		float				_intensity;
		float				_range;

		DirectX::XMFLOAT3	_color;

		void Update();

	public:

		Spotlight(float nearClip, float farClip, float fov, int width, int height, float intensity = 1.0f, float range = 10.0f, DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
		~Spotlight();

		void SetPosition(DirectX::XMFLOAT3 position);
		void SetRotation(DirectX::XMFLOAT3 rotation);
		void SetPositionAndRotation(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation);

		void SetIntensity(float intensity);
		void SetRange(float range);
		void SetColor(DirectX::XMFLOAT3 color);

		float GetIntensity() const;
		float GetRange() const;
		float GetAngle() const;
		DirectX::XMFLOAT3 GetColor() const;

		DirectX::XMFLOAT3 GetPosition() const;
		DirectX::XMFLOAT3 GetRotation() const;
		DirectX::XMFLOAT3 GetDirection() const;

		DirectX::XMMATRIX* GetViewMatrix();
		DirectX::XMMATRIX* GetProjectionMatrix();

		bool IsPotentiallyInLight(DirectX::XMFLOAT3 position);

		//Overloading these guarantees 16B alignment of XMMATRIX
		void* operator new(size_t i);
		void operator delete(void* p);

	};
}

