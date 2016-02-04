#pragma once

#include <vector>
#include "Camera.h"
#include "InputDevice.h"
#include "Window.h"
#include "GameObject.h"
#include "Tilemap.h"
#include "VectorMath.h"
#include "Shapes.h"

using namespace std;
using namespace DirectX;



class PickingDevice
{
private:
	System::Camera* _camera;
	System::Window* _window;

	

	Ray CalculatePickRay(long x, long y);
	std::vector<Vec2> CreatePickBox(Vec3 points[4]);
	vector<GameObject*> SortByDistance(vector<GameObject*> pickedObjects);


public:
	PickingDevice(System::Camera* camera = nullptr, System::Window* window = nullptr);
	~PickingDevice();
	POINT _firstBoxPoint;
	//This function only works if the level is aligned worldspace, must check x&y>=0 && <height&width /Markus
	AI::Vec2D PickTile(POINT mousePoint);

	XMFLOAT3 PickPoint(POINT mousePoint);
	vector<GameObject*> PickObjects(POINT mousePoint, vector<GameObject*> pickableObjects);
	vector<GameObject*> BoxPickObjects(POINT mousePoint, vector<GameObject*> pickableObjects);
	vector<GameObject*> PickTilemap(POINT mousePoint, Tilemap* tilemap);

	//Used to save the first mouseposition when boxselecting
	void SetFirstBoxPoint(POINT mousePoint);
};

