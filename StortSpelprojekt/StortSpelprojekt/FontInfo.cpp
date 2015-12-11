#include "FontInfo.h"

FontInfo::FontInfo(const WCHAR* filePath_,
	const WCHAR* fontName_,
	DirectX::XMFLOAT2 position_,
	float fontSize_,
	UINT32 color_,
	std::wstring text_,
	int textId_,
	bool custom_,
	ID3D11Device* device_)
{
	_filePath = filePath_;
	_fontName = fontName_;
	_text.push_back(TextInfo(position_, fontSize_, color_, text_, textId_));

	//Setting up the fontDevice
	if (!custom_)
	{
		_fontDevice.Initialize(device_, filePath_);
	}
	else
	{
		_fontDevice.Initialize(device_, fontName_, filePath_);
	}
}

FontInfo::~FontInfo()
{

}

void FontInfo::Release()
{
	_fontDevice.Release();
	_text.clear();
	//removes all allocated space.
	std::vector<TextInfo>().swap(_text);
}

void FontInfo::Render(ID3D11DeviceContext* deviceContext)
{
	const WCHAR* wText;
	
	for (auto j : _text)
	{
		wText = j._text.c_str();
		_fontDevice.DrawString(deviceContext, wText, j._text.size(), j._fontSize, j._position.x, j._position.y, j._color);
	}
}

void FontInfo::AddText(DirectX::XMFLOAT2 position, float fontSize, UINT32 color, std::wstring text, int textId)
{
	_text.push_back(TextInfo(position, fontSize, color, text, textId));
}

void FontInfo::OnResize(float scale)
{
	for (auto &j : _text)
	{
		//j.fontSize *= scale;
	}
}

bool FontInfo::RemoveText(int id)
{
	bool rv = false;
	int incr = 0;

	for (auto i : _text)
	{
		if (i._textId == id)
		{
			rv = true;
		}
		else
		{
			incr++;
		}
	}
	if (rv)
	{
		_text.erase(_text.begin() + incr);
	}

	return rv;
}

const WCHAR* FontInfo::GetFilePath()const
{
	return _filePath;
}

const WCHAR* FontInfo::GetFileName()const
{
	return _fontName;
}