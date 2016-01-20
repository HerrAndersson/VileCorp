#include "Node.h"

namespace GUI
{
	Node::Node(FontWrapper* fontWrapper,
		DirectX::XMFLOAT2 position,
		DirectX::XMFLOAT2 scale,
		ID3D11ShaderResourceView* texture,
		const std::string& id,
		const std::wstring& text,
		UINT32 color,
		float fontSize)
	{
		_fontWrapper = fontWrapper;
		_position = position;
		_scale = scale;
		_id = id;
		_text = text;
		_texture = texture;
		_fontSize = fontSize;
		_color = color;

		UpdateMatrix();
		UpdateFont();
	}


	Node::~Node()
	{
	}

	void Node::UpdateMatrix()
	{
		DirectX::XMMATRIX transform = DirectX::XMMatrixTranslation(_position.x, _position.y, 0);
		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(_scale.x, _scale.y, 1);

		_modelMatrix = DirectX::XMMatrixMultiply(scale, transform);
	}

	void Node::UpdateFont()
	{
		IDWriteFactory* writeFactory = _fontWrapper->GetWriteFactory();

		HRESULT hr;
		if (_font._textFormat)
		{
			_font._textFormat->Release();
		}
		hr = writeFactory->CreateTextFormat(
				_fontWrapper->GetFontName(),
				_fontWrapper->GetFontCollection(),
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				_fontSize,
				_fontWrapper->GetFontName(),
				&_font._textFormat
			);
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to update TextFormat");
		}
		//Update text layout
		if (_font._textLayout)
		{
			_font._textLayout->Release();
		}
		hr = writeFactory->CreateTextLayout(
				_text.c_str(),
				_text.length(),
				_font._textFormat,
				//TODO: Set text bounding box
				0.0f,
				0.0f,
				&_font._textLayout
			);
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to update TextLayout");
		}

		unsigned int uintTextSize = (UINT32)_text.length();
		DWRITE_TEXT_RANGE allText = { 0, uintTextSize };
		_font._textLayout->SetFontSize(uintTextSize, allText);
		_font._textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	}

	void Node::SetPosition(DirectX::XMFLOAT2 position)
	{
		_position = position;
		UpdateMatrix();
	}

	void Node::SetScale(DirectX::XMFLOAT2 scale)
	{
		_scale = scale;
		UpdateMatrix();
		UpdateFont();
	}

	void Node::SetId(const std::string& id)
	{
		_id = id;
	}
	void Node::SetText(const std::wstring& text)
	{
		_text = text;
		UpdateFont();
	}
	void Node::SetColor(UINT32 color)
	{
		_color = color;
		UpdateFont();
	}
	void Node::SetFontSize(float fontSize)
	{
		_fontSize = fontSize;
		UpdateFont();
	}
	void Node::SetTexture(ID3D11ShaderResourceView* texture)
	{
		_texture = texture;
	}

	DirectX::XMFLOAT2 Node::GetPosition() const
	{
		return _position;
	}
	DirectX::XMFLOAT2 Node::GetScale() const
	{
		return _scale;		
	}
	std::string Node::GetId() const
	{
		return _id;
	}
	std::wstring& Node::GetText()
	{
		return _text;
	}
	UINT32 Node::GetColor() const
	{
		return _color;
	}
	float Node::GetFontSize() const
	{
		return _fontSize;
	}

	ID3D11ShaderResourceView* Node::GetTexture()
	{
		return _texture;
	}

	DirectX::XMMATRIX* Node::GetModelMatrix()
	{
		return &_modelMatrix;
	}

	std::vector<GUI::Node*>* Node::GetChildren()
	{
		return &_children;
	}
	void Node::AddChild(Node* child)
	{
		_children.push_back(child);
	}
	void* Node::operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}
	void Node::operator delete(void* p)
	{
		_mm_free(p);
	}
}