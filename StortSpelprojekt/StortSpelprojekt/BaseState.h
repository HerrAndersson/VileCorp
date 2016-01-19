#pragma once

#include "InitVar.h"
#include "UITree.h"

enum State
{
	SPLASHSTATE,
	MENUSTATE,
	PLAYSTATE,
	OPTIONSSTATE,
	EXITSTATE,
};

class BaseState
{
private:
	static State BaseState::_newStateRequest;

protected:
	ObjectHandler*			_objectHandler;
	GUI::UITree				_uiTree;
	System::InputDevice*	_inputDevice;
	
	void ChangeState(State newState);
public:
	BaseState(InitVar initVar, const std::string& filename, const std::string& statename, AssetManager* assetManager);
	virtual ~BaseState();

	virtual void Update(float deltaTime) = 0;
	virtual void OnStateEnter() = 0;
	virtual void OnStateExit() = 0;

	State GetNewStateRequest() const;
	GUI::UITree* GetUITree();
};
