#include "LevelEditState.h"

LevelEditState::LevelEditState(System::Controls* controls, ObjectHandler* objectHandler, UIHandler* uiHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice)
	: BaseState(_controls, _objectHandler, _uiHandler, _inputDevice, _camera, _pickingDevice)
{
	_controls = controls;
	_objectHandler = objectHandler;
	_uiHandler = uiHandler;
	_inputDevice = inputDevice;
	_camera = camera;
	_pickingDevice = pickingDevice;
}

LevelEditState::~LevelEditState()
{}

void LevelEditState::Update(float deltaTime)
{
	_levelEdit.Update(deltaTime);

	//tempAddObj

	//R Adds Floor
	if (_inputDevice->IsPressed(0x52))
	{
		_floorChosen = true;
		vector<GameObject*>* vec = &_objectHandler->GetGameObjects()->at(FLOOR);
		if (vec->empty())
		{
			_levelEdit.Add(FLOOR, FLOOR);
		}
		else
		{
			bool taken = false;
			if (_levelEdit.Marked(WALL) || _levelEdit.Marked(FLOOR)) 
			{
				taken = true;
			}
			if (!taken)
			{
				_levelEdit.Add(FLOOR, FLOOR);

			}
		}
	}
	if (_floorChosen == true)
	{
		if (_inputDevice->IsDown(System::Input::Shift))
		{
			if (_inputDevice->IsPressed(System::Input::LeftArrow) || _inputDevice->IsPressed(System::Input::RightArrow)
				|| _inputDevice->IsPressed(System::Input::UpArrow) || _inputDevice->IsPressed(System::Input::DownArrow))
			{
				bool taken = false;
				if (_levelEdit.Marked(WALL) || _levelEdit.Marked(FLOOR)) 
				{
					taken = true;
				}
				if (!taken)
				{
					_levelEdit.Add(FLOOR, FLOOR);

				}
			}
		}
	}
	
	//T adds Wall
	if (_inputDevice->IsPressed(0x54))
	{
		_wallChosen = true;
		vector<GameObject*>* vec = &_objectHandler->GetGameObjects()->at(WALL);
		if (vec->empty())
		{
			_levelEdit.Add(WALL, WALL);
		}
		else
		{
			bool taken = false;
			if (_levelEdit.Marked(WALL) || _levelEdit.Marked(FLOOR)) 
			{
				taken = true;
			}
			if (!taken)
			{
				_levelEdit.Add(WALL, WALL);

			}
		}
	}
	if (_wallChosen == true)
	{
		if (_inputDevice->IsDown(System::Input::Shift))
		{
			if (_inputDevice->IsPressed(System::Input::LeftArrow) || _inputDevice->IsPressed(System::Input::RightArrow)
				|| _inputDevice->IsPressed(System::Input::UpArrow) || _inputDevice->IsPressed(System::Input::DownArrow))
			{
				bool taken = false;
				if (_levelEdit.Marked(WALL) || _levelEdit.Marked(FLOOR))
				{
					taken = true;
				}
				if (!taken)
				{
					_levelEdit.Add(WALL, WALL);

				}
			}
		}
	}

	if (_inputDevice->IsPressed(System::Input::Enter))
	{
		_floorChosen = false;
		_wallChosen = false;
	}

	//Y adds Unit
	if (_inputDevice->IsPressed(0x59))
	{
		_levelEdit.Add(ENEMY, ENEMY);
	}

	//U adds loot
	if (_inputDevice->IsPressed(System::Input::U))
	{
		_levelEdit.Add(LOOT, LOOT);
	}

	else if (_inputDevice->IsDown(System::Input::Delete))
	{
		for (int i = 0; i < NR_OF_TYPES; i++)
		{
			_levelEdit.Delete((Type)i);
		}
		
	}
}

void LevelEditState::OnStateEnter()
{
	_levelEdit.Initialize(_objectHandler, _inputDevice, _controls, _pickingDevice, _camera, _uiHandler);
}

void LevelEditState::OnStateExit()
{

}

