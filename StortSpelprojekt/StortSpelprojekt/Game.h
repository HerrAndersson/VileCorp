#pragma once


#include "stdafx.h"
#include "StateMachine.h"
#include "Window.h"
#include "RenderModule.h"
#include "ObjectHandler.h"

#include "Trap.h"
#include "Timer.h"

class Game
{
private:

	const float MS_PER_FRAME = 1000/60;

	StateMachine		 _SM;
	System::Window*		 _window;
	Renderer::RenderModule* _renderModule;
	ObjectHandler* _objectHandler;

	System::Timer timer;

	void HandleInput();
	void Update();
	void Render();

public:

	Game(HINSTANCE hInstance, int nCmdShow);
	~Game();

	int Run();
};