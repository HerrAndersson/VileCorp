#include "Deferred.h"
#include "stdafx.h"

using namespace std;
using namespace DirectX;

Deferred::Deferred(ID3D11Device* device, int textureWidth, int textureHeight)
{
	this->textureWidth = textureWidth;
	this->textureHeight = textureHeight;

	InitializeBuffers(device);
}

Deferred::~Deferred()
{
	SAFE_RELEASE(_depthStencilView);
	SAFE_RELEASE(_depthStencilBuffer);

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		SAFE_RELEASE(_shaderResourceViewArray[i]);
		SAFE_RELEASE(_renderTargetViewArray[i]);
	}
}

void Deferred::InitializeBuffers(ID3D11Device* device)
{
	HRESULT result;

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	//Render target texture description
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D*			 _renderTargetTextureArray[BUFFER_COUNT];

	//Create the render target textures
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		result = device->CreateTexture2D(&textureDesc, NULL, &_renderTargetTextureArray[i]);
		if (FAILED(result))
		{
			throw runtime_error("Error creating render target textures");
		}
	}

	//Description of the render target view.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	//Create the render target views
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		result = device->CreateRenderTargetView(_renderTargetTextureArray[i], &renderTargetViewDesc, &_renderTargetViewArray[i]);
		if (FAILED(result))
		{
			throw runtime_error("Error creating render target views");
		}
	}

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
		result = device->CreateShaderResourceView(_renderTargetTextureArray[i], &shaderResourceViewDesc, &_shaderResourceViewArray[i]);
		if (FAILED(result))
		{
			throw runtime_error("Error creating shader resource views");
		}
	}

	//Description of the depth buffer
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &_depthStencilBuffer);
	if (FAILED(result))
	{
		throw runtime_error("Error creating texture for the depth buffer");
	}

	//Stencil view description.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//Create the depth stencil view
	result = device->CreateDepthStencilView(_depthStencilBuffer, &depthStencilViewDesc, &_depthStencilView);
	if (FAILED(result))
	{
		throw runtime_error("Error creating depth stencil view");
	}

	//Viewport for rendering
	_viewport.Width = (float)textureWidth;
	_viewport.Height = (float)textureHeight;
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;
	_viewport.TopLeftX = 0.0f;
	_viewport.TopLeftY = 0.0f;

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		SAFE_RELEASE(_renderTargetTextureArray[i]);
	}

}

void Deferred::SetRenderTargets(ID3D11DeviceContext* deviceContext)
{
	//Bind the render target view array and depth stencil buffer to the output render pipeline
	deviceContext->OMSetRenderTargets(BUFFER_COUNT, _renderTargetViewArray, _depthStencilView);
	deviceContext->RSSetViewports(1, &_viewport);
}

void Deferred::ClearRenderTargets(ID3D11DeviceContext* deviceContext, float r, float g, float b, float a)
{
	float color[4] = { r, g, b, a };

	//Clear the render target buffers
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		deviceContext->ClearRenderTargetView(_renderTargetViewArray[i], color);
	}

	deviceContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void Deferred::ResizeRenderTargets(ID3D11Device* device, int textureWidth, int textureHeight)
{
	this->textureWidth = textureWidth;
	this->textureHeight = textureHeight;

	SAFE_RELEASE(_depthStencilView);
	SAFE_RELEASE(_depthStencilBuffer);

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		SAFE_RELEASE(_shaderResourceViewArray[i]);
		SAFE_RELEASE(_renderTargetViewArray[i]);
	}

	InitializeBuffers(device);
}

ID3D11ShaderResourceView** Deferred::GetShaderResourceViews(int& count)
{
	count = BUFFER_COUNT;
	return _shaderResourceViewArray;
}