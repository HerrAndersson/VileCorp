#include "BlueprintNode.h"
namespace GUI
{
	BlueprintNode::BlueprintNode(Node::NodeInfo* info, int type, int subtype, int textureId, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale, DirectX::XMFLOAT4 colorOffset, ID3D11ShaderResourceView* texture, const std::string& id, const std::wstring& text, UINT32 color, float fontSize, bool centered, bool hidden)
		: Node(info, position, scale, colorOffset, texture, id, text, color, fontSize, centered, hidden)
	{
		_textureId = textureId;
		_type = type;
		_subtype = subtype;
	}
	BlueprintNode::~BlueprintNode()
	{
	}
	int BlueprintNode::GetType() const
	{
		return _type;
	}
	int BlueprintNode::GetSubType() const
	{
		return _subtype;
	}
	int BlueprintNode::GetTextureId() const
	{
		return _textureId;
	}
}