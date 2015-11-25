#include "DirectX.h"

namespace Renderer
{
	DirectX::DirectX(HWND hwnd, int screenWidth, int screenHeight)
	{
		HRESULT hResult;

		_deferredRenderTargetViews = new ID3D11RenderTargetView*[_R_TARGETS];
		_deferredShaderResourceViews = new ID3D11ShaderResourceView*[_R_TARGETS];
		ID3D11Texture2D** textureArray = new ID3D11Texture2D*[_R_TARGETS];

		//Initialize render targets
		for (int i = 0; i < _R_TARGETS; i++)
		{
			_deferredRenderTargetViews[i] = nullptr;
			_deferredShaderResourceViews[i] = nullptr;
		}

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
			throw std::runtime_error("DirectX: Error creating swap chain");

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

		//Create the textures
		for (int i = 0; i < _R_TARGETS; i++)
		{
			hResult = _device->CreateTexture2D(&textureDesc, NULL, &textureArray[i]);
			if (FAILED(hResult))
				throw std::runtime_error("DirectX: Error creating texture");
		}

		//Get back buffer pointer and get render target view with it
		ID3D11Texture2D* backBufferPointer;
		hResult = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPointer);
		if (FAILED(hResult))
			throw std::runtime_error("DirectX: Could not get swap chain pointer");

		hResult = _device->CreateRenderTargetView(backBufferPointer, NULL, &_mainRenderTargetView);
		if (FAILED(hResult))
			throw std::runtime_error("DirectX: Error creating render target view ");

		for (int i = 0; i < _R_TARGETS; i++)
		{
			hResult = _device->CreateRenderTargetView(textureArray[i], NULL, &_deferredRenderTargetViews[i]);
			if (FAILED(hResult))
				throw std::runtime_error("DirectX: Error creating render target views ");
		}

		backBufferPointer->Release();
		backBufferPointer = nullptr;

		//Setup the description of the shader resource view.
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));

		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;


		//Create shader resource views

		//ID3D11Texture2D* t = nullptr;
		//hResult = _device->CreateTexture2D(&textureDesc, NULL, &t);
		//if (FAILED(hResult))
		//	throw std::runtime_error("DirectX: Error creating texture");

		//hResult = _device->CreateShaderResourceView(t, &shaderResourceViewDesc, &_mainShaderResourceView);
		//if (FAILED(hResult))
		//	throw std::runtime_error("DirectX: Error creating shaderResource views ");

		for (int i = 0; i < _R_TARGETS; i++)
		{
			hResult = _device->CreateShaderResourceView(textureArray[i], &shaderResourceViewDesc, &_deferredShaderResourceViews[i]);
			if (FAILED(hResult))
				throw std::runtime_error("DirectX: Error creating shaderResource views ");
		}

		//Release textures
		for (int i = 0; i < _R_TARGETS; i++) 
		{ 
			textureArray[i]->Release(); 
		}
		delete[] textureArray;
		//t->Release();

		//Depth buffer
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

		//Depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		hResult = _device->CreateDepthStencilView(_depthBuffer, &depthStencilViewDesc, &_depthView);
		if (FAILED(hResult))
			throw std::runtime_error("DirectX: Error creating depth stencil view");

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
			throw std::runtime_error("DirectX: Error creating rasterizer state");

		rasterDesc.CullMode = D3D11_CULL_FRONT;

		hResult = _device->CreateRasterizerState(&rasterDesc, &_rasterizerStateFront);
		if (FAILED(hResult))
			throw std::runtime_error("DirectX: Error creating rasterizer state");

		//Set up viewport
		_viewport.Width = (float)screenWidth;
		_viewport.Height = (float)screenHeight;
		_viewport.MinDepth = 0.0f;
		_viewport.MaxDepth = 1.0f;
		_viewport.TopLeftX = 0.0f;
		_viewport.TopLeftY = 0.0f;

		//Set context data 
		_deviceContext->RSSetState(_rasterizerStateBack);
	}

	DirectX::~DirectX()
	{
		//Must be done since DirectX can't exit properly in fullscreen /Blunder
		_swapChain->SetFullscreenState(false, NULL);

		_swapChain->Release();
		_device->Release();
		_deviceContext->Release();
		_mainRenderTargetView->Release();
		//_mainShaderResourceView->Release();
		for (int i = 0; i < _R_TARGETS; i++)
		{
			_deferredRenderTargetViews[i]->Release();
			_deferredShaderResourceViews[i]->Release();
		}
		delete[] _deferredRenderTargetViews;
		delete[] _deferredShaderResourceViews;
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

	void DirectX::ClearShaderResources()
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

	void DirectX::ClearGeometryPassRTVs(float r, float g, float b, float a)
	{
		float clearColor[] = { r, g, b, a };

		for (int i = 0; i < _R_TARGETS; i++)
		{
			_deviceContext->ClearRenderTargetView(_deferredRenderTargetViews[i], clearColor);
		}

	}

	void DirectX::SetGeometryPassRTVs()
	{
		_deviceContext->OMSetRenderTargets(_R_TARGETS, _deferredRenderTargetViews, _depthView);
	}

	void DirectX::SetLightPassRTVs()
	{
		_deviceContext->OMSetRenderTargets(1, &_mainRenderTargetView, _depthView);
	}

	void DirectX::ResizeResources(HWND hwnd, int windowWidth, int windowHeight)
	{
		if (_swapChain)
		{
			HRESULT hr;
			_deviceContext->OMSetRenderTargets(0, 0, 0);
			_mainRenderTargetView->Release();

			DXGI_MODE_DESC modeDesc;
			ZeroMemory(&modeDesc, sizeof(modeDesc));
			modeDesc.Width = windowWidth;
			modeDesc.Height = windowHeight;
			modeDesc.Format = DXGI_FORMAT_UNKNOWN;

			_swapChain->ResizeTarget(&modeDesc);

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
			hr = _device->CreateRenderTargetView(pBuffer, NULL, &_mainRenderTargetView);
			if (FAILED(hr))
				throw std::runtime_error("DirectX::ResizeResources: Error creating the renderTargetView");

			pBuffer->Release();

			_deviceContext->OMSetRenderTargets(1, &_mainRenderTargetView, NULL);

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
		}
	}

	void DirectX::BeginScene(float red, float green, float blue, float alpha)
	{
		float color[] = { red, green, blue, alpha };

		/*deviceContext->OMSetDepthStencilState(dsDepthEnable, 1);
		_deviceContext->OMSetRenderTargets(1, &_mainRenderTargetView, _depthView);
		//_deviceContext->RSSetState(rsBackCulling);
		_deviceContext->RSSetViewports(1, &_viewport);*/

		//Clear main RTV
		_deviceContext->ClearRenderTargetView(_mainRenderTargetView, color);
		//Clear depth stencil view
		_deviceContext->ClearDepthStencilView(_depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);

		//ClearShaderResources();
		ClearGeometryPassRTVs(red, green, blue, alpha);
	}

	void DirectX::EndScene()
	{
		_swapChain->Present(0, 0);
	}
}