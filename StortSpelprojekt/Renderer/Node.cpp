#include "Node.h"

namespace GUI
{
	Node::Node(DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale,
		ID3D11ShaderResourceView* texture,
		const std::string& id,
		const std::wstring& text,
		UINT32 _color,
		float _fontSize)
	{
		_position = position;
		_scale = scale;
		_id = id;
		_text = text;
		_texture = texture;
	}


	Node::~Node()
	{
	}

	void Node::Update()
	{
		DirectX::XMMATRIX transform = DirectX::XMMatrixTranslation(_position.x, _position.y, 0);
		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(_scale.x, _scale.y, 1);

		_modelMatrix = DirectX::XMMatrixMultiply(scale, transform);
	}

	void Node::SetPosition(DirectX::XMFLOAT2 position)
	{
		_position = position;
		Update();
	}

	void Node::SetScale(DirectX::XMFLOAT2 scale)
	{
		_scale = scale;
		Update();
	}

	void Node::SetId(const std::string& id)
	{
		_id = id;
	}
	void Node::SetText(const std::wstring& text)
	{
		_text = text;
	}
	void Node::SetColor(UINT32 color)
	{
		_color = color;
	}
	void Node::SetFontSize(float fontSize)
	{
		_fontSize = fontSize;
	}
	void Node::SetTexture(ID3D11ShaderResourceView* texture)
	{
		_texture = texture;
	}

	DirectX::XMMATRIX* Node::GetModelMatrix()
	{
		return &_modelMatrix;
	}

	std::vector<GUI::Node*>* Node::GetChildren()
	{
		return &_children;
	}

	ID3D11ShaderResourceView* Node::GetTexture()
	{
		return _texture;
	}

	void Node::AddChild(Node* child)
	{
		_children.push_back(child);
	}
}