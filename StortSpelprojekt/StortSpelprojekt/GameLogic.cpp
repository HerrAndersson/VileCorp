#include "GameLogic.h"
#include <DirectXMath.h>


GameLogic::GameLogic()
{

}

GameLogic::~GameLogic()
{

}

void GameLogic::Initialize(System::InputDevice* inputDevice, ObjectHandler* objectHandler, UIHandler* uiHandler, System::Camera* camera)
{
	_inputDevice = inputDevice;
	_objectHandler = objectHandler;
	_uiHandler = uiHandler;
	_camera = camera;

	//Needs a Tilemap in the objectHandler /Markus
	//_objectHandler->Add(TRAP, 0, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	//_objectHandler->Add(TRAP, 0, XMFLOAT3(0.5f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	_objectHandler->LoadLevel(6);

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