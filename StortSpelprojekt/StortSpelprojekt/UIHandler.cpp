#include "UIHandler.h"

UIHandler::UIHandler(ID3D11Device* device)
{
	_device = device;
}

UIHandler::~UIHandler()
{
	for (auto i : _fonts)
	{
		i.Release();
	}
	_fonts.clear();
	std::vector<FontInfo>().swap(_fonts);
}

void UIHandler::Update()
{
}

int UIHandler::AddFont(const WCHAR* fontName, DirectX::XMFLOAT2 position, float fontSize, UINT32 color, const WCHAR* text)
{
	for (auto i : _fonts)
	{
		//if filepaths are not equal.
		if (0 != wcscmp(i.filePath, fontName))
		{
			//Then create a new font.
			_fonts.push_back(FontInfo(fontName, position, fontSize, color, text, _textId, false, _device));
		}
		else
		{
			//If filepaths are equal. Then we shall create a string inside that font.
			i.AddText(position, fontSize, color, text, _textId);
		}
	}
	
	return _textId++;
}

int UIHandler::AddCustomFont(const WCHAR* fontName, DirectX::XMFLOAT2 position, float fontSize, UINT32 color, const WCHAR* text)
{
	for (auto i : _fonts)
	{
		//if filepaths are not equal.
		if (0 != wcscmp(i.filePath, fontName))
		{
			//Then create a new font.
			_fonts.push_back(FontInfo(fontName, position, fontSize, color, text, _textId, true, _device));
		}
		else
		{
			//If filepaths are equal. Then we shall create a string inside that font.
			i.AddText(position, fontSize, color, text, _textId);
		}
	}

	return _textId++;
}

void UIHandler::Render(ID3D11DeviceContext* _deviceContext)
{
	for (auto i : _fonts)
	{
		for (auto j : i.text)
		{
			i.fontDevice.DrawString(_deviceContext, j.text, j.fontSize, j.position.x, j.position.y, j.color);
		}
	}
}

bool UIHandler::RemoveFont(const WCHAR* filePathOrName)
{
	bool rv = false;
	for (auto i : _fonts)
	{
		if (0 != wcscmp(i.filePath, filePathOrName))
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
	int incr = 0;
	for (auto i : _fonts)
	{
		incr = 0;
		for (auto j : i.text)
		{
			if (j.textId == id)
			{
				i.text.erase(i.text.begin()+incr);
				rv = true;
			}
			incr++;
		}
	}
	return rv;
}