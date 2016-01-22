#pragma once

#define SYSTEM_EXPORT __declspec(dllexport)

#include "VectorMath.h"
#include <vector>


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
	Vec3 _position;
	Vec3 _normal;
	float _offset;

	Plane(Vec3 position = Vec3(), Vec3 normal = Vec3(), float offset = 0.0f)
	{
		_position = position;
		_normal = normal.Normalize();
		_offset = offset;
	}
};

struct Triangle
{
	Vec3 _pos1, _pos2, _pos3;

	Triangle(Vec3 pos1 = Vec3(), Vec3 pos2 = Vec3(), Vec3 pos3 = Vec3())
	{
		_pos1 = pos1;
		_pos2 = pos2;
		_pos3 = pos3;
	}
};

struct Square
{
	Vec2 _minPos, _maxPos;

	Square(Vec2 position = Vec2(), float height = 0.0f, float width = 0.0f)
	{
		_minPos = Vec2(position._x - width, position._y - height);
		_maxPos = Vec2(position._x + width, position._y + height);
	}
	Square(Vec2 minPos, Vec2 maxPos)
	{
		_minPos = minPos;
		_maxPos = maxPos;
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

	Box(float xLength = 0.0f, float yLength = 0.0f , float zLength = 0.0f, Vec3 position = Vec3(), Vec3 rotation = Vec3())
	{
		_position = position;

		XMMATRIX rotationMatrix = XMMatrixTranspose(XMMatrixRotationQuaternion(rotation.convertToXMVECTOR()));

		XMVECTOR vecX = XMVector3TransformNormal(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), rotationMatrix);
		XMVECTOR vecY = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), rotationMatrix);
		XMVECTOR vecZ = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f), rotationMatrix);

		_xSlab = Plane(position, Vec3(vecX), xLength*0.5f);
		_ySlab = Plane(position, Vec3(vecY), yLength *0.5f);
		_zSlab = Plane(position, Vec3(vecZ), zLength*0.5f);
	}
	Box(Vec3 position, float xLength = 0.0f, float yLength = 0.0f, float zLength = 0.0f, Vec3 xNormal = Vec3(1.0f, 0.0f,0.0f), Vec3 yNormal = Vec3(0.0f, 1.0f, 0.0f), Vec3 zNormal = Vec3(0.0f, 0.0f, 1.0f))
	{
		_position = position;

		_xSlab = Plane(position, xNormal, xLength*0.5f);
		_ySlab = Plane(position, yNormal, yLength *0.5f);
		_zSlab = Plane(position, zNormal, zLength*0.5f);
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

//The Projection Formula
static Vec3 FindClosestPointOnVector(Vec3 point, Ray vector)
{
	Vec3 pointVector = point - vector._origin;
	float dotProduct = pointVector.Dot(vector._direction);
	float denominator = vector._direction.Dot(vector._direction);

	return vector._direction * (dotProduct / denominator);
}

//Separating Axis Theorem check for vectors
static bool SATVectorCheck(Ray axis, std::vector<Vec3> firstObjectCorners, std::vector<Vec3> secondObjectCorners )
{
	bool collision = false;

	Vec3 startingPoint = FindClosestPointOnVector(firstObjectCorners[0], axis);
	Vec3 firstMinPoint = startingPoint;
	Vec3 firstMaxPoint = startingPoint;

	for (unsigned int i = 0; i < firstObjectCorners.size(); i++)
	{
		if (startingPoint.Dot(firstObjectCorners[i]) < startingPoint.Dot(firstMinPoint))
		{
			firstMinPoint = FindClosestPointOnVector(firstObjectCorners[i], axis);
		}
		else if (startingPoint.Dot(firstObjectCorners[i]) > startingPoint.Dot(firstMaxPoint))
		{
			firstMaxPoint = FindClosestPointOnVector(firstObjectCorners[i], axis);
		}
	}

	startingPoint = FindClosestPointOnVector(secondObjectCorners[0], axis);
	Vec3 secondMinPoint = startingPoint;
	Vec3 secondMaxPoint = startingPoint;

	for (unsigned int i = 0; i < secondObjectCorners.size(); i++)
	{
		if (startingPoint.Dot(secondObjectCorners[i]) < startingPoint.Dot(secondMinPoint))
		{
			secondMinPoint = FindClosestPointOnVector(secondObjectCorners[i], axis);
		}
		else if (startingPoint.Dot(secondObjectCorners[i]) > startingPoint.Dot(secondMaxPoint))
		{
			secondMaxPoint = FindClosestPointOnVector(secondObjectCorners[i], axis);
		}
	}


	if (axis._direction._x != 0.0f)
	{
		if (firstMinPoint._x > secondMinPoint._x && firstMinPoint._x < secondMaxPoint._x)
		{
			collision = true;
		}
		else if (secondMinPoint._x > firstMinPoint._x && secondMinPoint._x < firstMaxPoint._x)
		{
			collision = true;
		}
	}
	else if (axis._direction._y != 0.0f)
	{
		if (firstMinPoint._y > secondMinPoint._y && firstMinPoint._y < secondMaxPoint._y)
		{
			collision = true;
		}
		else if (secondMinPoint._y > firstMinPoint._y && secondMinPoint._y < firstMaxPoint._y)
		{
			collision = true;
		}
	}
	else if (axis._direction._z != 0.0f)
	{
		if (firstMinPoint._z > secondMinPoint._z && firstMinPoint._z < secondMaxPoint._z)
		{
			collision = true;
		}
		else if (secondMinPoint._z > firstMinPoint._z && secondMinPoint._z < firstMaxPoint._z)
		{
			collision = true;
		}
	}

	return collision;
}

//Finds the cornerpoints in a shape
static std::vector<Vec3> FindCorners(Box box)
{
	std::vector<Vec3> corners;

	corners.push_back(box._position
		+ (box._xSlab._normal * box._xSlab._offset)
		+ (box._ySlab._normal * box._ySlab._offset)
		+ (box._zSlab._normal * box._zSlab._offset));

	corners.push_back(box._position
		- (box._xSlab._normal * box._xSlab._offset)
		+ (box._ySlab._normal * box._ySlab._offset)
		+ (box._zSlab._normal * box._zSlab._offset));

	corners.push_back(box._position
		+ (box._xSlab._normal * box._xSlab._offset)
		- (box._ySlab._normal * box._ySlab._offset)
		+ (box._zSlab._normal * box._zSlab._offset));

	corners.push_back(box._position
		+ (box._xSlab._normal * box._xSlab._offset)
		+ (box._ySlab._normal * box._ySlab._offset)
		- (box._zSlab._normal * box._zSlab._offset));

	corners.push_back(box._position
		- (box._xSlab._normal * box._xSlab._offset)
		- (box._ySlab._normal * box._ySlab._offset)
		+ (box._zSlab._normal * box._zSlab._offset));

	corners.push_back(box._position
		- (box._xSlab._normal * box._xSlab._offset)
		+ (box._ySlab._normal * box._ySlab._offset)
		- (box._zSlab._normal * box._zSlab._offset));

	corners.push_back(box._position
		+ (box._xSlab._normal * box._xSlab._offset)
		- (box._ySlab._normal * box._ySlab._offset)
		- (box._zSlab._normal * box._zSlab._offset));

	corners.push_back(box._position
		- (box._xSlab._normal * box._xSlab._offset)
		- (box._ySlab._normal * box._ySlab._offset)
		- (box._zSlab._normal * box._zSlab._offset));

	return corners;
}



static bool Collision(Ray ray, Plane plane)
{
	return ray._direction.Dot(plane._normal) != 0;
}

static bool Collision(Ray ray, Sphere sphere)
{
	Vec3 objectVector =  sphere._position - ray._origin;
	Vec3 projectedPoint = FindClosestPointOnVector(sphere._position, ray);
	float length = (objectVector - projectedPoint).Length();

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

static bool Collision(Box box1, Box box2)
{
	bool collision = true;
	std::vector<Vec3>firstBoxCorners = FindCorners(box1);
	std::vector<Vec3>secondBoxCorners = FindCorners(box2);


	if (!SATVectorCheck(Ray(Vec3(), box1._xSlab._normal), firstBoxCorners, secondBoxCorners))
	{
		collision = false;
	}
	else if (!SATVectorCheck(Ray(Vec3(), box1._ySlab._normal), firstBoxCorners, secondBoxCorners))
	{
		collision = false;
	}
	else if (!SATVectorCheck(Ray(Vec3(), box1._zSlab._normal), firstBoxCorners, secondBoxCorners))
	{
		collision = false;
	}
	else if (!SATVectorCheck(Ray(Vec3(), box2._xSlab._normal), firstBoxCorners, secondBoxCorners))
	{
		collision = false;
	}
	else if (!SATVectorCheck(Ray(Vec3(), box2._ySlab._normal), firstBoxCorners, secondBoxCorners))
	{
		collision = false;
	}
	else if (!SATVectorCheck(Ray(Vec3(), box2._zSlab._normal), firstBoxCorners, secondBoxCorners))
	{
		collision = false;
	}

	return collision;
}

static bool Collision(Vec3 point, Square square)
{
	return (point._x < square._maxPos._x && point._x > square._minPos._x &&
		point._z < square._maxPos._y && point._z > square._minPos._y);
}

static bool Collision(Triangle triangle, Square square)
{
	bool collision = false;

	if (Collision(triangle._pos1, square) || Collision(triangle._pos2, square) || Collision(triangle._pos3, square))
	{
		collision = true;
	}

	// SAT check
	if (!collision)
	{





	}







	return collision;
}

