#ifndef DIRECTX_H
#define DIRECTX_H

#define RENDERER_EXPORT __declspec(dllexport)

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <stdexcept>
#include "Deferred.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

namespace Renderer
{
	class RENDERER_EXPORT DirectX
	{
	private:

		/* Render target views
		[0] = Color
		[1] = Normals
		[2] = World Pos
		Sebastian*/

		IDXGISwapChain*				_swapChain;
		ID3D11Device*			    _device;
		ID3D11DeviceContext*		_deviceContext;
		ID3D11RenderTargetView*		_renderTargetView;
		D3D11_VIEWPORT				_viewport;
		Deferred*                   _deferredShader;

		ID3D11DepthStencilView*     _depthStencilView;
		ID3D11Texture2D*			_depthStencilBuffer;
		ID3D11DepthStencilState*	_depthStencilState;
	

		ID3D11RasterizerState*		_rasterizerStateBack;
		ID3D11RasterizerState*		_rasterizerStateFront;

	public:
		const int _R_TARGETS = 3;

		DirectX(HWND hwnd, int screenWidth, int screenHeight);
		~DirectX();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();

		void ClearShaderResources();
		void ClearGeometryPassRTVs(float r, float g, float b, float a);
		void SetGeometryPassRTVs();
		void SetLightPassRTVs();

		void ResizeResources(HWND hwnd, int windowWidth, int windowHeight);

		void BeginScene(float red, float green, float blue, float alpha);
		void EndScene();
	};
}

#endif