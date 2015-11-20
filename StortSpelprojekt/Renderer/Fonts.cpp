#include "Fonts.h"

namespace Renderer
{
	Fonts::Fonts()
	{}

	void Fonts::Release()
	{
		SAFE_RELEASE(_factory);
		SAFE_RELEASE(_fontWrapper);
	}

	HRESULT Fonts::Initialize(ID3D11Device* device)
	{
		HRESULT hr = S_OK;

		hr = FW1CreateFactory(FW1_VERSION, &_factory);
		IF_HR(hr);

		hr = _factory->CreateFontWrapper(device, L"Arial", &_fontWrapper);
		IF_HR(hr);

		return hr;
	}

	void Fonts::DrawString(ID3D11DeviceContext* deviceContext, std::string text, float fontSize, float x, float y, UINT32 color, UINT flags)
	{
		std::wstring widestr = std::wstring(text.begin(), text.end());
		const wchar_t* s = widestr.c_str();
		_fontWrapper->DrawString(deviceContext, s, fontSize, x, y, color, flags);
	}
}