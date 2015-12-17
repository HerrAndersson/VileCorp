#include "GameLogic.h"
#include <DirectXMath.h>


GameLogic::GameLogic()
{

}

GameLogic::~GameLogic()
{

}

void GameLogic::Initialize(InitVar* initVar)
{
	_inputDevice = initVar->_inputDevice;
	_objectHandler = initVar->_objectHandler;
	_uiHandler = initVar->_uiHandler;
	_camera = initVar->_camera;

	//Needs a Tilemap in the objectHandler /Markus
	//_objectHandler->Add(TRAP, 0, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	//_objectHandler->Add(TRAP, 0, XMFLOAT3(0.5f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	_objectHandler->LoadLevel(3);

	_objectHandler->InitPathfinding();
}

void GameLogic::Update()
{
	HandleInput();
	_objectHandler->Update();
}

void GameLogic::HandleInput()
{
	
}