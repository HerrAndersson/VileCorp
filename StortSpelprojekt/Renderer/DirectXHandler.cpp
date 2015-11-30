#include "DirectXHandler.h"

namespace Renderer
{
	DirectXHandler::DirectXHandler(HWND hwnd, int screenWidth, int screenHeight)
	{
		HRESULT hResult;

		//Describe the swap chain
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

		swapChainDesc.Windowed = true;

		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		//Create swap chain
		D3D_FEATURE_LEVEL featLvl = D3D_FEATURE_LEVEL_11_0;
		hResult = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
			&featLvl, 1, D3D11_SDK_VERSION, &swapChainDesc, &_swapChain, &_device, NULL, &_deviceContext);
		if (FAILED(hResult))
			throw std::runtime_error("DirectXHandler: Error creating swap chain");

		//Setup texture desctiption
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		textureDesc.Width = screenWidth;
		textureDesc.Height = screenHeight;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		//Get back buffer pointer and get render target view with it
		ID3D11Texture2D* backBufferPointer;
		hResult = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPointer);
		if (FAILED(hResult))
			throw std::runtime_error("DirectXHandler: Could not get swap chain pointer");

		hResult = _device->CreateRenderTargetView(backBufferPointer, NULL, &_renderTargetView);
		if (FAILED(hResult))
			throw std::runtime_error("DirectXHandler: Error creating render target view ");

		backBufferPointer->Release();
		backBufferPointer = nullptr;

		//Setup the description of the shader resource view.
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));

		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		//Rasterizer setup
		D3D11_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(rasterDesc));

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

		//Create rasterizer states
		hResult = _device->CreateRasterizerState(&rasterDesc, &_rasterizerStateBack);
		if (FAILED(hResult))
			throw std::runtime_error("DirectXHandler: Error creating rasterizer state");

		rasterDesc.CullMode = D3D11_CULL_FRONT;

		hResult = _device->CreateRasterizerState(&rasterDesc, &_rasterizerStateFront);
		if (FAILED(hResult))
			throw std::runtime_error("DirectXHandler: Error creating rasterizer state");

		//Set up viewport
		_viewport.Width = (float)screenWidth;
		_viewport.Height = (float)screenHeight;
		_viewport.MinDepth = 0.0f;
		_viewport.MaxDepth = 1.0f;
		_viewport.TopLeftX = 0.0f;
		_viewport.TopLeftY = 0.0f;

		_deferredShader = new Deferred(_device, screenWidth, screenHeight);

		//Set context data 
		_deviceContext->RSSetState(_rasterizerStateBack);
	}

	DirectXHandler::~DirectXHandler()
	{
		_swapChain->SetFullscreenState(false, NULL);

		_swapChain->Release();
		_device->Release();
		_deviceContext->Release();
		_renderTargetView->Release();

		delete _deferredShader;
	}

	ID3D11Device* DirectXHandler::GetDevice()
	{
		return _device;
	}

	ID3D11DeviceContext* DirectXHandler::GetDeviceContext()
	{
		return _deviceContext;
	}

	void DirectXHandler::ClearShaderResources()
	{
		ID3D11ShaderResourceView** nullArray = new ID3D11ShaderResourceView*[_R_TARGETS];
		for (int i = 0; i < _R_TARGETS; i++)
		{
			nullArray[i] = nullptr;
		}

		_deviceContext->PSSetShaderResources(0, _R_TARGETS, nullArray);

		//Not sure if this crashes /Sebastian
		delete[] nullArray;
		nullArray = nullptr;
	}

	void DirectXHandler::ClearGeometryPassRTVs(float r, float g, float b, float a)
	{
		_deferredShader->ClearRenderTargets(_deviceContext, r, g, b, a);
	}

	void DirectXHandler::SetGeometryPassRTVs()
	{
		_deferredShader->SetRenderTargets(_deviceContext);
	}

	void DirectXHandler::SetLightPassRTVs()
	{
		_deviceContext->OMSetRenderTargets(1, &_renderTargetView, nullptr);

		int count = 0;
		ID3D11ShaderResourceView** b = _deferredShader->GetShaderResourceViews(count);
		_deviceContext->PSSetShaderResources(0, count, b);
	}

	void DirectXHandler::ResizeResources(HWND hwnd, int windowWidth, int windowHeight)
	{
		if (_swapChain)
		{
			HRESULT hr;
			_deviceContext->OMSetRenderTargets(0, 0, 0);
			_renderTargetView->Release();

			DXGI_MODE_DESC modeDesc;
			ZeroMemory(&modeDesc, sizeof(modeDesc));
			modeDesc.Width = windowWidth;
			modeDesc.Height = windowHeight;
			modeDesc.Format = DXGI_FORMAT_UNKNOWN;

			_swapChain->ResizeTarget(&modeDesc);

			//Preserve the existing buffer count and format. Automatically choose the width and height to match the client rect for HWNDs.
			hr = _swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			if (FAILED(hr))
				throw std::runtime_error("DirectXHandler::ResizeResources: Error resizing buffers");

			//Get buffer
			ID3D11Texture2D* pBuffer;
			hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
			if (FAILED(hr))
				throw std::runtime_error("DirectXHandler::ResizeResources: Error getting the buffer from the swapchain");

			//Create the new renderTargetView.
			hr = _device->CreateRenderTargetView(pBuffer, NULL, &_renderTargetView);
			if (FAILED(hr))
				throw std::runtime_error("DirectXHandler::ResizeResources: Error creating the renderTargetView");

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

			_viewport = vp;

			_deferredShader->ResizeRenderTargets(_device, windowWidth, windowHeight);
		}
	}

	void DirectXHandler::BeginScene(float red, float green, float blue, float alpha)
	{
		float color[] = { red, green, blue, alpha };

		_deviceContext->RSSetState(_rasterizerStateBack);

		//Clear main RTV
		_deviceContext->ClearRenderTargetView(_renderTargetView, color);

		//Clear deferred RTVs
		ClearGeometryPassRTVs(red, green, blue, alpha);
	}

	void DirectXHandler::EndScene()
	{
		_swapChain->Present(0, 0);
	}
}