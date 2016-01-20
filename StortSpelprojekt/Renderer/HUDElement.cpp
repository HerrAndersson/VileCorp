#include"HUDElement.h"


namespace Renderer
{
	HUDElement::HUDElement(DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale, ID3D11ShaderResourceView* texture, bool visible)
	{
		_position = position;
		_scale = scale;
		_texture = texture;
		_visible = visible;
		Update();
	}

	void HUDElement::Update()
	{
		DirectX::XMMATRIX transform = DirectX::XMMatrixTranslation(_position.x, _position.y, 0);
		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(_scale.x, _scale.y, 1);

		_modelMatrix = DirectX::XMMatrixMultiply(scale, transform);
		_modelMatrix = DirectX::XMMatrixTranspose(_modelMatrix);
	}

	void HUDElement::SetPosition(DirectX::XMFLOAT2 position)
	{
		_position = position;
		Update();
	}

	void HUDElement::SetScale(DirectX::XMFLOAT2 scale)
	{
		_scale = scale;
		Update();
	}

	void HUDElement::SetVisibility(bool visible)
	{
		_visible = visible;
	}

	DirectX::XMMATRIX* HUDElement::GetModelMatrix()
	{
		return &_modelMatrix;
	}
	ID3D11ShaderResourceView* HUDElement::GetTexture()
	{
		return _texture;
	}
	bool HUDElement::GetVisibility()
	{
		return _visible;
	}
}