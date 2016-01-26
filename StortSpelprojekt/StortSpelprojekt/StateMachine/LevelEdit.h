#pragma once

#include <cstdio>
#include <ShlObj.h>

#include "../ObjectHandler.h"
#include "../Grid.h"
#include "AStar.h"
#include "../Controls.h"
#include "../PickingDevice.h"
#include "../System/Camera.h"
//#include "../Renderer/RenderModule.h"

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
	//Renderer::RenderModule*	_renderModule;

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
	

	void HandleInput();
	void LoadLevel(int levelID);

public:
	LevelEdit();
	~LevelEdit();

	void Initialize(ObjectHandler* objectHandler, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera);

	GameObject* GetSelectedObject();
	void ResetSelectedObj();
	bool Add(Type type, std::string name);
	bool Delete(Type type);
	bool TypeOn(Type type);
	void DragAndDrop(Type type);
	void DragAndDrop();


	void Update(float deltaTime);
};