#include "PlacementState.h"

PlacementState::PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings, System::SoundModule* soundModule)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, "PLACEMENT", assetManager, fontWrapper, settings, soundModule)
{
	_controls = controls;
	_objectHandler = objectHandler;
	_camera = camera;
	_pickingDevice = pickingDevice;
	_budget = 1000;
	_baseEdit = nullptr;
}


PlacementState::~PlacementState()
{
	delete _baseEdit;
	_baseEdit = nullptr;
}

void PlacementState::Update(float deltaTime)
{
	_baseEdit->Update(deltaTime);

	HandleInput();
	HandleButtons();
}

void PlacementState::OnStateEnter()
{
	//TODO: Move this function to LevelSelection when that state is created. /Alex
	_objectHandler->LoadLevel(3);

	_baseEdit = new BaseEdit(_objectHandler, _controls, _pickingDevice, _camera);
	_objectHandler->DisableSpawnPoints();

	XMFLOAT3 campos;
	campos.x = _objectHandler->GetTileMap()->GetWidth() / 2;
	campos.y = 15;
	campos.z = _objectHandler->GetTileMap()->GetHeight() / 2 - 10;
	_camera->SetPosition(campos);
}

void PlacementState::OnStateExit()
{
	delete _baseEdit;
	_baseEdit = nullptr;
}

void PlacementState::HandleInput()
{
	_baseEdit->DragAndDrop(TRAP);
	_baseEdit->DragAndDrop(GUARD);
	_baseEdit->DragAndDrop(CAMERA);

	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		ChangeState(PAUSESTATE);
	}
}

void PlacementState::HandleButtons()
{
	System::MouseCoord coord = _controls->GetMouseCoord();

	if (_uiTree.IsButtonColliding("Play", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		ChangeState(PLAYSTATE);
	}

	if (_uiTree.IsButtonColliding("AnvilTrap", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		// Temp, should be replaced with blueprint
		_toPlace._type = TRAP;
		_toPlace._name = "trap_proto";

		if (_baseEdit->IsSelection() && !_baseEdit->IsPlace())
		{
			_baseEdit->DragActivate(_toPlace._type, _toPlace._name, SPIKE);
		}
	}
	if (_uiTree.IsButtonColliding("TeslaTrap", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		_toPlace._type = TRAP;
		_toPlace._name = "trap_proto";

		if (_baseEdit->IsSelection() && !_baseEdit->IsPlace())
		{
			_baseEdit->DragActivate(_toPlace._type, _toPlace._name, TESLACOIL);
		}
	}
	if (_uiTree.IsButtonColliding("Camera", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		_toPlace._type = CAMERA;
		_toPlace._name = "camera_proto";

		if (_baseEdit->IsSelection() && !_baseEdit->IsPlace())
		{
			_baseEdit->DragActivate(_toPlace._type, _toPlace._name);
		}
	}

	if (_uiTree.IsButtonColliding("Guard", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		// Temp, should be replaced with blueprint
		_toPlace._type = GUARD;
		_toPlace._name = "guard_proto";

		if (_baseEdit->IsSelection())
		{
			_baseEdit->DragActivate(_toPlace._type, _toPlace._name);
		}
	}
	//if (_uiTree.IsButtonColliding("Camera", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	//{
	//	
	//}

}


