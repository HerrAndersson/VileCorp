#pragma once

#include "stdafx.h"

#include "InitVar.h"
#include "BaseState.h"
#include "SplashState.h"
#include "MenuState.h"
#include "PlayState.h"
#include "OptionsState.h"

__declspec(align(16))class StateMachine
{
private:
	State		_currentState;
	BaseState*	_baseState[State::EXITSTATE];

public:
	StateMachine(InitVar initVar);
	~StateMachine();

	void Update(float deltaTime);

private:
	void ProcessStateRequest();

public:
	void* operator new(size_t i)
	{
		return _mm_malloc(i,16);
	}

		void operator delete(void* p)
	{
		_mm_free(p);
	}
};