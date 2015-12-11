#pragma once

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
public:
	LevelEdit();
	~LevelEdit();

	void Update(float deltaTime);
	void Initialize(InitVar* initVar);
};