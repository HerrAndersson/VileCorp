#include "SplashState.h"

SplashState::SplashState(InitVar initVar) : BaseState(initVar)
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