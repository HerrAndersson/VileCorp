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

		struct ScreenQuadVertex
		{
			float x, y, z;
			float u, v;
		};


		DirectX* _d3d;
		ShaderHandler* _shaderHandler;

		ID3D11Buffer* _screenQuad;

		//TEMP!
		struct Vertex
		{
			float x, y, z;
			float u, v;
			float nx, ny, nz;
		};
		ID3D11Buffer* _vertexBuffer;

	public:

		RenderModule(HWND hwnd, int screenWidth, int screenHeight);
		~RenderModule();

		void ResizeResources(HWND hwnd, int windowWidth, int windowHeight);
		void SetResources();
		void SetShaderStage(int stage);
		void BeginScene(float red, float green, float blue, float alpha);
		void Render();
		void RenderLightQuad();
		void EndScene();

		ID3D11Device* GetDevice() const;
		ID3D11DeviceContext* GetDeviceContext() const;
	};
}

#endif