#include "PlacementState.h"

PlacementState::PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, "PLACEMENT", assetManager, fontWrapper, width, height)
{
	_controls = controls;
	_objectHandler = objectHandler;
	
	_camera = camera;
	_pickingDevice = pickingDevice;
}


PlacementState::~PlacementState()
{
}

void PlacementState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("PLACEMENT:MENU"))
	{
		ChangeState(MENUSTATE);
	}
	else if (_controls->IsFunctionKeyDown("PLACEMENT:CLICK"))
	{
		System::MouseCoord coord = _controls->GetMouseCoord();
		if (_uiTree.IsButtonColliding("Play", coord._pos.x, coord._pos.y))
		{
			ChangeState(PLAYSTATE);
		}
	}

	_levelEdit.Update(deltaTime);
}

void PlacementState::OnStateEnter()
{
	_levelEdit.Initialize(_objectHandler, _controls, _pickingDevice, _camera, &_uiTree);
}

void PlacementState::OnStateExit()
{

}