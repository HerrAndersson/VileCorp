#pragma once

#define SYSTEM_EXPORT __declspec(dllexport)

#include <DirectXMath.h>

using namespace DirectX;

struct Vec2
{
	float _x, _y;

	Vec2(float x = 0.0f, float y = 0.0f)
	{
		_x = x;
		_y = y;
	}

	Vec2(XMFLOAT2 xmfloat)
	{
		_x = xmfloat.x;
		_y = xmfloat.y;

	}
	Vec2(XMVECTOR xmvector)
	{
		XMFLOAT2 xmfloat;
		XMStoreFloat2(&xmfloat, xmvector);
		_x = xmfloat.x;
		_y = xmfloat.y;
	}

	XMFLOAT2 convertToXMFLOAT()
	{
		return XMFLOAT2(_x, _y);
	}

	XMVECTOR convertToXMVECTOR()
	{
		return XMVectorSet(_x, _y, 1.0f, 1.0f);
	}

	float Length()
	{
		XMVECTOR vector = XMVector4Length(XMVectorSet(_x, _y, 1.0f, 1.0f));

		XMFLOAT4 float4;
		XMStoreFloat4(&float4, vector);

		return float4.x;
	}
	Vec2 operator+(const Vec2& rhs)
	{
		return Vec2(
			_x + rhs._x,
			_y + rhs._y);
	}
	Vec2 operator-(const Vec2& rhs)
	{
		return Vec2(
			_x - rhs._x,
			_y - rhs._y);
	}
	Vec2 operator*(const float& rhs)
	{
		return Vec2(_x * rhs, _y * rhs);
	}

	float Dot(Vec2 vector)
	{
		XMFLOAT4 temp;
		XMStoreFloat4(&temp, XMVector2Dot(XMVectorSet(_x, _y, 1.0f, 1.0f), XMVectorSet(vector._x, vector._y, 1.0f, 1.0f)));

		return temp.x;
	}
	Vec2 Cross(Vec2 vector)
	{
		XMFLOAT4 temp;
		XMStoreFloat4(&temp, XMVector2Cross(XMVectorSet(_x, _y, 1.0f, 1.0f), XMVectorSet(vector._x, vector._y, 1.0f, 1.0f)));

		return Vec2(temp.x, temp.y);
	}
	Vec2 Normalize()
	{
		XMFLOAT4 temp;
		XMStoreFloat4(&temp, XMVector3Normalize(XMVectorSet(_x, _y, 1.0f, 1.0f)));

		return Vec2(temp.x, temp.y);
	}

};

struct Vec3
{
	float _x, _y, _z;

	Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
	{
		_x = x;
		_y = y;
		_z = z;
	}
	Vec3(XMFLOAT3 xmfloat)
	{
		_x = xmfloat.x;
		_y = xmfloat.y;
		_z = xmfloat.z;
	}
	Vec3(XMVECTOR xmvector)
	{
		XMFLOAT3 xmfloat;
		XMStoreFloat3(&xmfloat, xmvector);
		_x = xmfloat.x;
		_y = xmfloat.y;
		_z = xmfloat.z;

	}

	XMFLOAT3 convertToXMFLOAT()
	{
		return XMFLOAT3(_x, _y, _z);
	}

	XMVECTOR convertToXMVECTOR()
	{
		return XMVectorSet(_x, _y, _z, 1.0f);
	}

	float Length()
	{
		XMVECTOR vector = XMVector4Length(XMVectorSet(_x, _y, _z, 1.0f));

		XMFLOAT4 float4;
		XMStoreFloat4(&float4, vector);

		return float4.x;
	}
	Vec3 operator+(const Vec3& rhs)
	{
		return Vec3(
			_x + rhs._x,
			_y + rhs._y,
			_z + rhs._z);
	}
	Vec3 operator-(const Vec3& rhs)
	{
		return Vec3(
			_x - rhs._x,
			_y - rhs._y,
			_z - rhs._z);
	}
	Vec3 operator*(const float& rhs)
	{
		return Vec3(_x * rhs, _y * rhs, _z * rhs);
	}
	float Dot(Vec3 vector)
	{
		XMFLOAT4 temp;
		XMStoreFloat4(&temp, XMVector3Dot(convertToXMVECTOR(), vector.convertToXMVECTOR()));

		return temp.x;
	}
	Vec3 Cross(const Vec3& vector)
	{
		XMFLOAT4 temp;
		XMStoreFloat4(&temp, XMVector3Cross(XMVectorSet(_x, _y, _z, 1.0f), XMVectorSet(vector._x, vector._y, vector._z, 1.0f)));

		return Vec3(temp.x, temp.y, temp.z);
	}
	Vec3 Normalize()
	{
		XMFLOAT4 temp;
		XMStoreFloat4(&temp, XMVector3Normalize(XMVectorSet(_x, _y, _z, 1.0f)));

		return Vec3(temp.x, temp.y, temp.z);
	}
};

struct Vec4
{
	float _x, _y, _z, _w;

	Vec4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f)
	{
		_x = x;
		_y = y;
		_z = z;
		_w = w;
	}
	Vec4(XMFLOAT4 xmfloat)
	{
		_x = xmfloat.x;
		_y = xmfloat.y;
		_z = xmfloat.z;
		_w = xmfloat.w;
	}
	Vec4(XMVECTOR xmvector)
	{
		XMFLOAT4 xmfloat;
		XMStoreFloat4(&xmfloat, xmvector);
		_x = xmfloat.x;
		_y = xmfloat.y;
		_z = xmfloat.z;
		_w = xmfloat.w;
	}

	XMFLOAT4 convertToXMFLOAT()
	{
		return XMFLOAT4(_x, _y, _z, _w);
	}

	XMVECTOR convertToXMVECTOR()
	{
		return XMVectorSet(_x, _y, _z, _w);
	}

	float Length()
	{
		XMVECTOR vector = XMVector4Length(XMVectorSet(_x, _y, _z, _w));

		XMFLOAT4 float4;
		XMStoreFloat4(&float4, vector);

		return float4.x;
	}
	Vec4 operator+(const Vec4& rhs)
	{
		return Vec4(
			_x + rhs._x,
			_y + rhs._y,
			_z + rhs._z,
			_w + rhs._w);

	}
	Vec4 operator-(const Vec4& rhs)
	{
		return Vec4(
			_x - rhs._x,
			_y - rhs._y,
			_z - rhs._z,
			_w - rhs._w);
	}
	Vec4 operator*(const float& rhs)
	{
		return Vec4(_x * rhs, _y * rhs, _z * rhs, _w * rhs);
	}
	float Dot(const Vec4& vector)
	{
		XMFLOAT4 temp;
		XMStoreFloat4(&temp, XMVector4Dot(XMVectorSet(_x, _y, _z, _w), XMVectorSet(vector._x, vector._y, vector._z, vector._w)));

		return temp.x;
	}

	Vec4 Cross(const Vec4& vector)
	{
		XMFLOAT4 temp;
		XMStoreFloat4(&temp, XMVector3Cross(XMVectorSet(_x, _y, _z, _w), XMVectorSet(vector._x, vector._y, vector._z, vector._w)));

		return Vec4(temp.x, temp.y, temp.z, temp.w);
	}

	Vec4 Normalize()
	{
		XMFLOAT4 temp;
		XMStoreFloat4(&temp, XMVector3Normalize(XMVectorSet(_x, _y, _z, _w)));

		return Vec4(temp.x, temp.y, temp.z, temp.w);
	}
};
