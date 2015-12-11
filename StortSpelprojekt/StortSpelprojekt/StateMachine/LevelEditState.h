#pragma once

#include "BaseState.h"
#include "../LevelEdit.h"

class LevelEditState : public BaseState
{
private:
	LevelEdit _levelEdit;
	InitVar _initVar;
public:
	LevelEditState(InitVar initVar);
	~LevelEditState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};