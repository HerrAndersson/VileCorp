#ifndef CAMERA_H
#define CAMERA_H
#include <DirectXMath.h>

#include "Settings/settings.h"


#define CAMERA_EXPORT __declspec(dllexport)
//Disable warning about DirectX  FLOAT3/MATRIX
#pragma warning( disable: 4251 )

namespace System
{
	enum CamMode
	{
		FREE_CAM,
		LOCKED_CAM
	};

	class CAMERA_EXPORT Camera
	{
	private:
		DirectX::XMFLOAT3	_position;
		DirectX::XMFLOAT3	_right;
		DirectX::XMFLOAT3	_rotatedRight;
		DirectX::XMFLOAT3	_up;
		DirectX::XMFLOAT3	_forward;
		DirectX::XMFLOAT3	_rotatedForward;
		DirectX::XMFLOAT3	_rotation;

		DirectX::XMMATRIX	_view;
		DirectX::XMMATRIX	_proj;
		DirectX::XMMATRIX	_ortho;
		DirectX::XMMATRIX	_baseView;

		CamMode				_mode;

	public:

		Camera(System::Settings* settings);
		~Camera();

		void Update();
		void Resize(System::Settings* settings);

		void SetPosition(const DirectX::XMFLOAT3& position);
		void Move(const DirectX::XMFLOAT3& offset, double deltaTime);
		void SetRotation(const DirectX::XMFLOAT3& rotation);
		void Rotate(const DirectX::XMFLOAT3& offset);

		DirectX::XMFLOAT3 GetPosition()const;
		DirectX::XMFLOAT3 GetRotation()const;

		DirectX::XMMATRIX* GetViewMatrix();
		DirectX::XMMATRIX* GetProjectionMatrix();

		DirectX::XMFLOAT3 GetForwardVector() const;
		DirectX::XMFLOAT3 GetRightVector() const;

		CamMode GetMode()const;
		void SetMode(CamMode mode);

		//Overloading these guarantees 16B alignment of XMMATRIX
		void* operator new(size_t i);
		void operator delete(void* p);

	};
}

#endif