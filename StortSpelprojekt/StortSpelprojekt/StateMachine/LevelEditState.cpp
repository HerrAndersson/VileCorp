#include "LevelEditState.h"

LevelEditState::LevelEditState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, "LEVELEDIT", assetManager, fontWrapper, width, height)
{
	_controls = controls;
	_objectHandler = objectHandler;
	
	_camera = camera;
	_pickingDevice = pickingDevice;
}

LevelEditState::~LevelEditState()
{}

void LevelEditState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("MAP_EDIT:MENU"))
	{
		ChangeState(MENUSTATE);
	}
	
	System::MouseCoord coord = _controls->GetMouseCoord();
	if (_uiTree.IsButtonColliding("Traps", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("PLACEMENT:CLICK"))
	{
		_uiTree.GetNode("TrapLeaf")->SetHidden(false);
		_uiTree.GetNode("UnitLeaf")->SetHidden(true);
		_uiTree.GetNode("DecLeaf")->SetHidden(true);
	}
	
	if (_uiTree.IsButtonColliding("Units", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("PLACEMENT:CLICK"))
	{
		_uiTree.GetNode("UnitLeaf")->SetHidden(false);
		_uiTree.GetNode("TrapLeaf")->SetHidden(true);
		_uiTree.GetNode("DecLeaf")->SetHidden(true);
	}


	if (_uiTree.IsButtonColliding("Decorations", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("PLACEMENT:CLICK"))
	{
		_uiTree.GetNode("DecLeaf")->SetHidden(false);
		_uiTree.GetNode("TrapLeaf")->SetHidden(true);
		_uiTree.GetNode("UnitLeaf")->SetHidden(true);
	}


	


	_levelEdit.Update(deltaTime);
}

void LevelEditState::OnStateEnter()
{
	_levelEdit.Initialize(_objectHandler, _controls, _pickingDevice, _camera, &_uiTree);
	_uiTree.GetNode("TrapLeaf")->SetHidden(true);
	_uiTree.GetNode("UnitLeaf")->SetHidden(true);
	_uiTree.GetNode("DecLeaf")->SetHidden(true);
}

void LevelEditState::OnStateExit()
{

}

