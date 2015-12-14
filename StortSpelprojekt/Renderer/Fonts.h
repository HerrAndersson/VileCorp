#ifndef FONTS_H
#define FONTS_H

/*

THIS CLASS USES FW1FONTWRAPPER. CREDIT TO THAT BELONGS TO
ERIK RUFELT. https://fw1.codeplex.com/ FOR MORE ABOUT THE WRAPPER.

INFORMATION ABOUT LICENSE CAN BE FOUND HERE: http://fw1.codeplex.com/license

*/

/*

	HOW THIS CLASS WORKS:
	INITIALIZE ONE FONT OBJECT WITH 1 FONT. IF YOU ADD MORE STRINGS
	IT WILL USE THAT FONT. SO ONE OBJECT PER FONT.

*/

#define RENDERER_EXPORT __declspec(dllexport)
#include <d3d11.h>
#include "stdafx.h"


#include "CCollectionLoader.h"

//dll
#include "FW/FW1FontWrapper.h"
//Lib
#pragma comment(lib, "FW1FontWrapper.lib")

namespace Renderer
{
	__declspec(align(16))class RENDERER_EXPORT Fonts
	{
	private:
		bool				_externalWrapper;
		IFW1Factory*		_factory;
		IFW1FontWrapper*	_fontWrapper;

		//Custom font components
		struct CustomFont
		{
			IDWriteFactory*			writeFactory;
			IDWriteTextLayout*		writeTextLayout;
			IDWriteFontCollection*	fontCollection;
			IDWriteTextLayout*		textLayout;
			IDWriteTextFormat*		textFormat;
			LPCWSTR					fontName;
			float					fontSize;
			const WCHAR*			fontText;
		};

		CustomFont					_cFont;

	public:
		Fonts();
		void Release();

		//Create empty Wrapper
		void CreateFontWrapper(ID3D11Device* device);
		IFW1FontWrapper* GetFontWrapper();

		//Default 
		HRESULT Initialize(ID3D11Device* device, LPCWSTR fontName);
		//Custom font - USING ITS OWN WRAPPERDEVICE.
		HRESULT Initialize(ID3D11Device* device, LPCWSTR fontName, LPCWSTR filePath);
		//Custom font - USING EXTERNAL WRAPPERDEVICE.
		HRESULT Initialize(IFW1FontWrapper* fontWrapper, ID3D11Device* device, LPCWSTR fontName, LPCWSTR filePath);

		HRESULT DrawString(ID3D11DeviceContext* device, const WCHAR* text, int textSize, float fontSize, float x, float y, UINT32 color);
	};
}

#endif