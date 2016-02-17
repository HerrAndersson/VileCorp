#include "PickingDevice.h"

PickingDevice::PickingDevice(System::Camera* camera, System::Settings* settings)
{
	_pickPlane = Plane(Vec3(), Vec3(0.0f, 1.0f, 0.0f), 0.0f);
	_camera = camera;
	_settings = settings;
	_firstBoxPoint = POINT();
}

PickingDevice::~PickingDevice()
{
}

Ray PickingDevice::CalculatePickRay(long x, long y)
{
	XMFLOAT3 mouseViewPos;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projMatrix;
	XMStoreFloat4x4(&projMatrix, *_camera->GetProjectionMatrix());

	//Translating mouseposition to viewSpace
	float width = (float)_settings->_screenWidth;
	float xPos = (float)x;

	float height = (float)_settings->_screenHeight;
	float yPos = (float)y;

	mouseViewPos.x = (((2 * xPos) / width) - 1.0f) / projMatrix._11;
	mouseViewPos.y = (((-2 * yPos) / height) + 1.0f) / projMatrix._22;
	mouseViewPos.z = 1.0f;

	XMVECTOR determinant;
	XMMATRIX inverseViewMatrix = XMMatrixInverse(&determinant, *_camera->GetViewMatrix());
	XMStoreFloat3(&mouseViewPos, XMVector3TransformCoord(XMVectorSet(mouseViewPos.x, mouseViewPos.y, mouseViewPos.z, 0.0f), inverseViewMatrix));

	Ray ray = Ray(Vec3(_camera->GetPosition()), (Vec3(mouseViewPos) - Vec3(_camera->GetPosition())));

	return ray;
}

std::vector<Vec2> PickingDevice::CreatePickBox(Vec3 points[4])
{
	std::vector<Vec2> pickBox;
	pickBox.reserve(4);
	for (int i = 0; i < 4; i++)
	{
		pickBox.push_back(Vec2(points[i]._x, points[i]._z));
	}

	return pickBox;
}

vector<GameObject*> PickingDevice::SortByDistance(vector<GameObject*> pickedObjects)
{
	vector<GameObject*> sortedObjects;

	for (unsigned int i = 0; i < pickedObjects.size(); i++)
	{
		int shortestIndex = -1;
		float shortestDistance = FLT_MAX;

		for (unsigned int j = 0; j < pickedObjects.size(); j++)
		{
			float objectToCameraLength = (Vec3(pickedObjects[j]->GetPosition()) - Vec3(_camera->GetPosition())).Length();

			if (objectToCameraLength < shortestDistance)
			{
				shortestIndex = j;
				shortestDistance = objectToCameraLength;
			}
		}
		sortedObjects.push_back(pickedObjects[shortestIndex]);
		pickedObjects.erase(pickedObjects.begin() + shortestIndex);
	}

	return sortedObjects;
}

vector<GameObject*> PickingDevice::SinglePickObjects(POINT mousePoint, vector<GameObject*> pickableObjects)
{
	vector<GameObject*> pickedObjects;

	Ray ray = CalculatePickRay(mousePoint.x, mousePoint.y);

	for (unsigned int i = 0; i < pickableObjects.size(); i++)
	{
		//Sphere pickObject = Sphere(Vec3(pickableObjects[i]->GetPosition()), 2.0f);
		Box pickObject;

		Hitbox* hitbox = pickableObjects.at(i)->GetRenderObject()->_mesh._hitbox;
		if (hitbox != nullptr)
		{
			pickObject = Box(hitbox->_depth, hitbox->_height, hitbox->_width, Vec3(hitbox->_center[0], hitbox->_center[1], hitbox->_center[2]), pickableObjects[i]->GetRotation());
		}
		else
		{
			pickObject = Box(0.8f, 1.8f, 0.8f, Vec3(pickableObjects[i]->GetPosition()) + Vec3(0.0f, 1.0f, 0.0f));
		}

		if (Collision(ray, pickObject))
		{
			pickedObjects.push_back(pickableObjects[i]);
		}
	}

	for (unsigned int i = 0; i < pickableObjects.size(); i++)
	{
		if (pickableObjects[i]->GetTilePosition() == PickTile(mousePoint))
		{
			pickedObjects.push_back(pickableObjects[i]);
		}
	}

	return SortByDistance(pickedObjects);
}
vector<GameObject*> PickingDevice::BoxPickObjects(POINT mousePoint, vector<GameObject*> pickableObjects)
{
	vector<GameObject*> pickedObjects;


	Ray rays[4] = { CalculatePickRay(_firstBoxPoint.x, _firstBoxPoint.y),
		CalculatePickRay(mousePoint.x, _firstBoxPoint.y),
		CalculatePickRay(mousePoint.x, mousePoint.y),
		CalculatePickRay(_firstBoxPoint.x, mousePoint.y)
	};

	Vec3 points[4];

	bool legitForReal = true;

	for (int i = 0; i < 4; i++)
	{
		if (Collision(rays[i], _pickPlane))
		{
			points[i] = Intersection(rays[i], _pickPlane);
		}
		else
		{
			legitForReal = false;
		}
	}

	if (legitForReal)
	{
		std::vector<Vec2> point;
		std::vector<Vec2> box = CreatePickBox(points);
		for (unsigned int i = 0; i < pickableObjects.size(); i++)
		{
			Vec3 pos = Vec3(pickableObjects[i]->GetPosition());
			point.push_back(Vec2(pos._x, pos._z));
			if (Collision(&point, &box))
			{
				pickedObjects.push_back(pickableObjects[i]);
			}
			point.clear();
		}
	}

	return pickedObjects;
}


AI::Vec2D PickingDevice::PickDirection(POINT mousePoint, Tilemap* tilemap)
{
	AI::Vec2D returnValue = AI::Vec2D(0, 0);
	//Get picked tile
	AI::Vec2D pickedTile = PickTile(mousePoint);
	//Get all objects on that tile. Dont care of the type, so just take the first.
	vector<GameObject*> object = tilemap->GetAllObjectsOnTile(pickedTile);
	//Actually click something
	if (object.size() > 0)
	{

		//Get tile actual position as origin
		DirectX::XMFLOAT3 origin = object.at(0)->GetPosition();
		Vec2 v2origin = Vec2(origin.x, origin.z);
		//compare it to ray point as point
		DirectX::XMFLOAT3 point = PickPoint(mousePoint);
		Vec2 v2Point = Vec2(point.x - origin.x, point.z - origin.z);

		//We want to create 4 vectors that represent the directions we are taking
		Vec2 vectors[4];
		//left side
		vectors[0] = Vec2(-1, 1);
		vectors[1] = Vec2(-1, 0);
		vectors[2] = Vec2(-1, -1);
		//bottom center
		vectors[3] = Vec2(0, -1);

		Vec2 nearestPoint[4];
		float tempLength;
		float nearestLength = FLT_MAX;
		int nearestVector;
		float x, y;


		for (int i = 0; i < 4; i++)
		{
			nearestPoint[i] = FindClosestPointOnVector(v2Point, vectors[i]);
			x = (nearestPoint[i]._x - v2Point._x);
			y = (nearestPoint[i]._y - v2Point._y);
			tempLength = sqrt(x*x + y*y);
			if (tempLength < nearestLength)
			{
				nearestLength = tempLength;
				nearestVector = i;
			}
		}

		//if horizontal/vertical lines
		if ((nearestPoint[nearestVector]._x < 0 && nearestPoint[nearestVector]._y == 0))
		{
			returnValue = AI::Vec2D(-1, 0);
		}
		else if ((nearestPoint[nearestVector]._x > 0 && nearestPoint[nearestVector]._y == 0))
		{
			returnValue = AI::Vec2D(1, 0);
		}
		else if ((nearestPoint[nearestVector]._x == 0 && nearestPoint[nearestVector]._y < 0))
		{
			returnValue = AI::Vec2D(0, -1);
		}
		else if ((nearestPoint[nearestVector]._x == 0 && nearestPoint[nearestVector]._y > 0))
		{
			returnValue = AI::Vec2D(0, 1);
		}
		//if diagonal lines
		else if ((nearestPoint[nearestVector]._x < 0 && nearestPoint[nearestVector]._y < 0))
		{
			returnValue = AI::Vec2D(-1, -1);
		}
		else if ((nearestPoint[nearestVector]._x > 0 && nearestPoint[nearestVector]._y > 0))
		{
			returnValue = AI::Vec2D(1, 1);
		}
		else if ((nearestPoint[nearestVector]._x > 0 && nearestPoint[nearestVector]._y < 0))
		{
			returnValue = AI::Vec2D(1, -1);
		}
		else if ((nearestPoint[nearestVector]._x < 0 && nearestPoint[nearestVector]._y > 0))
		{
			returnValue = AI::Vec2D(-1, 1);
		}
	}

	return returnValue;
}
AI::Vec2D PickingDevice::PickTile(POINT mousePoint)
{
	Vec3 pickedPoint;

	Ray ray = CalculatePickRay(mousePoint.x, mousePoint.y);
	if (Collision(ray, _pickPlane))
	{
		pickedPoint = Intersection(ray, _pickPlane);
	}

	return AI::Vec2D((int)(pickedPoint._x + 0.5f), (int)(pickedPoint._z + 0.5f));
}
XMFLOAT3 PickingDevice::PickPoint(POINT mousePoint)
{
	Plane pickPlane = Plane(Vec3(), Vec3(0.0f, 1.0f, 0.0f), 0.0f);
	Vec3 pickedPoint;

	Ray ray = CalculatePickRay(mousePoint.x, mousePoint.y);
	if (Collision(ray, pickPlane))
	{
		pickedPoint = Intersection(ray, pickPlane);
	}

	return pickedPoint.convertToXMFLOAT();
}
vector<GameObject*> PickingDevice::PickObjects(POINT mousePoint, vector<GameObject*> pickableObjects)
{
	vector<GameObject*> pickedObjects;

	if ((_firstBoxPoint.x == mousePoint.x) && (_firstBoxPoint.y == mousePoint.y))
	{
		pickedObjects = SinglePickObjects(mousePoint, pickableObjects);
	}
	else
	{
		pickedObjects = BoxPickObjects(mousePoint, pickableObjects);
	}

	return pickedObjects;
}
vector<GameObject*> PickingDevice::PickTilemap(POINT mousePoint, Tilemap* tilemap)
{
	return tilemap->GetAllObjectsOnTile(PickTile(mousePoint));
}

void PickingDevice::SetFirstBoxPoint(POINT mousePoint)
{
	_firstBoxPoint = mousePoint;
}
