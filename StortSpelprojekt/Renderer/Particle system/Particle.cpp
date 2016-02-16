#include "Particle.h"
#include <stdexcept>
using namespace DirectX;

namespace Renderer
{
	Particle::Particle()
	{
		_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		_direction = XMFLOAT3(0.0f, 1.0f, 0.0f);
		_isActive = false;
	}

	Particle::Particle(const XMFLOAT3& position, const XMFLOAT3& target, const XMFLOAT3& direction)
	{
		_position = position;
		_direction = direction;
		_target = target;
	}

	Particle::~Particle()
	{
	}

	void Particle::Reset(const XMFLOAT3& position, const XMFLOAT3& target, const XMFLOAT3& direction)
	{
		_position = position;
		_direction = direction;
		_target = target;
	}

	XMFLOAT3 Particle::GetPosition() const
	{
		return _position;
	}

	XMFLOAT3 Particle::GetDirection() const
	{
		return _direction;
	}

	DirectX::XMFLOAT3 Particle::GetTargetPosition() const
	{
		return _target;
	}

	void Particle::SetPosition(const XMFLOAT3& position)
	{
		_position = position;
	}

	void Particle::SetDirection(const XMFLOAT3& direction)
	{
		_direction = direction;
	}

	void Particle::SetTargetPosition(const DirectX::XMFLOAT3& target)
	{
		_target = target;
	}

	bool Particle::IsActive() const
	{
		return _isActive;
	}

	void Particle::Activate()
	{
		_isActive = true;
	}

	void Particle::Deactivate()
	{
		_isActive = false;
	}
}
