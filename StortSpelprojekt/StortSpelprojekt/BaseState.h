#pragma once
#include "InitVar.h"
#include "Controls.h"

enum State
{
	SPLASHSTATE,
	MENUSTATE,
	PLAYSTATE,
	OPTIONSSTATE,
	EXITSTATE,
};

class BaseState
{
private:
	static State BaseState::_newStateRequest;

protected:
	System::Controls*		_controls;
	ObjectHandler*			_objectHandler;
	UIHandler*				_uiHandler;
	System::InputDevice*	_inputDevice;
	
	void ChangeState(State newState);
public:
	BaseState(InitVar initVar);
	virtual ~BaseState();

	virtual void Update(float deltaTime) = 0;
	virtual void OnStateEnter() = 0;
	virtual void OnStateExit() = 0;

	State GetNewStateRequest()const;
};
