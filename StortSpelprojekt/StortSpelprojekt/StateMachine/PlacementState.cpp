#include "PlacementState.h"

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

	int cost = 20;

	//T adds Trap
	if (_inputDevice->IsPressed(0x54))
	{
		_trapChosen = true;
		if (_budget - cost >= 0)
		{
			vector<GameObject*>* vec = &_objectHandler->GetGameObjects()->at(TRAP);
			if (vec->empty())
			{
				_levelEdit.Add(TRAP, TRAP);
				_budget -= cost;

			}
			else
			{
				bool taken = false;
				if (_levelEdit.Marked(TRAP) || _levelEdit.Marked(WALL))
				{
					taken = true;
				}
				if (!taken)
				{
					_levelEdit.Add(TRAP, TRAP);
					_budget -= cost;

				}
			}
		}
	}
	else if (_inputHandler->IsPressed(System::Input::Delete))
	{
		if (!_levelEdit.Delete(TRAP))
		{
			_budget += cost;
		}
		
	}

	if (_trapChosen == true)
	{
		if (_inputHandler->IsDown(System::Input::Shift))
		{
			if (_inputHandler->IsPressed(System::Input::LeftArrow) || _inputHandler->IsPressed(System::Input::RightArrow)
				|| _inputHandler->IsPressed(System::Input::UpArrow) || _inputHandler->IsPressed(System::Input::DownArrow))
			{
				if (_budget - cost >= 0)
				{
					bool taken = false;
					if (_levelEdit.Marked(TRAP) || _levelEdit.Marked(WALL)) 
					{
						taken = true;
					}
					if (!taken)
					{
						_levelEdit.Add(TRAP, TRAP);
						_budget -= cost;

					}

				}
			}
		}
	}

	if (_inputHandler->IsPressed(System::Input::Enter))
	{
		_trapChosen = false;
	}

}

void PlacementState::OnStateEnter()
{
	_levelEdit.Initialize(_objectHandler, _inputDevice, _controls, _pickingDevice, _camera, _uiHandler);
}

void PlacementState::OnStateExit()
{

}


