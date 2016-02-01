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
		friend class UITree;
	public:
		struct NodeInfo
		{
			FontWrapper* _fontWrapper;
			int _screenWidth;
			int _screenHeight;

			NodeInfo(FontWrapper* fontWrapper, int screenWidth, int screenHeight)
			{
				_fontWrapper = fontWrapper;
				_screenWidth = screenWidth;
				_screenHeight = screenHeight;
			}
		};
	private:

		DirectX::XMMATRIX _modelMatrix;
		DirectX::XMFLOAT2 _position;
		DirectX::XMFLOAT2 _scale;
		DirectX::XMFLOAT3 _colorOffset;

		std::string _id;
		NodeInfo* _info;

		//Text info
		std::wstring _text;
		UINT32 _color;
		float _fontSize;
		FontWrapper::CustomFont _font;

		//Image info
		ID3D11ShaderResourceView* _texture;

		void UpdateMatrix();
	protected:
		std::vector<Node*> _children;
	public:
		Node(NodeInfo* info, DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(0.0f, 0.0f),
			DirectX::XMFLOAT2 scale = DirectX::XMFLOAT2(1.0f, 1.0f), 
			DirectX::XMFLOAT3 colorOffset = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
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
		void SetColorOffset(DirectX::XMFLOAT3 colorOffset);

		DirectX::XMFLOAT3 GetColorOffset();
		DirectX::XMFLOAT2 GetPosition() const;
		DirectX::XMFLOAT2 GetScale() const;
		std::string GetId() const;
		std::wstring& GetText();
		UINT32 GetColor() const;
		float GetFontSize() const;
		FontWrapper::CustomFont* GetFont();
		ID3D11ShaderResourceView* GetTexture();
		DirectX::XMMATRIX* Node::GetModelMatrix();
		std::vector<GUI::Node*>* GetChildren();

		void AddChild(Node* child);
		void UpdateFont();
		//Overloading these guarantees 16B alignment of XMMATRIX
		void* operator new(size_t i);
		void operator delete(void* p);
	};
}