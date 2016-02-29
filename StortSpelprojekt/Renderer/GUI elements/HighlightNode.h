#pragma once
#include "Node.h"

namespace GUI
{
	class __declspec(dllexport) HighlightNode
	{
	public:
		GUI::Node* _triggerNode;
		GUI::Node* _highlightNode;
		DirectX::XMFLOAT4 _color;
	public:
		HighlightNode(GUI::Node* triggerNode, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f), GUI::Node* highlightNode = nullptr);
		~HighlightNode();
	};
}