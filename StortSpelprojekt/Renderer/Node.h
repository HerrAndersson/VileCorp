#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>

#pragma warning (disable : 4251)

namespace GUI
{
	class __declspec(dllexport) Node
	{
	private:
		DirectX::XMFLOAT2 _position;
		DirectX::XMFLOAT2 _scale;
		DirectX::XMMATRIX _modelMatrix;
		void Update();
	protected:
		std::vector<Node*> _children;
	public:
		Node(DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale);
		virtual ~Node();

		void Node::SetPosition(DirectX::XMFLOAT2 position);
		void Node::SetScale(DirectX::XMFLOAT2 scale);
		DirectX::XMMATRIX* Node::GetModelMatrix();
		std::vector<GUI::Node*>* GetChildren();
	};
}