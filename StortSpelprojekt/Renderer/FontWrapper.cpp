#include "FontWrapper.h"

FontWrapper::FontWrapper(ID3D11Device* device, LPWSTR fontPath, LPWSTR fontName)
{
	HRESULT hr;

	_fontName = fontName;

	//Create _fontWrapper
	FW1CreateFactory(FW1_VERSION, &_factory);
	hr = _factory->CreateFontWrapper(device, NULL, &_fontWrapper);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create font wrapper");
	}
	

	//Create custom font
	hr = _fontWrapper->GetDWriteFactory(&_writeFactory);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create write factory");
	}

	// Set up custom font collection
	IDWriteFontCollectionLoader *pCollectionLoader = new CCollectionLoader(fontPath);

	hr = _writeFactory->RegisterFontCollectionLoader(pCollectionLoader);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to register collection loader");
	}

	hr = _writeFactory->CreateCustomFontCollection(pCollectionLoader, NULL, 0, &_fontCollection);
	_writeFactory->UnregisterFontCollectionLoader(pCollectionLoader);
	pCollectionLoader->Release();

	if (FAILED(hr))
	{
		throw std::runtime_error("Custom font failed to create or is not compatible");
	}
}

FontWrapper::~FontWrapper()
{
	_fontWrapper->Release();
	_factory->Release();
}

IFW1FontWrapper* FontWrapper::GetFontWrapper()
{
	return _fontWrapper;
}

IDWriteFactory* FontWrapper::GetWriteFactory()
{
	return _writeFactory;
}

IDWriteFontCollection* FontWrapper::GetFontCollection()
{
	return _fontCollection;
}

LPCWSTR FontWrapper::GetFontName()
{
	return _fontName;
}