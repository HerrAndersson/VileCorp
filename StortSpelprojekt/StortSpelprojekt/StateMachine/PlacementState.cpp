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
	_levelEdit.Update(deltaTime);

	//tempAddObj

	int cost = 20;

	//T adds Trap
	if (_controls->IsFunctionKeyDown("PLACEMENT:BUILD_TRAP") && _budget - cost >= 0)
	{
		AI::Vec2D pickedTileCoord = _pickingDevice->pickTile(_controls->GetMouseCoord()._pos);
		bool added = _objectHandler->Add(TRAP, XMFLOAT3(pickedTileCoord._x, 0, pickedTileCoord._y), XMFLOAT3(0, 0, 0));

		if (added)
		{
			_budget -= cost;
		}
		/*else
		{
			bool taken = false;
			if (_levelEdit.TypeOn(WALL))
			{
				taken = true;
			}
			if (!taken && _levelEdit.Add(TRAP))
			{
				_budget -= cost;
			}
		}*/
	}
	else if (_controls->IsFunctionKeyDown("PLACEMENT:HIRE_GUARD") && _budget - cost >= 0)
	{
		AI::Vec2D pickedTileCoord = _pickingDevice->pickTile(_controls->GetMouseCoord()._pos);
		bool added = _objectHandler->Add(GUARD, XMFLOAT3(pickedTileCoord._x, 0, pickedTileCoord._y), XMFLOAT3(0, 0, 0));

		if (added)
		{
			_budget -= cost;
		}
		/*else
		{
			bool taken = false;
			if (_levelEdit.TypeOn(WALL))
			{
				taken = true;
			}
			if (!taken && _levelEdit.Add(GUARD))
			{
				_budget -= cost;
			}
		}*/
	}
	else if (_controls->IsFunctionKeyDown("PLACEMENT:DELETE"))
	{
		if (!_levelEdit.Delete(TRAP))
		{
			_budget += cost;
		}
		if (!_levelEdit.Delete(GUARD))
		{
			_budget += cost;
		}
	}
	else if (_controls->IsFunctionKeyDown("PLACEMENT:CHANGE_TO_PLAY") == true)
	{
		ChangeState(State::PLAYSTATE);
	}
	else if (_controls->IsFunctionKeyDown("PLACEMENT:MENU"))
	{
		ChangeState(MENUSTATE);
	}
}

void PlacementState::OnStateEnter()
{
	_levelEdit.Initialize(_objectHandler, _controls, _pickingDevice, _camera);
}

void PlacementState::OnStateExit()
{

}