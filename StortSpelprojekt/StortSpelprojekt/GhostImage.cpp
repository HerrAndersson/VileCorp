#include "GhostImage.h"

GhostImage::GhostImage(ObjectHandler* objectHandler, PickingDevice* pickingDevice)
{
	_objectHandler = objectHandler;
	_pickingDevice = pickingDevice;
	_gameObjectId = -1;
}

GhostImage::~GhostImage()
{}

void GhostImage::AddGhostImage(SpecificBlueprint blueprint, System::MouseCoord coord)
{
	AI::Vec2D pos = _pickingDevice->PickTile(coord._pos);
	_objectHandler->Add(blueprint._blueprint, blueprint._textureId, DirectX::XMFLOAT3(pos._x, 0, pos._y), DirectX::XMFLOAT3(0, 0, 0), true);
	_gameObjectId = _objectHandler->GetIdCount()-1;
}

void GhostImage::Update(System::MouseCoord coord)
{
	AI::Vec2D pos = _pickingDevice->PickTile(coord._pos);

	if (_gameObjectId != -1)
	{
		_objectHandler->Find(_gameObjectId)->SetPosition(XMFLOAT3(pos._x, 0, pos._y));
	}
}

void GhostImage::RemoveGhostImages()
{
	if (_gameObjectId != -1)
	{
		_objectHandler->Remove(_gameObjectId);
	}
	_gameObjectId = -1;
}