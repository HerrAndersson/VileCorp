#pragma once

#include "BaseState.h"

class SplashState : public BaseState
{
private:
public:
	SplashState(InitVar initVar, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper);
	~SplashState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};