#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <stdexcept>
#include "stdafx.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

/*

Constant buffer registers setup:
----Reg--------Resource----------------------------------------------------------------------------------------------------------------------------------------
|    0	  |	   GeoPerFrame. Camera view and projection matrices. Used when rendering objects to give them the correct position from the view of the camera.   |
|         |    ShadowMapPerFrame. Light view and projection matrices. Used to generate the shadow map.                                                        |
|    1	  |	   PerObject. World matrix of the object.																				                          |
|    2	  |	   LightPerFrame. Inverted camera view and projection. Used to reconstruct world position from the cam depth map.			                      |
|    3	  |    LightPerLight. Spotlight data such as angle and range etc.															                          |
---------------------------------------------------------------------------------------------------------------------------------------------------------------

Render targets:
-----Name-----------Pos-------Description----------------------------------------------------------------------------------------------------------------------
|	 Backbuffer	     x    |   This is the render target that is ultimately drawn to the screen in EndScene. Does not have a shader resource view              |
|    Diffuse         0    |	  Holds the diffuse colors of all rendered objects. Sampled from to e.g. calculate lighting.                                      |
|    Normals         1    |	  Holds the normals of the rendered pixels. Sampled from to e.g. calculate lighting.		                                      |
|    CameraDepthMap  2    |	  Technically not a render target, but an automatically generated depth map from the view of the camera against rendered objects. |
|						  |   Sampled from to e.g. reconstruct the world position of a pixel.											                      |
|    ShadowMap       3    |   Technically not a render target, but an automatically generated depth map from the view of the light against rendered objects.  |
|					      |   Sampled from to calculate lighting/shadows. Located in ShadowMap-class, set in RenderModule.						              |
---------------------------------------------------------------------------------------------------------------------------------------------------------------

Important:

~ For blending to work, never use 1.0f as alpha. The alpha value determines how much of the new color should be applied. If 1, then it simply overwrites.

*/

namespace Renderer
{
	class DirectXHandler
	{
	private:

		static const int BUFFER_COUNT = 2;

		int _textureWidth;
		int _textureHeight;

		ID3D11RenderTargetView*		 _deferredRTVArray[BUFFER_COUNT + 2];
		ID3D11ShaderResourceView*	 _deferredSRVarray[BUFFER_COUNT + 2];

		ID3D11RenderTargetView*		 _backBufferRTV;

		ID3D11DepthStencilView*		 _backBufferDSV;
		ID3D11ShaderResourceView*	 _backBufferDepthSRV;

		IDXGISwapChain*				 _swapChain;
		ID3D11Device*			     _device;
		ID3D11DeviceContext*		 _deviceContext;
		D3D11_VIEWPORT				 _viewport;
									 
		ID3D11DepthStencilState*	 _depthStateEnable;
		ID3D11DepthStencilState*	 _depthStateDisable;
									 
		ID3D11RasterizerState*		 _rasterizerStateBack;
		ID3D11RasterizerState*		 _rasterizerStateFront;
		ID3D11RasterizerState*		 _rasterizerStateNone;
									 
		ID3D11BlendState*			 _blendStateEnable;
		ID3D11BlendState*			 _blendStateDisable;

		void InitializeDeferredBuffers();

	public:

		enum CullingState { BACK, FRONT, NONE };
		enum BlendState { ENABLE, DISABLE };

		DirectXHandler(HWND hwnd, int screenWidth, int screenHeight);
		~DirectXHandler();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();

		int SetGeometryStage();
		int SetShadowGenerationStage();
		int SetLightStage();
		void SetHUDStage();
		void SetGridStage();

		void SetCullingState(CullingState state);
		void SetBlendState(BlendState state);

		void ResizeResources(HWND hwnd, int windowWidth, int windowHeight);

		void BeginScene(float red, float green, float blue, float alpha);
		void EndScene();
	};
}
