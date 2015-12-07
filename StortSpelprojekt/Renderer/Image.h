#ifndef IMAGE_H
#define IMAGE_H

#define RENDERER_EXPORT __declspec(dllexport)

#include <DirectXMath.h>
#include <d3d11.h>

namespace Renderer
{
	struct RENDERER_EXPORT Image
	{
		DirectX::XMFLOAT2 Position;
		DirectX::XMFLOAT2 Size;
		ID3D11ShaderResourceView* Texture;
		Image(DirectX::XMFLOAT2 Position_,
			DirectX::XMFLOAT2 Size_,
			ID3D11ShaderResourceView* Texture_)
		{
			Position	= Position_;
			Size		= Size_;
			Texture		= Texture_;
		}
	};
}

#endif