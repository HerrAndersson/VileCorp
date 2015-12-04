#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "BaseState.h"

class MenuState : public BaseState
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