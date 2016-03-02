#include "RadioButtonCollection.h"



GUI::RadioButtonCollection::RadioButtonCollection()
{
	_selectedRadioButtonNode = nullptr;
}

GUI::RadioButtonCollection::~RadioButtonCollection()
{

}

void GUI::RadioButtonCollection::AddRadioButtonNode(GUI::Node* radioButtonNode)
{
	_notSelectedRadioButtons.push_back(radioButtonNode);
}

void GUI::RadioButtonCollection::SelectRadioButtonNode(GUI::Node* selectedRadioButtonNode)
{
	if (selectedRadioButtonNode != _selectedRadioButtonNode)
	{
		GUI::Node* oldSelectedRadioButtonNode = _selectedRadioButtonNode;
		_selectedRadioButtonNode = selectedRadioButtonNode;

		if (oldSelectedRadioButtonNode != nullptr)
		{
			oldSelectedRadioButtonNode->SwapTexture(_selectedRadioButtonNode);
		}

		for (int i = 0; i < _notSelectedRadioButtons.size(); i++)
		{
			if (_notSelectedRadioButtons[i]->GetId() == _selectedRadioButtonNode->GetId())
			{
				_notSelectedRadioButtons.erase(_notSelectedRadioButtons.begin() + i);
				i = _notSelectedRadioButtons.size();
			}
		}
		if (oldSelectedRadioButtonNode != nullptr)
		{
			_notSelectedRadioButtons.push_back(oldSelectedRadioButtonNode);
		}
	}
}

GUI::Node* GUI::RadioButtonCollection::GetSelectedRadioButtonNode()
{
	return _selectedRadioButtonNode;
}

std::vector<GUI::Node*>* GUI::RadioButtonCollection::GetNotSelectedRadioButtonNodes()
{
	return &_notSelectedRadioButtons;
}
