#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include "BaseState.h"

class PlayState : public BaseState
{
private:
public:
	PlayState(InitVar initVar);
	~PlayState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};

#endif