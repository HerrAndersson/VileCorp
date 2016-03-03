#pragma once
#include "../CombinedMeshGenerator.h"
#include "BaseState.h"
#include "GUI elements\HighlightNode.h"

class MenuState : public BaseState
{
private:
	CombinedMeshGenerator* _combinedMeshGenerator;
public:
	MenuState(System::Controls * controls, ObjectHandler * objectHandler, System::Camera * camera, PickingDevice * pickingDevice, const std::string & filename, AssetManager * assetManager, FontWrapper * fontWrapper, System::SettingsReader * settingsReader, System::SoundModule * soundModule, CombinedMeshGenerator* combinedMeshGenerator);
	virtual ~MenuState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};
