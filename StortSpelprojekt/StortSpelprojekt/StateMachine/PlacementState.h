#pragma once

#include "BaseState.h"
#include "LevelEdit.h"

class PlacementState : public BaseState
{
private:
	LevelEdit _levelEdit;
	InitVar _initVar;

	int _budget;


public:
	PlacementState(InitVar initVar);
	~PlacementState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};

