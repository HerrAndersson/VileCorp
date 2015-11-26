#ifndef CAMERA_H
#define CAMERA_H

#define SYSTEM_EXPORT __declspec(dllexport)
#include <DirectXMath.h>

namespace Renderer
{
	__declspec(align(16)) class Camera
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
		Camera();
		~Camera();

		void Initialize(float nearClip, float farClip, int width, int height);
		void Update();

		DirectX::XMFLOAT3 GetPosition()const;
		void SetPosition(DirectX::XMFLOAT3 position);
		DirectX::XMFLOAT3 GetRotation()const;
		void SetRotation(DirectX::XMFLOAT3 rotation);


		DirectX::XMMATRIX GetViewMatrix()const;
		DirectX::XMMATRIX GetProjectionMatrix()const;
		//Get View-matrix multiplied with Projection-matrix. Can be used with Model space->world coordinates.
		DirectX::XMMATRIX GetViewProjectionMatrix()const;
		//OrthoMatrix is commonly used for anything 2D. Commonly used in the "projection slot".
		DirectX::XMMATRIX GetOrthoMatrix()const;
		//BaseViewMatrix is used for 2D HUD. Commonly used in the "view slot".
		DirectX::XMMATRIX GetBaseViewMatrix()const;
	};
}

#endif