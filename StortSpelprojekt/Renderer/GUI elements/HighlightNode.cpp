#include "HighlightNode.h"

namespace GUI
{
	HighlightNode::HighlightNode(GUI::Node* triggerNode, DirectX::XMFLOAT4 color, GUI::Node* highlightNode)
	{
		_triggerNode = triggerNode;
		_color = color;
		if (highlightNode)
		{
			_highlightNode = highlightNode;
		}
		else
		{
			_highlightNode = triggerNode;
		}
	}

	HighlightNode::~HighlightNode()
	{
	}
}