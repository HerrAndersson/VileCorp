#include "MenuState.h"

MenuState::MenuState(InitVar initVar) : BaseState (initVar)
{
	_uiHandler = initVar.uiHandler;

}

MenuState::~MenuState()
{
	
}

void MenuState::Update(float deltaTime)
{
}

void MenuState::OnStateEnter()
{
	_uiHandler->AddCustomFont(L"../../Output/Bin/x86/Debug/Assets/Fonts/Calibri.ttf", L"Calibri", XMFLOAT2(0, 0), 32.0f, 0xff0099ff, L"Continue");
	_uiHandler->Add2DTexture("floor.png", DirectX::XMFLOAT2(-0.8, -0.8f), DirectX::XMFLOAT2(0.2f, 0.2f));
	
	/*
	_uiHandler->AddCustomFont(L"../../Output/Bin/x86/Debug/Assets/Fonts/Calibri.ttf", L"Calibri", XMFLOAT2(0, 100), 32.0f, 0xff0099ff, L"Start");
	_uiHandler->AddCustomFont(L"../../Output/Bin/x86/Debug/Assets/Fonts/Calibri.ttf", L"Calibri", XMFLOAT2(0, 200), 32.0f, 0xff0099ff, L"Options");
	_uiHandler->AddCustomFont(L"../../Output/Bin/x86/Debug/Assets/Fonts/Calibri.ttf", L"Calibri", XMFLOAT2(0, 300), 32.0f, 0xff0099ff, L"Credits");
	_uiHandler->AddCustomFont(L"../../Output/Bin/x86/Debug/Assets/Fonts/Calibri.ttf", L"Calibri", XMFLOAT2(0, 400), 32.0f, 0xff0099ff, L"Exit");
	*/
	//_uiHandler->RemoveFont(L"../../Output/Bin/x86/Debug/Assets/Fonts/Calibri.ttf");
	
	/*
	_uiHandler->Add2DImage(filepath, position, size);
	_uiHandler->AddButton(filepath, position, size);
	*/
}

void MenuState::OnStateExit()
{

}