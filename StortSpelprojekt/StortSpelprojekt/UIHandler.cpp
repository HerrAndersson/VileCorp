#include "UIHandler.h"

UIHandler::UIHandler(ID3D11Device* device)
{
	_device = device;
}

UIHandler::~UIHandler()
{}

void UIHandler::Update()
{
}

void UIHandler::AddFont(const WCHAR* filePath, float fontSize, float position, const WCHAR* text)
{
	for (std::vector<font>::iterator it = _fonts.begin(); it != _fonts.end(); ++it)
	{
		if (it->filePath != filePath)
		{
			
		}
	}
}

void UIHandler::Render()
{
	
}