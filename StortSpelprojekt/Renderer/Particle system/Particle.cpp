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
		_speed = 0;
		_timeLeft = 0;
	}

	Particle::Particle(const XMFLOAT3& position, float speed, float timeLeft, const XMFLOAT3& direction)
	{
		_position = position;
		_direction = direction;
		_speed = speed;
		_timeLeft = timeLeft;
	}

	Particle::~Particle()
	{
	}

	XMFLOAT3 Particle::GetPosition() const
	{
		return _position;
	}

	XMFLOAT3 Particle::GetDirection() const
	{
		return _direction;
	}

	float Particle::GetSpeed() const
	{
		return _speed;
	}

	float Particle::GetTimeLeft() const
	{
		return _timeLeft;
	}

	void Particle::SetPosition(const XMFLOAT3& position)
	{
		_position = position;
	}

	void Particle::SetDirection(const XMFLOAT3& direction)
	{
		_direction = direction;
	}

	void Particle::SetSpeed(float speed)
	{
		_speed = speed;
	}

	void Particle::ResetTimeLeft(float time)
	{
		_timeLeft = time;
	}

	void Particle::DecreaseTimeLeft(float deltaTime)
	{
		_timeLeft -= deltaTime;
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
