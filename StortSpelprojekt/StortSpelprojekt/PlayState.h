#pragma once

#include "BaseState.h"

class PlayState : public BaseState
{
private:
public:
	PlayState(InitVar initVar, const std::string& filename);
	~PlayState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};