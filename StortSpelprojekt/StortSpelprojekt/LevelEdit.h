#pragma once

#include <cstdio>
#include <ShlObj.h>
#include "UIHandler.h"
#include "ObjectHandler.h"
#include "InputDevice.h"
#include "StateMachine/InitVar.h"
#include "Grid.h"
#include "AStar.h"

class LevelEdit
{
private:
	ObjectHandler*			_objectHandler;
	UIHandler*				_uiHandler;
	System::InputDevice*	_inputHandler;
	System::Camera*			_camera;
	Grid*					_grid;
	AI::AStar*				_aStar;
	Tilemap*				_tileMap;


	struct LevelHeader
	{
		int _version;
		int _levelSizeX;
		int _levelSizeY;
		int _nrOfTileObjects;
	};

	struct MapData
	{
		int _posX;
		int _posZ;
		float _rotY;
		int _tileType;
	};

	XMFLOAT3 _lastObjPosition = XMFLOAT3(0.0, 0.0, 0.0);

	//temps
	GameObject* _selectedObj;
	GameObject* _lastSelected;
	int _tileMultiplier;

	int _tilemapHeight, _tilemapWidth;


	void HandleInput();
	void HandleSelected();
	void LoadLevel(int levelID);
	void InitNewLevel();
	void ResetSelectedObj();
	void DeleteObject();

	void ExportLevel();

	void MoveSelectedObject();

public:
	LevelEdit();
	~LevelEdit();

	void Update(float deltaTime);
	void Initialize(InitVar* initVar);
};