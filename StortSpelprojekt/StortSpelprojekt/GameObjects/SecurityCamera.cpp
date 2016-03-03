#include "SecurityCamera.h"

void SecurityCamera::Rotate()
{
	if (_direction._x != 0 || _direction._y != 0)
	{
		if (_direction._x == 0)
		{
			_rotation.y = DirectX::XM_PIDIV2 * (_direction._y + 1);
		}
		else if (_direction._x == -1)
		{
			_rotation.y = DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4 * _direction._y;
		}
		else
		{
			_rotation.y = 3 * DirectX::XM_PIDIV2 - DirectX::XM_PIDIV4 * _direction._y;
		}
		CalculateMatrix();
	}
	//_visionCone->ColorVisibleTiles({0,0,0});
	_visionCone->FindVisibleTiles(_tilePosition, _direction);
	//_visionCone->ColorVisibleTiles({0,0,3});
	CheckVisibleTiles();
}

SecurityCamera::SecurityCamera()
	: GameObject()
{
	_tileMap = nullptr;
	_direction = {0,-1};
	_visionRadius = 0;
	_visionCone = nullptr;
}

SecurityCamera::SecurityCamera(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject * renderObject, System::SoundModule* soundModule, const Tilemap * tileMap)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject, soundModule)
{
	_tileMap = tileMap;
	_direction = {0,-1};
	_visionRadius = 7;
	_visionCone = new VisionCone(_visionRadius, _tileMap);
	_visionCone->FindVisibleTiles(_tilePosition, _direction);
	_subType = 0;
}

SecurityCamera::~SecurityCamera()
{
	delete _visionCone;
}

AI::Vec2D SecurityCamera::GetDirection() const
{
	return _direction;
}

void SecurityCamera::SetDirection(AI::Vec2D direction)
{
	_direction = direction;
	_visionCone->FindVisibleTiles(_tilePosition, _direction);

}

void SecurityCamera::CheckVisibleTiles()
{
	Enemy* test = nullptr;
	//_visionCone->ColorVisibleTiles({0,0,3});
	AI::Vec2D tile = {0,0};
	for (int i = 0; i < _visionCone->GetNrOfVisibleTiles(); i++)
	{
		tile = _visionCone->GetVisibleTile(i);
		if (_tileMap->IsEnemyOnTile(tile))
		{
			Enemy* enemy;
			enemy = static_cast<Enemy*>(_tileMap->GetObjectOnTile(tile, System::ENEMY));
			if (enemy != nullptr)
			{
				enemy->ResetVisibilityTimer();
			}
		}
	}
}

void SecurityCamera::Update(float deltaTime)
{
	CheckVisibleTiles();
}

void SecurityCamera::Release()
{}

void SecurityCamera::SetTilePosition(AI::Vec2D pos)
{
	GameObject::SetTilePosition(pos);
	_visionCone->FindVisibleTiles(_tilePosition, _direction);
}

int SecurityCamera::GetVisionRadius() const
{
	return _visionRadius;
}
