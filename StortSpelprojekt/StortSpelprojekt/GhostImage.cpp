#include "GhostImage.h"

GhostImage::GhostImage(ObjectHandler* objectHandler, PickingDevice* pickingDevice)
{
	_objectHandler = objectHandler;
	_pickingDevice = pickingDevice;
	_gameObjectId = -1;
}

GhostImage::~GhostImage()
{}

void GhostImage::AddGhostImage(System::SpecificBlueprint blueprint, System::MouseCoord coord)
{
	AI::Vec2D pos = _pickingDevice->PickTile(coord._pos);
	_gameObjectId = _objectHandler->Add(blueprint._blueprint, blueprint._textureId, DirectX::XMFLOAT3(pos._x, 0, pos._y), DirectX::XMFLOAT3(0, 0, 0), false);
}

void GhostImage::Update(System::MouseCoord coord)
{
	AI::Vec2D pos = _pickingDevice->PickTile(coord._pos);
	GameObject* object;
	bool placeable = true;

	//if active ghostimage
	if (_gameObjectId != -1)
	{
		//find it
		object = _objectHandler->Find(_gameObjectId);
		//update position
		object->SetPosition(XMFLOAT3(pos._x, 0, pos._y));
		//update color
		vector<GameObject*> tileObjects = _objectHandler->GetTileMap()->GetAllObjectsOnTile(pos);
		if (tileObjects.size() > 0)
		{
			for (GameObject* i : tileObjects)
			{
				if (i->GetType() != System::Type::FLOOR)
				{
					placeable = false;
				}
			}
		}
		else
		{
			placeable = false;
		}
		if (placeable)
		{
			object->SetColorOffset(XMFLOAT3(0.0f, 1.0f, 0.0f));
		}
		else
		{
			object->SetColorOffset(XMFLOAT3(1.0f, 0.0f, 0.0f));
		}
	}
}

void GhostImage::RemoveGhostImage()
{
	if (_gameObjectId != -1)
	{
		_objectHandler->Remove(_gameObjectId);
	}
	_gameObjectId = -1;
}

bool GhostImage::IsGhostImageActive()
{
	if (_gameObjectId == -1)
	{
		return false;
	}
	else
	{
		return true;
	}
}
