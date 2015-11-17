#pragma once

#include "stdafx.h"
#include "StateMachine.h"

class Game
{
private:

	StateMachine _SM;

public:
	Game();
	~Game();

	HRESULT Initialize(HINSTANCE hInstance, int nCmdShow);
	int Run();
	void Release();
};