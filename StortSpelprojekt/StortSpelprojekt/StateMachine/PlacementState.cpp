#include "PlacementState.h"

PlacementState::PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: BaseState(controls, objectHandler, inputDevice, camera, pickingDevice, filename, "PLACEMENT", assetManager, fontWrapper, width, height)
{
	_controls = controls;
	_objectHandler = objectHandler;
	
	_inputDevice = inputDevice;
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

	if (_levelEdit.GetSelectedObject() != nullptr)
	{
		//T adds Trap
		if (_controls->IsFunctionKeyDown("PLACEMENT:BUILD_TRAP"))
		{
			if (_budget - cost >= 0)
			{
				vector<GameObject*>* vec = &_objectHandler->GetGameObjects()->at(TRAP);
				if (vec->empty() && _levelEdit.Add(TRAP))
				{
					_budget -= cost;
				}
				else
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
				}
			}
		}
		else if (_controls->IsFunctionKeyDown("PLACEMENT:HIRE_GUARD"))
		{
			if (_budget - cost >= 0)
			{
				vector<GameObject*>* vec = &_objectHandler->GetGameObjects()->at(GUARD);
				if (vec->empty() && _levelEdit.Add(GUARD))
				{
					_budget -= cost;
				}
				else
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
				}
			}
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
	}

	if (_controls->IsFunctionKeyDown("PLACEMENT:CHANGE_TO_PLAY") == true)
	{
		ChangeState(State::PLAYSTATE);
	}
}

void PlacementState::OnStateEnter()
{
	_levelEdit.Initialize(_objectHandler, _inputDevice, _controls, _pickingDevice, _camera);
}

void PlacementState::OnStateExit()
{

}