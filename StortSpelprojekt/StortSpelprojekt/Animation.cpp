#include"Animation.h"

Animation::Animation(Skeleton* skeleton)
{
	_skeleton = skeleton;
	toRootTransforms.resize(_skeleton->_skeleton.size());
	toParentTransforms.resize(_skeleton->_skeleton.size());
	finalTransforms.resize(_skeleton->_skeleton.size());
	for (unsigned int i = 0; i < finalTransforms.size(); i++)
	{
		finalTransforms[i] = DirectX::XMMatrixIdentity();
	}
	_animTime = 0.0f;
	_currentAction = -1;
	_currentCycle = 0;
}

Animation::~Animation()
{

}

void Animation::Update(float time)
{
	_animTime += time / 1000;
	if (_currentAction != -1)
	{
		if (_skeleton->_actions[_currentAction]._bones[0]._frameTime.back() < _animTime)
		{
			_currentAction = -1;
			_animTime = 0.0f;
		}
		else
		{
			for (unsigned int i = 0; i < _skeleton->_skeleton.size(); i++)
			{
				toParentTransforms[i] = Interpolate(i, _currentAction);
			}
		}
	}
	if (_currentAction == -1)
	{
		if (_skeleton->_actions[_currentCycle]._bones[0]._frameTime.back() < _animTime)
		{
			_animTime -= _skeleton->_actions[_currentCycle]._bones[0]._frameTime.back();
		}
		for (unsigned int i = 0; i < _skeleton->_skeleton.size(); i++)
		{
			toParentTransforms[i] = Interpolate(i, _currentCycle);
		}
	}

	toRootTransforms[0] = toParentTransforms[0];

	for (unsigned int i = 1; i < _skeleton->_skeleton.size(); i++)
	{
		// Current bone transform relative to its parent
		toRootTransforms[i] = toParentTransforms[i] * toRootTransforms[_skeleton->_skeleton[i]._parent];
	}

	for (unsigned int i = 0; i < _skeleton->_skeleton.size(); i++)
	{
		finalTransforms[i] = DirectX::XMMatrixTranspose(_skeleton->_skeleton[i]._inverseBindpose * toRootTransforms[i]);
	}
}

void Animation::SetActionAsCycle(int action)
{
	_animTime = 0.0f;
	_currentCycle = action;
}

void Animation::PlayAction(int action)
{
	_animTime = 0.0f;
	_currentAction = action;
}

DirectX::XMMATRIX Animation::Interpolate(unsigned int boneID, int action)
{
	BoneFrames* boneptr = &_skeleton->_actions[action]._bones[boneID];
	DirectX::XMMATRIX matrix;
	_lastFrame = boneptr->_frameTime.size() - 1;
	if (_animTime <= boneptr->_frameTime[0])
	{
		matrix = DirectX::XMMatrixAffineTransformation(
			boneptr->_frames[0]._scale,
			_zeroVector,
			boneptr->_frames[0]._rotation,
			boneptr->_frames[0]._translation);
	}

	else if (_animTime >= boneptr->_frameTime[_lastFrame])
	{
		matrix = DirectX::XMMatrixAffineTransformation(
			boneptr->_frames[_lastFrame]._scale,
			_zeroVector,
			boneptr->_frames[_lastFrame]._rotation,
			boneptr->_frames[_lastFrame]._translation);
	}

	else
	{
		for (unsigned int i = 0; i <= boneptr->_frameTime.size(); i++)
		{
			_currTime = boneptr->_frameTime[i];
			_nextTime = boneptr->_frameTime[i + 1];
			if (_animTime >= _currTime && _animTime <= _nextTime)
			{
				_lerpPercent = (_animTime - _currTime) / (_nextTime - _currTime);
				/*
				DirectX::XMVECTOR s0 = boneptr->_frames[i]._scale;
				DirectX::XMVECTOR s1 = boneptr->_frames[i + 1]._scale;

				DirectX::XMVECTOR p0 = boneptr->_frames[i]._translation;
				DirectX::XMVECTOR p1 = boneptr->_frames[i + 1]._translation;

				DirectX::XMVECTOR q0 = boneptr->_frames[i]._rotation;
				DirectX::XMVECTOR q1 = boneptr->_frames[i + 1]._rotation;

				DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, _lerpPercent);
				DirectX::XMVECTOR p = DirectX::XMVectorLerp(p0, p1, _lerpPercent);
				DirectX::XMVECTOR q = DirectX::XMQuaternionSlerp(q0, q1, _lerpPercent);

				matrix = DirectX::XMMatrixAffineTransformation(s, _zeroVector, q, p);
				*/

				matrix = DirectX::XMMatrixAffineTransformation(DirectX::XMVectorLerp(boneptr->_frames[i]._scale, boneptr->_frames[i + 1]._scale, _lerpPercent),
					_zeroVector,
					DirectX::XMQuaternionSlerp(boneptr->_frames[i]._rotation, boneptr->_frames[i + 1]._rotation, _lerpPercent),
					DirectX::XMVectorLerp(boneptr->_frames[i]._translation, boneptr->_frames[i + 1]._translation, _lerpPercent));

				break;
			}
		}
	}
	return matrix;
}

void* Animation::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void Animation::operator delete(void* p)
{
	_mm_free(p);
}