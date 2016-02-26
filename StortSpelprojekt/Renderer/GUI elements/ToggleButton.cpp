#include "ToggleButton.h"



GUI::ToggleButton::ToggleButton(Node* attachedGUINode, bool isToggled /*= false*/, DirectX::XMFLOAT4 colorOffset /*= DirectX::XMFLOAT4( 0.4f, 0.4f, 0.4f, 1.0f)*/)
{
	_attachedGUINode = attachedGUINode;
	_isToggled = isToggled;
	_colorOffset = colorOffset;
}

GUI::ToggleButton::~ToggleButton()
{}

GUI::Node* GUI::ToggleButton::GetAttachedGUINode() const
{
	return _attachedGUINode;
}

void GUI::ToggleButton::SetAttachedGUINode(GUI::Node* attachedGUINode)
{
	_attachedGUINode = attachedGUINode;
}

bool GUI::ToggleButton::GetIsToggled() const
{
	return _isToggled;
}

void GUI::ToggleButton::SetIsToggled(bool isToggled)
{
	if (isToggled != _isToggled)
	{
		_isToggled = isToggled;
		DirectX::XMFLOAT4 currentColorOffset = _attachedGUINode->GetColorOffset();
		if (_isToggled)
		{
			currentColorOffset.x += _colorOffset.x;
			currentColorOffset.y += _colorOffset.y;
			currentColorOffset.z += _colorOffset.z;
		}
		else
		{
			currentColorOffset.x -= _colorOffset.x;
			currentColorOffset.y -= _colorOffset.y;
			currentColorOffset.z -= _colorOffset.z;
		}
		_attachedGUINode->SetColorOffset(currentColorOffset);
	}
	
}

void GUI::ToggleButton::Toggle()
{
	SetIsToggled(!_isToggled);
}
