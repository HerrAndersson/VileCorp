#pragma once

#include <vector>
#include "Fonts.h"
#include <DirectXMath.h>

class FontInfo
{
private:
	struct TextInfo
	{
		DirectX::XMFLOAT2		_position;
		float					_fontSize;
		UINT32					_color;
		std::wstring			_text;
		int						_textId;

		TextInfo(DirectX::XMFLOAT2 position, float fontSize, UINT32 color, std::wstring text, int textId)
		{
			_position = position;
			_fontSize = fontSize;
			_color = color;
			_text = text;
			_textId = textId;
		}
	};
	const WCHAR*			_filePath;
	const WCHAR*			_fontName;
	std::vector<TextInfo>	_text;
	Renderer::Fonts			_fontDevice;

public:
	FontInfo(const WCHAR* filePath,
		const WCHAR* fontName,
		DirectX::XMFLOAT2 position,
		float fontSize,
		UINT32 color,
		std::wstring text,
		int textId,
		bool custom,
		ID3D11Device* device);
	~FontInfo();

	void Release();
	void Render(ID3D11DeviceContext* deviceContext);
	void AddText(DirectX::XMFLOAT2 position_, float fontSize_, UINT32 color_, std::wstring text_, int textId_);
	void OnResize(float scale);
	bool RemoveText(int id);

	const WCHAR* GetFilePath()const;
	const WCHAR* GetFileName()const;

};