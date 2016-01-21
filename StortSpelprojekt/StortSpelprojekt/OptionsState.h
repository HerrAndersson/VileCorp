#pragma once

#include "BaseState.h"

class OptionsState : public BaseState
{
private:
public:
	OptionsState(InitVar initVar, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height);
	~OptionsState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();

};