#include "StateMachine.h"

StateMachine::StateMachine(System::Controls* controls, ObjectHandler* objectHandler, UIHandler* uiHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice)
{
	_currentState = State::SPLASHSTATE;

	for (int i = 0; i < State::EXITSTATE; i++)
	{
		switch (i)
		{
		case SPLASHSTATE:
		{
			_baseStates.push_back(new SplashState(controls, objectHandler, uiHandler, inputDevice, camera, pickingDevice));
			break;
		}
		case MENUSTATE:
		{
			_baseStates.push_back(new MenuState(controls, objectHandler, uiHandler, inputDevice, camera, pickingDevice));
			break;
		}
		case PLAYSTATE:
		{
			_baseStates.push_back(new PlayState(controls, objectHandler, uiHandler, inputDevice, camera, pickingDevice));
			break;
		}
		case LEVELEDITSTATE:
		{
			_baseStates.push_back(new LevelEditState(controls, objectHandler, uiHandler, inputDevice, camera, pickingDevice));
			break;
		}
		case OPTIONSSTATE:
		{
			_baseStates.push_back(new OptionsState(controls, objectHandler, uiHandler, inputDevice, camera, pickingDevice));
			break;
		}
		}
	}
}

StateMachine::~StateMachine()
{
	for (auto s : _baseStates)
	{
		delete s;
	}
}

void StateMachine::Update(float deltaTime)
{
	switch (_currentState)
	{
	case EXITSTATE:
	{
		break;
	}
	default:
	{
		_baseStates[_currentState]->Update(deltaTime);
		break;
	}
	}
	ProcessStateRequest();
}

void StateMachine::ProcessStateRequest()
{
	//if currentState does not match with the new requested state.
	if (_currentState != _baseStates[_currentState]->GetNewStateRequest())
	{
		_baseStates[_currentState]->OnStateExit();
		_currentState = _baseStates[_currentState]->GetNewStateRequest();
		_baseStates[_currentState]->OnStateEnter();
	}
}

State StateMachine::GetState() 
{
	return _currentState;
}