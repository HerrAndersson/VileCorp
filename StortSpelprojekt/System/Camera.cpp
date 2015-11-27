#include "Camera.h"

namespace System
{
	Camera::Camera(float nearClip, float farClip, int width, int height)
	{
		_nearClip			= nearClip; //0.5f;
		_farClip			= farClip; // 1000.0f;
		_fieldOfView		= DirectX::XM_PIDIV4;
		_aspectRatio		= (float)width / (float)height;

		_position			= DirectX::XMFLOAT3(0, 0, 0);
		_right				= DirectX::XMFLOAT3(1, 0, 0);
		_up					= DirectX::XMFLOAT3(0, 1, 0);
		_forward			= DirectX::XMFLOAT3(0, 0, 1);
		_rotation			= DirectX::XMFLOAT3(0, 0, 0);

		//Prepare vectors for Matrix initialization
		DirectX::XMVECTOR vPos, vFor, vUp;
		vPos				= DirectX::XMLoadFloat3(&_position);
		vFor				= DirectX::XMLoadFloat3(&_forward);
		vUp					= DirectX::XMLoadFloat3(&_up);

		_view				= DirectX::XMMatrixLookAtLH(vPos, vFor, vUp);
		_proj				= DirectX::XMMatrixPerspectiveFovLH(_fieldOfView, _aspectRatio, _nearClip, _farClip);
		_ortho				= DirectX::XMMatrixOrthographicLH(width, height, _nearClip, _farClip);
		_baseView			= DirectX::XMMatrixLookAtLH(DirectX::XMVectorNegate(vFor), vFor, vUp);
	}

	Camera::~Camera()
	{}

	void Camera::Update()
	{
		DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z);
		
		XMStoreFloat3(&_right, DirectX::XMVector3TransformNormal(XMLoadFloat3(&_right), rotation));
		XMStoreFloat3(&_up, DirectX::XMVector3TransformNormal(XMLoadFloat3(&_up), rotation));
		XMStoreFloat3(&_forward, DirectX::XMVector3TransformNormal(XMLoadFloat3(&_forward), rotation));
		
		DirectX::XMVECTOR vPos, vFor, vUp;
		vPos = DirectX::XMLoadFloat3(&_position);
		vFor = DirectX::XMLoadFloat3(&_forward);
		vUp = DirectX::XMLoadFloat3(&_up);
		_view = DirectX::XMMatrixLookAtLH(vPos, DirectX::XMVectorAdd(vPos, vFor), vUp);
	}

	DirectX::XMFLOAT3 Camera::GetPosition()const
	{
		return _position;
	}

	void Camera::SetPosition(DirectX::XMFLOAT3 position)
	{
		_position = position;
	}

	DirectX::XMFLOAT3 Camera::GetRotation()const
	{
		return _rotation;
	}

	void Camera::SetRotation(DirectX::XMFLOAT3 rotation)
	{
		_rotation = rotation;
	}

	DirectX::XMMATRIX* Camera::GetViewMatrix()
	{
		return &_view;
	}
	DirectX::XMMATRIX* Camera::GetProjectionMatrix()
	{
		return &_proj;
	}
	DirectX::XMMATRIX* Camera::GetOrthoMatrix()
	{
		return &_ortho;
	}
	DirectX::XMMATRIX* Camera::GetBaseViewMatrix()
	{
		return &_baseView;
	}

}