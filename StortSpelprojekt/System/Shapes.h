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

//TODO unfinished
struct Cylinder
{
	Vec3 _position;
	Vec3 _direction;
	float _radius;
	float _height;

	Cylinder(Vec3 position = Vec3(), float radius = 0.0f, float height = 0.0f)
	{
		_position = position;
		_direction = Vec3(0.0f, 1.0f, 0.0f);
		_radius = radius;
		_height = height;
	}
};

//TODO unfinished
struct Triangle
{
	Vec3 _point1, _point2, _point3;

	Triangle(Vec3 point1 = Vec3(), Vec3 point2 = Vec3(), Vec3 point3 = Vec3())
	{
		_point1 = point1;
		_point2 = point2;
		_point3 = point3;
	}
};

//TODO unfinished
struct Cone
{
	Vec3 _origin;
	Vec3 _direction;
	float _radius;

	Cone(Vec3 origin = Vec3(), Vec3 direction = Vec3(), float radius = 0.0f)
	{
		_origin = origin;
		_direction = direction;
		_radius = radius;
	}
};

//Aligned square
struct Square
{
	Vec3 _point1, _point2;

	Square(Vec3 point1 = Vec3(), Vec3 point2 = Vec3())
	{
		_point1 = point1;
		_point2 = point2;
	}
	Square(Vec3 position, float height, float width)
	{
		_point1 = Vec3(position._x - (height*0.5f), position._y, position._z - (width*0.5f));
		_point2 = Vec3(position._x + (height*0.5f), position._y, position._z + (width*0.5f));
	}
};

struct Box
{
	Vec3 _position;
	Plane _xSlab;
	Plane _ySlab;
	Plane _zSlab;

	Box(float xLength, float yLength, float zLength, Vec3 position = Vec3(), Vec3 direction = Vec3(0.0f, 0.0f, 1.0f))
	{
		_position = position;
		_zSlab = Plane(direction, zLength*0.5f);
		_ySlab = Plane(Vec3(0.0f, 1.0f, 0.0f), yLength *0.5f);
		_xSlab = Plane(direction.Cross(_ySlab._normal), xLength*0.5f);
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

static bool Collision(Ray ray, Cylinder cylinder)
{
	Vec3 objectVector = cylinder._position - ray._origin;
	Vec3 tMax = Vec3();
	Vec3 tMin = Vec3();
	float smallestTMax = FLT_MAX;
	float greatestTMin = -FLT_MAX;
	bool hit = false;

	Plane topPlane = Plane(cylinder._direction, cylinder._height);

	//The ray is not parallell with the plane
	if (Collision(ray, topPlane))
	{
		tMax._y = ((cylinder._position._y + cylinder._height) - ray._origin._y) / ray._direction._y;
		tMin._y = ((cylinder._position._y) - ray._origin._y) / ray._direction._y;

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

		//Check the hit points in y-axis as well as cylinder radius to make sure it isn't too far off to the side
		if ((greatestTMin <= smallestTMax) && Collision(ray, Sphere(cylinder._position, cylinder._radius)))
		{
			hit = true;
		}
	}

	return hit;
}