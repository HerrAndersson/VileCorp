#include "Node.h"

namespace GUI
{
	Node::Node(DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale)
	{
		_position = position;
		_scale = scale;
	}


	Node::~Node()
	{
	}

	void Node::Update()
	{
		DirectX::XMMATRIX transform = DirectX::XMMatrixTranslation(_position.x, _position.y, 0);
		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(_scale.x, _scale.y, 1);

		_modelMatrix = DirectX::XMMatrixMultiply(scale, transform);
		_modelMatrix = DirectX::XMMatrixTranspose(_modelMatrix);
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

	DirectX::XMMATRIX* Node::GetModelMatrix()
	{
		return &_modelMatrix;
	}

	std::vector<GUI::Node*>* Node::GetChildren()
	{
		return &_children;
	}
}