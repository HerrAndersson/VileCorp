#include "UIHandler.h"

using namespace std;
using namespace rapidjson;
using namespace GUI;

UIHandler::UIHandler(ID3D11Device* device, System::WindowSettings windowSettings, AssetManager* assetManager)
{
	_device			= device;
	_textId			= 0;
	_windowSettings	= windowSettings;
	_AM				= assetManager;
	_textureId		= 0;

	_fontWrapper.CreateFontWrapper(device);

	ifstream file("../../Output/Bin/x86/Debug/Assets/gui.json");
	if (!file.good())
	{
		throw std::runtime_error("Failed to open gui.json");
	}
	string str((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	file.close();
	
	Document d;
	d.Parse(str.c_str());

	_root = new GUI::Node(XMFLOAT2(0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));


	LoadGUITree(_root, d.MemberBegin(), d.MemberEnd());
}

void UIHandler::LoadGUITree(GUI::Node* current, rapidjson::Value::ConstMemberIterator start, rapidjson::Value::ConstMemberIterator end)
{
	Value::ConstMemberIterator it = start;
	for (; it != end; ++it)
	{
		string id(it->name.GetString());
		Node* newNode = nullptr;
		if (id == "_parent") //This is a transform node
		{
			XMFLOAT2 position;
			XMFLOAT2 scale;
			for (auto i = it->value.MemberBegin(); i != it->value.MemberEnd(); ++i)
			{
				if (i->name == "position")
				{
					if (i->value.IsArray())
					{
						position = XMFLOAT2((float)i->value[0].GetDouble(),
											(float)i->value[1].GetDouble());
					}
				}
				else if (i->name == "scale")
				{
					if (i->value.IsArray())
					{
						scale = XMFLOAT2(	(float)i->value[0].GetDouble(),
											(float)i->value[1].GetDouble());
					}
				}
			}
			newNode = new Node(position, scale);
		}
		else //This is an element
		{

		}

		current->GetChildren()->push_back(newNode);
	}
}

UIHandler::~UIHandler()
{
	for (auto i : _fonts)
	{
		i.Release();
	}
	Release(_root);
}

void UIHandler::Release(GUI::Node* node)
{
	for (Node* i : *node->GetChildren())
	{
		Release(i);
	}
	if (node != nullptr)
	{
		delete node;
	}
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
		_fonts.push_back(FontInfo(_fontWrapper.GetFontWrapper(), filePath, fontName, position, fontSize, color, text, _textId, true, _device));
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
			_fonts.push_back(FontInfo(_fontWrapper.GetFontWrapper(), filePath, fontName, position, fontSize, color, text, _textId, true, _device));
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
	for (auto &i : _fonts)
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
	for (auto &i : _fonts)
	{
		if (0 == wcscmp(i.GetFilePath(), filePath))
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
	for (auto &i : _fonts)
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
	int buttonId = Add2DTexture(filePath, position, size);

	return buttonId;
}

std::vector<Renderer::HUDElement>* UIHandler::GetTextureData()
{
	return &_textures;
}

GUI::Node* UIHandler::GetRootNode() const
{
	return _root;
}

void* UIHandler::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void UIHandler::operator delete(void* p)
{
	_mm_free(p);
}