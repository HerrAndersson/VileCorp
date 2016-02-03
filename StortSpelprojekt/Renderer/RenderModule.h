#pragma once

#define RENDERER_EXPORT __declspec(dllexport)

#include "DirectXHandler.h"
#include "ShaderHandler.h"
#include <DirectXMath.h>
#include "RenderUtils.h"
#include "ShadowMap.h"
#include "Spotlight.h"
#include "Pointlight.h"
#include "Node.h"
#include "FontWrapper.h"

/*

Constant buffer register setup:
----Reg--------Resource----------------------------------------------------------------------------------------------------------------------------------------
|    0	  |	   GeoPerFrame. Camera view and projection matrices. Used when rendering objects to give them the correct position from the view of the camera.   |
|    1	  |	   PerObject. World matrix  and color offset of the object.																				          |
|    2	  |	   LightPerFrame. Inverted camera view and projection. Used to reconstruct world position from the cam depth map.			                      |
|    3	  |    LightPerLight. Spotlight data such as angle and range etc. Pointlight data such as range and position										  |
|    4    |    ShadowMapPerFrame. Light view and projection matrices. Used to generate the shadow map.                                                        |
|    5    |    PerSkinnedObject. Animation data matrices																									  |
---------------------------------------------------------------------------------------------------------------------------------------------------------------

Render targets:
-----Name-----------Pos-------Description----------------------------------------------------------------------------------------------------------------------
|	 Backbuffer	     x    |   This is the render target that is ultimately drawn to the screen in EndScene. Does not have a shader resource view              |
|    Diffuse         0    |	  Holds the diffuse colors of all rendered objects. Sampled from to e.g. calculate lighting.                                      |
|    Normals         1    |	  Holds the normals of the rendered pixels. Sampled from to e.g. calculate lighting.		                                      |
|	 TempBackBuffer  2	  |   Holds backbuffer copy for FXAA to be able to write and read from backbuffer												      |
|    CameraDepthMap  3    |	  Technically not a render target, but an automatically generated depth map from the view of the camera against rendered objects. |
|						  |   Sampled from to e.g. reconstruct the world position of a pixel.											                      |
|    ShadowMap       4    |   Technically not a render target, but an automatically generated depth map from the view of the light against rendered objects.  |
|					      |   Sampled from to calculate lighting/shadows. Located in ShadowMap-class, set in RenderModule.						              |
---------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPORTANT:

~ For blending to work, never use 1.0f as alpha during the geometry pass.

Before rendering to shadow map:

~ Activate FRONT-FACE culling
~ Set vertex buffer before the Draw/DrawIndexed call
~ Set topology

*/

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

		struct MatrixBufferPerObject
		{
			DirectX::XMMATRIX _world;
			DirectX::XMFLOAT3 _colorOffset;
		};
		
		struct MatrixBufferPerSkinnedObject
		{
			DirectX::XMFLOAT4X4 _bones[30];
		};

		struct MatrixBufferPerFrame
		{
			DirectX::XMMATRIX _viewMatrix;
			DirectX::XMMATRIX _projectionMatrix;
			DirectX::XMFLOAT3 _ambientLight;
		};

		struct MatrixBufferHud
		{
			DirectX::XMMATRIX _model;
			DirectX::XMFLOAT4 _colorOffset;
		};

		struct MatrixBufferLightPassPerFrame
		{
			DirectX::XMMATRIX _invertedView;
			DirectX::XMMATRIX _invertedProjection;
			int _screenWidth;
			int _screenHeight;
		};

		struct MatrixBufferLightPassPerSpotlight
		{
			DirectX::XMMATRIX _viewMatrix;
			DirectX::XMMATRIX _projectionMatrix;
			DirectX::XMFLOAT3 _position;
			float			  _angle;
			DirectX::XMFLOAT3 _direction;
			float			  _intensity;
			DirectX::XMFLOAT3 _color;
			float			  _range;
			int				  _shadowMapDimensions;
		};

		struct MatrixBufferLightPassPerPointlight
		{
			DirectX::XMFLOAT3 _position;
			float			  _intensity;
			DirectX::XMFLOAT3 _color;
			float			  _range;
		};

		ID3D11Buffer*		_matrixBufferPerObject;
		ID3D11Buffer* 		_matrixBufferPerSkinnedObject;
		ID3D11Buffer*		_matrixBufferPerFrame;
		ID3D11Buffer*		_matrixBufferLightPassPerFrame;
		ID3D11Buffer*		_matrixBufferLightPassPerSpotlight;
		ID3D11Buffer*		_matrixBufferLightPassPerPointlight;
		ID3D11Buffer*		_screenQuad;
		ID3D11Buffer*		_matrixBufferHUD;

		DirectXHandler*		_d3d;
		ShaderHandler*		_shaderHandler;

		int _screenWidth;
		int _screenHeight;

		void InitializeConstantBuffers();
		void InitializeScreenQuadBuffer();

		void SetDataPerObject(DirectX::XMMATRIX* world, DirectX::XMFLOAT3 colorOffset = DirectX::XMFLOAT3(0, 0, 0));
		void SetDataPerSkinnedObject(DirectX::XMMATRIX* world, std::vector<DirectX::XMFLOAT4X4>* extra, DirectX::XMFLOAT3 colorOffset = DirectX::XMFLOAT3(0, 0, 0));
		void SetDataPerMesh(ID3D11Buffer* vertexBuffer, int vertexSize);
		void SetShadowMapDataPerObject(DirectX::XMMATRIX* world);

		void Render(GUI::Node* current, DirectX::XMMATRIX* transform, FontWrapper* fontWrapper);

		ShadowMap* _shadowMap;

	public:

		//TODO: Import these from settings file
		const int SHADOWMAP_DIMENSIONS = 256;
		const DirectX::XMFLOAT3 AMBIENT_LIGHT = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);

		enum ShaderStage { GEO_PASS, SHADOW_GENERATION, LIGHT_APPLICATION_SPOTLIGHT, LIGHT_APPLICATION_POINTLIGHT, GRID_STAGE, ANIM_STAGE, HUD_STAGE, AA_STAGE };

		RenderModule(HWND hwnd, int screenWidth, int screenHeight, bool fullScreen);
		~RenderModule();

		void ResizeResources(HWND hwnd, int windowWidth, int windowHeight);

		void SetDataPerFrame(DirectX::XMMATRIX* view, DirectX::XMMATRIX* projection);
		void SetDataPerObjectType(RenderObject* renderObject);
		void SetDataPerLineList(ID3D11Buffer* lineList, int vertexSize);

		void SetShadowMapDataPerObjectType(RenderObject* renderObject);
		void SetShadowMapDataPerSpotlight(DirectX::XMMATRIX* lightView, DirectX::XMMATRIX* lightProjection);

		void SetLightDataPerFrame(DirectX::XMMATRIX* camView, DirectX::XMMATRIX* camProjection);
		void SetLightDataPerSpotlight(Spotlight* spotlight);
		void SetLightDataPerPointlight(Pointlight* pointlight);

		void SetShaderStage(ShaderStage stage);

		void BeginScene(float red, float green, float blue, float alpha);
		void Render(DirectX::XMMATRIX* world, int vertexBufferSize, DirectX::XMFLOAT3 colorOffset = DirectX::XMFLOAT3(0, 0, 0));
		void RenderAnimation(DirectX::XMMATRIX* world, int vertexBufferSize, std::vector<DirectX::XMFLOAT4X4>* extra = nullptr, DirectX::XMFLOAT3 colorOffset = DirectX::XMFLOAT3(0, 0, 0));
		void Render(GUI::Node* root, FontWrapper* fontWrapper);
		void RenderLineList(DirectX::XMMATRIX* world, int nrOfPoints, DirectX::XMFLOAT3 colorOffset = DirectX::XMFLOAT3(0,0,0));
		void RenderShadowMap(DirectX::XMMATRIX* world, int vertexBufferSize);
		void RenderScreenQuad();
		void EndScene();

		void RenderLightVolume(ID3D11Buffer* volume, DirectX::XMMATRIX* world, int vertexCount, int vertexSize);

		ID3D11Device* GetDevice() const;
		ID3D11DeviceContext* GetDeviceContext() const;
	};
}