#ifndef OPTIONSSTATE_H
#define OPTIONSSTATE_H

#include "BaseState.h"

__declspec(align(16))class OptionsState : public BaseState
{
private:
public:
	OptionsState(InitVar initVar);
	~OptionsState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();

};

#endif