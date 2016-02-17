#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <stdexcept>
#include "stdafx.h"

#include "../System/Settings/settings.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

namespace Renderer
{
	class DirectXHandler
	{
	private:

		static const int BUFFER_COUNT = 3;

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

		void InitializeDeferredBuffers(int width, int height);

	public:

		enum CullingState { BACK, FRONT, NONE };
		enum BlendState { ENABLE, DISABLE };

		DirectXHandler(HWND hwnd, System::Settings* settings);
		~DirectXHandler();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();

		int SetGeometryStage();
		void SetAntiAliasingState();
		void SetShadowGenerationStage();
		int SetLightStage();
		void SetHUDStage();
		void SetGridStage();

		void SetCullingState(CullingState state);
		void SetBlendState(BlendState state);

		void ResizeResources(System::Settings* settings);

		void BeginScene(float red, float green, float blue, float alpha);
		void EndScene();
	};
}
