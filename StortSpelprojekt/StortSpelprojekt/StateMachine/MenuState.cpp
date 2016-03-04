#include "MenuState.h"

MenuState::MenuState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, CombinedMeshGenerator* combinedMeshGenerator)
	: BaseState (controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	_soundModule->AddSound("theme", 0.4f, 1.0f, true, true, true);
	_soundModule->AddSound("page", 1.0f, 1.0f, true, false);

	XMFLOAT4 color(0.1f, 0.1f, 0.1f, 1.0f);
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("tutorialbutton"), color, _uiTree.GetNode("tutorial"))); 
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("playbutton"), color, _uiTree.GetNode("play")));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("leveleditbutton"), color, _uiTree.GetNode("leveledit")));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("optionsbutton"), color, _uiTree.GetNode("options")));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("exitbutton"), color, _uiTree.GetNode("exit")));

	_combinedMeshGenerator = combinedMeshGenerator;
}

MenuState::~MenuState()
{
	
}

void MenuState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree.ReloadTree("../../../../StortSpelprojekt/Assets/GUI/menu.json");
	}

	System::MouseCoord coord = _controls->GetMouseCoord();

	HandleButtonHighlight(coord);

	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		if (_uiTree.IsButtonColliding("tutorialbutton", coord._pos.x, coord._pos.y))
		{
			_soundModule->Stop("theme");
			_soundModule->Play("page");

			std::string levelBinaryPath = System::SKIRMISH_FOLDER_PATH;
			levelBinaryPath += "tutorial1.bin";
			_objectHandler->LoadLevel(levelBinaryPath);

			//Combining objects into bigger meshes used for rendering to reduce draw calls
			_combinedMeshGenerator->Reset();
			_combinedMeshGenerator->CombineAndOptimizeMeshes(_objectHandler->GetTileMap(), System::FLOOR);
			_combinedMeshGenerator->CombineMeshes(_objectHandler->GetTileMap(), System::WALL);
			ChangeState(State::TUTORIALSTATE);
		}
		if (_uiTree.IsButtonColliding("playbutton", coord._pos.x, coord._pos.y))
		{
			_soundModule->Stop("theme");
			_soundModule->Play("page");

			ChangeState(State::LEVELSELECTSTATE);
		}
		if (_uiTree.IsButtonColliding("optionsbutton", coord._pos.x, coord._pos.y))
		{
			_soundModule->Play("page");
			ChangeState(State::OPTIONSSTATE);
		}
		if (_uiTree.IsButtonColliding("leveleditbutton", coord._pos.x, coord._pos.y))
		{
			_soundModule->Stop("theme");
			ChangeState(State::LEVELEDITSTATE);
		}
		if (_uiTree.IsButtonColliding("exitbutton", coord._pos.x, coord._pos.y))
		{
			ChangeState(State::EXITSTATE);
		}
	}
	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		ChangeState(State::EXITSTATE);
	}
}

void MenuState::OnStateEnter()
{
	_objectHandler->UnloadLevel();

	//Play theme
	_soundModule->Play("theme");
}

void MenuState::OnStateExit()
{
	
}