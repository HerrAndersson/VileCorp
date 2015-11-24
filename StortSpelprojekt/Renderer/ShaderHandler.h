#ifndef SHADERHANDLER_H
#define SHADERHANDLER_H
#include <d3d11.h>

namespace Renderer
{
	class ShaderHandler
	{
	private:

		enum ShaderType 
		{ 
			VERTEX_SHADER, 
			HULL_SHADER, 
			GEOMETRY_SHADER, 
			DOMAIN_SHADER, 
			PIXEL_SHADER,
			COMPUTE_SHADER
		};

		//Layouts
		ID3D11InputLayout*		_layoutPosUvNorm;
		ID3D11InputLayout*		_geoPassLayout;

		//Vertex shaders
		ID3D11VertexShader*		_defaultVS;
		ID3D11VertexShader*		_geoPassVS;

		//Geometry shaders
		ID3D11GeometryShader*	_geoPassGS;

		//Pixel shaders
		ID3D11PixelShader*		_defaultPS;
		ID3D11PixelShader*		_geoPassPS;


		ID3D11DeviceChild* CreateShader(ID3D11Device* device, LPCWSTR fileName, ShaderType type, D3D11_INPUT_ELEMENT_DESC* inputDesc = nullptr, int inputDescSize = -1);

	public:

		ShaderHandler(ID3D11Device* device);
		~ShaderHandler();

		void SetDefaultShaders(ID3D11DeviceContext* deviceContext);
		void SetGeometryPassShaders(ID3D11DeviceContext* deviceContext);
		//void SetShadowMapShaders() etc
	};
}

#endif