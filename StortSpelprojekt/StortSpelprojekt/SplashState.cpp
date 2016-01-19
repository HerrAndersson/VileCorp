#include "SplashState.h"

SplashState::SplashState(InitVar initVar, const std::string& filename) : BaseState(initVar, filename, "SPLASH")
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