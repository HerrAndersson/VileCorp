#pragma once

#include <cstdio>
#include <ShlObj.h>
#include "../ObjectHandler.h"
#include "../Grid.h"
#include "AStar.h"
#include "../Controls.h"
#include "../PickingDevice.h"
#include "../System/Camera.h"
#include "../StateMachine/States.h"
#include "../UITree.h"
#include "..\JsonStructs.h"
#include "..\..\System\JsonParser.h"

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
	GUI::UITree*			_uiTree;
	LevelLoad				_levelLoad;
	
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
	std::vector<GameObject*> _objects;
	GameObject* _selectedObject = nullptr;
	bool _place = false;
	bool _guardPlace = false;
	bool _trapPlace = false;
	int _budget = 100;
	int _trapCost = 25;
	int _guardCost = 50;
	GUI::Node* _uiNode;

	System::MouseCoord _mouseCoord;
	int _tileMultiplier;
	int _tilemapHeight, _tilemapWidth;
	bool _isSelectionMode = true;
	std::vector<ButtonInfo> buttonInfo;
	
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

	void Initialize(ObjectHandler* objectHandler, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera, GUI::UITree* uiTree);

	GameObject* GetSelectedObject();
	bool Add(Type type);
	bool Delete(Type type);
	bool TypeOn(Type type);

	void Update(float deltaTime);
};