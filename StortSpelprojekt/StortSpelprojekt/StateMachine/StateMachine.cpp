#include "StateMachine.h"

StateMachine::StateMachine(System::Controls* controls,
	ObjectHandler* objectHandler,
	System::Camera* camera,
	PickingDevice* pickingDevice,
	const std::string& filename,
	AssetManager* assetManager,
	FontWrapper* fontWrapper,
	System::Settings* settings,
	System::SettingsReader* settingsReader,
	System::SoundModule* soundModule,
	DirectX::XMFLOAT3* ambientLight)
{
	_currentState = State::SPLASHSTATE;

	_baseStates.push_back(new SplashState(controls, objectHandler, camera, pickingDevice, "Assets/GUI/splash.json", assetManager, fontWrapper, settingsReader, soundModule));
	_baseStates.push_back(new MenuState(controls, objectHandler, camera, pickingDevice, "Assets/GUI/menu.json", assetManager, fontWrapper, settingsReader, soundModule));
	_baseStates.push_back(new PlayState(controls, objectHandler, camera, pickingDevice, "Assets/GUI/play.json", assetManager, fontWrapper, settingsReader, soundModule, ambientLight));
	_baseStates.push_back(new PlacementState(controls, objectHandler, camera, pickingDevice, "Assets/GUI/placement.json", assetManager, fontWrapper, settingsReader, soundModule, ambientLight));
	_baseStates.push_back(new TutorialState(controls, objectHandler, camera, pickingDevice, "Assets/GUI/placement.json", assetManager, fontWrapper, settingsReader, soundModule, ambientLight));
	_baseStates.push_back(new LevelEditState(controls, objectHandler, camera, pickingDevice, System::LEVELEDIT_GUI_PATH, assetManager, fontWrapper, settingsReader, soundModule, ambientLight));
	_baseStates.push_back(new LevelSelectState(controls, objectHandler, camera, pickingDevice, "Assets/GUI/levelselect.json", assetManager, fontWrapper, settingsReader, soundModule));
	_baseStates.push_back(new OptionsState(controls, objectHandler, camera, pickingDevice, "Assets/GUI/options.json", assetManager, fontWrapper, settingsReader, soundModule));
	_baseStates.push_back(new PauseState(controls, objectHandler, camera, pickingDevice, "Assets/GUI/pause.json", assetManager, fontWrapper, settingsReader, soundModule));
}

StateMachine::~StateMachine()
{
	for (auto s : _baseStates)
	{
		delete s;
	}
}

bool StateMachine::Update(float deltaTime)
{
	_baseStates[_currentState]->Update(deltaTime);
	ProcessStateRequest();
	
	if (_currentState == EXITSTATE)
	{
		return false;
	}
	return true;
}

void StateMachine::ProcessStateRequest()
{
	//if currentState does not match with the new requested state.
	if (_currentState != _baseStates[_currentState]->GetNewStateRequest())
	{
		_baseStates[_currentState]->OnStateExit();
		_currentState = _baseStates[_currentState]->GetNewStateRequest();
		if (_currentState != State::EXITSTATE)
		{
			_baseStates[_currentState]->OnStateEnter();
		}
	}
}

State StateMachine::GetState() 
{
	return _currentState;
}

BaseState* StateMachine::GetCurrentStatePointer() const
{
	return _baseStates[_currentState];
}

void StateMachine::Resize(System::Settings* settings)
{
	for (auto i : _baseStates)
	{
		i->Resize(settings);
	}
}