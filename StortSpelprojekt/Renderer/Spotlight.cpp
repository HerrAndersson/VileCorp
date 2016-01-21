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

		_rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(_rotation.x), XMConvertToRadians(_rotation.y), XMConvertToRadians(_rotation.z));
		_worldMatrix = _rotationMatrix * XMMatrixTranslation(_position.x, _position.y, _position.z);

		//Prepare vectors for Matrix initialization
		XMVECTOR vPos = XMLoadFloat3(&_position);
		XMVECTOR vDir = XMVector3TransformCoord(XMLoadFloat3(&_direction), _rotationMatrix);
		XMVECTOR vUp = XMLoadFloat3(&_up);

		_viewMatrix = XMMatrixLookAtLH(vPos, vPos + vDir, vUp);
		_projectionMatrix = XMMatrixPerspectiveFovLH(fov, (float)width / (float)height, nearClip, farClip);

		/// --------------------------------------- Create a cone that represents the light as a volume --------------------------------------- ///
		XMVECTOR pos = XMVector4Normalize(XMVectorSet(_position.x, _position.y, _position.z, 1));
		XMVECTOR dir = XMVector4Normalize(XMVectorSet(_direction.x, _direction.y, _direction.z, 1));																												

		XMVECTOR baseCenter = pos + dir * range; //Center of the cone base.
		double baseRadius = range * sin(fov); //Radius of the cone base.

		//Calculate a vector X that is normal to direction
		XMVECTOR T = XMVectorSet(1, 0, 0, 0);
		XMVECTOR X = XMVector3Cross(dir, T);													
		X = XMVector4Normalize(X);
		if (XMVectorGetX(XMVector3Length(X)) < 0.00000001f)
		{
			T = XMVectorSet(0, 1, 0, 0);
			X = XMVector3Cross(dir, T);
			X = XMVector4Normalize(X);
		}

		//Given this x, calculate another vector y = cross(d, x)
		XMVECTOR Y = XMVector3Cross(dir, X);
		Y = XMVector4Normalize(Y);

	    //The vertices of the cone base are given by: v(t) = B + w * (x * cos t + y * sin t), with t varying from 0 to 2*pi.
		double pi2 = 2 * XM_PI;
		std::vector<XMFLOAT3> points;
		XMFLOAT3 element;
		for (double t = 0; t < pi2; t += pi2 / resolution)
		{
			XMVECTOR v = baseCenter + (float)baseRadius * (X * (float)cos(t) + Y * (float)sin(t));
			XMStoreFloat3(&element, v);
			points.push_back(element);
		}

		int pc = 0, pc2 = 0;
		std::vector<XMFLOAT3>triangles;
		XMFLOAT3 basePos;
		XMStoreFloat3(&basePos, baseCenter);
		for (int i = 0; i < resolution-1; i++)
		{
			triangles.push_back(_position);
			triangles.push_back(points[pc]);
			triangles.push_back(points[++pc]);

			triangles.push_back(basePos);
			triangles.push_back(points[pc2]);
			triangles.push_back(points[++pc2]);
		}
		//Fix the connection between the last and the first
		triangles.push_back(_position);
		triangles.push_back(points[pc]);
		triangles.push_back(points[0]);

		triangles.push_back(basePos);
		triangles.push_back(points[pc2]);
		triangles.push_back(points[0]);

		//Create buffer
		_vertexCount = resolution * 3 * 2;
		_vertexSize = sizeof(XMFLOAT3);

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = _vertexSize * _vertexCount;

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
		_worldMatrix = rotationMatrix * XMMatrixTranslation(_position.x, _position.y, _position.z);
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

	XMMATRIX* Spotlight::GetWorldMatrix()
	{
		return &_worldMatrix;
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

	int Spotlight::GetVertexCount() const
	{
		return _vertexCount;
	}

	int Spotlight::GetVertexSize() const
	{
		return _vertexSize;
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
