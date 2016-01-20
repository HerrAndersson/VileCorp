#pragma once

#define RENDERER_EXPORT __declspec(dllexport)

#include "DirectXHandler.h"
#include "ShaderHandler.h"
#include <DirectXMath.h>
#include "RenderUtils.h"
#include "HUDElement.h"
#include "ShadowMap.h"
#include "Spotlight.h"

namespace Renderer
{
	class RENDERER_EXPORT RenderModule
	{
	private:

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
		
		struct MatrixBufferPerSkinnedObject
		{
			DirectX::XMMATRIX   _world;
			DirectX::XMFLOAT4X4 _bones[30];
		};

		struct MatrixBufferPerFrame
		{
			DirectX::XMMATRIX _viewMatrix;
			DirectX::XMMATRIX _projectionMatrix;
		};

		struct MatrixBufferHud
		{
			DirectX::XMMATRIX _model;
		};

		struct MatrixBufferLightPassPerFrame
		{
			DirectX::XMMATRIX _invertedView;
			DirectX::XMMATRIX _invertedProjection;
		};

		struct MatrixBufferLightPassPerLight
		{
			DirectX::XMMATRIX _viewMatrix;
			DirectX::XMMATRIX _projectionMatrix;
			DirectX::XMFLOAT3 _position;
			float			  _angle;
			DirectX::XMFLOAT3 _direction;
			float			  _intensity;
			DirectX::XMFLOAT3 _color;
			float			  _range;
		};

		ID3D11Buffer*		_matrixBufferPerObject;
		ID3D11Buffer* 		_matrixBufferPerSkinnedObject;
		ID3D11Buffer*		_matrixBufferPerFrame;
		ID3D11Buffer*		_matrixBufferLightPassPerFrame;
		ID3D11Buffer*		_matrixBufferLightPassPerLight;
		ID3D11Buffer*		_screenQuad;
		ID3D11Buffer*		_matrixBufferHUD;

		DirectXHandler*		_d3d;
		ShaderHandler*		_shaderHandler;

		void InitializeConstantBuffers();

		void SetDataPerObject(DirectX::XMMATRIX* world, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* specular);
		void SetDataPerObject(DirectX::XMMATRIX* world, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* specular, std::vector<DirectX::XMFLOAT4X4>* extra);
		void SetDataPerMesh(ID3D11Buffer* vertexBuffer, int vertexSize);
		void SetShadowMapDataPerObject(DirectX::XMMATRIX* world);

		//TODO: TEMP! (?) /Jonas
		ShadowMap* _shadowMap;

	public:

		enum ShaderStage { GEO_PASS, SHADOW_GENERATION, LIGHT_APPLICATION, GRID_PASS, ANIM_PASS, HUD_PASS };

		RenderModule(HWND hwnd, int screenWidth, int screenHeight);
		~RenderModule();

		void ResizeResources(HWND hwnd, int windowWidth, int windowHeight);

		void SetDataPerFrame(DirectX::XMMATRIX* view, DirectX::XMMATRIX* projection);
		void SetShadowMapDataPerSpotLight(DirectX::XMMATRIX* lightView, DirectX::XMMATRIX* lightProjection);

		void SetLightDataPerFrame(DirectX::XMMATRIX* camView, DirectX::XMMATRIX* camProjection);
		void SetLightDataPerLight(Spotlight* spotlight);

		void SetShaderStage(ShaderStage stage);

		void BeginScene(float red, float green, float blue, float alpha);
		void Render(DirectX::XMMATRIX* world, RenderObject* renderObject, std::vector<DirectX::XMFLOAT4X4>* extra = nullptr);
		void Render(std::vector<HUDElement>* imageData);
		void RenderLineList(DirectX::XMMATRIX* world, ID3D11Buffer* lineList, int nrOfPoints);
		void RenderShadowMap(DirectX::XMMATRIX* world, RenderObject* renderObject);
		void RenderScreenQuad();
		void EndScene();

		void DEBUG_RenderLightVolume(ID3D11Buffer* volume, DirectX::XMMATRIX* world, int vertexCount, int vertexSize);

		ID3D11Device* GetDevice() const;
		ID3D11DeviceContext* GetDeviceContext() const;
	};
}