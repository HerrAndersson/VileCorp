#pragma once

#define RENDERER_EXPORT __declspec(dllexport)

#include <DirectXMath.h>
#include <d3d11.h>

//Disable warning about DirectX  FLOAT3/MATRIX
#pragma warning( disable: 4251 )

namespace Renderer
{
	class __declspec(align(16), dllexport) HUDElement
	{
	private:
		DirectX::XMFLOAT2			_position;
		DirectX::XMFLOAT2			_scale;
		ID3D11ShaderResourceView*	_texture;
		DirectX::XMMATRIX			_modelMatrix;

		void Update();

	public:
		HUDElement(DirectX::XMFLOAT2 position,
			DirectX::XMFLOAT2 size,
			ID3D11ShaderResourceView* texture);
		
		void SetPosition(DirectX::XMFLOAT2 position);
		void SetScale(DirectX::XMFLOAT2 scale);
		DirectX::XMMATRIX*	GetModelMatrix();
		ID3D11ShaderResourceView*	GetTexture();
	};
}