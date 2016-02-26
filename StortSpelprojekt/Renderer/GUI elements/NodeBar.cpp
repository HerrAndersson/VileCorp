#include "NodeBar.h"



GUI::NodeBar::NodeBar(bool verticalOrientation, DirectX::XMFLOAT3 position, float nodeOffset)
{
	_verticalOrientation = verticalOrientation;
	_position = position;
	_nodeOffset = nodeOffset;
}

GUI::NodeBar::~NodeBar()
{
}

std::vector<GUI::Node*>* GUI::NodeBar::GetAttachedGUINodes()
{
	return &_attachedGUINodes;
}

void GUI::NodeBar::OrganizeNodes()
{
	//DirectX::XMFLOAT2 
	//if (_verticalOrientation)
	//{
	//	_attachedGUINodes.at(0)->GetLocalPosition();
	//}
}