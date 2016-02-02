#pragma once

#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>
#include "RenderUtils.h"

using namespace DirectX;

class Animation
{
private:

	XMMATRIX Interpolate(unsigned int boneID, int action);

	float _time;
	float _currTime;
	float _nextTime;
	float _lerpPercent;
	unsigned _lastFrame, _boneCount;
	XMVECTOR _zeroVector = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMMATRIX* toRootTransforms;
	XMMATRIX* toParentTransforms;
	XMMATRIX* finalTransforms;
	Skeleton* _skeleton;
	float _animTime;
	int _currentCycle, _currentAction;

public:
	Animation(Skeleton* skeleton);
	~Animation();

	void Update(float time);
	void SetActionAsCycle(int action);
	void PlayAction(int action);
	XMMATRIX* GetTransforms()
	{
		return finalTransforms;
	}
	int GetBoneCount() const
	{
		return _boneCount;
	} 
};