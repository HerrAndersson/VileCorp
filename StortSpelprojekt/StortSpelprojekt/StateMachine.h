#pragma once

#include "stdafx.h"

class StateMachine
{
private:
	enum State
	{
		SPLASHSTATE,
		MENUSTATE,
		PLAYSTATE,
		OPTIONSSTATE,
		EXITSTATE,
	};
	State _currentState;

public:
	StateMachine();
	~StateMachine();

	void Initialize();
	HRESULT Update();
	void Render();

};