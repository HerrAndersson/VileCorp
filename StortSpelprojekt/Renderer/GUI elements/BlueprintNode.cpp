#include "BlueprintNode.h"
namespace GUI
{
	BlueprintNode::BlueprintNode(Node::NodeInfo* info, System::Blueprint* blueprint, int textureId, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale, DirectX::XMFLOAT4 colorOffset, Texture* texture, const std::string& id, const std::wstring& text, UINT32 color, float fontSize, bool centered, bool hidden)
		: Node(info, position, scale, colorOffset, texture, id, text, color, fontSize, centered, hidden)
	{
		_textureId = textureId;
		_blueprint = blueprint;
	}
	BlueprintNode::BlueprintNode(GUI::Node &node, System::Blueprint * blueprint, int textureId)
		: Node(node)
	{
		_textureId = textureId;
		_blueprint = blueprint;
	}

	BlueprintNode::~BlueprintNode()
	{
	}

	int BlueprintNode::GetType() const
	{
		return _type;
	}

	void BlueprintNode::SetType(const int& type)
	{
		_type = type;
	}

	int BlueprintNode::GetSubType() const
	{
		return _subType;
	}

	void BlueprintNode::SetSubType(const int& subType)
	{
		_subType = subType;
	}

	System::Blueprint* BlueprintNode::GetBlueprint() const
	{
		return _blueprint;
	}

	void BlueprintNode::SetBlueprint(System::Blueprint* blueprint)
	{
		_blueprint = blueprint;
	}

	int BlueprintNode::GetTextureId() const
	{
		return _textureId;
	}

	void BlueprintNode::SetTextureId(const int& textureId)
	{
		_textureId = textureId;
	}
}