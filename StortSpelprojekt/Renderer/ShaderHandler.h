#ifndef SHADERHANDLER_H
#define SHADERHANDLER_H
#include <d3d11.h>
#include <string>
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
		VertexShaderData*		_geoPassVS;
		VertexShaderData*		_passthroughVS;
		VertexShaderData*		_animPassVS;
		VertexShaderData*		_animShadowMapVS;
		VertexShaderData*		_shadowMapVS;
		VertexShaderData*		_linestripVS;
		VertexShaderData*		_hudPassVS;
		VertexShaderData*		_lightApplyLightVolumeVS;
		VertexShaderData*		_billboardingVS;

		//Pixel shaders
		ID3D11PixelShader*		_geoPassPS;
		ID3D11PixelShader*		_linestripPS;
		ID3D11PixelShader*		_lightVolumeSpotPS;
		ID3D11PixelShader*		_lightVolumePointPS;
		ID3D11PixelShader*		_lightVolumeSpotNoShadowsPS;
		ID3D11PixelShader*		_hudPassPS;
		ID3D11PixelShader*		_fxaaPassPS;
		ID3D11PixelShader*		_billboardingPS;
		ID3D11PixelShader*		_noFxaaPS;

		//Geometry shaders
		ID3D11GeometryShader*   _billboardingGS;

		//Samplers
		ID3D11SamplerState*		_samplerWRAP;
		ID3D11SamplerState*		_samplerPOINT;
		ID3D11SamplerState*		_samplerCLAMP;
		ID3D11SamplerState*		_samplerCMP;

		VertexShaderData* CreateVertexShader(ID3D11Device* device, const std::wstring& fileName, D3D11_INPUT_ELEMENT_DESC* inputDesc, int inputDescSize);

		ID3D11HullShader* CreateHullShader(ID3D11Device* device, const std::wstring& fileName);
		ID3D11GeometryShader* CreateGeometryShader(ID3D11Device* device, const std::wstring& fileName);
		ID3D11DomainShader* CreateDomainShader(ID3D11Device* device, const std::wstring& fileName);
		ID3D11PixelShader* CreatePixelShader(ID3D11Device* device, const std::wstring& fileName);
		ID3D11ComputeShader* CreateComputeShader(ID3D11Device* device, const std::wstring& fileName);

	public:

		ShaderHandler(ID3D11Device* device);
		~ShaderHandler();

		void SetGeometryStageShaders(ID3D11DeviceContext* deviceContext);
		void SetAnimationPassShaders(ID3D11DeviceContext* deviceContext);
		void SetLinestripShaders(ID3D11DeviceContext* deviceContest);
		void SetAnimaShadowGenerationShaders(ID3D11DeviceContext* deviceContext);
		void SetShadowGenerationShaders(ID3D11DeviceContext* deviceContext);
		void SetSpotlightApplicationShaders(ID3D11DeviceContext* deviceContext, bool shadowsEnabled = true);
		void SetPointlightApplicationShaders(ID3D11DeviceContext* deviceContext);
		void SetHUDPassShaders(ID3D11DeviceContext* deviceContext);
		void SetFXAAPassShaders(ID3D11DeviceContext* deviceContext, bool enabled);
		void SetBillboardingStageShaders(ID3D11DeviceContext* deviceContext);
	};
}

#endif