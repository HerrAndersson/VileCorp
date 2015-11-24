#include "RenderModule.h"

namespace Renderer
{
	RenderModule::RenderModule()
	{
		
	}

	RenderModule::RenderModule(HWND hwnd, int screenWidth = 1024, int screenHeight = 768)
	{
		_d3d = new DirectX(hwnd, screenWidth, screenHeight);
	}

	RenderModule::~RenderModule()
	{
		delete _d3d;
	}
}