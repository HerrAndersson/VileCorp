#include "Particle.h"
#include <stdexcept>
using namespace DirectX;

namespace Renderer
{
	Particle::Particle()
	{
		_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		_direction = XMFLOAT3(0.0f, 1.0f, 0.0f);
		_color = XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f);
		_isActive = false;
	}

	Particle::Particle(const XMFLOAT3& position, const XMFLOAT3& direction, const XMFLOAT4& color)
	{
		_position = position;
		_direction = direction;
		_color = color;
	}

	Particle::~Particle()
	{
	}

	void Particle::Reset(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT4& color)
	{
		_position = position;
		_direction = direction;
		_color = color;
	}

	void Particle::Update(double deltaTime, const ParticleType& type)
	{
		//TODO: Update postions according to the type and how it moves
		switch (type)
		{
			case SPLASH:
			{
				float p = deltaTime / 50.0f;
				_position.x += p;
				_position.y++;
				_position.z += p;
				break;
			}
			case SMOKE:
			{

				break;
			}
			case ELECTRICITY:
			{

				break;
			}
			default:
			{
				throw std::runtime_error("Particle::Update: Invalid particle type");
				break;
			}
		}
	}

	XMFLOAT3 Particle::GetPosition() const
	{
		return _position;
	}

	XMFLOAT3 Particle::GetDirection() const
	{
		return _direction;
	}

	XMFLOAT4 Particle::GetColor() const
	{
		return _color;
	}

	void Particle::SetPosition(const XMFLOAT3& position)
	{
		_position = position;
	}

	void Particle::SetDirection(const XMFLOAT3& direction)
	{
		_direction = direction;
	}

	void Particle::SetColor(const XMFLOAT4& color)
	{
		_color = color;
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