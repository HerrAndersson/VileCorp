#ifndef RENDERMODULE_H
#define RENDERMODULE_H

#define RENDERER_EXPORT __declspec(dllexport)

#include "DirectXHandler.h"
#include "ShaderHandler.h"
#include <DirectXMath.h>
#include "RenderUtils.h"
#include "ShadowMap.h"
#include "Spotlight.h"

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

		struct MatrixBufferLightPassPerFrame
		{
			DirectX::XMMATRIX invertedView;
			DirectX::XMMATRIX invertedProjection;
		};

		struct MatrixBufferLightPassPerLight
		{
			DirectX::XMMATRIX viewMatrix;
			DirectX::XMMATRIX projectionMatrix;
			DirectX::XMFLOAT3 position;
			float angle;
			DirectX::XMFLOAT3 direction;
			float intensity;
			DirectX::XMFLOAT3 color;
			float range;
		};

		ID3D11Buffer* _matrixBufferPerObject;
		ID3D11Buffer* _matrixBufferPerFrame;
		ID3D11Buffer* _matrixBufferLightPassPerFrame;
		ID3D11Buffer* _matrixBufferLightPassPerLight;

		DirectXHandler* _d3d;
		ShaderHandler* _shaderHandler;

		ID3D11Buffer* _screenQuad;

		void InitializeConstantBuffers();

		void SetDataPerObject(DirectX::XMMATRIX* world, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* specular);
		void SetDataPerMesh(ID3D11Buffer* vertexBuffer, int vertexSize);
		void SetShadowMapDataPerObject(DirectX::XMMATRIX* world);


		//TEMP!

		ShadowMap* _shadowMap;

	public:

		enum ShaderStage { GEO_PASS, LIGHT_PASS, SHADOW_GENERATION, LIGHT_APPLICATION };

		RenderModule(HWND hwnd, int screenWidth, int screenHeight);
		~RenderModule();

		void ResizeResources(HWND hwnd, int windowWidth, int windowHeight);

		void SetDataPerFrame(DirectX::XMMATRIX* view, DirectX::XMMATRIX* projection);
		void SetShadowMapDataPerSpotLight(DirectX::XMMATRIX* lightView, DirectX::XMMATRIX* lightProjection);

		void SetLightPassDataPerFrame(DirectX::XMMATRIX* camView, DirectX::XMMATRIX* camProjection);
		void SetLightPassDataPerLight(Spotlight* spotlight);

		void SetShaderStage(ShaderStage stage);

		void BeginScene(float red, float green, float blue, float alpha);
		void Render(DirectX::XMMATRIX* world, RenderObject* renderObject);
		void RenderShadowMap(DirectX::XMMATRIX* world, RenderObject* renderObject);
		void RenderLightQuad();
		void EndScene();

		ID3D11Device* GetDevice() const;
		ID3D11DeviceContext* GetDeviceContext() const;
	};
}

#endif