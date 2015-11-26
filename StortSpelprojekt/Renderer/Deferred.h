#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <Windows.h>
#include <d3dcompiler.h>
#include <string>
#include <fstream>

#define RENDERER_EXPORT __declspec(dllexport)

class RENDERER_EXPORT Deferred
{

private:

	//Diffuse, Normal, Position, WorldPosition
	static const int BUFFER_COUNT = 4;

	int textureWidth;
	int textureHeight;

	ID3D11Texture2D*			 _renderTargetTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView*		 _renderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView*	 _shaderResourceViewArray[BUFFER_COUNT];
	ID3D11Texture2D*			 _depthStencilBuffer;
	ID3D11DepthStencilView*		 _depthStencilView;
	D3D11_VIEWPORT				 _viewport;

	void InitializeBuffers(ID3D11Device* device);

public:

	Deferred(ID3D11Device* device, int textureWidth, int textureHeight);
	virtual ~Deferred();

	void SetRenderTargets(ID3D11DeviceContext* deviceContext);
	void ClearRenderTargets(ID3D11DeviceContext* deviceContext, float r, float g, float b, float a);

	//TODO: Resize buffers on window resize! /Jonas

	ID3D11ShaderResourceView** GetShaderResourceViews(int& count);
};
