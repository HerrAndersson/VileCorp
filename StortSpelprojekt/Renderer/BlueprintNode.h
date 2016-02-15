#pragma once
#include "Node.h"

namespace GUI
{
	class __declspec(dllexport) BlueprintNode : public Node
	{
		friend class UITree;

	private:
		int _type, _subtype, _textureId;

	public:
		BlueprintNode(Node::NodeInfo* info,
			int type = 0,
			int subtype = 0,
			int textureId = 0,
			DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(0.0f, 0.0f),
			DirectX::XMFLOAT2 scale = DirectX::XMFLOAT2(1.0f, 1.0f),
			DirectX::XMFLOAT4 colorOffset = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
			ID3D11ShaderResourceView* texture = nullptr,
			const std::string& id = "parent",
			const std::wstring& text = L"",
			UINT32 color = 0xFF0000FF,
			float fontSize = 32.0f,
			bool centered = false,
			bool hidden = false);
		~BlueprintNode();
		int GetType() const;
		int GetSubType() const;
		int GetTextureId() const;
	};
}