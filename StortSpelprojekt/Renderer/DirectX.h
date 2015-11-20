#ifndef DIRECTX_H
#define DIRECTX_H

#define RENDERER_EXPORT __declspec(dllexport)

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <stdexcept>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

namespace Renderer
{
	class RENDERER_EXPORT DirectX
	{
	private:

		IDXGISwapChain*				_swapChain;
		ID3D11Device*				_device;
		ID3D11DeviceContext*		_deviceContext;

		ID3D11RenderTargetView*		_renderTargetView;
		D3D11_VIEWPORT				_viewport;

		ID3D11Texture2D*			_depthBuffer;
		ID3D11DepthStencilView*		_depthView;

		ID3D11RasterizerState*		_rasterizerStateBack;
		ID3D11RasterizerState*		_rasterizerStateFront;

	public:

		DirectX(HWND hwnd, int screenWidth, int screenHeight);
		~DirectX();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();

		void ResizeResources(HWND hwnd, int windowWidth, int windowHeight);
	};
}

#endif