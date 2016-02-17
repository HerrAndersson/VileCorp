#pragma once

#include <cstdio>
#include <ShlObj.h>

#include "../ObjectHandler.h"
#include "Grid.h"
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

	// FLAGS
	bool _extendedMode;
	bool _isSelectionMode;
	bool _isDragAndDropMode;
	bool _isDragAndPlaceMode;
	bool _isPlace;
	bool _modeLock;
	
	bool CheckValidity(AI::Vec2D tile, Type type);

	void MarkerMoveEvent();
	void DragEvent(Type type);
	void DropEvent();

	void HandleMouseInput();
	void HandleKeyInput(double deltaTime);

	// Used for Drag&Place
	void CreateMarkers(Type type, const std::string& objectName);
	void ReleaseMarkers();

public:
	BaseEdit(ObjectHandler* objectHandler, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera, bool extendedMode);
	~BaseEdit();

	void CreateMarker(Type type, const std::string& objectName, int subType = 0);
	GameObject* GetMarkedObject();
	bool DeleteMarkedObject();

	// Should be reworked 
	void DragAndPlace(Type type, const std::string& objectName);
	
	void ChangePlaceState();
	bool IsSelection() const;
	bool IsDragAndPlace() const;
	bool IsPlace() const;

	void Update(float deltaTime);
};