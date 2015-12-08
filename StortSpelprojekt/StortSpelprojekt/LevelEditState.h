#ifndef LEVELEDITSTATE_H
#define LEVELEDITSTATE_H

#include "BaseState.h"

class LevelEditState : public BaseState
{
private:
public:
	LevelEditState(InitVar initVar);
	~LevelEditState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};

#endif