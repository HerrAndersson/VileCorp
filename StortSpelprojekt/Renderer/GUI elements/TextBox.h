#pragma once
#include <cctype>
#include "Node.h"
#include "CommonUtils.h"

namespace GUI
{
	class __declspec(dllexport) TextBox
	{
	private:
		const std::wstring TEXT_MARKER = L"|";
		const int TOGGLE_TEXT_MARKER_ON_FRAME = 30;

		Node* _textNode;
		Node* _boxNode;
		int _characterLimit;
		bool _allowMultipleLines;
		bool _allowOnlyNumbers;
		bool _clearDefaultTextOnFirstEnter;
		bool _firstTimeEditingText;
		bool _isSelectedForEditing;
		bool _showTextMarker;
		int _textMarkerFrameTimer;
		std::wstring _oldText;

		bool IsInteger(const std::string & s);

	public:
		TextBox(Node* textNode, int characterLimit = 1000, bool allowMultipleLines = false, bool allowOnlyNumbers = false, bool clearDefaultTextOnFirstEnter = false);
		~TextBox();
		void Update();
		GUI::Node* GetTextNode() const;
		void SetTextNode(GUI::Node* textNode);
		GUI::Node* GetBoxNode() const;
		void SetBoxNode(GUI::Node* boxNode);
		std::wstring GetText() const;
		void SetText(std::wstring text);
		int GetCharacterLimit() const;
		void SetCharacterLimit(const int &characterLimit);
		bool GetOnlyNumbersAllowed();
		void SetOnlyNumbersAllowed(bool &allowOnlyNumbers);
		void SelectTextBox();
		void DeselectTextBox();
		bool GetAllowMultipleLines() const;
	};
}