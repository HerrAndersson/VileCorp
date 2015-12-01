#include "Menu.h"

Menu::Menu(UIHandler* uiHandler)
{
	_uiHandler = uiHandler;

	_uiHandler->AddFont(L"Arial", DirectX::XMFLOAT2(0, 0), 32.0f, 0xff0099ff, L"Continue");
	_uiHandler->AddFont(L"Arial", DirectX::XMFLOAT2(0, 0), 32.0f, 0xff0099ff, L"Start Game");
	_uiHandler->AddFont(L"Arial", DirectX::XMFLOAT2(0, 0), 32.0f, 0xff0099ff, L"Options");
	_uiHandler->AddFont(L"Arial", DirectX::XMFLOAT2(0, 0), 32.0f, 0xff0099ff, L"Credits");
	_uiHandler->AddFont(L"Arial", DirectX::XMFLOAT2(0, 0), 32.0f, 0xff0099ff, L"Quit");
	
}

Menu::~Menu()
{
	
}

void Menu::Update()
{
}

