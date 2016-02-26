#pragma once

#include "ObjectHandler.h"
#include <vector>
#include "Controls.h"
#include "PickingDevice.h"
#include "CommonUtils.h"

class GhostImage
{
private:
	ObjectHandler* _objectHandler;
	int _gameObjectId;
	PickingDevice* _pickingDevice;

public:
	GhostImage(ObjectHandler* objectHandler, PickingDevice* pickingDevice);
	~GhostImage();
	
	void AddGhostImage(System::SpecificBlueprint blueprint, System::MouseCoord coord);
	void Update(System::MouseCoord coord);
	void RemoveGhostImage();
	bool IsGhostImageActive();
};