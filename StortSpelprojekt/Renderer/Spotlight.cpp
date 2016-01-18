#include "Spotlight.h"
#include <vector>
#include <stdexcept>

using namespace DirectX;

namespace Renderer
{
	Spotlight::Spotlight(ID3D11Device* device, float nearClip, float farClip, float fov, int width, int height, float intensity, float range, DirectX::XMFLOAT3 color, int resolution)
	{
		_position = XMFLOAT3(0, 0, 0);
		_rotation = XMFLOAT3(0, 0, 0);
		_direction = XMFLOAT3(0, 0, 1);
		_up = XMFLOAT3(0, 1, 0);

		_angle = fov;
		_intensity = intensity;
		_range = range;

		_color = color;

		_worldMatrix = XMMatrixTranslation(_position.x, _position.y, _position.z);
		_rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(_rotation.x), XMConvertToRadians(_rotation.y), XMConvertToRadians(_rotation.z));

		//Prepare vectors for Matrix initialization
		XMVECTOR vPos = XMLoadFloat3(&_position);
		XMVECTOR vDir = XMVector3TransformCoord(XMLoadFloat3(&_direction), _rotationMatrix);
		XMVECTOR vUp = XMLoadFloat3(&_up);

		_viewMatrix = XMMatrixLookAtLH(vPos, vPos + vDir, vUp);
		_projectionMatrix = XMMatrixPerspectiveFovLH(fov, (float)width / (float)height, nearClip, farClip);

		//Create a cone that represents the light as a volume
		XMVECTOR L = XMVectorSet(_position.x, _position.y, _position.z, 1);					
		XMVECTOR d = XMVectorSet(_direction.x, _direction.y, _direction.z, 1);				
		float r = range;																	
		float a = XMConvertToDegrees(fov);													

		XMVECTOR B = L + d*r; //Center of the cone base.
		float w = r * sin(a); //Radius of the cone base.

		//Calculate a vector X that is normal to direction
		XMVECTOR T = XMVectorSet(1, 0, 0, 0);
		XMVECTOR X = XMVector3Cross(d, T);													
																							
		if (XMVectorGetX(XMVector3Length(X)) == 0)
		{
			T = XMVectorSet(0, 1, 0, 0);
			X = XMVector3Cross(d, T);
		}
		X = XMVector4Normalize(X);

		//Given this x, calculate another vector y = cross(d, x)
		XMVECTOR Y = XMVector3Cross(d, X);

	    //The vertices of the cone base are given by: v(t) = B + w * (x * cos t + y * sin t), with t varying from 0 to 2*pi.
		double pi2 = 2 * XM_PI;
		std::vector<XMFLOAT3> points;
		XMFLOAT3 element;
		for (double t = 0; t < pi2; t += pi2 / resolution)
		{
			XMVECTOR v = B + w * (X * cos(t) + Y * sin(t));
			XMStoreFloat3(&element, v);
			points.push_back(element);
		}

		int pc = 0, pc2 = 0;
		std::vector<XMFLOAT3>triangles;
		XMFLOAT3 basePos;
		XMStoreFloat3(&basePos, B);
		for (int i = 0; i < resolution-1; i++)
		{
			triangles.push_back(_position);
			triangles.push_back(points[pc]);
			triangles.push_back(points[++pc]);

			triangles.push_back(basePos);
			triangles.push_back(points[pc2]);
			triangles.push_back(points[++pc2]);
		}
		//Fix the last connection between the last and the first
		triangles.push_back(_position);
		triangles.push_back(points[pc]);
		triangles.push_back(points[0]);

		triangles.push_back(basePos);
		triangles.push_back(points[pc2]);
		triangles.push_back(points[0]);

		//Create buffer
		_nrOfTriangles = resolution * 3 * 2;
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(XMFLOAT3) * _nrOfTriangles;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = triangles.data();
		HRESULT result = device->CreateBuffer(&bufferDesc, &data, &_lightConeVolume);
		if (FAILED(result))
			throw std::runtime_error("Spotlight: Failed to create vertex buffer");
	}

	Spotlight::~Spotlight()
	{
		_lightConeVolume->Release();
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

	ID3D11Buffer* Spotlight::GetVolumeBuffer() const
	{
		return _lightConeVolume;
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
}
