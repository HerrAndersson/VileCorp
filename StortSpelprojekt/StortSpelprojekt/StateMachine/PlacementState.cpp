#include "PlacementState.h"

bool compareFloat3(XMFLOAT3 a, XMFLOAT3 b)
{
	if (a.x == b.x && a.z == b.z)
	{
		return true;
	}
	return false;
}

PlacementState::PlacementState(System::Controls* controls, ObjectHandler* objectHandler, UIHandler* uiHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice)
	: BaseState(_controls, _objectHandler, _uiHandler, _inputDevice, _camera, _pickingDevice)
{
	_controls = controls;
	_objectHandler = objectHandler;
	_uiHandler = uiHandler;
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

	int cost = 200;

	//T adds Trap
	if (_inputDevice->IsPressed(0x54))
	{	
		if (_budget - cost >= 0)
		{
			Trap* selected = (Trap*)_levelEdit.GetSelectedObject();
			vector<GameObject*>* vec = &_objectHandler->GetGameObjects()->at(TRAP);
			
			if (vec->empty())
			{
				_levelEdit.Add(TRAP, TRAP);
				_budget -= cost;
			}
			else
			{
				bool taken = false;
				for (GameObject* g : *vec)
				{
					if (selected != g && compareFloat3(selected->GetPosition(), g->GetPosition()))
					{
						taken = true;
						break;
					}
				}
				if (!taken)
				{
					_levelEdit.Add(TRAP, TRAP);
					_budget -= cost;
				}
			}
		}
	}
	else if (_inputDevice->IsPressed(System::Input::Backspace))
	{
		Trap* selected = (Trap*)_levelEdit.GetSelectedObject();
		vector<GameObject*>* vec = &_objectHandler->GetGameObjects()->at(TRAP);
		for (GameObject* g : *vec)
		{
			if (selected != g && compareFloat3(selected->GetPosition(), g->GetPosition()))
			{
				_budget += cost;
				_objectHandler->Remove(g->GetID());
				break;
			}
		}
	}
}

void PlacementState::OnStateEnter()
{
	_levelEdit.Initialize(_objectHandler, _inputDevice, _controls, _pickingDevice, _camera, _uiHandler);
}

void PlacementState::OnStateExit()
{

}


