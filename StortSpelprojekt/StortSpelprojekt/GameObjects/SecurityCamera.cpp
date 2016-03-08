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

SecurityCamera::SecurityCamera(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject * renderObject, System::SoundModule* soundModule, Renderer::ParticleEventQueue* particleEventQueue, const Tilemap * tileMap, AI::Vec2D direction)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject, soundModule, particleEventQueue, DirectX::XMFLOAT3(0, 0, 0), 0, direction)
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
	_particleEventQueue->Insert(new ParticleUpdateMessage(GetID(), false));
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
	ShowAreaOfEffect();
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
	ShowAreaOfEffect();
}

int SecurityCamera::GetVisionRadius() const
{
	return _visionRadius;
}

//Info colors
void SecurityCamera::ShowAreaOfEffect()
{
	HideAreaOfEffect();
	ParticleRequestMessage* msg;

	XMFLOAT3 pos = this->_position;
	pos.y += 0.04f;

	GameObject::ShowAreaOfEffect();

	for (int i = 0; i < _visionCone->GetNrOfVisibleTiles(); i++)
	{
		AI::Vec2D tile = _visionCone->GetVisibleTiles()[i];
		XMFLOAT3 pos = XMFLOAT3(tile._x, 0.04f, tile._y);

		msg = new ParticleRequestMessage(ParticleType::STATIC_ICON, ParticleSubType::AOE_YELLOW_SUBTYPE, _ID, pos, XMFLOAT3(0, 1, 0), 1.0f, 1, 0.27f, true, true);
		_particleEventQueue->Insert(msg);
	}
}