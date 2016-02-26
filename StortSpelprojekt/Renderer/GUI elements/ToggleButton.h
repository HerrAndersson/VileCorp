#pragma once
#include "Node.h"
#include "CommonUtils.h"

namespace GUI
{
	class __declspec(dllexport) ToggleButton
	{
	private:
		Node* _attachedGUINode;
		bool _isToggled;
		DirectX::XMFLOAT4 _colorOffset;

	public:
		ToggleButton(Node* attachedGUINode, bool isToggled = false, DirectX::XMFLOAT4 colorOffset = DirectX::XMFLOAT4( 0.4f, 0.4f, 0.4f, 1.0f));
		~ToggleButton();
		GUI::Node* GetAttachedGUINode() const;
		void SetAttachedGUINode(GUI::Node* attachedGUINode);
		bool GetIsToggled() const;
		void SetIsToggled(bool isToggled);
		void Toggle();
	};
}