#pragma once
#include "Node.h"
#include "CommonUtils.h"

namespace GUI
{
	class __declspec(dllexport) RadioButtonCollection 
	{
	private:
		Node* _selectedRadioButtonNode;
		std::vector<GUI::Node*> _notSelectedRadioButtons;

	public:
		RadioButtonCollection();
		~RadioButtonCollection();
		void AddRadioButtonNode(GUI::Node* radioButtonNode);
		void SelectRadioButtonNode(GUI::Node* selectedRadioButtonNode);
		GUI::Node* GetSelectedRadioButtonNode();
		std::vector<GUI::Node*>* GetNotSelectedRadioButtonNodes();
	};
}