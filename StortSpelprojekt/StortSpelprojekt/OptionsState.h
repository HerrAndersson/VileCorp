#pragma once

#include "BaseState.h"

class OptionsState : public BaseState
{
private:
public:
	OptionsState(InitVar initVar);
	~OptionsState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();

};