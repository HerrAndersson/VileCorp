#include "ShadowMap.h"

using std::runtime_error;
using std::string;

using namespace DirectX;

namespace Renderer
{
	ShadowMap::ShadowMap(ID3D11Device* device, int dimensions)
	{
		this->dimensions = dimensions;

		HRESULT hr;
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* pVS = nullptr;

		ID3D11Texture2D* shadowMap;

		///////////////////////////////////////////////////////// T2D, DSV, SRV /////////////////////////////////////////////////////////
		D3D11_TEXTURE2D_DESC shadowMapDesc;
		ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
		//shadowMapDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		shadowMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		shadowMapDesc.MipLevels = 1;
		shadowMapDesc.ArraySize = 1;
		shadowMapDesc.SampleDesc.Count = 1;
		shadowMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		shadowMapDesc.Height = (UINT)dimensions;
		shadowMapDesc.Width = (UINT)dimensions;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		//depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		/*shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;*/
		shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		hr = device->CreateTexture2D(&shadowMapDesc, nullptr, &shadowMap);
		if (FAILED(hr))
			throw runtime_error("ShadowMap: Could not create Shadow map Texture2D");

		hr = device->CreateDepthStencilView(shadowMap, &depthStencilViewDesc, &_shadowDepthStencilView);
		if (FAILED(hr))
			throw runtime_error("ShadowMap: Could not create Shadow map DSV");

		hr = device->CreateShaderResourceView(shadowMap, &shaderResourceViewDesc, &_shadowShaderResourceView);
		if (FAILED(hr))
			throw runtime_error("ShadowMap: Could not create Shadow map SRV");

		/////////////////////////////////////////////////////////// Buffers /////////////////////////////////////////////////////////////
		D3D11_BUFFER_DESC matrixBufferDesc;
		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;

		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerObject);

		hr = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBufferPerObject);
		if (FAILED(hr))
			throw std::runtime_error("ShadowMap: Failed to create matrixBufferPerObject");

		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerFrame);

		hr = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBufferPerFrame);
		if (FAILED(hr))
			throw std::runtime_error("ShadowMap: Failed to create matrixBufferPerFrame");

		///////////////////////////////////////////////////////////// Other /////////////////////////////////////////////////////////////
		ZeroMemory(&_shadowViewport, sizeof(D3D11_VIEWPORT));
		_shadowViewport.Height = (FLOAT)dimensions;
		_shadowViewport.Width = (FLOAT)dimensions;
		_shadowViewport.MinDepth = 0.f;
		_shadowViewport.MaxDepth = 1.f;

		D3D11_DEPTH_STENCIL_DESC shadowDepthStencilDesc;
		ZeroMemory(&shadowDepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		shadowDepthStencilDesc.DepthEnable = true;
		shadowDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		shadowDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		shadowDepthStencilDesc.StencilEnable = false;

		hr = device->CreateDepthStencilState(&shadowDepthStencilDesc, &_shadowDepthStencilState);
		if (FAILED(hr))
			throw runtime_error("ShadowMap: Could not create Depth stencil state");

		if (shadowMap)
			shadowMap->Release();
	}

	ShadowMap::~ShadowMap()
	{
		_shadowDepthStencilView->Release();
		_shadowShaderResourceView->Release();
	}

	void ShadowMap::SetShadowGenerationStage(ID3D11DeviceContext* deviceContext)
	{
		ID3D11ShaderResourceView* nullSrv = nullptr;
		deviceContext->PSSetShaderResources(0, 1, &nullSrv);

		deviceContext->ClearDepthStencilView(_shadowDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		deviceContext->OMSetRenderTargets(0, nullptr, _shadowDepthStencilView);
		deviceContext->OMSetDepthStencilState(_shadowDepthStencilState, 1);
		deviceContext->RSSetViewports(1, &_shadowViewport);
	}

	void ShadowMap::SetDataPerObject(ID3D11DeviceContext* deviceContext, XMMATRIX* modelWorld)
	{
		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		//XMMATRIX tw = XMMatrixTranspose(*modelWorld);
		XMMATRIX tw = *modelWorld;

		hr = deviceContext->Map(_matrixBufferPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
			throw runtime_error("ShadowMap::SetDataPerObject: Could not Map _matrixBufferPerObject");

		MatrixBufferPerObject* matrixDataBuffer = (MatrixBufferPerObject*)mappedResource.pData;

		matrixDataBuffer->modelWorld = tw;

		deviceContext->Unmap(_matrixBufferPerObject, 0);
		deviceContext->VSSetConstantBuffers(1, 1, &_matrixBufferPerObject);
	}

	void ShadowMap::SetDataPerFrame(ID3D11DeviceContext* deviceContext, XMMATRIX* lightView, XMMATRIX* lightProjection)
	{
		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		XMMATRIX tv = XMMatrixTranspose(*lightView);
		XMMATRIX tp = XMMatrixTranspose(*lightProjection);

		hr = deviceContext->Map(_matrixBufferPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
			throw runtime_error("ShadowMap::SetDataPerFrame: Could not Map _matrixBufferPerFrame");

		MatrixBufferPerFrame* matrixDataBuffer = (MatrixBufferPerFrame*)mappedResource.pData;

		matrixDataBuffer->lightView = tv;
		matrixDataBuffer->lightProjection = tp;

		deviceContext->Unmap(_matrixBufferPerFrame, 0);
		deviceContext->VSSetConstantBuffers(0, 1, &_matrixBufferPerFrame);
	}

	ID3D11ShaderResourceView* ShadowMap::GetShadowSRV()
	{
		return _shadowShaderResourceView;
	}

	int ShadowMap::GetSize()
	{
		return dimensions;
	}

}