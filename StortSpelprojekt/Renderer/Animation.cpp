#include"Animation.h"

Animation::Animation(Skeleton* skeleton, bool firstFrame, bool frozen)
{
	_skeleton = skeleton;
	_boneCount = _skeleton->_parents.size();
	toRootTransforms = (XMMATRIX*)_aligned_malloc(sizeof(XMMATRIX) * _boneCount, 16);
	toParentTransforms = (XMMATRIX*)_aligned_malloc(sizeof(XMMATRIX) * _boneCount, 16);
	finalTransforms = (XMMATRIX*)_aligned_malloc(sizeof(XMMATRIX) * _boneCount, 16);

	_animTime = 0.0f;
	_currentAction = -1;
	_currentCycle = 0;
	_inactive = false;
	_lastFrame = false;
	_isFinished = true;

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
		}
	}
	for (int i = 0; i < _skeleton->_actions.size(); i++)
	{
		_length.push_back(int());
		float maxTime = 0.0f;
		for (int j = 0; j < _boneCount; j++)
		{
			for (int k = 0; k < _skeleton->_actions[i]._bones[j]._frameTime.size(); k++)
			{
				if (_skeleton->_actions[i]._bones[j]._frameTime[k] > maxTime)
				{
					_length[i] = _skeleton->_actions[i]._bones[j]._frameTime[k] * 60;
				}
			}
		}
	}

	_frozen = frozen;
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

	_animTime += (time / 1000);

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
	}
}

XMMATRIX* Animation::GetTransforms()
{
	return finalTransforms;
}

int Animation::GetBoneCount() const
{
	return _boneCount;
}

void Animation::SetActionAsCycle(int action, bool reset)
{
	if (reset)
	{
		_animTime = 0.0f;
	}
	_currentCycle = action;
}

void Animation::Freeze(bool freeze)
{
	_frozen = freeze;
}

void Animation::PlayAction(int action, bool freeze, bool lastFrame, bool reset)
{
	_frozen = false;
	_animTime = 0.0f;
	_currentAction = action;
	_isFinished = false;
	if (freeze)
	{
		_inactive = true;
	}
	if (reset)
	{
		_animTime = 0.0f;
	}
	_lastFrameRender = lastFrame;
}

bool Animation::GetisFinished()
{
	return _isFinished;
}

float Animation::GetLength(int animation)
{
	int test = _length[animation];
	return test;
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