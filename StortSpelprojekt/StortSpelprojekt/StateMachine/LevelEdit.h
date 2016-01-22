#pragma once

#include <cstdio>
#include <ShlObj.h>

#include "../ObjectHandler.h"
#include "../System/InputDevice.h"
#include "../Grid.h"
#include "AStar.h"
#include "../Controls.h"
#include "../PickingDevice.h"
#include "../System/Camera.h"

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
	Grid*					_grid;

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
	int someOneActive = false;
	std::vector<bool> _rootInfo;
	int _rootAmount;
	bool _root = true;
	int _floorAmount;
	bool _floor = false;
	int _wallAmount;
	bool _wall = false;

	XMFLOAT3 _lastObjPosition = XMFLOAT3(0.0, 0.0, 0.0);
	GameObject* _selectedObj;
	GameObject* _lastSelected;
	System::MouseCoord _mouseCoord;
	int _tileMultiplier;
	int _tilemapHeight, _tilemapWidth;
	bool _isSelectionMode = true;
	std::vector<ButtonInfo> buttonInfo;

	void HandleInput();
	void HandleHUD();
	void LoadLevel(int levelID);
	void InitNewLevel();
	void ResetSelectedObj();
	void HandleSelected();

	void ExportLevel();
	void SelectObject(GameObject* selectedObject);

public:
	LevelEdit();
	~LevelEdit();

	GameObject* GetSelectedObject();
	void Add(Type type, int renderObjectID);
	void Initialize(ObjectHandler* objectHandler, System::InputDevice* inputDevice, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera);
	void Update(float deltaTime);
};