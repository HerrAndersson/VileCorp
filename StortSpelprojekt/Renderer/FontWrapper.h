#pragma once
#define RENDERER_EXPORT __declspec(dllexport)
#include <d3d11.h>
#include "stdafx.h"

#include "CCollectionLoader.h"

//dll
#include "FW/FW1FontWrapper.h"
//Lib
#pragma comment(lib, "FW1FontWrapper.lib")


class RENDERER_EXPORT FontWrapper
{
private:
	IFW1FontWrapper*	_fontWrapper;
	IDWriteFactory*		_writeFactory;
	IDWriteFontCollection*	_fontCollection;
	LPCWSTR					_fontName;
public:
	//Custom font components
	struct CustomFont
	{
		CustomFont()
		{
			_textLayout = nullptr;
			_textFormat = nullptr;
		}
		IDWriteTextLayout*		_textLayout;
		IDWriteTextFormat*		_textFormat;
	};

	FontWrapper(ID3D11Device* device, LPWSTR fontPath, LPWSTR fontName);
	~FontWrapper();

	IFW1FontWrapper* GetFontWrapper();
	IDWriteFactory* GetWriteFactory();
	IDWriteFontCollection* GetFontCollection();
	LPCWSTR GetFontName();
};