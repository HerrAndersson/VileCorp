#include "MenuState.h"

MenuState::MenuState(InitVar initVar) : BaseState (initVar)
{
	_uiHandler = initVar.uiHandler;

	_uiHandler->AddFont(NULL, DirectX::XMFLOAT2(0, 100), 32.0f, 0xff0099ff, L"Demo");
	
	//_uiHandler->AddFont(L"Arial", DirectX::XMFLOAT2(0, 100), 32.0f, 0xff0099ff, L"Start Game");
	//_uiHandler->AddFont(L"Arial", DirectX::XMFLOAT2(0, 200), 32.0f, 0xff0099ff, L"Options");
	//_uiHandler->AddFont(L"Arial", DirectX::XMFLOAT2(0, 300), 32.0f, 0xff0099ff, L"Credits");
	//_uiHandler->AddFont(L"Arial", DirectX::XMFLOAT2(0, 400), 32.0f, 0xff0099ff, L"Quit");
}

MenuState::~MenuState()
{
	
}

void MenuState::Update(float deltaTime)
{
}

void MenuState::OnStateEnter()
{

}

void MenuState::OnStateExit()
{

}