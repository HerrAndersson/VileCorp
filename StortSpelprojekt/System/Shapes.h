#pragma once

#define SYSTEM_EXPORT __declspec(dllexport)

#include "VectorMath.h"


struct Ray
{
	Vec3 _origin;
	Vec3 _direction;

	Ray(Vec3 origin = Vec3(), Vec3 direction = Vec3())
	{
		_origin = origin;
		_direction = direction.Normalize();
	}
};

struct Plane
{
	Vec3 _normal;
	float _offset;

	Plane(Vec3 normal = Vec3(), float offset = 0.0f)
	{
		_normal = normal.Normalize();
		_offset = offset;
	}
};

struct Sphere
{
	Vec3 _position;
	float _radius;

	Sphere(Vec3 position = Vec3(), float radius = 0.0f)
	{
		_position = position;
		_radius = radius;
	}
};

struct Box
{
	Vec3 _position;
	Plane _xSlab;
	Plane _ySlab;
	Plane _zSlab;

	Box(float xLength, float yLength, float zLength, Vec3 position = Vec3(), Vec3 rotation = Vec3())
	{
		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYawFromVector(rotation.convertToXMVECTOR());

		_position = position;
		_xSlab = Plane(Vec3(XMVector3TransformCoord(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationMatrix)), xLength*0.5f);
		_ySlab = Plane(Vec3(XMVector3TransformCoord(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationMatrix)), yLength *0.5f);
		_zSlab = Plane(Vec3(XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix)), zLength*0.5f);
		
	}

};

//No exception for /0, always use collision() first
static Vec3 Intersection(Ray ray, Plane plane)
{
	float dotProduct = ray._origin.Dot(plane._normal);
	dotProduct += plane._offset;
		
	float denominator = ray._direction.Dot(plane._normal);
	float t = -(dotProduct) / denominator;

	return ray._origin + ray._direction * t;
}

static bool Collision(Ray ray, Plane plane)
{
	return ray._direction.Dot(plane._normal) != 0;
}

static bool Collision(Ray ray, Sphere sphere)
{
	Vec3 objectVector =  sphere._position - ray._origin;
	float dotProduct = objectVector.Dot(ray._direction);
	float denominator = ray._direction.Dot(ray._direction);
	Vec3 projectedVector = ray._direction * (dotProduct / denominator);
	float length = (objectVector - projectedVector).Length();

	return (length <= sphere._radius);
}

static bool Collision(Ray ray, Box box)
{
	Vec3 tMax = Vec3();
	Vec3 tMin = Vec3();
	float smallestTMax = FLT_MAX;
	float greatestTMin = -FLT_MAX;

	if (Collision(ray, box._xSlab))
	{
		tMax._x = ((box._position._x + box._xSlab._offset) - ray._origin._x) / ray._direction._x;
		tMin._x = ((box._position._x - box._xSlab._offset) - ray._origin._x) / ray._direction._x;

		if (tMax._x < tMin._x)
		{
			float temp = tMax._x;
			tMax._x = tMin._x;
			tMin._x = temp;
		}
		if (tMin._x > greatestTMin && tMin._x)
		{
			greatestTMin = tMin._x;
		}
		if (tMax._x < smallestTMax && tMax._x)
		{
			smallestTMax = tMax._x;
		}
	}

	if (Collision(ray, box._ySlab))
	{

		tMax._y = ((box._position._y + box._ySlab._offset) - ray._origin._y) / ray._direction._y;
		tMin._y = ((box._position._y - box._ySlab._offset) - ray._origin._y) / ray._direction._y;

		if (tMax._y < tMin._y)
		{
			float temp = tMax._y;
			tMax._y = tMin._y;
			tMin._y = temp;
		}
		if (tMin._y > greatestTMin)
		{
			greatestTMin = tMin._y;
		}

		if (tMax._y < smallestTMax)
		{
			smallestTMax = tMax._y;
		}
	}

	if (Collision(ray, box._zSlab))
	{
		tMax._z = ((box._position._z + box._zSlab._offset) - ray._origin._z) / ray._direction._z;
		tMin._z = ((box._position._z - box._zSlab._offset) - ray._origin._z) / ray._direction._z;

		if (tMax._z < tMin._z)
		{
			float temp = tMax._z;
			tMax._z = tMin._z;
			tMin._z = temp;
		}
		if (tMin._z > greatestTMin && tMin._z)
		{
			greatestTMin = tMin._z;
		}
		if (tMax._z < smallestTMax && tMax._z)
		{
			smallestTMax = tMax._z;
		}
	}

	return (greatestTMin <= smallestTMax);
}

