#ifndef EXAMPLE_H
#define EXAMPLE_H

#define RENDERER_EXPORT __declspec(dllexport)

namespace Renderer
{
	class RENDERER_EXPORT Example 
	{
	private:
		int _value;
	public:
		Example(int value);
		int GetValue()const;
	};
}

#endif