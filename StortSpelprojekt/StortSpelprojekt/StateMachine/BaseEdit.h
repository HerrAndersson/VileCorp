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

class BaseEdit
{
private:
	System::Controls*		_controls;
	System::Camera*			_camera;
	Tilemap*				_tileMap;
	ObjectHandler*			_objectHandler;
	PickingDevice*			_pickingDevice;
	//Renderer::RenderModule*	_renderModule;

	struct ButtonInfo
	{
		int parent;
		bool active = false;
	};

	GameObject* _marker;
	AI::Vec2D* _markedTile;
	
	// FLAGS
	bool _isSelectionMode;
	bool _isDragAndPlaceMode;
	bool _isPlace;

	

	// No placement grids?

	void HandleInput();
	void LoadLevel(int levelID);

public:
	BaseEdit();
	~BaseEdit();

	void Initialize(ObjectHandler* objectHandler, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera);
	void Release();

	GameObject* GetSelectedObject();
	bool Add(Type type, const std::string& name);
	bool Delete(Type type);
	bool TypeOn(Type type);
	void DragAndDrop(Type type);
	void DragAndDrop();
	void DragAndPlace(Type type, const std::string& objectName);
	
	void DragActivate(Type type, const std::string& objectName);
	void ChangePlaceState();
	bool IsSelection() const;
	bool IsDragAndPlace() const;
	bool IsPlace() const;

	void Update(float deltaTime);
};