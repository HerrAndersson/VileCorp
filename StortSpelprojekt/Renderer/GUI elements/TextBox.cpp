#include "TextBox.h"

GUI::TextBox::TextBox(Node* textNode, int characterLimit, bool allowMultipleLines, bool allowOnlyNumbers, bool clearDefaultTextOnFirstEnter)
{
	_textNode = textNode;
	_boxNode = _textNode->GetParent();
	_characterLimit = characterLimit;
	_allowMultipleLines = allowMultipleLines;
	_allowOnlyNumbers = allowOnlyNumbers;
	_clearDefaultTextOnFirstEnter = clearDefaultTextOnFirstEnter;
	_firstTimeEditingText = true;
	_isSelectedForEditing = false;
}

GUI::TextBox::~TextBox()
{

}

GUI::Node* GUI::TextBox::GetTextNode() const
{
	return _textNode;
}

void GUI::TextBox::SetTextNode(GUI::Node* textNode)
{
	_textNode = textNode;
}

GUI::Node* GUI::TextBox::GetBoxNode() const
{
	return _boxNode;
}

void GUI::TextBox::SetBoxNode(GUI::Node* boxNode)
{
	_boxNode = boxNode;
}

std::wstring GUI::TextBox::GetText() const
{
	if (_textNode != nullptr)
	{
		return _textNode->GetText();
	}
}

void GUI::TextBox::SetText(const std::wstring &text)
{
	if (_textNode != nullptr)
	{
		_textNode->SetText(text);
	}
}

int GUI::TextBox::GetCharacterLimit() const
{
	return _characterLimit;
}

void GUI::TextBox::SetCharacterLimit(const int &characterLimit)
{
	_characterLimit = characterLimit;
}

bool GUI::TextBox::GetOnlyNumbersAllowed()
{
	return _allowOnlyNumbers;
}

void GUI::TextBox::SetOnlyNumbersAllowed(bool &allowOnlyNumbers)
{
	_allowOnlyNumbers = allowOnlyNumbers;
}

void GUI::TextBox::SelectTextBox()
{
	//TODO: Add code for blinking text marker here /Rikhard

	if (_clearDefaultTextOnFirstEnter && _firstTimeEditingText)
	{
		SetText(System::StringToWstring(""));
		_firstTimeEditingText = false;
	}
}

void GUI::TextBox::DeselectTextBox()
{

}

bool GUI::TextBox::GetAllowMultipleLines() const
{
	return _allowMultipleLines;
}
