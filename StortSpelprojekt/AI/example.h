#ifndef EXAMPLE_H
#define EXAMPLE_H

#define AI_EXPORT __declspec(dllexport)

namespace AI
{
	class AI_EXPORT Example 
	{
	private:
		int _value;
	public:
		Example(int value);
		int GetValue()const;
	};
}

#endif