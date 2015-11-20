#include "DirectX.h"

namespace Renderer
{
	DirectX::DirectX(HWND hwnd, int screenWidth = 1024, int screenHeight = 768)
	{
		HRESULT hResult;

		/*
		Describe the swap chain
		*/
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

		swapChainDesc.BufferCount = 1;

		swapChainDesc.BufferDesc.Width = screenWidth;
		swapChainDesc.BufferDesc.Height = screenHeight;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		swapChainDesc.Windowed = false;

		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		/*
		Create swap chain
		*/
		D3D_FEATURE_LEVEL featLvl = D3D_FEATURE_LEVEL_11_0;
		hResult = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
			&featLvl, 1, D3D11_SDK_VERSION, &swapChainDesc, &_swapChain, &_device, NULL, &_deviceContext);
		if (FAILED(hResult))
			throw std::runtime_error("DirectX: Error creating swap chain");

		/*
		Get back buffer pointer and get render target view with it
		*/
		ID3D11Texture2D* backBufferPointer;
		hResult = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPointer);
		if (FAILED(hResult))
			throw std::runtime_error("DirectX: Could not get swap chain pointer");

		hResult = _device->CreateRenderTargetView(backBufferPointer, NULL, &_renderTargetView);
		if (FAILED(hResult))
			throw std::runtime_error("DirectX: Error creating render target view ");

		backBufferPointer->Release();
		backBufferPointer = nullptr;

		/*
		Depth buffer
		*/
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

		depthBufferDesc.Width = screenWidth;
		depthBufferDesc.Height = screenHeight;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		hResult = _device->CreateTexture2D(&depthBufferDesc, NULL, &_depthBuffer);
		if (FAILED(hResult))
			throw std::runtime_error("DirectX: Error creating depth buffer");

		/*
		Depth stencil view
		*/
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		hResult = _device->CreateDepthStencilView(_depthBuffer, &depthStencilViewDesc, &_depthView);
		if (FAILED(hResult))
			throw std::runtime_error("DirectX: Error creating depth stencil view");

		/*
		Rasterizer setup
		*/
		D3D11_RASTERIZER_DESC rasterDesc;

		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		/*
		Create rasterizer states
		*/
		hResult = _device->CreateRasterizerState(&rasterDesc, &_rasterizerStateBack);
		if (FAILED(hResult))
			throw std::runtime_error("DirectX: Error creating rasterizer state");

		rasterDesc.CullMode = D3D11_CULL_FRONT;

		hResult = _device->CreateRasterizerState(&rasterDesc, &_rasterizerStateFront);
		if (FAILED(hResult))
			throw std::runtime_error("DirectX: Error creating rasterizer state");


		/*
		Set up viewport
		*/
		_viewport.Width = (float)screenWidth;
		_viewport.Height = (float)screenHeight;
		_viewport.MinDepth = 0.0f;
		_viewport.MaxDepth = 1.0f;
		_viewport.TopLeftX = 0.0f;
		_viewport.TopLeftY = 0.0f;

		/*
		Set context data
		*/
		_deviceContext->OMSetRenderTargets(1, &_renderTargetView, NULL);
		_deviceContext->RSSetState(_rasterizerStateBack);
		_deviceContext->RSSetViewports(1, &_viewport);
	}

	DirectX::~DirectX()
	{
		//Must be done since DirectX can't exit properly in fullscreen /Blunder
		_swapChain->SetFullscreenState(false, NULL);

		_swapChain->Release();
		_device->Release();
		_deviceContext->Release();
		_renderTargetView->Release();
		_depthBuffer->Release();
		_depthView->Release();
	}

	ID3D11Device* DirectX::GetDevice()
	{
		return _device;
	}

	ID3D11DeviceContext* DirectX::GetDeviceContext()
	{
		return _deviceContext;
	}

	void DirectX::ResizeResources(HWND hwnd, int windowWidth, int windowHeight)
	{
		if (_swapChain)
		{
			HRESULT hr;
			_deviceContext->OMSetRenderTargets(0, 0, 0);
			_renderTargetView->Release();

			//Preserve the existing buffer count and format. Automatically choose the width and height to match the client rect for HWNDs.
			hr = _swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			if (FAILED(hr))
				throw std::runtime_error("DirectX::ResizeResources: Error resizing buffers");

			//Get buffer
			ID3D11Texture2D* pBuffer;
			hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
			if (FAILED(hr))
				throw std::runtime_error("DirectX::ResizeResources: Error getting the buffer from the swapchain");

			//Create the new renderTargetView.
			hr = _device->CreateRenderTargetView(pBuffer, NULL, &_renderTargetView);
			if (FAILED(hr))
				throw std::runtime_error("DirectX::ResizeResources: Error creating the renderTargetView");

			pBuffer->Release();

			_deviceContext->OMSetRenderTargets(1, &_renderTargetView, NULL);

			//Set up the new viewport.
			D3D11_VIEWPORT vp;
			vp.Width = (FLOAT)windowWidth;
			vp.Height = (FLOAT)windowHeight;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			_deviceContext->RSSetViewports(1, &vp);
		}
	}
}