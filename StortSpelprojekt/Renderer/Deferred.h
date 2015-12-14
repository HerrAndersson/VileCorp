#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <Windows.h>
#include <d3dcompiler.h>
#include <string>
#include <fstream>

class Deferred
{

private:

	/* Render target views
	[0] = Diffuse Color
	[1] = Normal
	[2] = Depth from view
	[3] = Shadow map / light accumulation - Set in SetLightPassRTVs.
	*/
	static const int BUFFER_COUNT = 2;

	int textureWidth;
	int textureHeight;

	ID3D11RenderTargetView*		 _renderTargetViewArray[BUFFER_COUNT+2];
	ID3D11ShaderResourceView*	 _shaderResourceViewArray[BUFFER_COUNT+2];
	
	ID3D11DepthStencilView*		 _depthStencilView;
	ID3D11ShaderResourceView*	 _depthShaderResourceView;

	void InitializeBuffers(ID3D11Device* device);

public:

	Deferred(ID3D11Device* device, int textureWidth, int textureHeight);
	virtual ~Deferred();

	int SetRenderTargets(ID3D11DeviceContext* deviceContext);
	void ClearRenderTargets(ID3D11DeviceContext* deviceContext, float r, float g, float b, float a);

	void ResizeRenderTargets(ID3D11Device* device, int textureWidth, int textureHeight);

	ID3D11ShaderResourceView** GetShaderResourceViews(int& count);
};
