#ifndef BASESTATE_H
#define BASESTATE_H

#include "InitVar.h"

enum State
{
	SPLASHSTATE,
	MENUSTATE,
	PLAYSTATE,
	OPTIONSSTATE,
	EXITSTATE,
};

__declspec(align(16))class BaseState
{
private:
	State					_newStateRequest;
protected:
	ObjectHandler*		_objectHandler;
	UIHandler*			_uiHandler;
	void ChangeState(State newState);
public:
	BaseState(InitVar initVar);
	~BaseState();

	virtual void Update(float deltaTime) = 0;
	virtual void OnStateEnter() = 0;
	virtual void OnStateExit() = 0;

	State GetNewStateRequest()const;

	void* operator new(size_t i)
	{
		return _mm_malloc(i,16);
	}

		void operator delete(void* p)
	{
		_mm_free(p);
	}
};

#endif