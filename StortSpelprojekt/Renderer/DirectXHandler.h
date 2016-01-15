#ifndef DIRECTXHANDLER_H
#define DIRECTXHANDLER_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <stdexcept>
#include "Deferred.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

namespace Renderer
{
	class DirectXHandler
	{
	private:

		IDXGISwapChain*				_swapChain;
		ID3D11Device*			    _device;
		ID3D11DeviceContext*		_deviceContext;
		ID3D11RenderTargetView*		_renderTargetView;
		D3D11_VIEWPORT				_viewport;
		Deferred*                   _deferredShader;

		ID3D11DepthStencilState*	_depthEnable;
		ID3D11DepthStencilState*	_depthDisable;

		ID3D11RasterizerState*		_rasterizerStateBack;
		ID3D11RasterizerState*		_rasterizerStateFront;

		ID3D11BlendState*			_blendEnable;
		ID3D11BlendState*			_blendDisable;

	public:

		enum CullingState { BACK, FRONT };

		DirectXHandler(HWND hwnd, int screenWidth, int screenHeight);
		~DirectXHandler();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();

		void ClearGeometryPassRTVs(float r, float g, float b, float a);
		int SetGeometryPassRTVs();
		int SetFinalPassRTVs();
		int SetLightPassRTVs();

		void SetCullingState(CullingState state);

		void ResizeResources(HWND hwnd, int windowWidth, int windowHeight);

		void BeginScene(float red, float green, float blue, float alpha);
		void EndScene();
	};
}

#endif