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
	Blueprint* _blueprint;
	int _textureId;
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

	// FLAGS
	bool _extendedMode;
	bool _isSelectionMode;
	bool _isDragAndDropMode;
	bool _isDragAndPlaceMode;
	bool _isPlace;
	bool _modeLock;

	void MarkerMoveEvent();
	void DragEvent(Type type);
	void DropEvent();

	// Should be reworked 
	void DragAndPlace(SpecificBlueprint* sB);

	void HandleMouseInput();
	void HandleKeyInput(double deltaTime);

	void HandleCamMode();
	void HandleCamZoom(float deltaTime);
	void HandleCamRot();
	void HandleCamMove(float deltaTime);

	bool CheckValidity(AI::Vec2D tile, Type type);
	// Used for Drag&Place
	void CreateMarker();
	void CreateMarkers();
	void ReleaseMarkers();

public:

	BaseEdit(ObjectHandler* objectHandler, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera, bool extendedMode);
	~BaseEdit();

	GameObject* GetMarkedObject();
	bool DeleteMarkedObject();

	void HandleBlueprint(SpecificBlueprint* sB);

	void ChangePlaceState();
	bool IsSelection() const;
	bool IsDragAndPlace() const;
	bool IsPlace() const;

	void Update(float deltaTime);
};