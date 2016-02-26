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
		DirectX::XMFLOAT3 _position;
		float _nodeOffset;

	public:
		NodeBar(bool verticalOrientation, DirectX::XMFLOAT3 position, float nodeOffset);
		~NodeBar();
		std::vector<GUI::Node*>* GetAttachedGUINodes();
		void OrganizeNodes();
	};
}