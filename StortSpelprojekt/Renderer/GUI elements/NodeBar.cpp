#include "NodeBar.h"



GUI::NodeBar::NodeBar(bool verticalOrientation, DirectX::XMFLOAT2 position, float nodeOffset, std::vector<Node*>* attachedGUINodes)
{
	_verticalOrientation = verticalOrientation;
	_position = position;
	_nodeOffset = nodeOffset;
	_attachedGUINodes = attachedGUINodes;
}

GUI::NodeBar::~NodeBar()
{
}

std::vector<GUI::Node*>* GUI::NodeBar::GetAttachedGUINodes()
{
	return _attachedGUINodes;
}

void GUI::NodeBar::OrganizeNodes()
{
	if (!_attachedGUINodes->empty())
	{
		DirectX::XMFLOAT2 iteratedPosition = _position;
		for each (Node* node in *_attachedGUINodes)
		{
			node->SetPosition(iteratedPosition);
			if (_verticalOrientation)
			{
				iteratedPosition.y += _nodeOffset;
			}
			else
			{
				iteratedPosition.x += _nodeOffset;
			}
		}
	}
}