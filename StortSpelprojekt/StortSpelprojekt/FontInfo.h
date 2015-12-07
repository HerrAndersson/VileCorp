#ifndef FONTINFO_H
#define FONTINFO_H

#include <vector>
#include "Fonts.h"
#include <DirectXMath.h>

class FontInfo
{
private:
	struct TextInfo
	{
		DirectX::XMFLOAT2		position;
		float					fontSize;
		UINT32					color;
		std::wstring			text;
		int						textId;

		TextInfo(DirectX::XMFLOAT2 position_, float fontSize_, UINT32 color_, std::wstring text_, int textId_)
		{
			position = position_;
			fontSize = fontSize_;
			color = color_;
			text = text_;
			textId = textId_;
		}
	};
	const WCHAR*			_filePath;
	const WCHAR*			_fontName;
	std::vector<TextInfo>	_text;
	Renderer::Fonts			_fontDevice;

public:
	FontInfo(const WCHAR* filePath_,
		const WCHAR* fontName_,
		DirectX::XMFLOAT2 position_,
		float fontSize_,
		UINT32 color_,
		std::wstring text_,
		int textId_,
		bool custom_,
		ID3D11Device* device_);
	~FontInfo();

	void Release();
	void Render(ID3D11DeviceContext* deviceContext);
	void AddText(DirectX::XMFLOAT2 position_, float fontSize_, UINT32 color_, std::wstring text_, int textId_);
	void OnResize(float scale);
	bool RemoveText(int id);

	const WCHAR* GetFilePath()const;
	const WCHAR* GetFileName()const;

};

#endif