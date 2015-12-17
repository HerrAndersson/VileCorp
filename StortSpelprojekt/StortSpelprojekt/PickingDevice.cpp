#include "PickingDevice.h"




Ray PickingDevice::calculatePickRay(POINT mousePoint)
{
	//TODO maek work
	XMFLOAT3 mouseWorldPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projMatrix;
	XMStoreFloat4x4(&projMatrix, *_camera->GetProjectionMatrix());

	//Translating mouseposition to viewSpace
	float width = (float)_window->GetWindowSettings()._width;
	float xPos = (float)mousePoint.x / width;
	xPos = (xPos * 2.0f) - 1.0f;

	float height = (float)_window->GetWindowSettings()._height;
	float yPos = (float)mousePoint.y / height;
	yPos = (yPos * -2.0f) + 1.0f;

	mouseWorldPos.x = xPos / projMatrix._11;
	mouseWorldPos.y = yPos / projMatrix._22;


	XMMATRIX inverseViewMatrix = XMMatrixInverse(nullptr, *_camera->GetViewMatrix());
	XMStoreFloat3(&mouseWorldPos, XMVector3TransformCoord(XMVectorSet(mouseWorldPos.x, mouseWorldPos.y, mouseWorldPos.z, 1.0f), inverseViewMatrix));

	//Ray ray = Ray(Vec3(_camera->GetPosition()), (Vec3(mouseWorldPos)) - Vec3(_camera->GetPosition()));
	Ray ray = Ray(Vec3(_camera->GetPosition()), Vec3(_camera->GetForwardVector()));


	return ray;
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
		Sphere pickObject = Sphere(Vec3(pickableObjects[i]->GetPosition()), 2.0f);
		//Box pickObject = Box(1.0f, 3.0f, 1.0f, Vec3(pickableObjects[i]->GetPosition()));


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
	
	return pickedObjects;
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
