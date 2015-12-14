#pragma once

#include <cstdio>
#include <ShlObj.h>
#include "UIHandler.h"
#include "ObjectHandler.h"
#include "InputDevice.h"
#include "StateMachine/InitVar.h"
#include "Grid.h"

class LevelEdit
{
private:
	ObjectHandler*			_objectHandler;
	UIHandler*				_uiHandler;
	System::InputDevice*	_inputHandler;
	System::Camera*			_camera;
	Grid*					_grid;


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
	int _selectedObj;
	int _tileMultiplier;

	int _lastSelected;


	void HandleInput();
	void HandleSelected();
	void LoadLevel();
	void InitNewLevel();
	void ResetSelectedObj();
	void DeleteObject();

	void ExportLevel();

public:
	LevelEdit();
	~LevelEdit();

	void Update(float deltaTime);
	void Initialize(InitVar* initVar);
};