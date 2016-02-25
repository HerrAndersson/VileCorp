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
	bool _isSelectionMode;
	bool _isDragAndPlaceMode;
	bool _isPlace;
	bool _modeLock;

	void HandleInput(double deltaTime);
	void HandleCamMode();
	void HandleCamZoom(float deltaTime);
	void HandleCamRot();
	void HandleCamMove(float deltaTime);
	
	bool CheckValidity(AI::Vec2D tile, System::Type type);
	void SetValidity(Marker* m, System::Type type);
	void MarkerMoveEvent(System::Type type);
	void DragAndDropEvent(System::Type type);

	void CreateBlueprints(System::Type type, const std::string& objectName);


	void ReleaseBlueprints();

public:
	BaseEdit(ObjectHandler* objectHandler, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera);
	~BaseEdit();

	GameObject* GetSelectedObject();
	bool DeleteSelectedObject();

	bool Add(System::Type type, const std::string& name);
	bool Delete(System::Type type);
	bool TypeOn(System::Type type);

	void DragAndDrop(System::Type type);
	void DragAndDrop();
	void DragAndPlace(System::Type type, const std::string& objectName);
	void DragActivate(System::Type type, const std::string& objectName, int subType = 0);


	void ChangePlaceState();
	bool IsSelection() const;
	bool IsDragAndPlace() const;
	bool IsPlace() const;

	void Update(float deltaTime);
};