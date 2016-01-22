#pragma once

#include <cstdio>
#include <ShlObj.h>
#include "../ObjectHandler.h"
#include "../Grid.h"
#include "AStar.h"
#include "../Controls.h"
#include "../PickingDevice.h"
#include "../System/Camera.h"
#include "..\StateMachine\States.h"

class LevelEdit
{
private:
	AI::AStar*				_aStar;
	System::Controls*		_controls;
	System::Camera*			_camera;
	Tilemap*				_tileMap;
	ObjectHandler*			_objectHandler;
	PickingDevice*			_pickingDevice;
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

	GameObject* _marker;
	GameObject* _lastSelected;

	System::MouseCoord _mouseCoord;
	int _tileMultiplier;
	int _tilemapHeight, _tilemapWidth;
	bool _isSelectionMode = true;
	std::vector<ButtonInfo> buttonInfo;
	
	
	void DragAndDrop(Type type);
	void DragAndDrop();

	void HandleInput();



	void HandleHUD();
	void LoadLevel(int levelID);
	void InitNewLevel();
	void ResetSelectedObj();
	void HandleSelected();
	void SelectObject(GameObject* selectedObject);

	void ExportLevel();

public:
	LevelEdit();
	~LevelEdit();

	void Initialize(ObjectHandler* objectHandler, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera);

	GameObject* GetSelectedObject();
	bool Add(Type type);
	bool Delete(Type type);
	bool TypeOn(Type type);

	void Update(float deltaTime);
};