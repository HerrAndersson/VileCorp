#pragma once
#include <DirectXMath.h>

class AmbientLight
{
private:
	DirectX::XMFLOAT3 _ambientLight;
	int _scale;
	float _scaleIncrease;

public:
	AmbientLight();
	~AmbientLight();

	void DayTime();
	void NightTime();
	void SetScale(int scale);

	DirectX::XMFLOAT3 GetAmbientLight()const;
	int GetScale()const;
};