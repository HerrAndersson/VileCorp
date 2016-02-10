#pragma once

#include <cstdio>
#include <ShlObj.h>

#include "../ObjectHandler.h"
#include "../Grid.h"
#include "AStar.h"
#include "../Controls.h"
#include "../PickingDevice.h"
#include "../System/Camera.h"

class BaseEdit
{
private:
	System::Controls*		_controls;
	System::Camera*			_camera;
	Tilemap*				_tileMap;
	ObjectHandler*			_objectHandler;
	PickingDevice*			_pickingDevice;

	struct ButtonInfo
	{
		int parent;
		bool active = false;
	};

	struct Marker
	{
		GameObject* _g = nullptr;
		AI::Vec2D _origPos;
		bool _placeable = true;
		bool _created = true;

		void Reset()
		{
			_g = nullptr;
			bool _placeable = true;
			bool _created = true;
		}
	} _marker, _baseMarker;
	//AI::Vec2D* _markedTile; // Make into Marker instead...

	/*
	TODO: For Drag&Place (by Zache):
	Update _marker dynamically when mouse is moved and 
	_markedTile statically when left button down.
	Change color of the tiles between _marker and _markedTile.
	Use GREEN for valid and RED for invalid.
	This is a cheaper approach than creating/destroying objects
	every time the mousepointer moves to another tile.
	*/

	// FLAGS
	bool _isSelectionMode;
	bool _isDragAndPlaceMode;
	bool _isPlace;
	bool _modeLock;

	void HandleInput();
	void LoadLevel(int levelID);
	
	bool CheckValidity(AI::Vec2D tile, Type type);
	void SetValidity(Marker* m, Type type);
	void MarkerMoveEvent(Type type);
	void DragAndDropEvent();

	void ReleaseBlueprints();

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
	void DragActivate(Type type, const std::string& objectName, int subType = 0);


	void ChangePlaceState();
	bool IsSelection() const;
	bool IsDragAndPlace() const;
	bool IsPlace() const;

	void Update(float deltaTime);
};