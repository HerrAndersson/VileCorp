#pragma once

#define SYSTEM_EXPORT __declspec(dllexport)

#include "VectorMath.h"

namespace System
{
	struct Point
	{
		Vec3 _position;

		Point(Vec3 position = Vec3())
		{
			_position = position;
		}
	};

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
		Cylinder(Vec3 position, Vec3 direction, float radius, float height)
		{
			_position = Vec3();
			_direction = Vec3();
			_radius = 0.0f;
			_height = 0.0f;
		}
	};

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
			_point1 = Vec3(position._x - (height*0.5), position._y, position._z - (width*0.5));
			_point2 = Vec3(position._x + (height*0.5), position._y, position._z + (width*0.5));
		}
	};

	struct Box
	{
		Plane _backFace;
		Plane _frontFace;
		Plane _topFace;
		Plane _bottomFace;
		Plane _rightFace;
		Plane _leftFace;

		Box(float xLength, float yLength, float zLength, Vec3 position = Vec3(), Vec3 direction = Vec3(0.0f, 0.0f, 1.0f))
		{
			_backFace = Plane(direction * -1.0f, position._z);
			_frontFace = Plane(direction, position._z + (zLength));

			_topFace = Plane(Vec3(0.0f, 1.0f, 0.0f), position._y + (yLength));
			_bottomFace = Plane(Vec3(0.0f, -1.0f, 0.0f), position._y);

			_rightFace = Plane(direction.Cross(_bottomFace._normal), position._x);
			_leftFace = Plane(direction.Cross(_topFace._normal), position._x + (xLength));
		}

		Box(Plane backFace, Plane frontFace, Plane topFace, Plane bottomFace, Plane rightFace, Plane leftFace)
		{
			_backFace = backFace;
			_frontFace = frontFace;
			_topFace = topFace;
			_bottomFace = bottomFace;
			_rightFace = rightFace;
			_leftFace = leftFace;
		}
	};

	//No exception for /0, always use collision() first
	static Vec3 Intersection(Ray ray, Plane plane)
	{
		float t = -(ray._origin.Dot(plane._normal) + plane._offset) / ray._direction.Dot(plane._normal);
		return ray._origin + ray._direction * t;
	}

	static	bool Collision(Ray ray, Plane plane)
	{
		return ray._direction.Dot(plane._normal) != 0;
	}

	static	bool Collision(Ray ray, Sphere sphere)
	{
		bool collision = false;


		Vec3 objectVector =  sphere._position - ray._origin;

		float dotProduct = objectVector.Dot(ray._direction);
		//float dotProduct = ray._direction.Dot(objectVector);
		float denominator = ray._direction.Dot(ray._direction);
		//float denominator = objectVector.Dot(objectVector);
		//float denominator = objectVector.Length() * objectVector.Length();

		Vec3 projectedVector = ray._direction * (dotProduct / denominator);
		//Vec3 projectedVector = objectVector * (dotProduct / denominator);

		float length = (objectVector - projectedVector).Length();
		if (length <= sphere._radius)
		{
			collision = true;
		}



		//Vec3 coefficient = Vec3();
		//float discriminant = 0.0f;

		//coefficient._x = (ray._direction._x * ray._direction._x) + (ray._direction._y * ray._direction._y) + (ray._direction._z * ray._direction._z);
		//coefficient._y = ((ray._direction._x * ray._origin._x) + (ray._direction._y * ray._origin._y) + (ray._direction._z * ray._origin._z)) * 2.0f;
		//coefficient._z = ((ray._origin._x * ray._origin._x) + (ray._origin._y * ray._origin._y) + (ray._origin._z * ray._origin._z)) - (sphere._radius * sphere._radius);
		//discriminant = (coefficient._y * coefficient._y) - (4.0f * coefficient._x * coefficient._z);

		//if (discriminant >= 0.0f)
		//{
		//	collision = true;
		//}

		return collision;
	}

	static	bool Collision(Ray ray, Cylinder cylinder)
	{
		bool collision = false;
		Vec3 objectVector = cylinder._position - ray._origin;
		float dotProduct = objectVector.Dot(ray._direction);
		float denominator = ray._direction.Dot( ray._direction);
		Vec3 projectedVector = ray._direction * (dotProduct / denominator);

		//Stuff

		return collision;
	}

	static	bool Collision(Ray ray, Box box)
	{
		Vec3 tMax = Vec3();
		Vec3 tMin = Vec3();

		float greatestTMin = FLT_MIN;
		float smallestTMax = FLT_MAX;

		//Front to Back
		if (Collision(ray, box._backFace))
		{
			tMax._z = Intersection(ray, box._frontFace)._z;
			tMin._z = Intersection(ray, box._backFace)._z;
			
			if (tMax._z < tMin._z)
			{
				float temp = tMax._z;
				tMax._z = tMin._z;
				tMin._z = temp;
			}
		}
		//Top to bottom
		if (Collision(ray, box._topFace))
		{
			tMax._y = Intersection(ray, box._topFace)._y;
			tMin._y = Intersection(ray, box._bottomFace)._y;

			if (tMax._y < tMin._y)
			{
				float temp = tMax._y;
				tMax._y = tMin._y;
				tMin._y = temp;
			}

		}
		//Right to left
		if (Collision(ray, box._rightFace))
		{
			tMax._x = Intersection(ray, box._rightFace)._x;
			tMin._x = Intersection(ray, box._leftFace)._x;

			if (tMax._x < tMin._x)
			{
				float temp = tMax._x;
				tMax._x = tMin._x;
				tMin._x = temp;
			}
		}
		
		if (tMin._x > greatestTMin)
		{
			greatestTMin = tMin._x;
		}
		if (tMin._y > greatestTMin)
		{
			greatestTMin = tMin._y;
		}
		if (tMin._z > greatestTMin)
		{
			greatestTMin = tMin._z;
		}
		

		if (tMax._x < smallestTMax)
		{
			smallestTMax = tMax._x;
		}
		if (tMax._y < smallestTMax)
		{
			smallestTMax = tMax._y;
		}
		if (tMax._z < smallestTMax)
		{
			smallestTMax = tMax._z;
		}

		return (greatestTMin < smallestTMax);

	}
}
