#ifndef CAMERA_H
#define CAMERA_H

#define SYSTEM_EXPORT __declspec(dllexport)
#include <DirectXMath.h>

//Disable warning about DirectX  FLOAT3/MATRIX
#pragma warning( disable: 4251 )

namespace System
{
	class SYSTEM_EXPORT Camera
	{
	private:
		DirectX::XMFLOAT3	_position;
		DirectX::XMFLOAT3	_right;
		DirectX::XMFLOAT3	_up;
		DirectX::XMFLOAT3	_forward;
		DirectX::XMFLOAT3	_rotation;

		float				_nearClip;
		float				_farClip;
		float				_fieldOfView;
		float				_aspectRatio;

		DirectX::XMMATRIX	_view;
		DirectX::XMMATRIX	_proj;
		DirectX::XMMATRIX	_ortho;
		DirectX::XMMATRIX	_baseView;

	public:
		Camera(float nearClip, float farClip, int width, int height);
		~Camera();

		void Update();
		void Resize(int width, int height);

		DirectX::XMFLOAT3 GetPosition()const;
		void SetPosition(DirectX::XMFLOAT3 position);
		DirectX::XMFLOAT3 GetRotation()const;
		void SetRotation(DirectX::XMFLOAT3 rotation);

		DirectX::XMMATRIX* GetViewMatrix();
		DirectX::XMMATRIX* GetProjectionMatrix();
		//OrthoMatrix is commonly used for anything 2D. Commonly used in the "projection slot".
		DirectX::XMMATRIX* GetOrthoMatrix();
		//BaseViewMatrix is used for 2D HUD. Commonly used in the "view slot".
		DirectX::XMMATRIX* GetBaseViewMatrix();

		//Overloading these guarantees 16B alignment of XMMATRIX
		void* operator new(size_t i);
		void operator delete(void* p);
	};
}

#endif