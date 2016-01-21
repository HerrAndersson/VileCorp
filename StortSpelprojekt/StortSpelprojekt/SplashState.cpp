#include "SplashState.h"

SplashState::SplashState(InitVar initVar, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height) : BaseState(initVar, filename, "SPLASH", assetManager, fontWrapper, width, height)
{}

SplashState::~SplashState()
{}

void SplashState::Update(float deltaTime)
{
	ChangeState(State::MENUSTATE);
}

void SplashState::OnStateEnter()
{

}

void SplashState::OnStateExit()
{

}