#pragma once
#include "Node.h"
#include "CommonUtils.h"

namespace GUI
{
	class __declspec(dllexport) TextBox
	{
	private:
		Node* _attachedGUINode;
		int _characterLimit;  //TODO: Enforce this /Rikhard
		bool _allowMultipleLines;
		bool _allowOnlyNumbers;  //TODO: Enforce this /Rikhard
		bool _clearDefaultTextOnFirstEnter;
		bool _firstTimeEditingText;
		bool _isSelectedForEditing;

	public:
		TextBox(Node* attachedGUINode, int characterLimit = 1000, bool allowMultipleLines = false, bool allowOnlyNumbers = false, bool clearDefaultTextOnFirstEnter = false);
		~TextBox();
		GUI::Node* GetAttachedGUINode() const;
		void SetAttachedGUINode(GUI::Node* attachedGUINode);
		std::wstring GetText() const;
		void SetText(const std::wstring &text);
		void SelectTextBox();
		void DeselectTextBox();
		bool GetAllowMultipleLines() const;
	};
}