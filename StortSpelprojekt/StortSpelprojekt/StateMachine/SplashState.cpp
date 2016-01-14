#include "SplashState.h"

SplashState::SplashState(InitVar initVar) : BaseState(initVar)
{}

SplashState::~SplashState()
{}

void SplashState::Update(float deltaTime)
{
	ChangeState(State::PLACEMENTSTATE);
}

void SplashState::OnStateEnter()
{

}

void SplashState::OnStateExit()
{

}