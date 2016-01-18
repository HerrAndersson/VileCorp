#include "Spotlight.h"

using namespace DirectX;

namespace Renderer
{
	Spotlight::Spotlight(float nearClip, float farClip, float fov, int width, int height, float intensity, float range, DirectX::XMFLOAT3 color)
	{
		_position = XMFLOAT3(0, 0, 0);
		_rotation = XMFLOAT3(0, 0, 0);
		_direction = XMFLOAT3(0, 0, 1);
		_up = XMFLOAT3(0, 1, 0);

		_angle = fov;
		_intensity = intensity;
		_range = range;

		_color = color;

		//Prepare vectors for Matrix initialization
		XMVECTOR vPos = XMLoadFloat3(&_position);
		XMVECTOR vDir = XMVector3TransformCoord(XMLoadFloat3(&_direction), XMMatrixRotationRollPitchYaw(XMConvertToRadians(_rotation.x), XMConvertToRadians(_rotation.y), XMConvertToRadians(_rotation.z)));
		XMVECTOR vUp = XMLoadFloat3(&_up);

		_viewMatrix = XMMatrixLookAtLH(vPos, vPos + vDir, vUp);
		_projectionMatrix = XMMatrixPerspectiveFovLH(fov, (float)width / (float)height, nearClip, farClip);





		XMVECTOR L = XMVectorSet(_position.x, _position.y, _position.z, 1);					//L light position
		XMVECTOR d = XMVectorSet(_direction.x, _direction.y, _direction.z, 1);				//d light direction(unit vector)
		float r = range;																	//r light radius
		float a = XMConvertToDegrees(fov);													//a opening angle

		XMVECTOR B = L + d*r;																//B = L + d*r is the center of the cone base.
		float w = r * sin(a);																//w = r * sin a is the radius of the cone base.

		XMVECTOR T = XMVectorSet(1, 0, 0, 0);
		XMVECTOR X = XMVector3Cross(d, T);													//Calculate a vector x that is normal to d. 
																							
		if (XMVectorGetX(XMVector3Length(X)) == 0)											//if cross(d, (1, 0, 0)) != 0 this is your x, else cross(d, (0, 1, 0)).
		{
			T = XMVectorSet(0, 1, 0, 0);
			X = XMVector3Cross(d, T);
		}

		X = XMVector4Normalize(X);															//You also have to normalize it after that.

		XMVECTOR Y = XMVector3Cross(d, X);													//Given this x, calculate another vector y = cross(d, x).y should be unit length already, you don't need to normalize it.


	//		Now the vertices of the cone base are given by :
	//	v(t) = B + w * (x * cos t + y * sin t)
	//		with t varying from 0 to 2pi in as many steps as you want... With these you can draw two triangle fans, one around B, one around L.

		double pi2 = 2 * XM_PI;
		double resolution = 5;
		for (double t = 0; t < pi2; t += pi2 / resolution)
		{
			XMVECTOR v = B + w *(X*cos(t) + Y * sin(t));

			float f = t;
		}
	}

	Spotlight::~Spotlight()
	{

	}

	void Spotlight::Update()
	{
		//Prepare vectors for Matrix initialization
		XMFLOAT3 dir = DirectX::XMFLOAT3(0, 0, 1);
		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(_rotation.x), XMConvertToRadians(_rotation.y), XMConvertToRadians(_rotation.z));

		XMVECTOR vPos = DirectX::XMLoadFloat3(&_position);
		XMVECTOR vDir = XMVector3TransformCoord(DirectX::XMLoadFloat3(&dir), rotationMatrix);
		XMVECTOR vUp = XMVector3TransformCoord(DirectX::XMLoadFloat3(&_up), rotationMatrix);

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

	void Spotlight::SetIntensity(float intensity)
	{
		_intensity = intensity;
	}

	void Spotlight::SetRange(float range)
	{
		_range = range;
	}

	void Spotlight::SetColor(XMFLOAT3 color)
	{
		_color = color;
	}

	float Spotlight::GetIntensity() const
	{
		return _intensity;
	}

	float Spotlight::GetRange() const
	{
		return _range;
	}

	float Spotlight::GetAngle() const
	{
		return _angle;
	}

	XMFLOAT3 Spotlight::GetColor() const
	{
		return _color;
	}

	//bool Spotlight::IsPotentiallyInLight(XMFLOAT3 position)
	//{
	//	return false;
	//}

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* Spotlight::operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

		void Spotlight::operator delete(void* p)
	{
		_mm_free(p);
	}
}
