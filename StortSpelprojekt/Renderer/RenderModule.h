#pragma once

#define RENDERER_EXPORT __declspec(dllexport)

#include "DirectXHandler.h"
#include "ShaderHandler.h"
#include <DirectXMath.h>
#include "RenderUtils.h"
#include "HUDElement.h"

namespace Renderer
{
	class RENDERER_EXPORT RenderModule
	{
	private:
		// Any point of this? Isn't used... - Zache
		struct ScreenQuadVertex
		{
			float _x, _y, _z;
			float _u, _v;
		};

		//Constant buffers
		struct MatrixBufferPerObject
		{
			DirectX::XMMATRIX _world;
		};

		struct MatrixBufferPerFrame
		{
			DirectX::XMMATRIX _viewMatrix;
			DirectX::XMMATRIX _projectionMatrix;
		};

		struct MatrixBufferHud
		{
			DirectX::XMMATRIX model;
		};

		ID3D11Buffer* _matrixBufferPerObject;
		ID3D11Buffer* _matrixBufferPerFrame;

		DirectXHandler* _d3d;
		ShaderHandler* _shaderHandler;

		ID3D11Buffer* _screenQuad;
		ID3D11Buffer* _matrixBufferHUD;

		void InitializeConstantBuffers();

		void SetResourcesPerObject(DirectX::XMMATRIX* world, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* specular);
		void SetResourcesPerMesh(ID3D11Buffer* vertexBuffer, int vertexSize);

	public:

		enum ShaderStage { GEO_PASS, LIGHT_PASS, HUD_PASS };

		RenderModule(HWND hwnd, int screenWidth, int screenHeight);
		~RenderModule();

		void ResizeResources(HWND hwnd, int windowWidth, int windowHeight);

		void SetResourcesPerFrame(DirectX::XMMATRIX* view, DirectX::XMMATRIX* projection);
		void SetShaderStage(ShaderStage stage);

		void BeginScene(float red, float green, float blue, float alpha);
		void Render(DirectX::XMMATRIX* world, RenderObject* renderObject);
		void Render(std::vector<HUDElement>* imageData);
		void RenderLightQuad();
		void EndScene();

		ID3D11Device* GetDevice() const;
		ID3D11DeviceContext* GetDeviceContext() const;
	};
}