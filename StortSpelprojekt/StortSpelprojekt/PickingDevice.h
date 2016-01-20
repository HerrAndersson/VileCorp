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

	POINT _firstBoxPoint;

	Ray calculatePickRay(long x, long y);
	Box createPickBox(Vec3 points[4]);
	vector<GameObject*> sortByDistance(vector<GameObject*> pickedObjects);


public:
	PickingDevice(System::Camera* camera = nullptr, System::Window* window = nullptr);
	~PickingDevice();

	//This function only works if the level is aligned worldspace, must check x&y>=0 && <height&width /Markus
	AI::Vec2D pickTile(POINT mousePoint);

	vector<GameObject*> pickObjects(POINT mousePoint, vector<GameObject*> pickableObjects);
	vector<GameObject*> boxPickObjects(POINT mousePoint, vector<GameObject*> pickableObjects);
	vector<GameObject*> pickTilemap(POINT mousePoint, Tilemap* tilemap);

	//Used to save the first mouseposition when boxselecting
	void setFirstBoxPoint(POINT mousePoint);
};

