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

	Square(Vec2 position = Vec2(), float height = 0.0f, float width = 1.0f)
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

struct Circle
{
	Vec2 _position;
	float _radius;

	Circle(Vec2 position = Vec2(), float radius = 0.0f)
	{
		_position = position;
		_radius = radius;
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

	Box(float xLength = 0.0f, float yLength = 0.0f, float zLength = 0.0f, Vec3 position = Vec3(), Vec3 rotation = Vec3())
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
	Box(Vec3 position, float xLength = 0.0f, float yLength = 0.0f, float zLength = 0.0f, Vec3 xNormal = Vec3(1.0f, 0.0f, 0.0f), Vec3 yNormal = Vec3(0.0f, 1.0f, 0.0f), Vec3 zNormal = Vec3(0.0f, 0.0f, 1.0f))
	{
		_position = position;

		_xSlab = Plane(position, xNormal, xLength*0.5f);
		_ySlab = Plane(position, yNormal, yLength *0.5f);
		_zSlab = Plane(position, zNormal, zLength*0.5f);
	}


};



//No exception for /0, always use collision() first
static Vec3 Intersection(Ray &ray, Plane &plane)
{
	float dotProduct = ray._origin.Dot(plane._normal);
	dotProduct += plane._offset;

	float denominator = ray._direction.Dot(plane._normal);
	float t = -(dotProduct) / denominator;

	return ray._origin + ray._direction * t;
}

//The Projection Formula
static Vec2 FindClosestPointOnVector(Vec2 &point, Vec2 &vector)
{
	Vec2 pointVector = point - Vec2();
	float dotProduct = pointVector.Dot(vector);
	float denominator = vector.Dot(vector);

	return vector * (dotProduct / denominator);
}

static Vec3 FindClosestPointOnVector(Vec3 &point, Ray &vector)
{
	Vec3 pointVector = point - vector._origin;
	float dotProduct = pointVector.Dot(vector._direction);
	float denominator = vector._direction.Dot(vector._direction);

	return vector._direction * (dotProduct / denominator);
}




//Separating Axis Theorem check for vectors
static bool SATVectorCheck(Vec2 &axis, std::vector<Vec2> *firstObjectCorners, std::vector<Vec2> *secondObjectCorners)
{
	bool collision = false;
	float firstMinPoint = axis.Dot(firstObjectCorners->at(0));
	float firstMaxPoint = firstMinPoint;

	unsigned int size = firstObjectCorners->size();
	for (unsigned int i = 0; i < size; i++)
	{
		float point = axis.Dot(firstObjectCorners->at(i));

		if (point < firstMinPoint)
		{
			firstMinPoint = point;
		}
		else if (point > firstMaxPoint)
		{
			firstMaxPoint = point;
		}
	}

	float secondMinPoint = axis.Dot(secondObjectCorners->at(0));
	float secondMaxPoint = secondMinPoint;

	size = secondObjectCorners->size();
	for (unsigned int i = 0; i < size; i++)
	{
		float point = axis.Dot(secondObjectCorners->at(i));

		if (point < secondMinPoint)
		{
			secondMinPoint = point;
		}
		else if (point > secondMaxPoint)
		{
			secondMaxPoint = point;
		}
	}

	if (firstMinPoint > secondMinPoint && firstMinPoint < secondMaxPoint)
	{
		collision = true;
	}
	else if (secondMinPoint > firstMinPoint && secondMinPoint < firstMaxPoint)
	{
		collision = true;
	}

	return collision;
}

static bool SATVectorCheck(Vec3 &axis, std::vector<Vec3> *firstObjectCorners, std::vector<Vec3> *secondObjectCorners)
{
	bool collision = false;
	float firstMinPoint = axis.Dot(firstObjectCorners->at(0));
	float firstMaxPoint = firstMinPoint;

	unsigned int size = firstObjectCorners->size();
	for (unsigned int i = 0; i < size; i++)
	{
		float point = axis.Dot(firstObjectCorners->at(i));

		if (point < firstMinPoint)
		{
			firstMinPoint = point;
		}
		else if (point > firstMaxPoint)
		{
			firstMaxPoint = point;
		}
	}

	float secondMinPoint = axis.Dot(secondObjectCorners->at(0));
	float secondMaxPoint = secondMinPoint;

	size = secondObjectCorners->size();
	for (unsigned int i = 0; i < size; i++)
	{
		float point = axis.Dot(secondObjectCorners->at(i));

		if (point < secondMinPoint)
		{
			secondMinPoint = point;
		}
		else if (point > secondMaxPoint)
		{
			secondMaxPoint = point;
		}
	}

	if (firstMinPoint > secondMinPoint && firstMinPoint < secondMaxPoint)
	{
		collision = true;
	}
	else if (secondMinPoint > firstMinPoint && secondMinPoint < firstMaxPoint)
	{
		collision = true;
	}

	return collision;
}





//Finds the cornerpoints in a shape
static std::vector<Vec2>* FindSquareCorners(Square &square)
{
	std::vector<Vec2>* squarePoints = new std::vector<Vec2>;
	squarePoints->reserve(4);
	squarePoints->push_back(square._minPos);
	squarePoints->push_back(Vec2(square._maxPos._x, square._minPos._y));
	squarePoints->push_back(Vec2(square._minPos._x, square._maxPos._y));
	squarePoints->push_back(square._maxPos);

	return squarePoints;
}

static std::vector<Vec3>* FindBoxCorners(Box &box)
{
	std::vector<Vec3>* corners = new std::vector<Vec3>();
	corners->reserve(8);

	corners->push_back(box._position
		+ (box._xSlab._normal * box._xSlab._offset)
		+ (box._ySlab._normal * box._ySlab._offset)
		+ (box._zSlab._normal * box._zSlab._offset));

	corners->push_back(box._position
		- (box._xSlab._normal * box._xSlab._offset)
		+ (box._ySlab._normal * box._ySlab._offset)
		+ (box._zSlab._normal * box._zSlab._offset));

	corners->push_back(box._position
		+ (box._xSlab._normal * box._xSlab._offset)
		- (box._ySlab._normal * box._ySlab._offset)
		+ (box._zSlab._normal * box._zSlab._offset));

	corners->push_back(box._position
		+ (box._xSlab._normal * box._xSlab._offset)
		+ (box._ySlab._normal * box._ySlab._offset)
		- (box._zSlab._normal * box._zSlab._offset));

	corners->push_back(box._position
		- (box._xSlab._normal * box._xSlab._offset)
		- (box._ySlab._normal * box._ySlab._offset)
		+ (box._zSlab._normal * box._zSlab._offset));

	corners->push_back(box._position
		- (box._xSlab._normal * box._xSlab._offset)
		+ (box._ySlab._normal * box._ySlab._offset)
		- (box._zSlab._normal * box._zSlab._offset));

	corners->push_back(box._position
		+ (box._xSlab._normal * box._xSlab._offset)
		- (box._ySlab._normal * box._ySlab._offset)
		- (box._zSlab._normal * box._zSlab._offset));

	corners->push_back(box._position
		- (box._xSlab._normal * box._xSlab._offset)
		- (box._ySlab._normal * box._ySlab._offset)
		- (box._zSlab._normal * box._zSlab._offset));

	return corners;
}



static const bool Collision(Vec2 &point, Square &square)
{
	return (point._x < square._maxPos._x &&
		point._x > square._minPos._x &&
		point._y < square._maxPos._y &&
		point._y > square._minPos._y);
}

static const bool Collision(Vec2 &point, Triangle &triangle)
{
	Vec2 p0, p1, p2;

	p0 = Vec2(triangle._pos1._x, triangle._pos1._z);
	p1 = Vec2(triangle._pos2._x, triangle._pos2._z);
	p2 = Vec2(triangle._pos3._x, triangle._pos3._z);

	float area = 1 / 2 * (-p1._y*p2._x + p0._y*(-p1._x + p2._x) + p0._x*(p1._y - p2._y) + p1._x*p2._y);

	float s = 1 / (2 * area)*(p0._y*p2._x - p0._x*p2._y + (p2._y - p0._y)* point._x + (p0._x - p2._x)*point._y);
	float t = 1 / (2 * area)*(p0._x*p1._y - p0._y*p1._x + (p0._y - p1._y)*point._x + (p1._x - p0._x)*point._y);

	return s > 0 && t > 0 && 1 - s - t > 0;
}

static const bool Collision(Vec2 &point, Circle &circle)
{
	return (point - circle._position).Length() < circle._radius;
}

static const bool Collision(Triangle &triangle, Square &square)
{
	bool collision = true;

	std::vector<Vec2>* trianglePoints = new std::vector<Vec2>();
	std::vector<Vec2>* squarePoints = FindSquareCorners(square);

	trianglePoints->push_back(Vec2(triangle._pos1._x, triangle._pos1._z));
	trianglePoints->push_back(Vec2(triangle._pos2._x, triangle._pos2._z));
	trianglePoints->push_back(Vec2(triangle._pos3._x, triangle._pos3._z));

	Vec2 vec1 = trianglePoints->at(0) - trianglePoints->at(1);
	Vec2 vec2 = trianglePoints->at(0) - trianglePoints->at(2);
	Vec2 vec3 = trianglePoints->at(1) - trianglePoints->at(2);


	if (!SATVectorCheck(Vec2(-vec1._y, vec1._x).Normalize(), trianglePoints, squarePoints))
	{
		collision = false;
	}

	else if (!SATVectorCheck(Vec2(-vec2._y, vec2._x).Normalize(), trianglePoints, squarePoints))
	{
		collision = false;
	}

	else if (!SATVectorCheck(Vec2(-vec3._y, vec3._x).Normalize(), trianglePoints, squarePoints))
	{
		collision = false;
	}

	else if (!SATVectorCheck(Vec2(1.0f, 0.0f).Normalize(), trianglePoints, squarePoints))
	{
		collision = false;
	}

	else if (!SATVectorCheck(Vec2(0.0f, 1.0f).Normalize(), trianglePoints, squarePoints))
	{
		collision = false;
	}

	delete squarePoints;
	delete trianglePoints;

	return collision;
}

static const bool Collision(Circle &circle, Square &square)
{
	bool collision = false;
	//Hit if any of the corners are in the circle
	if (Collision(square._maxPos, circle))
	{
		collision = true;
	}
	else if (Collision(square._minPos, circle))
	{
		collision = true;
	}
	else if (Collision(Vec2(square._maxPos._x, square._minPos._y), circle))
	{
		collision = true;
	}
	else if (Collision(Vec2(square._minPos._x, square._maxPos._y), circle))
	{
		collision = true;
	}
	//if the circle is inside the square and don't touch any corners
	else if ((circle._position._x + circle._radius<square._maxPos._x || circle._position._x - circle._radius > square._minPos._x) &&
		(circle._position._y + circle._radius<square._maxPos._y || circle._position._y - circle._radius > square._minPos._y))
	{
		collision = true;
	}

	return collision;
}

static const bool Collision(Square &square1, Square &square2)
{
	bool collision = true;

	std::vector<Vec2>* squarePoints1 = FindSquareCorners(square1);
	std::vector<Vec2>* squarePoints2 = FindSquareCorners(square2);


	if (!SATVectorCheck(Vec2(1.0f, 0.0f), squarePoints1, squarePoints2))
	{
		collision = false;
	}

	else if (!SATVectorCheck(Vec2(0.0f, 1.0f), squarePoints1, squarePoints2))
	{
		collision = false;
	}

	delete squarePoints1;
	delete squarePoints2;
	return collision;
}

static const bool Collision(Square &square, std::vector<Vec2>* polygon)
{
	bool collision = true;
	std::vector<Vec2>* squarePoints = FindSquareCorners(square);


	if (!SATVectorCheck(Vec2(1.0f, 0.0f).Normalize(), polygon, squarePoints))
	{
		collision = false;
	}

	else if (!SATVectorCheck(Vec2(0.0f, 1.0f).Normalize(), polygon, squarePoints))
	{
		collision = false;
	}

	else
	{
		unsigned int size = polygon->size();
		for (unsigned int i = 1; i < size; i++)
		{
			Vec2 vec = (polygon->at(i) - polygon->at(i - 1)).Normalize();
			vec = Vec2(-vec._y, vec._x);

			if (!SATVectorCheck(vec, polygon, squarePoints))
			{
				collision = false;
			}
		}
	}

	delete squarePoints;
	return collision;
}

static const bool Collision(std::vector<Vec2>* polygon1, std::vector<Vec2>* polygon2)
{
	bool collision = true;

	unsigned int size = polygon1->size();
	for (unsigned int i = 1; i < size && collision; i++)
	{
		Vec2 vec = (polygon1->at(i) - polygon1->at(i - 1)).Normalize();
		vec = Vec2(-vec._y, vec._x);

		if (!SATVectorCheck(vec, polygon1, polygon2))
		{
			collision = false;
		}
	}

	size = polygon2->size();
	for (unsigned int i = 1; i < size && collision; i++)
	{
		Vec2 vec = (polygon2->at(i) - polygon2->at(i - 1)).Normalize();
		vec = Vec2(-vec._y, vec._x);

		if (!SATVectorCheck(vec, polygon1, polygon2))
		{
			collision = false;
		}
	}

	return collision;
}


static const bool Collision(Vec3 &point, Square &square)
{
	return (point._x < square._maxPos._x &&
		point._x > square._minPos._x &&
		point._z < square._maxPos._y &&
		point._z > square._minPos._y);
}

static const bool Collision(Vec3 &point, Sphere &sphere)
{
	return (point - sphere._position).Length() < sphere._radius;
}

static const bool Collision(Ray &ray, Plane &plane)
{
	return ray._direction.Dot(plane._normal) != 0;
}

static const bool Collision(Ray &ray, Sphere &sphere)
{
	Vec3 objectVector = sphere._position - ray._origin;
	Vec3 projectedPoint = FindClosestPointOnVector(sphere._position, ray);
	float length = (objectVector - projectedPoint).Length();

	return (length <= sphere._radius);
}

static const bool Collision(Ray &ray, Box &box)
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

static const bool Collision(Box &box1, Box &box2)
{
	bool collision = true;
	std::vector<Vec3>* firstBoxCorners = FindBoxCorners(box1);
	std::vector<Vec3>* secondBoxCorners = FindBoxCorners(box2);


	if (!SATVectorCheck(box1._xSlab._normal, firstBoxCorners, secondBoxCorners))
	{
		collision = false;
	}
	else if (!SATVectorCheck(box1._ySlab._normal, firstBoxCorners, secondBoxCorners))
	{
		collision = false;
	}
	else if (!SATVectorCheck(box1._zSlab._normal, firstBoxCorners, secondBoxCorners))
	{
		collision = false;
	}
	else if (!SATVectorCheck(box2._xSlab._normal, firstBoxCorners, secondBoxCorners))
	{
		collision = false;
	}
	else if (!SATVectorCheck(box2._ySlab._normal, firstBoxCorners, secondBoxCorners))
	{
		collision = false;
	}
	else if (!SATVectorCheck(box2._zSlab._normal, firstBoxCorners, secondBoxCorners))
	{
		collision = false;
	}

	return collision;
}


static const bool Collision(Box &box, Square &square)
{
	bool collision = true;
	std::vector<Vec3>* boxCorners = FindBoxCorners(box);
	std::vector<Vec3> squareCorners;

	squareCorners.push_back(Vec3(square._minPos._x, 1.0f, square._minPos._y));
	squareCorners.push_back(Vec3(square._maxPos._x, 1.0f, square._minPos._y));
	squareCorners.push_back(Vec3(square._minPos._x, 1.0f, square._maxPos._y));
	squareCorners.push_back(Vec3(square._maxPos._x, 1.0f, square._maxPos._y));

	if (!SATVectorCheck(box._xSlab._normal, &squareCorners, boxCorners))
	{
		collision = false;
	}

	else if (!SATVectorCheck(box._zSlab._normal, &squareCorners, boxCorners))
	{
		collision = false;
	}
	else if (!SATVectorCheck(Vec3(1.0f, 0.0f, 0.0f), &squareCorners, boxCorners))
	{
		collision = false;
	}

	else if (!SATVectorCheck(Vec3(0.0f, 0.0f, 1.0f), &squareCorners, boxCorners))
	{
		collision = false;
	}

	return collision;

}


