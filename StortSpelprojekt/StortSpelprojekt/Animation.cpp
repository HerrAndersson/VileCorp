#include"Animation.h"

Animation::Animation(Skeleton* skeleton, bool firstFrame)
{
	_skeleton = skeleton;
	_boneCount = _skeleton->_parents.size();
	toRootTransforms = (XMMATRIX*)_aligned_malloc(sizeof(XMMATRIX) * _boneCount, 16);
	toParentTransforms = (XMMATRIX*)_aligned_malloc(sizeof(XMMATRIX) * _boneCount, 16);
	finalTransforms = (XMMATRIX*)_aligned_malloc(sizeof(XMMATRIX) * _boneCount, 16);
	finalFloats.resize(_boneCount);

	_animTime = 0.0f;
	_currentAction = -1;
	_currentCycle = 0;
	_inactive = false;
	_lastFrame = false;
	_speed = 1.0f;
	_isFinished = false;

	if (firstFrame)
	{
		_frozen = false;
		Update(_skeleton->_actions[0]._bones[0]._frameTime[0]);
		_frozen = true;
	}
	else
	{
		for (unsigned i = 0; i < _boneCount; i++)
		{
			finalTransforms[i] = XMMatrixIdentity();
			XMStoreFloat4x4(&finalFloats[i], XMMatrixIdentity());
		}
	}
}

Animation::~Animation()
{
	_aligned_free(toRootTransforms);
	_aligned_free(toParentTransforms);
	_aligned_free(finalTransforms);
}

void Animation::Update(float time)
{
	if (_frozen)
	{
		return;
	}
	_animTime += (time / 1000) * _speed;
	if (_currentAction != -1)
	{
		if (_skeleton->_actions[_currentAction]._bones[0]._frameTime.back() < _animTime)
		{
			_animTime = 0.0f;
			if (_lastFrameRender)
			{
				_animTime = _skeleton->_actions[_currentAction]._bones[0]._frameTime.back();
			}
			if (_inactive)
			{
				_inactive = false;
				_frozen = true;
			}
			_currentAction = -1;
			_isFinished = true;
		}
		else
		{
			for (unsigned i = 0; i < _boneCount; i++)
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
		for (unsigned i = 0; i < _boneCount; i++)
		{
			toParentTransforms[i] = Interpolate(i, _currentCycle);
		}
	}

	toRootTransforms[0] = toParentTransforms[0];

	for (unsigned i = 1; i < _boneCount; i++)
	{
		// Current bone transform relative to its parent
		toRootTransforms[i] = toParentTransforms[i] * toRootTransforms[_skeleton->_parents[i]];
	}

	for (unsigned i = 0; i < _boneCount; i++)
	{
		finalTransforms[i] = XMMatrixTranspose(_skeleton->_bindposes[i] * toRootTransforms[i]);
		XMStoreFloat4x4(&finalFloats[i], finalTransforms[i]);
	}
}

void Animation::SetActionAsCycle(int action, float speed, bool reset)
{
	if (reset)
	{
		_animTime = 0.0f;
	}
	_currentCycle = action;
	_speed = speed;
}

void Animation::Freeze(bool freeze)
{
	_frozen = freeze;
}

void Animation::SetSpeed(float speed)
{
	_speed = speed;
}

void Animation::PlayAction(int action, float speed, bool freeze, bool lastFrame)
{
	_frozen = false;
	_animTime = 0.0f;
	_currentAction = action;
	_isFinished = false;
	if (freeze)
	{
		_inactive = true;
	}
	_lastFrameRender = lastFrame;
	_speed = speed;
}

bool Animation::GetisFinished()
{
	return _isFinished;
}

XMMATRIX Animation::Interpolate(unsigned boneID, int action)
{
	BoneFrames* boneptr = &_skeleton->_actions[action]._bones[boneID];
	XMMATRIX matrix;
	_lastFrame = boneptr->_frameTime.size() - 1;
	if (_animTime <= boneptr->_frameTime[0])
	{
		matrix = XMMatrixAffineTransformation(
			boneptr->_frames[0]._scale,
			_zeroVector,
			boneptr->_frames[0]._rotation,
			boneptr->_frames[0]._translation);
	}

	else if (_animTime >= boneptr->_frameTime[_lastFrame])
	{
		matrix = XMMatrixAffineTransformation(
			boneptr->_frames[_lastFrame]._scale,
			_zeroVector,
			boneptr->_frames[_lastFrame]._rotation,
			boneptr->_frames[_lastFrame]._translation);
	}

	else
	{
		for (unsigned i = 0; i <= boneptr->_frameTime.size(); i++)
		{
			_currTime = boneptr->_frameTime[i];
			_nextTime = boneptr->_frameTime[i + 1];
			if (_animTime >= _currTime && _animTime <= _nextTime)
			{
				_lerpPercent = (_animTime - _currTime) / (_nextTime - _currTime);
				/*
				XMVECTOR s0 = boneptr->_frames[i]._scale;
				XMVECTOR s1 = boneptr->_frames[i + 1]._scale;

				XMVECTOR p0 = boneptr->_frames[i]._translation;
				XMVECTOR p1 = boneptr->_frames[i + 1]._translation;

				XMVECTOR q0 = boneptr->_frames[i]._rotation;
				XMVECTOR q1 = boneptr->_frames[i + 1]._rotation;

				XMVECTOR s = XMVectorLerp(s0, s1, _lerpPercent);
				XMVECTOR p = XMVectorLerp(p0, p1, _lerpPercent);
				XMVECTOR q = XMQuaternionSlerp(q0, q1, _lerpPercent);

				matrix = XMMatrixAffineTransformation(s, _zeroVector, q, p);
				*/

				matrix = XMMatrixAffineTransformation(XMVectorLerp(boneptr->_frames[i]._scale, boneptr->_frames[i + 1]._scale, _lerpPercent),
					_zeroVector,
					XMQuaternionSlerp(boneptr->_frames[i]._rotation, boneptr->_frames[i + 1]._rotation, _lerpPercent),
					XMVectorLerp(boneptr->_frames[i]._translation, boneptr->_frames[i + 1]._translation, _lerpPercent));

				break;
			}
		}
	}
	return matrix;
}