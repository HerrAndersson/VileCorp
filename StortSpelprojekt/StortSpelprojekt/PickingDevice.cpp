#include "PickingDevice.h"




Ray PickingDevice::calculatePickRay(POINT mousePoint)
{
	XMFLOAT3 mouseViewPos;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projMatrix;
	XMStoreFloat4x4(&projMatrix, *_camera->GetProjectionMatrix());

	//Translating mouseposition to viewSpace
	float width = (float)_window->GetWindowSettings()._width;
	float xPos = (float)mousePoint.x;


	float height = (float)_window->GetWindowSettings()._height;
	float yPos = (float)mousePoint.y;


	mouseViewPos.x = (((2 * xPos) / width)- 1.0f) / projMatrix._11;
	mouseViewPos.y = (((-2 * yPos) / height) + 1.0f) / projMatrix._22;
	mouseViewPos.z = 1.0f;


	XMVECTOR determinant;
	XMMATRIX inverseViewMatrix = XMMatrixInverse(&determinant, *_camera->GetViewMatrix());
	XMStoreFloat3(&mouseViewPos, XMVector3TransformCoord(XMVectorSet(mouseViewPos.x, mouseViewPos.y, mouseViewPos.z, 0.0f), inverseViewMatrix));

	Ray ray = Ray(Vec3(_camera->GetPosition()), (Vec3(mouseViewPos) - Vec3(_camera->GetPosition())));

	return ray;
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
}

PickingDevice::~PickingDevice()
{
	_camera = nullptr;
	_window = nullptr;
}

AI::Vec2D PickingDevice::pickTile(POINT mousePoint)
{
	Plane pickPlane = Plane(Vec3(0.0f, 1.0f, 0.0f), 0.0f);
	Vec3 pickedPoint;

	Ray ray = calculatePickRay(mousePoint);
	if (Collision(ray, pickPlane))
	{
		pickedPoint = Intersection(ray, pickPlane);
	}
	
	return AI::Vec2D((int)(pickedPoint._x + 0.5f), (int)(pickedPoint._z + 0.5f));
}



vector<GameObject*> PickingDevice::pickObjects(POINT mousePoint, vector<GameObject*> pickableObjects)
{
	vector<GameObject*> pickedObjects;


	Ray ray = calculatePickRay(mousePoint);

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


vector<GameObject*> PickingDevice::pickTilemap(POINT mousePoint, Tilemap* tilemap)
{
	vector<GameObject*> pickedObjects;

	AI::Vec2D tileIndex = pickTile(mousePoint);

	if (tileIndex._x >= 0 && tileIndex._x < tilemap->GetWidth() &&
		tileIndex._y >= 0 && tileIndex._y < tilemap->GetHeight())
	{
		GameObject* object = nullptr;

		for (int i = 0; i < 5; i++)
		{
			object = tilemap->GetObjectOnTile(tileIndex._x, tileIndex._y, i);

			if (object != nullptr)
			{
				pickedObjects.push_back(object);
			}
		}
	}


	return pickedObjects;
}
