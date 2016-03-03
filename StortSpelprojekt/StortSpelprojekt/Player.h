#pragma once
#include "Unit.h"
#include "Guard.h"
#include "PickingDevice.h"
#include "ObjectHandler.h"

class Player
{
private:
	ObjectHandler* _objectHandler;
	PickingDevice* _pickingDevice;
	vector<short> _selectedUnits;
	vector<GameObject*> _selectedObjects;
	vector<GameObject*> _selectedObjectsPreviousPos;
	bool _drag;
	AI::Vec2D _dragOrigin;
	
	void AddGuardIcon(Unit* unit);
	void AddPatrolIcons(Guard* guard);
	void RemoveGuardIcon(short guardID);
	void RemovePatrolIcons(Guard* guard);
	void ColorObject(GameObject* obj);
	void DecolorObject(GameObject* obj);

public:
	Player();
	Player(ObjectHandler* objectHandler);
	Player(ObjectHandler* objectHandler, PickingDevice* pickingDevice);
	~Player();


	//Unit Control
	void SelectUnit(Unit* pickedUnit);
	void DeselectUnits();
	bool AreUnitsSelected();
	vector<Unit*> GetSelectedUnits();
	void MoveUnits(AI::Vec2D movePoint);
	void PatrolUnits(AI::Vec2D patrolPoint);
	int GetNumberOfSelectedUnits();

	//Object Control
	void SelectObjects(GameObject* pickedObject);
	void SelectObjects(vector<vector<GameObject*>> pickedObjects);
	void DeselectObjects();
	void DeleteSelectedObjects();
	vector<GameObject*> GetSelectedObjects();
	unsigned int GetNumSelectedObjects();
	//Currently only updating dragged units
	void UpdateDragPositions(System::MouseCoord coord);
	bool IsDragging()const;
	void ActivateDragging(GameObject* originObj);
	void DeactivateDragging();
	bool IsSelectedObjects();
};

