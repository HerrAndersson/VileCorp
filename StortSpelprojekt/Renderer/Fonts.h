#ifndef FONTS_H
#define FONTS_H

#define RENDERER_EXPORT __declspec(dllexport)
#include <d3d11.h>
#include <string>
#include "stdafx.h"

//dll
#include "FW1FontWrapper.h"
//Lib
#pragma comment(lib, "FW1FontWrapper.lib")

namespace Renderer
{
	class RENDERER_EXPORT Fonts
	{
	private:
		IFW1Factory*		_factory;
		IFW1FontWrapper*	_fontWrapper;
		
	public:
		Fonts();
		void Release();

		HRESULT Initialize(ID3D11Device* device);
		void DrawString(ID3D11DeviceContext* device, std::string text, float fontSize, float x, float y, UINT32 color, UINT flags);
	};
}

#endif