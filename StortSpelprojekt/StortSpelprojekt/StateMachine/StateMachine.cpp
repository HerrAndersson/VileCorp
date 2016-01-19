#include "StateMachine.h"

StateMachine::StateMachine(System::Controls* controls, ObjectHandler* objectHandler, UIHandler* uiHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice)
{
	_currentState = State::SPLASHSTATE;

	_baseStates.push_back(new SplashState(controls, objectHandler, uiHandler, inputDevice, camera, pickingDevice));
	_baseStates.push_back(new MenuState(controls, objectHandler, uiHandler, inputDevice, camera, pickingDevice));
	_baseStates.push_back(new PlayState(controls, objectHandler, uiHandler, inputDevice, camera, pickingDevice));
	_baseStates.push_back(new PlacementState(controls, objectHandler, uiHandler, inputDevice, camera, pickingDevice));
	_baseStates.push_back(new LevelEditState(controls, objectHandler, uiHandler, inputDevice, camera, pickingDevice));
	_baseStates.push_back(new OptionsState(controls, objectHandler, uiHandler, inputDevice, camera, pickingDevice));
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