#include "Node.h"
namespace GUI
{
	Node::Node(NodeInfo* info, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale, DirectX::XMFLOAT4 colorOffset, Texture* texture,
		const std::string& id, const std::wstring& text, UINT32 color, float fontSize, bool centered, bool hidden)
	{
		_info = info;
		_position = position;
		_positionFinal = position;
		_scale = scale;
		_id = id;
		_text = text;
		_texture = texture;
		_fontSize = fontSize;
		_color = color;
		_colorOffset = colorOffset;
		_centered = centered;
		_hidden = hidden;
		UpdateMatrix();
		UpdateFont();
	}

	Node::~Node()
	{
		if (_texture)
		{
			_texture->DecrementUsers();
		}
	}

	void Node::UpdateMatrix()
	{
		_modelMatrix = DirectX::XMMatrixTranslation(_position.x, _position.y, 0);
	}

	void Node::UpdateFont()
	{
		IDWriteFactory* writeFactory = _info->_fontWrapper->GetWriteFactory();
		float scaledFontSize = _fontSize * (_info->_screenHeight / 1080.0f);

		HRESULT hr;
		if (_font._textFormat)
		{
			_font._textFormat->Release();
		}
		hr = writeFactory->CreateTextFormat(_info->_fontWrapper->GetFontName(), _info->_fontWrapper->GetFontCollection(),
			DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, scaledFontSize, _info->_fontWrapper->GetFontName(), &_font._textFormat);
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to update TextFormat");
		}
		//Update text layout
		if (_font._textLayout)
		{
			_font._textLayout->Release();
		}
		
		float x = _scale.x * _info->_screenWidth;
		float y = _scale.y * _info->_screenHeight;

		hr = writeFactory->CreateTextLayout(_text.c_str(), _text.length(), _font._textFormat, x, y, &_font._textLayout);
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to update TextLayout");
		}

		unsigned int uintTextSize = (UINT32)_text.length();
		DWRITE_TEXT_RANGE allText = { 0, uintTextSize };
		_font._textLayout->SetFontSize(scaledFontSize, allText);
		_font._textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_WHOLE_WORD);
		if (_centered)
		{
			_font._textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		}
	}

	void Node::SetPosition(DirectX::XMFLOAT2 position)
	{
		_positionFinal.x += position.x - _position.x;
		_positionFinal.y += position.y - _position.y;
		_position = position;
		for (Node* child : _children)
		{
			child->SetParentPosition(_positionFinal);
		}
		UpdateMatrix();
	}

	void Node::SetParentPosition(DirectX::XMFLOAT2 parentPosition)
	{
		_positionFinal.x = parentPosition.x + _position.x;
		_positionFinal.y = parentPosition.y + _position.y;
		for (Node* child : _children)
		{
			child->SetParentPosition(_positionFinal);
		}
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
	void Node::SetTexture(Texture* texture)
	{
		if (_texture)
		{
			_texture->DecrementUsers();
		}
		_texture = texture;
	}

	void Node::SwapTexture(Node* otherNode)
	{
		Texture* temp = _texture;
		_texture = otherNode->_texture;
		otherNode->_texture = temp;
	}

	void Node::SetColorOffset(const DirectX::XMFLOAT4& colorOffset)
	{
		_colorOffset = colorOffset;
	}

	void Node::SetAlpha(float alpha)
	{
		_colorOffset.w = alpha;
	}

	DirectX::XMFLOAT4 Node::GetColorOffset() const
	{
		return _colorOffset;
	}
	float Node::GetAlpha() const
	{
		return _colorOffset.w;
	}

	void Node::SetCentered(bool centered)
	{
		_centered = centered;
		UpdateFont();
	}

	void Node::SetHidden(bool hidden)
	{
		_hidden = hidden;
	}

	void Node::SetParent(Node* parent)
	{
		_parent = parent;
	}

	DirectX::XMFLOAT2 Node::GetFinalPosition() const
	{
		return _positionFinal;
	}
	DirectX::XMFLOAT2 Node::GetLocalPosition() const
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

	FontWrapper::CustomFont* Node::GetFont()
	{
		return &_font;
	}

	Texture* Node::GetTexture()
	{
		return _texture;
	}

	bool Node::GetCentered() const
	{
		return _centered;
	}

	bool Node::GetHidden() const
	{
		return _hidden;
	}

	Node* Node::GetParent() const
	{
		return _parent;
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