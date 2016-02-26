#pragma once

#include <cstdio>
#include <ShlObj.h>

#include "../ObjectHandler.h"
#include "Grid.h"
#include "AStar.h"
#include "../Controls.h"
#include "../PickingDevice.h"
#include "../System/Camera.h"

struct SpecificBlueprint
{
	SpecificBlueprint()
	{
		_blueprint = nullptr;
		_textureId = -1;
	}
	System::Blueprint* _blueprint = nullptr;
	int _textureId = -1;
};

class BaseEdit
{
private:
	System::Controls*		_controls;
	System::Camera*			_camera;
	Tilemap*				_tileMap;
	ObjectHandler*			_objectHandler;
	PickingDevice*			_pickingDevice;

	SpecificBlueprint* _sB;

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

	bool _droppedObject;
	GameObject* _createdObject = nullptr;
	System::Blueprint* _deletedObjectBlueprint = nullptr;

	// FLAGS
	bool _extendedMode;
	bool _isSelectionMode;
	bool _isDragAndDropMode;
	bool _isDragAndPlaceMode;
	bool _isPlace;
	bool _modeLock;
	bool _isInvalidateFloor;
	
	void MarkerMoveEvent();
	void DragEvent(System::Type type);
	void DropEvent();
	void BoxEvent();

	void HandleMouseInput();

	void HandleKeyInput(double deltaTime);


	bool CheckValidity(AI::Vec2D tile, System::Type type);
	void CreateMarker();	// Used for Drag&Drop and Click
	void CreateMarkers(); 	// Used for Drag&Place

public:

	BaseEdit(ObjectHandler* objectHandler, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera, bool extendedMode);
	~BaseEdit();

	GameObject* GetMarkedObject();
	bool DeleteMarkedObject();
	void ReleaseMarkers();

	void HandleBlueprint(SpecificBlueprint* sB);

	bool IsSelection() const;
	bool IsDragAndPlace() const;
	bool IsPlace() const;

	bool DroppedObject();
	GameObject* CreatedObject();
	System::Blueprint* DeletedObjectBlueprint();

	void Update(float deltaTime, bool clickedOnGUI);
};