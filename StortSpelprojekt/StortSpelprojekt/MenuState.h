#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "BaseState.h"

__declspec(align(16))class MenuState : public BaseState
{
private:
	UIHandler* _uiHandler;

public:
	MenuState(InitVar initVar);
	~MenuState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};

#endif