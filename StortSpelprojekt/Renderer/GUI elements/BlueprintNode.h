#pragma once
#include "Node.h"
#include "CommonUtils.h"

namespace GUI
{
	class __declspec(dllexport) BlueprintNode : public Node
	{
		friend class UITree;

	private:
		int _type;
		int _subType;
		int _textureId;
		System::Blueprint* _blueprint;

	public:
		BlueprintNode(Node::NodeInfo* info,
			System::Blueprint* blueprint = nullptr,
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
		BlueprintNode(GUI::Node &node,
			System::Blueprint* blueprint,
			int textureId = 0);
		~BlueprintNode();
		
		int GetType()const;
		void SetType(const int& type);
		int GetSubType()const;
		void SetSubType(const int& subType);
		System::Blueprint* GetBlueprint() const;
		void SetBlueprint(System::Blueprint* blueprint);
		int GetTextureId() const;
		void SetTextureId(const int& textureId);

	};
}