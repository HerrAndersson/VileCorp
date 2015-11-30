#ifndef UIHANDLER_H
#define UIHANDLER_H

#include <d3d11.h>
#include "Fonts.h"
#include <vector>

class UIHandler
{
private:
	struct TextInfo
	{
		DirectX::XMFLOAT2		position;
		float					fontSize;
		const WCHAR*			fontText;
	};
	struct FontInfo
	{
		std::vector<TextInfo>	text;
		const WCHAR*			filePath;
	};
private:
	Renderer::Fonts				_fontDevice;
	std::vector<FontInfo>		_fonts;

	ID3D11Device*				_device;

public:
	UIHandler(ID3D11Device* device);
	~UIHandler();

	void Update();
	void Render();
	

	void AddFont(const WCHAR* filePath, float fontSize, float position, const WCHAR* text);
};

#endif