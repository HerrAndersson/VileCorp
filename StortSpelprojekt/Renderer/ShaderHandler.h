#ifndef SHADERHANDLER_H
#define SHADERHANDLER_H
#include <d3d11.h>

#include "stdafx.h"

namespace Renderer
{
	class ShaderHandler
	{
	private:
		struct VertexShaderData
		{
			ID3D11VertexShader* _vertexShader;
			ID3D11InputLayout* _inputLayout;

			VertexShaderData(ID3D11VertexShader* vertexShader, ID3D11InputLayout* inputLayout)
			{
				_vertexShader = vertexShader;
				_inputLayout = inputLayout;
			}

			~VertexShaderData()
			{
				SAFE_RELEASE(_vertexShader);
				SAFE_RELEASE(_inputLayout);
			}
		};

		//Vertex shaders
		VertexShaderData*		_defaultVS;
		VertexShaderData*		_geoPassVS;
		VertexShaderData*		_lightPassVS;
		VertexShaderData*		_gridPassVS;

		//Pixel shaders
		ID3D11PixelShader*		_defaultPS;
		ID3D11PixelShader*		_geoPassPS;
		ID3D11PixelShader*		_lightPassPS;
		ID3D11PixelShader*		_gridPassPS;

		//Samplers
		ID3D11SamplerState*		_samplerWRAP;

		VertexShaderData* CreateVertexShader(ID3D11Device* device, LPCWSTR fileName, D3D11_INPUT_ELEMENT_DESC* inputDesc, int inputDescSize);

		ID3D11HullShader* CreateHullShader(ID3D11Device* device, LPCWSTR fileName);
		ID3D11GeometryShader* CreateGeometryShader(ID3D11Device* device, LPCWSTR fileName);
		ID3D11DomainShader* CreateDomainShader(ID3D11Device* device, LPCWSTR fileName);
		ID3D11PixelShader* CreatePixelShader(ID3D11Device* device, LPCWSTR fileName);
		ID3D11ComputeShader* CreateComputeShader(ID3D11Device* device, LPCWSTR fileName);

	public:

		ShaderHandler(ID3D11Device* device);
		~ShaderHandler();

		void SetDefaultShaders(ID3D11DeviceContext* deviceContext);
		void SetGeometryPassShaders(ID3D11DeviceContext* deviceContext);
		void SetLightPassShaders(ID3D11DeviceContext* deviceContext);
		void SetGridPassShaders(ID3D11DeviceContext* deviceContest);
	};
}

#endif