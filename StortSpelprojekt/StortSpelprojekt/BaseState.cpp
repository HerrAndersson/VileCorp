#include "BaseState.h"

State BaseState::_newStateRequest;

BaseState::BaseState(InitVar initVar, const std::string& filename, const std::string& statename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height) :
	_uiTree(filename, statename, assetManager, fontWrapper, width, height)
{
	_objectHandler		= initVar._objectHandler;
	_inputDevice		= initVar._inputDevice;
	_newStateRequest	= SPLASHSTATE;
}

BaseState::~BaseState()
{}

void BaseState::ChangeState(State newState)
{
	_newStateRequest = newState;
}

State BaseState::GetNewStateRequest()const
{
	return _newStateRequest;
}

GUI::UITree* BaseState::GetUITree()
{
	return &_uiTree;
}

void BaseState::Resize(int width, int height)
{
	_uiTree.Resize(width, height);
}
