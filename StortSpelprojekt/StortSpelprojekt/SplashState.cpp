#include "SplashState.h"

SplashState::SplashState(InitVar initVar, const std::string& filename, AssetManager* assetManager) : BaseState(initVar, filename, "SPLASH", assetManager)
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