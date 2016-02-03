#include "PlacementState.h"

PlacementState::PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, "PLACEMENT", assetManager, fontWrapper, width, height)
{
	_controls = controls;
	_objectHandler = objectHandler;

	_camera = camera;
	_pickingDevice = pickingDevice;
	_budget = 1000;


}


PlacementState::~PlacementState()
{
}

void PlacementState::Update(float deltaTime)
{
	_baseEdit.Update(deltaTime);

	HandleInput();
	HandleButtons();



}

void PlacementState::OnStateEnter()
{
	_baseEdit.Initialize(_objectHandler, _controls, _pickingDevice, _camera);
	_objectHandler->DisableSpawnPoints();
}

void PlacementState::OnStateExit()
{

}

void PlacementState::HandleInput()
{
	_baseEdit.DragAndDrop(TRAP);
	_baseEdit.DragAndDrop(GUARD);

	if (_controls->IsFunctionKeyDown("PLACEMENT:MENU"))
	{
		ChangeState(MENUSTATE);
	}
}

void PlacementState::HandleButtons()
{




	System::MouseCoord coord = _controls->GetMouseCoord();

	if (_uiTree.IsButtonColliding("Play", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("PLACEMENT:CLICK"))
	{
		ChangeState(PLAYSTATE);
	}


	if (_uiTree.IsButtonColliding("Trap", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("PLACEMENT:CLICK"))
	{
		// Temp, should be replaced with blueprint
		_toPlace._type = TRAP;
		_toPlace._name = "trap_proto";

		if (_baseEdit.IsSelection() && !_baseEdit.IsPlace())
		{
			_baseEdit.DragActivate(_toPlace._type, _toPlace._name);
		}
	}

	if (_uiTree.IsButtonColliding("Guard", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("PLACEMENT:CLICK"))
	{
		// Temp, should be replaced with blueprint
		_toPlace._type = GUARD;
		_toPlace._name = "guard_proto";

		if (_baseEdit.IsSelection())
		{
			_baseEdit.DragActivate(_toPlace._type, _toPlace._name);
		}
	}
}


