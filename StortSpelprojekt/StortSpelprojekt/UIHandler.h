#ifndef UIHANDLER_H
#define UIHANDLER_H

#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>

#include "Fonts.h"
#include "Window.h"
#include "FontInfo.h"
#include "AssetManager.h"
#include "HUDElement.h"

class UIHandler
{
private:
	std::vector<FontInfo>				_fonts;
	int									_textId;
	System::WindowSettings				_windowSettings;
	ID3D11Device*						_device;
	AssetManager*						_AM;
	std::vector<Renderer::HUDElement>	_textures;
	int									_textureId;
	

public:
	UIHandler(ID3D11Device* device, System::WindowSettings windowSettings);
	~UIHandler();

	void Update();
	void Render(ID3D11DeviceContext* _deviceContext);
	void OnResize(System::WindowSettings windowSettings);
	
	//Adds a default font in the windows library.
	int AddFont(const WCHAR* filePath, const WCHAR* fontName, DirectX::XMFLOAT2 position, float fontSize, UINT32 color, std::wstring text);
	//Adds a custom font from a sourcedirectory folder.
	int AddCustomFont(const WCHAR* filePath, const WCHAR* fontName, DirectX::XMFLOAT2 position, float fontSize, UINT32 color, std::wstring text);

	//Remove a font and all text below this font
	bool RemoveFont(const WCHAR* filePath);
	//Remove an individual text
	bool RemoveText(int id);


	int Add2DTexture(std::string filePath, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size);
	int AddButton(std::string filePath, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size);
	std::vector<Renderer::HUDElement>* GetTextureData();
};

#endif