#ifndef UIHANDLER_H
#define UIHANDLER_H

#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>

#include "Fonts.h"

__declspec(align(16))class UIHandler
{
private:
	__declspec(align(16))struct TextInfo
	{
		DirectX::XMFLOAT2		position;
		float					fontSize;
		UINT32					color;
		const WCHAR*			text;
		int						textId;

		TextInfo(DirectX::XMFLOAT2 position_, float fontSize_, UINT32 color_, const WCHAR* text_, int textId_)
		{
			position	= position_;
			fontSize	= fontSize_;
			color		= color_;
			text		= text_;
			textId		= textId_;
		}
	};
	__declspec(align(16))struct FontInfo
	{
		const WCHAR*			filePath;
		std::vector<TextInfo>	text;
		Renderer::Fonts			fontDevice;
		FontInfo(
			const WCHAR* fontPath_,
			DirectX::XMFLOAT2 position_,
			float fontSize_,
			UINT32 color_,
			const WCHAR* text_,
			int textId_,
			bool custom_,
			ID3D11Device* device_)
		{
			filePath = fontPath_;
			text.push_back(TextInfo(position_, fontSize_, color_, text_, textId_));
			//Setting up the fontDevice
			if (!custom_)
			{
				fontDevice.Initialize(device_, fontPath_);
			}
			else
			{
				fontDevice.Initialize(device_, fontPath_, fontPath_);
			}
		}
		void AddText(DirectX::XMFLOAT2 position_, float fontSize_, UINT32 color_, const WCHAR* text_, int textId_)
		{
			text.push_back(TextInfo(position_, fontSize_, color_, text_, textId_));
		}
		void Release()
		{
			text.clear();
			//removes all allocated space.
			std::vector<TextInfo>().swap(text);
		}
	};
private:
	std::vector<FontInfo>		_fonts;
	int							_textId;

	ID3D11Device*				_device;

public:
	UIHandler(ID3D11Device* device);
	~UIHandler();

	void Update();
	void Render(ID3D11DeviceContext* _deviceContext);
	
	//Adds a default font in the windows library.
	int AddFont(const WCHAR* fontName, DirectX::XMFLOAT2 position, float fontSize, UINT32 color, const WCHAR* text);
	//Adds a custom font from a sourcedirectory folder.
	int AddCustomFont(const WCHAR* filePath, DirectX::XMFLOAT2 position, float fontSize, UINT32 color, const WCHAR* text);

	//Remove a font and all text below this font
	bool RemoveFont(const WCHAR* filePathOrName);
	//Remove an individual text
	bool RemoveText(int id);

	void* operator new(size_t i)
	{
		return _mm_malloc(i,16);
	}

		void operator delete(void* p)
	{
		_mm_free(p);
	}
};

#endif