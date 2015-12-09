#include "UIHandler.h"

UIHandler::UIHandler(ID3D11Device* device, System::WindowSettings windowSettings)
{
	_device					= device;
	_textId					= 0;
	_windowSettings			= windowSettings;
	_AM						= new AssetManager(device);
	_textureId				= 0;
}

UIHandler::~UIHandler()
{
	for (auto i : _fonts)
	{
		i.Release();
	}
	delete _AM;
}

void UIHandler::Update()
{
}

int UIHandler::AddFont(const WCHAR* filePath, const WCHAR* fontName, DirectX::XMFLOAT2 position, float fontSize, UINT32 color, std::wstring text)
{
	if (_textId == 0) //if first element
	{
		_fonts.push_back(FontInfo(filePath, fontName, position, fontSize, color, text, _textId, false, _device));
	}
	else
	{
		int found = -1;
		int incr = 0;
		for (auto i : _fonts)
		{
			//if filepaths are equal.
			if (0 == wcscmp(i.GetFilePath(), fontName))
			{
				found = incr;
			}
			incr++;
		}
		//if no previous font found. Create a new font.
		if (found == -1)
		{
			//Then create a new font.
			_fonts.push_back(FontInfo(filePath, fontName, position, fontSize, color, text, _textId, false, _device));
		}
		else
		{
			//If filepaths are equal. Then we shall create a string inside that font.
			_fonts.at(found).AddText(position, fontSize, color, text, _textId);
		}
	}

	return _textId++;
}

int UIHandler::AddCustomFont(const WCHAR* filePath, const WCHAR* fontName, DirectX::XMFLOAT2 position, float fontSize, UINT32 color, std::wstring text)
{
	if (_textId == 0) //if first element
	{
		_fonts.push_back(FontInfo(filePath, fontName, position, fontSize, color, text, _textId, true, _device));
	}
	else
	{
		int found = -1;
		int incr = 0;
		for (auto i : _fonts)
		{
			//if filepaths are equal.
			if (0 == wcscmp(i.GetFilePath(), fontName))
			{
				found = incr;
			}
			incr++;
		}
		//if no previous font found. Create a new font.
		if (found == -1)
		{
			//Then create a new font.
			_fonts.push_back(FontInfo(filePath, fontName, position, fontSize, color, text, _textId, true, _device));
		}
		else
		{
			//If filepaths are equal. Then we shall create a string inside that font.
			_fonts.at(found).AddText(position, fontSize, color, text, _textId);
		}
	}

	return _textId++;
}

void UIHandler::Render(ID3D11DeviceContext* _deviceContext)
{
	for (auto i : _fonts)
	{
		i.Render(_deviceContext);
	}
}

void UIHandler::OnResize(System::WindowSettings windowSettings)
{
	System::WindowSettings newSettings = windowSettings;
	DirectX::XMFLOAT2 scale;
	scale.x = (float)windowSettings._width / (float)_windowSettings._width;
	scale.y = (float)windowSettings._height / (float)_windowSettings._height;
	
	for (auto &i : _fonts)
	{
		i.OnResize(scale.x);
	}
}

bool UIHandler::RemoveFont(const WCHAR* filePath)
{
	bool rv = false;
	for (auto i : _fonts)
	{
		if (0 != wcscmp(i.GetFilePath(), filePath))
		{
			i.Release();
			rv = true;
		}
	}

	return rv;
}

bool UIHandler::RemoveText(int id)
{
	bool rv = false;
	bool deleted = false;
	int incr = 0;
	for (auto i : _fonts)
	{
		deleted = i.RemoveText(id);
		if (deleted)
		{
			rv = true;
		}
	}
	return rv;
}

int UIHandler::Add2DTexture(std::string filePath, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size)
{
	ID3D11ShaderResourceView* tex = _AM->GetTexture(filePath);
	_textures.push_back(Renderer::HUDElement(position, size, tex));

	return _textureId++;
}

int UIHandler::AddButton(std::string filePath, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size)
{
	//Add2DTexture(filePath, position, size);
	//_textures
	return 0;
}

std::vector<Renderer::HUDElement>* UIHandler::GetTextureData()
{
	return &_textures;
}

