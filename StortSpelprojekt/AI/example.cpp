#include "example.h"

namespace AI
{
	Example::Example(int value)
	{
		_value = value;
	}

	int Example::GetValue()const
	{ 
		return _value; 
	}
}
