#pragma once

#include "stdafx.h"
#include "StateMachine.h"
#include "Window.h"

class Game
{
private:

	StateMachine		 _SM;
	System::Window		 _window;

public:

	Game();
	~Game();

	HRESULT Initialize(HINSTANCE hInstance, int nCmdShow);

	int Run();
	void Release();
};