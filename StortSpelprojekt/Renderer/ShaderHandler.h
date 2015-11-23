#ifndef SHADERHANDLER_H
#define SHADERHANDLER_H

namespace Renderer
{
	class ShaderHandler
	{
	private:

	public:
		ShaderHandler();
		~ShaderHandler();

		bool InitShaders();
		void SetDefaultShaders();
	};
}

#endif