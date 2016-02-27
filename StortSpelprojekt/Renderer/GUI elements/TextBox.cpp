#include "TextBox.h"

GUI::TextBox::TextBox(Node* attachedGUINode, int characterLimit, bool allowMultipleLines, bool allowOnlyNumbers, bool clearDefaultTextOnFirstEnter)
{
	_attachedGUINode = attachedGUINode;
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

GUI::Node* GUI::TextBox::GetAttachedGUINode() const
{
	return _attachedGUINode;
}

void GUI::TextBox::SetAttachedGUINode(GUI::Node* attachedGUINode)
{
	_attachedGUINode = attachedGUINode;
}

std::wstring GUI::TextBox::GetText() const
{
	if (_attachedGUINode != nullptr)
	{
		return _attachedGUINode->GetText();
	}
}

void GUI::TextBox::SetText(const std::wstring &text)
{
	if (_attachedGUINode != nullptr)
	{
		_attachedGUINode->SetText(text);
	}
}

void GUI::TextBox::SelectTextBox()
{
	//TODO: Add code for blinking text marker here /Rikhard

	if (_clearDefaultTextOnFirstEnter && _firstTimeEditingText)
	{
		SetText(StringToWstring(""));
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
