#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

#include "FontWrapper.h"

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

		FontWrapper::CustomFont _font;
		FontWrapper* _fontWrapper;
		//Image info
		ID3D11ShaderResourceView* _texture;

		void UpdateMatrix();
		void UpdateFont();
	protected:
		std::vector<Node*> _children;
	public:
		Node(FontWrapper* fontWrapper, DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(0.0f, 0.0f),
			DirectX::XMFLOAT2 scale = DirectX::XMFLOAT2(1.0f, 1.0f),
			ID3D11ShaderResourceView* texture = nullptr,
			const std::string& id = "parent",
			const std::wstring& text = L"",
			UINT32 color = 0xFF0000FF,
			float fontSize = 32.0f);
		virtual ~Node();

		void SetPosition(DirectX::XMFLOAT2 position);
		void SetScale(DirectX::XMFLOAT2 scale);
		void SetId(const std::string& id);
		void SetText(const std::wstring& text);
		void SetColor(UINT32 color);
		void SetFontSize(float fontSize);
		void SetTexture(ID3D11ShaderResourceView* texture);

		DirectX::XMFLOAT2 GetPosition() const;
		DirectX::XMFLOAT2 GetScale() const;
		std::string GetId() const;
		std::wstring& GetText();
		UINT32 GetColor() const;
		float GetFontSize() const;
		ID3D11ShaderResourceView* GetTexture();

		DirectX::XMMATRIX* Node::GetModelMatrix();
		std::vector<GUI::Node*>* GetChildren();

		void AddChild(Node* child);
		//Overloading these guarantees 16B alignment of XMMATRIX
		void* operator new(size_t i);
		void operator delete(void* p);
	};
}