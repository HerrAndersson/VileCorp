#include "AmbientLight.h"

AmbientLight::AmbientLight()
{
	_ambientLight = DirectX::XMFLOAT3(0, 0, 0);
	_scale = 5;
	_scaleIncrease = 0.06f;
}

AmbientLight::~AmbientLight() {}

void AmbientLight::DayTime()
{
	_ambientLight = DirectX::XMFLOAT3(0.3f+_scale*_scaleIncrease, 0.3f + _scale*_scaleIncrease, 0.3f + _scale*_scaleIncrease);
}
void AmbientLight::NightTime()
{
	_ambientLight = DirectX::XMFLOAT3(0.1f + _scale*_scaleIncrease, 0.1f + _scale*_scaleIncrease, 0.1f + _scale*_scaleIncrease);
}

void AmbientLight::SetScale(int scale)
{
	_scale = scale;
}

DirectX::XMFLOAT3 AmbientLight::GetAmbientLight()const
{
	return _ambientLight;
}