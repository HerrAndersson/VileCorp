#include "UIHandler.h"

UIHandler::UIHandler(ID3D11Device* device, System::WindowSettings windowSettings, AssetManager* assetManager)
{
	_device			= device;
	_textId			= 0;
	_windowSettings	= windowSettings;
	_AM				= assetManager;
	_textureId		= 0;

	_fontWrapper.CreateFontWrapper(device);
}

UIHandler::~UIHandler()
{
	for (auto i : _fonts)
	{
		i.Release();
	}
}

void UIHandler::Update()
{
}

int UIHandler::AddFont(const WCHAR* filePath, const WCHAR* fontName, DirectX::XMFLOAT2 position, float fontSize, UINT32 color, std::wstring text)
{
	if (_textId == 0) //if first element
	{
		_fonts.push_back(FontInfo(filePath, fontName, position, fontSize, color, text, _textId, false, _device));
	}
	else
	{
		int found = -1;
		int incr = 0;
		for (auto i : _fonts)
		{
			//if filepaths are equal.
			if (0 == wcscmp(i.GetFilePath(), fontName))
			{
				found = incr;
			}
			incr++;
		}
		//if no previous font found. Create a new font.
		if (found == -1)
		{
			//Then create a new font.
			_fonts.push_back(FontInfo(filePath, fontName, position, fontSize, color, text, _textId, false, _device));
		}
		else
		{
			//If filepaths are equal. Then we shall create a string inside that font.
			_fonts.at(found).AddText(position, fontSize, color, text, _textId);
		}
	}

	return _textId++;
}

int UIHandler::AddCustomFont(const WCHAR* filePath, const WCHAR* fontName, DirectX::XMFLOAT2 position, float fontSize, UINT32 color, std::wstring text)
{
	if (_textId == 0) //if first element
	{
		_fonts.push_back(FontInfo(_fontWrapper.GetFontWrapper(), filePath, fontName, position, fontSize, color, text, _textId, true, _device));
	}
	else
	{
		int found = -1;
		int incr = 0;
		for (auto i : _fonts)
		{
			//if filepaths are equal.
			if (0 == wcscmp(i.GetFilePath(), fontName))
			{
				found = incr;
			}
			incr++;
		}
		//if no previous font found. Create a new font.
		if (found == -1)
		{
			//Then create a new font.
			_fonts.push_back(FontInfo(_fontWrapper.GetFontWrapper(), filePath, fontName, position, fontSize, color, text, _textId, true, _device));
		}
		else
		{
			//If filepaths are equal. Then we shall create a string inside that font.
			_fonts.at(found).AddText(position, fontSize, color, text, _textId);
		}
	}

	return _textId++;
}

void UIHandler::Render(ID3D11DeviceContext* _deviceContext)
{
	for (auto &i : _fonts)
	{
		i.Render(_deviceContext);
	}
}

void UIHandler::OnResize(System::WindowSettings windowSettings)
{
	System::WindowSettings newSettings = windowSettings;
	DirectX::XMFLOAT2 scale;
	scale.x = (float)windowSettings._width / (float)_windowSettings._width;
	scale.y = (float)windowSettings._height / (float)_windowSettings._height;
	
	for (auto &i : _fonts)
	{
		i.OnResize(scale.x);
	}
}

bool UIHandler::RemoveFont(const WCHAR* filePath)
{
	bool rv = false;
	for (auto &i : _fonts)
	{
		if (0 == wcscmp(i.GetFilePath(), filePath))
		{
			i.Release();
			rv = true;
		}
	}

	return rv;
}

bool UIHandler::RemoveText(int id)
{
	bool rv = false;
	bool deleted = false;
	int incr = 0;
	for (auto &i : _fonts)
	{
		deleted = i.RemoveText(id);
		if (deleted)
		{
			rv = true;
		}
	}
	return rv;
}

void UIHandler::SetButtonVisibility(int id, bool visible)
{
	_textures[id].SetVisibility(visible);
}

bool UIHandler::GetButtonVisibility(int id)
{
	return _textures[id].GetVisibility();
}

bool UIHandler::Intersect(POINT mousePoint, int id)
{
	if (mousePoint.x > hitBox[id].x && mousePoint.y > hitBox[id].y)
	{
		if (mousePoint.x < hitBox[id].z && mousePoint.y < hitBox[id].w)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

int UIHandler::Add2DTexture(std::string filePath, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size, bool visible)
{
	ID3D11ShaderResourceView* tex = _AM->GetTexture(filePath);
	_textures.push_back(Renderer::HUDElement(position, size, tex, visible));

	return _textureId++;
}

int UIHandler::AddButton(std::string filePath, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size, bool visible)
{
	int buttonId = Add2DTexture(filePath, position, size, visible);
	hitBox.push_back(DirectX::XMFLOAT4(position.x - size.x, position.y + size.y, position.x + size.x, position.y - size.y));

	hitBox.back().x += 1.0f;
	hitBox.back().x /= 2.0f;
	hitBox.back().x *= (float)_windowSettings._width;
	hitBox.back().z += 1.0f;
	hitBox.back().z /= 2.0f;
	hitBox.back().z *= (float)_windowSettings._width;

	hitBox.back().y -= 1.0f;
	hitBox.back().y /= -2.0f;
	hitBox.back().y *= (float)_windowSettings._height;
	hitBox.back().w -= 1.0f;
	hitBox.back().w /= -2.0f;
	hitBox.back().w *= (float)_windowSettings._height;

	return buttonId;
}

std::vector<Renderer::HUDElement>* UIHandler::GetTextureData()
{
	return &_textures;
}

void* UIHandler::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void UIHandler::operator delete(void* p)
{
	_mm_free(p);
}