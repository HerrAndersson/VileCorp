#pragma once

#include "BaseState.h"

class MenuState : public BaseState
{
private:


public:
	MenuState(InitVar initVar);
	~MenuState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};
