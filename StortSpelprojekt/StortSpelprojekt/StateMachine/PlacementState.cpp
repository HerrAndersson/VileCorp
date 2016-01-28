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

	_baseEdit.DragAndDrop(TRAP);
	_baseEdit.DragAndDrop(GUARD);

	//tempAddObj

	int cost = 20;

	// Temp code
	if (_controls->IsFunctionKeyDown("PLACEMENT:BUILD_TRAP"))
	{
		ChangeState(PLAYSTATE);
	}

	if (_baseEdit.GetSelectedObject() != nullptr)
	{
		//T adds Trap
		if (_controls->IsFunctionKeyDown("PLACEMENT:BUILD_TRAP"))
		{
			if (_budget - cost >= 0)
			{
				vector<GameObject*>* vec = &_objectHandler->GetGameObjects()->at(TRAP);
				if (vec->empty() && _baseEdit.Add(TRAP, "trap_proto"))
				{
					_budget -= cost;
				}
				else
				{
					bool taken = false;
					if (_baseEdit.TypeOn(WALL))
					{
						taken = true;
					}
					if (!taken && _baseEdit.Add(TRAP, "trap_proto"))
					{
						_budget -= cost;
					}
				}
			}
		}
		else if (_controls->IsFunctionKeyDown("PLACEMENT:HIRE_GUARD"))
		{
			if (_budget - cost >= 0)
			{
				vector<GameObject*>* vec = &_objectHandler->GetGameObjects()->at(GUARD);
				if (vec->empty() && _baseEdit.Add(GUARD, "proto"))
				{
					_budget -= cost;
				}
				else
				{
					bool taken = false;
					if (_baseEdit.TypeOn(WALL))
					{
						taken = true;
					}
					if (!taken && _baseEdit.Add(GUARD, "proto"))
					{
						_budget -= cost;
					}
				}
			}
		}
		else if (_controls->IsFunctionKeyDown("PLACEMENT:DELETE"))
		{
			if (!_baseEdit.Delete(TRAP))
			{
				_budget += cost;
			}
			if (!_baseEdit.Delete(GUARD))
			{
				_budget += cost;
			}
		}
	}
}

void PlacementState::OnStateEnter()
{
	_baseEdit.Initialize(_objectHandler, _controls, _pickingDevice, _camera);
	_objectHandler->DisableSpawnPoints();
}

void PlacementState::OnStateExit()
{

}


