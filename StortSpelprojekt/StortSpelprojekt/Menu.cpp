#include "Menu.h"

Menu::Menu(UIHandler* uiHandler)
{
	_uiHandler = uiHandler;

	_uiHandler->AddFont(L"Continue");
	_uiHandler->AddFont(L"Start Game");
	_uiHandler->AddFont(L"Options");
	_uiHandler->AddFont(L"Credits");
	_uiHandler->AddFont(L"Quit");
}

Menu::~Menu()
{
	
}

void Menu::Update()
{
}

