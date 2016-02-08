#pragma once

#include "BaseState.h"

class SplashState : public BaseState
{
private:
public:
	SplashState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings);
	virtual ~SplashState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};