#include "example.h"

namespace Renderer
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
