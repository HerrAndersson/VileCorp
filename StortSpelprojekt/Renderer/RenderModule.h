#ifndef RENDERMODULE_H
#define RENDERMODULE_H

#define RENDERER_EXPORT __declspec(dllexport)

#include "DirectXHandler.h"
#include "ShaderHandler.h"
#include <DirectXMath.h>
#include "RenderUtils.h"

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

		//Constant buffers
		struct MatrixBufferPerObject
		{
			DirectX::XMMATRIX world;
		};

		struct MatrixBufferPerFrame
		{
			DirectX::XMMATRIX viewMatrix;
			DirectX::XMMATRIX projectionMatrix;
		};

		ID3D11Buffer* _matrixBufferPerObject;
		ID3D11Buffer* _matrixBufferPerFrame;


		DirectXHandler* _d3d;
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


		bool SetResourcesPerFrame(DirectX::XMMATRIX* view, DirectX::XMMATRIX* projection);
		//bool SetResourcesPerObject(GameObject* object);
		void SetResourcesPerMesh(Mesh* vertexBuffer);


	public:

		enum ShaderStage { GEO_PASS, LIGHT_PASS };

		RenderModule(HWND hwnd, int screenWidth, int screenHeight);
		~RenderModule();

		void ResizeResources(HWND hwnd, int windowWidth, int windowHeight);


		void SetShaderStage(ShaderStage stage);

		void BeginScene(float red, float green, float blue, float alpha);
		void Render();
		void RenderLightQuad();
		void EndScene();
	};
}

#endif