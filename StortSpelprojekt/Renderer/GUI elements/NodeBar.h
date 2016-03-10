#pragma once
#include "Node.h"
#include "CommonUtils.h"

namespace GUI
{
	class __declspec(dllexport) NodeBar
	{
	private:
		std::vector<Node*>* _buttons;
		std::vector<std::string>* _availableUnits;
		GUI::Node* _unitList;
		bool _verticalOrientation;
		DirectX::XMFLOAT2 _position;
		float _nodeOffset;

		void AddButtons();
		void PositionButtons();

	public:
		NodeBar(bool verticalOrientation = false, float nodeOffset = 0.1f, std::vector<Node*>* attachedGUINodes = nullptr, std::vector<std::string>* availableUnits = nullptr, GUI::Node* unitList = nullptr);
		~NodeBar();
		std::vector<GUI::Node*>* GetButtons();
		void OrganizeButtons();
	};
}