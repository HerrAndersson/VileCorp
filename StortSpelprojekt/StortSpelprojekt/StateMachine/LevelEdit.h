#pragma once

#include "../UIHandler.h"
#include "../ObjectHandler.h"
#include "../System/InputDevice.h"
#include "../StateMachine/InitVar.h"
#include "../Grid.h"

class LevelEdit
{
private:
	ObjectHandler*			_objectHandler;
	UIHandler*				_uiHandler;
	System::InputDevice*	_inputHandler;
	System::Camera*			_camera;
	Grid*					_grid;

	//int _selectedObjID;
	GameObject* _selectedObj;

	int _tileMultiplier;

	int _lastSelected;


	void HandleInput();
	void HandleSelected();
	void LoadLevel();
	void InitNewLevel();
	void ResetSelectedObj();
public:
	LevelEdit();
	~LevelEdit();

	GameObject* GetSelectedObject();

	void Add(Type type, int renderObjectID);

	void Update();
	void Initialize(InitVar* initVar);
};