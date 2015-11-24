#pragma once

#include "stdafx.h"
#include "StateMachine.h"
#include "Window.h"
#include "RenderModule.h"

class Game
{
private:

	StateMachine		 _SM;
	System::Window*		 _window;
	Renderer::RenderModule* _renderModule;

public:

	Game();
	~Game();

	HRESULT Initialize(HINSTANCE hInstance, int nCmdShow);

	int Run();
};