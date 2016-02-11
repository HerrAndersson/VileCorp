#include "DirectXHandler.h"

namespace Renderer
{
	DirectXHandler::DirectXHandler(HWND hwnd, System::Settings* settings)
	{
		HRESULT hResult;

		//////////////////////////////////////////////////// Swap chain, back buffer RTV //////////////////////////////////////////////////// 
		//Describe the swap chain
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = settings->_screenWidth;
		swapChainDesc.BufferDesc.Height = settings->_screenHeight;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		if (settings->_fullscreen)
		{
			swapChainDesc.Windowed = false;
		}
		else
		{
			swapChainDesc.Windowed = true;
		}

		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		//Create swap chain
		D3D_FEATURE_LEVEL featLvl = D3D_FEATURE_LEVEL_11_0;
		hResult = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featLvl, 1, D3D11_SDK_VERSION, &swapChainDesc, &_swapChain, &_device, NULL, &_deviceContext);
		if (FAILED(hResult))
		{
			throw std::runtime_error("DirectXHandler: Error creating swap chain");
		}

		//Get back buffer pointer and get render target view with it
		ID3D11Texture2D* backBufferPointer;
		hResult = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPointer);
		if (FAILED(hResult))
		{
			throw std::runtime_error("DirectXHandler: Could not get swap chain pointer");
		}

		hResult = _device->CreateRenderTargetView(backBufferPointer, NULL, &_backBufferRTV);
		if (FAILED(hResult))
		{
			throw std::runtime_error("DirectXHandler: Error creating render target view ");
		}

		backBufferPointer->Release();
		backBufferPointer = nullptr;

		///////////////////////////////////////////////////////// Rasterizer-states ///////////////////////////////////////////////////////// 
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
		{
			throw std::runtime_error("DirectXHandler: Error creating rasterizer state BACK");
		}

		rasterDesc.CullMode = D3D11_CULL_FRONT;

		hResult = _device->CreateRasterizerState(&rasterDesc, &_rasterizerStateFront);
		if (FAILED(hResult))
		{
			throw std::runtime_error("DirectXHandler: Error creating rasterizer state FRONT");
		}

		rasterDesc.CullMode = D3D11_CULL_NONE;

		hResult = _device->CreateRasterizerState(&rasterDesc, &_rasterizerStateNone);
		if (FAILED(hResult))
		{
			throw std::runtime_error("DirectXHandler: Error creating rasterizer state NONE");
		}

		/////////////////////////////////////////////////////// Depth stencil states ///////////////////////////////////////////////////////
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		//Create depth enable
		hResult = _device->CreateDepthStencilState(&depthStencilDesc, &_depthStateEnable);
		if (FAILED(hResult))
		{
			throw std::runtime_error("DirectXHandler: Error creating depth stencil ENABLE");
		}

		//Create depth disable
		depthStencilDesc.DepthEnable = false;

		hResult = _device->CreateDepthStencilState(&depthStencilDesc, &_depthStateDisable);
		if (FAILED(hResult))
		{
			throw std::runtime_error("DirectXHandler: Error creating depth stencil DISABLE");
		}

		////////////////////////////////////////////////////////// Blend-states //////////////////////////////////////////////////////////
		D3D11_BLEND_DESC omDesc;
		ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
		omDesc.RenderTarget[0].BlendEnable = true;
		omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		hResult = _device->CreateBlendState(&omDesc, &_blendStateEnable);
		if (FAILED(hResult))
		{
			throw std::runtime_error("DirectXHandler: Error creating blend ENABLE");
		}

		//Create blend disable
		omDesc.RenderTarget[0].BlendEnable = false;

		hResult = _device->CreateBlendState(&omDesc, &_blendStateDisable);
		if (FAILED(hResult))
		{
			throw std::runtime_error("DirectXHandler: Error creating blend DISABLE");
		}

		//////////////////////////////////////////////////////////// Other ////////////////////////////////////////////////////////////
		//Set up viewport
		_viewport.Width = (float)settings->_screenWidth;
		_viewport.Height = (float)settings->_screenHeight;
		_viewport.MinDepth = 0.0f;
		_viewport.MaxDepth = 1.0f;
		_viewport.TopLeftX = 0.0f;
		_viewport.TopLeftY = 0.0f;

		//Set context data and other
		InitializeDeferredBuffers(settings->_screenWidth, settings->_screenHeight);
		_deviceContext->RSSetState(_rasterizerStateFront);
		_deviceContext->OMSetDepthStencilState(_depthStateEnable, 1);
		_deviceContext->RSSetViewports(1, &_viewport);
	}

	void DirectXHandler::InitializeDeferredBuffers(int width, int height)
	{
		HRESULT result;

		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		//Render target texture description
		textureDesc.Width = (UINT)width;
		textureDesc.Height = (UINT)height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		//textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		ID3D11Texture2D* _renderTargetTextureArray[BUFFER_COUNT];
		ID3D11Texture2D* _depthStencilBuffer;

		/////////////////////////////////////////////////////// Render targets ///////////////////////////////////////////////////////
		//Create the render target textures
		for (int i = 0; i < BUFFER_COUNT; i++)
		{
			result = _device->CreateTexture2D(&textureDesc, NULL, &_renderTargetTextureArray[i]);
			if (FAILED(result))
			{
				throw std::runtime_error("DirectXHandler::InitializeDeferredBuffers: Error creating Render target textures");
			}
		}

		//Description of the render target view.
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		for (int i = 0; i < BUFFER_COUNT; i++)
		{
			result = _device->CreateRenderTargetView(_renderTargetTextureArray[i], &renderTargetViewDesc, &_deferredRTVArray[i]);
			if (FAILED(result))
			{
				throw std::runtime_error("DirectXHandler::InitializeDeferredBuffers: Error creating Render target views");
			}
		}

		/////////////////////////////////////////////////////// Shader resource views //////////////////////////////////////////////////////
		//Description of the shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		//Create the shader resource views
		for (int i = 0; i < BUFFER_COUNT; i++)
		{
			result = _device->CreateShaderResourceView(_renderTargetTextureArray[i], &shaderResourceViewDesc, &_deferredSRVarray[i]);
			if (FAILED(result))
			{
				throw std::runtime_error("DirectXHandler::InitializeDeferredBuffers: Error creating Shader resource views");
			}
		}

		/////////////////////////////////////////////////////// Depth buffer, DSV, SRV ///////////////////////////////////////////////////////
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
		depthBufferDesc.Width = width;
		depthBufferDesc.Height = height;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		// Create the texture for the depth buffer
		result = _device->CreateTexture2D(&depthBufferDesc, NULL, &_depthStencilBuffer);
		if (FAILED(result))
		{
			throw std::runtime_error("DirectXHandler::InitializeDeferredBuffers: Error creating Depth texture");
		}

		//Stencil view description.
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		//Create the depth stencil view
		result = _device->CreateDepthStencilView(_depthStencilBuffer, &depthStencilViewDesc, &_backBufferDSV);
		if (FAILED(result))
		{
			throw std::runtime_error("DirectXHandler::InitializeDeferredBuffers: Error creating Depth stencil view");
		}

		ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
		shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		result = _device->CreateShaderResourceView(_depthStencilBuffer, &shaderResourceViewDesc, &_backBufferDepthSRV);
		if (FAILED(result))
		{
			throw std::runtime_error("DirectXHandler::InitializeDeferredBuffers: Error creating depth shader resource view");
		}

		////////////////////////////////////////////////////////// Other //////////////////////////////////////////////////////////
		for (int i = 0; i < BUFFER_COUNT; i++)
		{
			SAFE_RELEASE(_renderTargetTextureArray[i]);
		}
		SAFE_RELEASE(_depthStencilBuffer);
	}

	DirectXHandler::~DirectXHandler()
	{
		_swapChain->SetFullscreenState(false, NULL);

		SAFE_RELEASE(_swapChain)
		SAFE_RELEASE(_device)
		SAFE_RELEASE(_deviceContext)
		SAFE_RELEASE(_backBufferRTV)
		SAFE_RELEASE(_blendStateEnable)
		SAFE_RELEASE(_blendStateDisable)
		SAFE_RELEASE(_rasterizerStateNone)
		SAFE_RELEASE(_rasterizerStateBack)
		SAFE_RELEASE(_rasterizerStateFront)

		SAFE_RELEASE(_backBufferDSV);
		SAFE_RELEASE(_backBufferDepthSRV);

		for (int i = 0; i < BUFFER_COUNT; i++)
		{
			SAFE_RELEASE(_deferredSRVarray[i]);
			SAFE_RELEASE(_deferredRTVArray[i]);
		}
	}

	ID3D11Device* DirectXHandler::GetDevice()
	{
		return _device;
	}

	ID3D11DeviceContext* DirectXHandler::GetDeviceContext()
	{
		return _deviceContext;
	}

	int DirectXHandler::SetGeometryStage()
	{
		_deviceContext->OMSetDepthStencilState(_depthStateEnable, 1);
		_deviceContext->RSSetViewports(1, &_viewport);
		_deviceContext->OMSetRenderTargets(BUFFER_COUNT, _deferredRTVArray, _backBufferDSV);

		return BUFFER_COUNT + 1;
	}

	void DirectXHandler::SetAntiAliasingState()
	{
		_deviceContext->OMSetDepthStencilState(_depthStateDisable, 1);
		_deviceContext->RSSetViewports(1, &_viewport);
		_deviceContext->OMSetRenderTargets(1, &_backBufferRTV, nullptr);

		_deviceContext->PSSetShaderResources(0, BUFFER_COUNT + 1, _deferredSRVarray);
	}

	void DirectXHandler::SetShadowGenerationStage()
	{
		_deviceContext->OMSetDepthStencilState(_depthStateEnable, 1);
	}

	int DirectXHandler::SetLightStage()
	{
		_deviceContext->OMSetRenderTargets(1, &_deferredRTVArray[2], nullptr);
		_deviceContext->RSSetViewports(1, &_viewport);

		//Setting Diffuse, Normal and Camera depth. Shadow map is set in RenderModule
		_deferredSRVarray[BUFFER_COUNT] = _backBufferDepthSRV;
		_deviceContext->PSSetShaderResources(0, BUFFER_COUNT + 1, _deferredSRVarray);

		return BUFFER_COUNT + 1;
	}

	void DirectXHandler::SetCullingState(CullingState state)
	{
		switch (state)
		{
		case Renderer::DirectXHandler::CullingState::BACK:
		{
			_deviceContext->RSSetState(_rasterizerStateBack);
			break;
		}
		case Renderer::DirectXHandler::CullingState::FRONT:
		{
			_deviceContext->RSSetState(_rasterizerStateFront);
			break;
		}
		case Renderer::DirectXHandler::CullingState::NONE:
		{
			_deviceContext->RSSetState(_rasterizerStateNone);
			break;
		}
		default:
			break;
		}
	}

	void DirectXHandler::SetBlendState(BlendState state)
	{
		FLOAT factor[] = { 0,0,0,0 };
		//Passing NULL as factor gives a default value of (1,1,1,1)
		switch (state)
		{
		case Renderer::DirectXHandler::BlendState::ENABLE:
		{
			_deviceContext->OMSetBlendState(_blendStateEnable, factor, 0xffffffff);
			break;
		}
		case Renderer::DirectXHandler::BlendState::DISABLE:
		{
			_deviceContext->OMSetBlendState(_blendStateDisable, factor, 0xffffffff);
			break;
		}
		default:
			break;
		}
	}

	void DirectXHandler::SetHUDStage()
	{
		_deviceContext->RSSetViewports(1, &_viewport);
		_deviceContext->OMSetRenderTargets(1, &_backBufferRTV, nullptr);
		_deviceContext->RSSetState(_rasterizerStateBack);
		_deviceContext->OMSetDepthStencilState(_depthStateDisable, 1);
	}

	void DirectXHandler::SetGridStage()
	{
		_deviceContext->OMSetDepthStencilState(_depthStateEnable, 1);
		_deviceContext->RSSetState(_rasterizerStateBack);
	}

	void DirectXHandler::ResizeResources(HWND hwnd, System::Settings* settings)
	{
		if (_swapChain && settings->_screenHeight > 0 && settings->_screenWidth > 0)
		{
			HRESULT hr;
			_deviceContext->OMSetRenderTargets(0, 0, 0);
			_backBufferRTV->Release();

			DXGI_MODE_DESC modeDesc;
			ZeroMemory(&modeDesc, sizeof(modeDesc));
			modeDesc.Width = settings->_screenWidth;
			modeDesc.Height = settings->_screenHeight;
			modeDesc.Format = DXGI_FORMAT_UNKNOWN;

			_swapChain->ResizeTarget(&modeDesc);

			//Preserve the existing buffer count and format. Automatically choose the width and height to match the client rect for HWNDs.
			hr = _swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			if (FAILED(hr))
			{
				throw std::runtime_error("DirectXHandler::ResizeResources: Error resizing buffers");
			}

			//Get buffer
			ID3D11Texture2D* pBuffer;
			hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
			if (FAILED(hr))
			{
				throw std::runtime_error("DirectXHandler::ResizeResources: Error getting the buffer from the swapchain");
			}

			//Create the new renderTargetView.
			hr = _device->CreateRenderTargetView(pBuffer, NULL, &_backBufferRTV);
			if (FAILED(hr))
			{
				throw std::runtime_error("DirectXHandler::ResizeResources: Error creating the renderTargetView");
			}

			pBuffer->Release();

			_deviceContext->OMSetRenderTargets(1, &_backBufferRTV, NULL);

			//Set up the new viewport.
			D3D11_VIEWPORT vp;
			vp.Width = (FLOAT)settings->_screenWidth;
			vp.Height = (FLOAT)settings->_screenHeight;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			_viewport = vp;

			_deviceContext->RSSetViewports(1, &_viewport);

			SAFE_RELEASE(_backBufferDSV);

			for (int i = 0; i < BUFFER_COUNT; i++)
			{
				SAFE_RELEASE(_deferredSRVarray[i]);
				SAFE_RELEASE(_deferredRTVArray[i]);
			}

			InitializeDeferredBuffers(settings->_screenWidth, settings->_screenHeight);
		}
	}

	void DirectXHandler::BeginScene(float red, float green, float blue, float alpha)
	{
		float color[] = { red, green, blue, alpha };

		//No need to clear all targets if the scene fills the whole screen. Then it will just be overwritten by the draw calls, and there will be no trailing effect 
		//_deviceContext->ClearRenderTargetView(_backBufferRTV, color);
		//for (int i = 0; i < BUFFER_COUNT; i++)
		//{
		//	_deviceContext->ClearRenderTargetView(_deferredRTVArray[i], color);
		//}

		// TODO: replace with something else
		_deviceContext->ClearRenderTargetView(_deferredRTVArray[2], color);

		_deviceContext->ClearDepthStencilView(_backBufferDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void DirectXHandler::EndScene()
	{
		_swapChain->Present(1, 0);
	}
}