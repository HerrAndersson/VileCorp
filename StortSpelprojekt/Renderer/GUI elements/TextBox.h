#pragma once
#include "Node.h"
#include "CommonUtils.h"

namespace GUI
{
	class __declspec(dllexport) TextBox
	{
	private:
		Node* _textNode;
		Node* _boxNode;
		int _characterLimit;
		bool _allowMultipleLines;
		bool _allowOnlyNumbers;  //TODO: Enforce this /Rikhard
		bool _clearDefaultTextOnFirstEnter;
		bool _firstTimeEditingText;
		bool _isSelectedForEditing;

	public:
		TextBox(Node* textNode, int characterLimit = 1000, bool allowMultipleLines = false, bool allowOnlyNumbers = false, bool clearDefaultTextOnFirstEnter = false);
		~TextBox();
		GUI::Node* GetTextNode() const;
		void SetTextNode(GUI::Node* textNode);
		GUI::Node* GetBoxNode() const;
		void SetBoxNode(GUI::Node* boxNode);
		std::wstring GetText() const;
		void SetText(const std::wstring &text);
		int GetCharacterLimit() const;
		void SetCharacterLimit(const int &characterLimit);
		bool GetOnlyNumbersAllowed();
		void SetOnlyNumbersAllowed(bool &allowOnlyNumbers);
		void SelectTextBox();
		void DeselectTextBox();
		bool GetAllowMultipleLines() const;
	};
}