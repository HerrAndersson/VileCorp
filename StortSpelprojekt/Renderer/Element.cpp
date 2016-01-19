#include "Element.h"


namespace GUI
{

	Element::Element(DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale, ID3D11ShaderResourceView* texture, const std::string& id, const std::wstring& text) : Node(position, scale)
	{
		_id = id;
		_text = text;
		_texture = texture;
	}


	Element::~Element()
	{
	}

	ID3D11ShaderResourceView* Element::GetTexture()
	{
		return _texture;
	}
}