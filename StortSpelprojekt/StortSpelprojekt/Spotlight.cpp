#include "Spotlight.h"

using namespace DirectX;

Spotlight::Spotlight(float nearClip, float farClip, float fov, int width, int height)
{
	_position = DirectX::XMFLOAT3(0, 0, 0);
	_rotation = DirectX::XMFLOAT3(0, 0, 0);
	_direction = DirectX::XMFLOAT3(0, 0, 1);
	_up = DirectX::XMFLOAT3(0, 1, 0);

	//Prepare vectors for Matrix initialization
	DirectX::XMVECTOR vPos = DirectX::XMLoadFloat3(&_position);
	DirectX::XMVECTOR vDir = XMVector3TransformCoord(DirectX::XMLoadFloat3(&_direction),  XMMatrixRotationRollPitchYaw(XMConvertToRadians(_rotation.x), XMConvertToRadians(_rotation.y), XMConvertToRadians(_rotation.z)));
	DirectX::XMVECTOR vUp = DirectX::XMLoadFloat3(&_up);
	
	_viewMatrix = DirectX::XMMatrixLookAtLH(vPos, vPos + vDir, vUp);
	_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fov, (float)width / (float)height, nearClip, farClip);
}

Spotlight::~Spotlight()
{
	
}

void Spotlight::Update()
{
	//Prepare vectors for Matrix initialization
	XMFLOAT3 dir = DirectX::XMFLOAT3(0, 0, 1);
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(_rotation.x), XMConvertToRadians(_rotation.y), XMConvertToRadians(_rotation.z));

	DirectX::XMVECTOR vPos = DirectX::XMLoadFloat3(&_position);
	DirectX::XMVECTOR vDir = XMVector3TransformCoord(DirectX::XMLoadFloat3(&dir), rotationMatrix);
	DirectX::XMVECTOR vUp = XMVector3TransformCoord(DirectX::XMLoadFloat3(&_up), rotationMatrix);

	XMStoreFloat3(&dir, vDir);
	XMStoreFloat3(&_up, vUp);
	_direction = dir;

	_viewMatrix = DirectX::XMMatrixLookAtLH(vPos, vPos + vDir, vUp);
}

void Spotlight::SetPosition(XMFLOAT3 position)
{
	_position = position;
	Update();
}

void Spotlight::SetRotation(DirectX::XMFLOAT3 rotation)
{
	_rotation = rotation;
	Update();
}

void Spotlight::SetPositionAndRotation(XMFLOAT3 position, XMFLOAT3 rotation)
{
	_rotation = rotation;
	_position = position;
	Update();
}

XMFLOAT3 Spotlight::GetPosition()const
{
	return _position;
}

XMFLOAT3 Spotlight::GetRotation()const
{
	return _rotation;
}

XMFLOAT3 Spotlight::GetDirection() const
{
	return _direction;
}

XMMATRIX* Spotlight::GetViewMatrix()
{
	return &_viewMatrix;
}
XMMATRIX* Spotlight::GetProjectionMatrix()
{
	return &_projectionMatrix;
}

//Overloading these guarantees 16B alignment of XMMATRIX
void* Spotlight::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void Spotlight::operator delete(void* p)
{
	_mm_free(p);
}
