#pragma once
#include "Node.h"
#include "CommonUtils.h"

namespace GUI
{
	class __declspec(dllexport) NodeBar
	{
	private:
		std::vector<Node*> _attachedGUINodes;
		bool _verticalOrientation;
		DirectX::XMFLOAT2 _position;
		float _nodeOffset;

	public:
		NodeBar(bool verticalOrientation = false, DirectX::XMFLOAT2 position = {0,0}, float nodeOffset = 0.2f);
		~NodeBar();
		std::vector<GUI::Node*>* GetAttachedGUINodes();
		void OrganizeNodes();
	};
}