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
		VertexShaderData*		_animPassVS;
		VertexShaderData*		_shadowMapVS;
		VertexShaderData*		_gridPassVS;
		VertexShaderData*		_hudPassVS;
		VertexShaderData*		_lightApplyLightVolumeVS;

		//Pixel shaders
		ID3D11PixelShader*		_defaultPS;
		ID3D11PixelShader*		_geoPassPS;
		ID3D11PixelShader*		_lightPassPS;
		ID3D11PixelShader*		_gridPassPS;
		ID3D11PixelShader*		_lightApplySpotlightVolumePS;
		ID3D11PixelShader*		_lightApplyPointlightVolumePS;
		ID3D11PixelShader*		_lightApplyScreenQuadPS;
		ID3D11PixelShader*		_hudPassPS;
		ID3D11PixelShader*		_fxaaPassPS;

		//Samplers
		ID3D11SamplerState*		_samplerWRAP;
		ID3D11SamplerState*		_samplerPOINT;
		ID3D11SamplerState*		_samplerCLAMP;
		ID3D11SamplerState*		_samplerCMP;

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
		void SetGeometryStageShaders(ID3D11DeviceContext* deviceContext);
		void SetFinalPassShaders(ID3D11DeviceContext* deviceContext);
		void SetAnimationPassShaders(ID3D11DeviceContext* deviceContext);
		void SetGridPassShaders(ID3D11DeviceContext* deviceContest);
		void SetShadowGenerationShaders(ID3D11DeviceContext* deviceContext);
		void SetSpotlightApplicationShaders(ID3D11DeviceContext* deviceContext);
		void SetPointlightApplicationShaders(ID3D11DeviceContext* deviceContext);
		void SetHUDPassShaders(ID3D11DeviceContext* deviceContext);
		void SetFXAAPassShaders(ID3D11DeviceContext* deviceContext);
	};
}

#endif