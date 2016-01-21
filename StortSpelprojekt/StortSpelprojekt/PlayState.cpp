#include "PlayState.h"

PlayState::PlayState(InitVar initVar, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height) : BaseState(initVar, filename, "PLAY", assetManager, fontWrapper, width, height)
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