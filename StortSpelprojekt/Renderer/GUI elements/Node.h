#pragma once
#include "../RenderUtils.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

#include "../FontWrapper.h"

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
			unsigned int _screenWidth;
			unsigned int _screenHeight;
			unsigned int _windowWidth;
			unsigned int _windowHeight;
			unsigned int _screenMode;

			NodeInfo(FontWrapper* fontWrapper, unsigned int screenWidth, unsigned int screenHeight, unsigned int windowWidth, unsigned int windowHeight, unsigned int screenMode)
			{
				_fontWrapper = fontWrapper;
				_screenWidth = screenWidth;
				_screenHeight = screenHeight;
				_windowWidth = windowWidth;
				_windowHeight = windowHeight;
				_screenMode = screenMode;
			}
		};

	private:

		DirectX::XMMATRIX _modelMatrix;
		DirectX::XMFLOAT2 _position, _positionFinal;
		DirectX::XMFLOAT2 _scale;
		DirectX::XMFLOAT4 _colorOffset;

		std::string _id;
		NodeInfo* _info;
		bool _hidden;
		
		//Text info
		std::wstring _text;
		UINT32 _color;
		float _fontSize;
		FontWrapper::CustomFont _font;
		bool _centered;

		//Image info
		Texture* _texture = nullptr;

		void UpdateMatrix();
	protected:
		Node* _parent = nullptr;
		std::vector<Node*> _children;
	public:

		Node(NodeInfo* info,
			DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(0.0f, 0.0f),
			DirectX::XMFLOAT2 scale = DirectX::XMFLOAT2(1.0f, 1.0f),
			DirectX::XMFLOAT4 colorOffset = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
			Texture* texture = nullptr,
			const std::string& id = "parent",
			const std::wstring& text = L"",
			UINT32 color = 0xFF0000FF,
			float fontSize = 32.0f,
			bool centered = false,
			bool hidden = false);

		virtual ~Node();

		void SetPosition(DirectX::XMFLOAT2 position);
		void SetParentPosition(DirectX::XMFLOAT2 position);
		void SetScale(DirectX::XMFLOAT2 scale);
		void SetId(const std::string& id);
		void SetText(const std::wstring& text);
		void SetColor(UINT32 color);
		void SetFontSize(float fontSize);
		void SetTexture(Texture* texture);
		void SwapTexture(Node* otherNode);
		void SetColorOffset(const DirectX::XMFLOAT4& colorOffset);
		void SetAlpha(float alpha);
		void SetCentered(bool centered);
		void SetHidden(bool hidden);
		void SetParent(Node* parent);

		DirectX::XMFLOAT4 GetColorOffset() const;
		float GetAlpha() const;
		DirectX::XMFLOAT2 GetFinalPosition() const;
		DirectX::XMFLOAT2 GetLocalPosition() const;
		DirectX::XMFLOAT2 GetScale() const;
		std::string GetId() const;
		std::wstring& GetText();
		UINT32 GetColor() const;
		float GetFontSize() const;
		FontWrapper::CustomFont* GetFont();
		Texture* GetTexture();
		bool GetCentered() const;
		bool GetHidden() const;
		Node* GetParent() const;

		DirectX::XMMATRIX* Node::GetModelMatrix();
		std::vector<GUI::Node*>* GetChildren();

		void AddChild(Node* child);
		void UpdateFont();

		//Overloading these guarantees 16B alignment of XMMATRIX
		void* operator new(size_t i);
		void operator delete(void* p);
	};
}