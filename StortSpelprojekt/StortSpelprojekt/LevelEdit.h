#pragma once

#include <cstdio>
#include <ShlObj.h>
#include "UIHandler.h"
#include "ObjectHandler.h"
#include "InputDevice.h"
#include "AStar.h"
#include "Controls.h"
#include "PickingDevice.h"

class LevelEdit
{
private:
	AI::AStar*				_aStar;
	Tilemap*				_tileMap;
	ObjectHandler*			_objectHandler;
	System::InputDevice*	_inputDevice;
	System::Controls*		_controls;
	PickingDevice*			_pickingDevice;
	System::Camera*			_camera;
	UIHandler*				_uiHandler;

	struct LevelHeader
	{
		int _version;
		int _levelSizeX;
		int _levelSizeY;
		int _nrOfGameObjects;
	};

	struct MapData
	{
		int _posX;
		int _posZ;
		float _rotY;
		int _tileType;
	};

	struct ButtonInfo
	{
		int parent;
		bool active = false;
	};

	XMFLOAT3 _lastObjPosition = XMFLOAT3(0.0, 0.0, 0.0);
	GameObject* _selectedObj;
	GameObject* _lastSelected;
	System::MouseCoord mouseCoord;
	int _tileMultiplier;
	int _tilemapHeight, _tilemapWidth;
	bool _isSelectionMode = true;
	std::vector<ButtonInfo> buttonInfo;

	void HandleInput();
	void HandleHUD();
	void LoadLevel(int levelID);
	void InitNewLevel();
	void ResetSelectedObj();
	void DeleteObject();

	void ExportLevel();
	void SelectObject(GameObject* selectedObject);
	void MoveSelectedObject();

public:
	LevelEdit();
	~LevelEdit();

	void Update(float deltaTime);
	void Initialize(ObjectHandler* objectHandler, 
		System::InputDevice* inputDevice, 
		System::Controls* controls, 
		PickingDevice* pickingDevice, 
		System::Camera* camera, 
		UIHandler* uiHandler);
};