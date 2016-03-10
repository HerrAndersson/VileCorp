#pragma once

#include <cstdio>
#include <ShlObj.h>

#include "../ObjectHandler.h"
#include "Grid.h"
#include "AStar.h"
#include "../Controls.h"
#include "../PickingDevice.h"
#include "../System/Camera.h"
#include "CommonUtils.h"
#include "Trap.h"

class BaseEdit
{
private:
	System::Controls*		_controls;
	Tilemap*				_tileMap;
	ObjectHandler*			_objectHandler;
	PickingDevice*			_pickingDevice;

	System::SpecificBlueprint* _sB;

	struct GhostImage
	{
		GameObject* _g = nullptr;
		AI::Vec2D _origPos;
		AI::Vec2D _origDir;
		XMFLOAT3 _origRot;
		bool _placeable = true;
		bool _created = true;

		void Reset()
		{
			_g = nullptr;
			_placeable = true;
			_created = true;
		}
	};
	GhostImage _movingGhostImage;
	GhostImage _baseGhostImage;

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
	bool _isObjectButtonReleased;
	
	void MarkerMoveEvent();
	void DragEvent(System::Type type);
	void DropEvent();
	void BoxEvent();

	void HandleMouseInput();

	void HandleKeyInput();


	bool CheckValidity(AI::Vec2D tile, GameObject* gameObject);
	void AddGhostImage();	// Used for Drag&Drop and Click
	void AddBoxGhostImage(); 	// Used for Drag&Place

public:

	BaseEdit(ObjectHandler* objectHandler, System::Controls* controls, PickingDevice* pickingDevice, bool extendedMode);
	~BaseEdit();

	// Removes the ghost image in objectHandler
	void RemoveGhostImage();

	// Adds a game object in objectHandler if in placing mode else resets to placement mode
	void HandleBlueprint(System::SpecificBlueprint* sB);

	// Returns true if an object was dropped on a valid spot
	bool IsObjectDropValid() const;

	// Returns the pointer of a created game object
	GameObject* GetCreatedObject();

	// Returns the blueprint of an deleted game object
	System::Blueprint* GetDeletedObjectBlueprint();

	//Updates the local tilemap variable. Used when a new level is loaded.
	void RefreshTileMap();

	void Update();
};