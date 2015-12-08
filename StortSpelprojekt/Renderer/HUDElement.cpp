#include"HUDElement.h"


namespace Renderer
{
	HUDElement::HUDElement(DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale, ID3D11ShaderResourceView* texture)
	{
		_position = position;
		_scale = scale;
		_texture = texture;
		Update();
	}

	void HUDElement::Update()
	{
		DirectX::XMMATRIX transform = DirectX::XMMatrixTranslation(_position.x, _position.y, 0);
		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(_scale.x, _scale.y, 1);

		_modelMatrix = DirectX::XMMatrixMultiply(transform, scale);
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

	DirectX::XMMATRIX* HUDElement::GetModelMatrix()
	{
		return &_modelMatrix;
	}
	ID3D11ShaderResourceView* HUDElement::GetTexture()
	{
		return _texture;
	}
}