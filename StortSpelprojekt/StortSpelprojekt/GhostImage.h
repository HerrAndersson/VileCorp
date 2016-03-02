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
	
	void AddGhostImage(System::SpecificBlueprint blueprint, const System::MouseCoord& coord);
	void Update(const System::MouseCoord& coord);
	void RemoveGhostImage();
	bool IsGhostImageActive();
};