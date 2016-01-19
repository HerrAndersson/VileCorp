#pragma once
#include "Node.h"
#include <d3d11.h>

namespace GUI
{

	class Element :
		public Node
	{
	public:
		Element(DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale, ID3D11ShaderResourceView* texture, const std::string& id, const std::wstring& text);
		~Element();
	private:
		std::string _id;
		//Text info
		std::wstring _text;
		UINT32 _color;
		float _fontSize;
		//Image info
		ID3D11ShaderResourceView* _texture;
	public:
		ID3D11ShaderResourceView* GetTexture();
	};
}