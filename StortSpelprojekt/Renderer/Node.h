#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

#pragma warning (disable : 4251)

namespace GUI
{
	class __declspec(dllexport) Node
	{
	private:
		DirectX::XMFLOAT2 _position;
		DirectX::XMFLOAT2 _scale;
		DirectX::XMMATRIX _modelMatrix;

		std::string _id;
		//Text info
		std::wstring _text;
		UINT32 _color;
		float _fontSize;
		//Image info
		ID3D11ShaderResourceView* _texture;

		void Update();
	protected:
		std::vector<Node*> _children;
	public:
		Node(DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(0.0f, 0.0f),
			DirectX::XMFLOAT2 scale = DirectX::XMFLOAT2(1.0f, 1.0f),
			ID3D11ShaderResourceView* texture = nullptr,
			const std::string& id = "parent",
			const std::wstring& text = L"",
			UINT32 _color = 0,
			float _fontSize = 0.0f);
		virtual ~Node();

		void SetPosition(DirectX::XMFLOAT2 position);
		void SetScale(DirectX::XMFLOAT2 scale);
		void SetId(const std::string& id);
		void SetText(const std::wstring& text);
		void SetColor(UINT32 color);
		void SetFontSize(float fontSize);
		void SetTexture(ID3D11ShaderResourceView* texture);


		ID3D11ShaderResourceView* GetTexture();

		DirectX::XMMATRIX* Node::GetModelMatrix();
		std::vector<GUI::Node*>* GetChildren();

		void AddChild(Node* child);
	};
}