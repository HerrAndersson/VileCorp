#ifndef RENDERMODULE_H
#define RENDERMODULE_H

#include "DirectX.h"
#include "ShaderHandler.h"

namespace Renderer
{
	class RenderModule
	{
	private:
		DirectX* _d3d;
		ShaderHandler* _shaderHandler;

	public:
		RenderModule();
		RenderModule(HWND hwnd, int screenWidth, int screenHeight);
		~RenderModule();
	};
}

#endif