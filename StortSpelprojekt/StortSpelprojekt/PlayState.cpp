#include "PlayState.h"

PlayState::PlayState(InitVar initVar, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper) : BaseState(initVar, filename, "PLAY", assetManager, fontWrapper)
{

}

PlayState::~PlayState()
{}

void PlayState::Update(float deltaTime)
{
	
}

void PlayState::OnStateEnter()
{

}

void PlayState::OnStateExit()
{

}