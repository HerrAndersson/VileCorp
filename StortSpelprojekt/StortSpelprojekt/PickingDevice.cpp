#include "PickingDevice.h"




Ray PickingDevice::calculatePickRay(long x, long y)
{
	XMFLOAT3 mouseViewPos;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projMatrix;
	XMStoreFloat4x4(&projMatrix, *_camera->GetProjectionMatrix());

	//Translating mouseposition to viewSpace
	float width = (float)_window->GetWindowSettings()._width;
	float xPos = (float)x;


	float height = (float)_window->GetWindowSettings()._height;
	float yPos = (float)y;


	mouseViewPos.x = (((2 * xPos) / width)- 1.0f) / projMatrix._11;
	mouseViewPos.y = (((-2 * yPos) / height) + 1.0f) / projMatrix._22;
	mouseViewPos.z = 1.0f;


	XMVECTOR determinant;
	XMMATRIX inverseViewMatrix = XMMatrixInverse(&determinant, *_camera->GetViewMatrix());
	XMStoreFloat3(&mouseViewPos, XMVector3TransformCoord(XMVectorSet(mouseViewPos.x, mouseViewPos.y, mouseViewPos.z, 0.0f), inverseViewMatrix));

	Ray ray = Ray(Vec3(_camera->GetPosition()), (Vec3(mouseViewPos) - Vec3(_camera->GetPosition())));

	return ray;
}

Box PickingDevice::createPickBox(Vec3 points[4])
{
	Box pickBox = Box(
		Vec3((points[0]._x + points[3]._x)*0.5f, (points[0]._y - points[3]._y)*0.5f, (points[0]._z - points[3]._z)*0.5f),
		std::abs(points[0]._x - points[3]._x),
		1.0f,
		std::abs(points[0]._z - points[3]._z),
		points[0]-points[3],
		Vec3(0.0f,1.0f,0.0f),
		points[3]-points[4]
		);

	return pickBox;
}

vector<GameObject*> PickingDevice::sortByDistance(vector<GameObject*> pickedObjects)
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
		pickedObjects.erase(pickedObjects.begin()+shortestIndex);
	}



	return sortedObjects;
}

PickingDevice::PickingDevice(System::Camera * camera, System::Window* window)
{
	_camera = camera;
	_window = window;
	_firstBoxPoint = POINT();
}

PickingDevice::~PickingDevice()
{
	_camera = nullptr;
	_window = nullptr;
}

AI::Vec2D PickingDevice::pickTile(POINT mousePoint)
{
	Plane pickPlane = Plane(Vec3(), Vec3(0.0f, 1.0f, 0.0f), 0.0f);
	Vec3 pickedPoint;

	Ray ray = calculatePickRay(mousePoint.x, mousePoint.y);
	if (Collision(ray, pickPlane))
	{
		pickedPoint = Intersection(ray, pickPlane);
	}
	
	return AI::Vec2D((int)(pickedPoint._x + 0.5f), (int)(pickedPoint._z + 0.5f));
}

XMFLOAT3 PickingDevice::pickPoint(POINT mousePoint)
{
	Plane pickPlane = Plane(Vec3(), Vec3(0.0f, 1.0f, 0.0f), 0.0f);
	Vec3 pickedPoint;

	Ray ray = calculatePickRay(mousePoint.x, mousePoint.y);
	if (Collision(ray, pickPlane))
	{
		pickedPoint = Intersection(ray, pickPlane);
	}

	return pickedPoint.convertToXMFLOAT();
}

vector<GameObject*> PickingDevice::pickObjects(POINT mousePoint, vector<GameObject*> pickableObjects)
{
	vector<GameObject*> pickedObjects;


	Ray ray = calculatePickRay(mousePoint.x, mousePoint.y);

	for (unsigned int i = 0; i < pickableObjects.size(); i++)
	{
		//Sphere pickObject = Sphere(Vec3(pickableObjects[i]->GetPosition()), 2.0f);
		Box pickObject = Box(1.0f, 3.0f, 1.0f, Vec3(pickableObjects[i]->GetPosition()));

		if (Collision(ray, pickObject))
		{
			pickedObjects.push_back(pickableObjects[i]);
		}
	}


	for (unsigned int i = 0; i < pickableObjects.size(); i++)
	{
		if (pickableObjects[i]->GetTilePosition() == pickTile(mousePoint))
		{
			pickedObjects.push_back(pickableObjects[i]);
		}
	}

	return sortByDistance(pickedObjects);
}

vector<GameObject*> PickingDevice::boxPickObjects(POINT mousePoint, vector<GameObject*> pickableObjects)
{
	vector<GameObject*> pickedObjects;

	Ray rays[4] = { calculatePickRay(_firstBoxPoint.x, _firstBoxPoint.y),
					calculatePickRay(mousePoint.x, _firstBoxPoint.y),
					calculatePickRay(_firstBoxPoint.x, mousePoint.y),
					calculatePickRay(mousePoint.x, mousePoint.y) };
	

	Vec3 points[4];
	Plane pickPlane = Plane(Vec3(), Vec3(0.0f, 1.0f, 0.0f), 0.0f);

	bool legitForReal = true;

	for (int i = 0; i < 4; i++)
	{
		if (Collision(rays[i], pickPlane))
		{
			points[i] = Intersection(rays[i], pickPlane);
		}
		else
		{
			legitForReal = false;
		}
	}

	if (legitForReal)
	{
		Vec3 minPoint = points[0];
		Vec3 maxPoint = points[0];

		for (int i = 0; i < 4; i++)
		{
			if (points[i]._x <= minPoint._x)
			{
				minPoint._x = points[i]._x;
			}
			if (points[i]._z <= minPoint._z)
			{
				minPoint._z = points[i]._z;
			}
			if (points[i]._x >= maxPoint._x)
			{
				maxPoint._x = points[i]._x;
			}
			if (points[i]._z >= maxPoint._z)
			{
				maxPoint._z = points[i]._z;
			}

		}

		for (unsigned int i = 0; i < pickableObjects.size(); i++)
		{
			Vec3 pos = Vec3(pickableObjects[i]->GetPosition());
			if (pos._x < maxPoint._x && pos._x > minPoint._x &&
				pos._z < maxPoint._z && pos._z > minPoint._z)
			{
				pickedObjects.push_back(pickableObjects[i]);
			}
		}
	}

	return pickedObjects;
}

vector<GameObject*> PickingDevice::pickTilemap(POINT mousePoint, Tilemap* tilemap)
{
	return tilemap->GetAllObjectsOnTile(pickTile(mousePoint));
}

void PickingDevice::setFirstBoxPoint(POINT mousePoint)
{
	_firstBoxPoint = mousePoint;
}
