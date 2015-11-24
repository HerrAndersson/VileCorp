#ifndef RENDERMODULE_H
#define RENDERMODULE_H

#define RENDERER_EXPORT __declspec(dllexport)

#include "DirectX.h"
#include "ShaderHandler.h"

namespace Renderer
{
	class RENDERER_EXPORT RenderModule
	{
	private:

		DirectX* _d3d;
		ShaderHandler* _shaderHandler;





		//TEMP!
		struct Vertex
		{
			float x, y, z;
			float u, v;
			float nx, ny, nz;
		};
		ID3D11Buffer* vertexBuffer;


	public:

		RenderModule(HWND hwnd, int screenWidth, int screenHeight);
		~RenderModule();

		void ResizeResources(HWND hwnd, int windowWidth, int windowHeight);

		void BeginScene(float red, float green, float blue, float alpha);

		void Render();

		void EndScene();
	};
}

#endif