#include "NodeBar.h"



GUI::NodeBar::NodeBar(std::vector<Node*>* buttons, std::vector<std::string>* availableUnits, GUI::Node* unitList)
{
	_buttons = buttons;
	_availableUnits = availableUnits;
	_unitList = unitList;
}

GUI::NodeBar::~NodeBar()
{
}

std::vector<GUI::Node*>* GUI::NodeBar::GetButtons()
{
	return _buttons;
}

void GUI::NodeBar::OrganizeButtons()
{
	if (_availableUnits != nullptr && _unitList != nullptr && _buttons != nullptr)
	{
		AddButtons();

		if (!_buttons->empty())
		{
			PositionButtons();
		}
	}
}

void GUI::NodeBar::PositionButtons()
{
	int maxNrOfButtons = _unitList->GetChildren()->size();
	int padding = 1;
	float spaceForSingleButton = 2.0f / (maxNrOfButtons + padding);
	float buttonNodesOffset = spaceForSingleButton * (_availableUnits->size() * 0.5f);
	DirectX::XMFLOAT2 iteratedPosition = {-buttonNodesOffset, 0};
	for each (Node* node in *_buttons)
	{
		node->SetPosition(iteratedPosition);
		if (_verticalOrientation)
		{
			iteratedPosition.y += spaceForSingleButton;
		}
		else
		{
			iteratedPosition.x += spaceForSingleButton;
		}
	}
}

void GUI::NodeBar::AddButtons()
{
	_buttons->clear();
	for (GUI::Node* button : *_unitList->GetChildren())
	{
		if (button->GetId() == "Camera")
		{
			_buttons->push_back(button);
		}
		else
		{
			button->SetHidden(true);
			for (std::string availableUnitName : *_availableUnits)
			{
				if (button->GetId() == availableUnitName)
				{
					_buttons->push_back(button);
					button->SetHidden(false);
				}
			}
		}
	}
}
