#pragma once
//#include "C:\Users\Grupp1 - Zache\Desktop\StortSpel-Grupp1\StortSpelprojekt\StortSpelprojekt\StateMachine\PlacementState.h"
#include "PlacementState.h"
class TutorialState : public PlacementState
{
private:
	enum TutorialStage
	{
		WELCOME,
		CONTROLS,
		OBJECTIVE,
		GUARDEXPLAINED,
		GUARDPLACE,
		ANVILEXPLAINED,
		ANVILPLACE,
		TESLAEXPLAINED,
		TESLAPLACE,
		SECURITYCAMERAEXPLAINED,
		SECURITYCAMERAPLACE,
		BUDGETEXPLAINED,
		PLAYEXPLAINED
	};

	TutorialStage _currentStage;
	bool _sCameraPlaced;
	bool _tutorialCompleted;
	float _time;
	bool _light;
	bool _objectPlaced;
	GUI::Node* _currentButton;			//The button the tutorial allows you to click

	void ChangeTutorialStages(bool forward = true);
	void PlacementGuideCheck(std::string nodeName, System::Type type, int subType = 0);

public:
	TutorialState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, DirectX::XMFLOAT3* ambientLight);
	virtual ~TutorialState();

	void Update(float deltaTime);
	void OnStateEnter();
	void ResetUiTree();
	bool IsTutorialCompleted()const;
	void HandleButtons();
};

