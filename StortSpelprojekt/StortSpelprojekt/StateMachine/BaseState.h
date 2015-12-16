#ifndef BASESTATE_H
#define BASESTATE_H

#include "InitVar.h"

enum State
{
	SPLASHSTATE,
	MENUSTATE,
	PLAYSTATE,
	LEVELEDITSTATE,
	OPTIONSSTATE,
	EXITSTATE,
};

class BaseState
{
private:
	static State BaseState::_newStateRequest;

protected:
	ObjectHandler*			_objectHandler;
	UIHandler*				_uiHandler;
	System::InputDevice*	_inputHandler;
	System::Camera*			_camera;
	PickingDevice*			_pickingDevice;
	
	void ChangeState(State newState);
public:
	BaseState(InitVar initVar);
	~BaseState();

	virtual void Update(float deltaTime) = 0;
	virtual void OnStateEnter() = 0;
	virtual void OnStateExit() = 0;

	State GetNewStateRequest()const;
};

#endif