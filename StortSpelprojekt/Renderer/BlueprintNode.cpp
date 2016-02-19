#include "BlueprintNode.h"
namespace GUI
{
	BlueprintNode::BlueprintNode(Node::NodeInfo* info, Blueprint* blueprint, int textureId, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale, DirectX::XMFLOAT4 colorOffset, ID3D11ShaderResourceView* texture, const std::string& id, const std::wstring& text, UINT32 color, float fontSize, bool centered, bool hidden)
		: Node(info, position, scale, colorOffset, texture, id, text, color, fontSize, centered, hidden)
	{
		_textureId = textureId;
		_blueprint = blueprint;
	}
	BlueprintNode::BlueprintNode(GUI::Node* node, Blueprint * blueprint, int textureId)
		: Node(*node)
	{
		_textureId = textureId;
		_blueprint = blueprint;
	}
	BlueprintNode::~BlueprintNode()
	{
	}
	Blueprint* BlueprintNode::GetBlueprint() const
	{
		return _blueprint;
	}
	int BlueprintNode::GetTextureId() const
	{
		return _textureId;
	}
}