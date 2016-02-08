#include "PickingDevice.h"

Ray PickingDevice::CalculatePickRay(long x, long y)
{
	XMFLOAT3 mouseViewPos;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projMatrix;
	XMStoreFloat4x4(&projMatrix, *_camera->GetProjectionMatrix());

	//Translating mouseposition to viewSpace
	float width = (float)_settings->_windowWidth;
	float xPos = (float)x;


	float height = (float)_settings->_windowHeight;
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

PickingDevice::PickingDevice(System::Camera* camera, System::Settings* settings)
{
	_camera = camera;
	_settings = settings;
	_firstBoxPoint = POINT();
}

PickingDevice::~PickingDevice()
{
}

AI::Vec2D PickingDevice::PickTile(POINT mousePoint)
{
	Plane pickPlane = Plane(Vec3(), Vec3(0.0f, 1.0f, 0.0f), 0.0f);
	Vec3 pickedPoint;

	Ray ray = CalculatePickRay(mousePoint.x, mousePoint.y);
	if (Collision(ray, pickPlane))
	{
		pickedPoint = Intersection(ray, pickPlane);
	}

	return AI::Vec2D((int)(pickedPoint._x), (int)(pickedPoint._z));
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


	Ray ray = CalculatePickRay(mousePoint.x, mousePoint.y);

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
		CalculatePickRay(_firstBoxPoint.x, mousePoint.y),
		CalculatePickRay(mousePoint.x, mousePoint.y) };


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

vector<GameObject*> PickingDevice::PickTilemap(POINT mousePoint, Tilemap* tilemap)
{
	return tilemap->GetAllObjectsOnTile(PickTile(mousePoint));
}

void PickingDevice::SetFirstBoxPoint(POINT mousePoint)
{
	_firstBoxPoint = mousePoint;
}
