#pragma once

#include "stdafx.h"
#include "StateMachine.h"
#include "Window.h"
#include "RenderModule.h"
#include "UIHandler.h"
#include "ObjectHandler.h"
#include "InitVar.h"

#include "Trap.h"

class Game
{
private:
	System::Window*			_window;
	Renderer::RenderModule* _renderModule;

	ObjectHandler*			_objectHandler;
	UIHandler*				_UI;
	StateMachine*			_SM;

	void HandleInput();
	void Update(float deltaTime);
	void Render();

public:

	Game(HINSTANCE hInstance, int nCmdShow);
	~Game();

	int Run();
};