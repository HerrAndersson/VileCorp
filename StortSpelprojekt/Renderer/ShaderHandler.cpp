#include "ShaderHandler.h"
#include <d3dcompiler.h>
#include <stdexcept>
#include <string>
#include <atlstr.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

namespace Renderer
{
	ShaderHandler::ShaderHandler(ID3D11Device* device)
	{

		////////////////////////////////////////////////////////////// Create samplers //////////////////////////////////////////////////////////////
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));

		// sampler description
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MipLODBias = 0;
		sampDesc.MaxAnisotropy = 1;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		HRESULT result = device->CreateSamplerState(&sampDesc, &_samplerWRAP);
		if (FAILED(result))
		{
			throw std::runtime_error("ShaderHandler::ShaderHandler: Error creating WRAP sampler");
		}

		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		//Create a WRAP texture sampler state description
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		result = device->CreateSamplerState(&samplerDesc, &_samplerWRAP);
		if (FAILED(result))
		{
			throw std::runtime_error("ShaderHandler: _samplerWRAP initialization failed.");
		}

		//Create a CLAMP texture sampler state description.
		//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

		result = device->CreateSamplerState(&samplerDesc, &_samplerCLAMP);
		if (FAILED(result))
		{
			throw std::runtime_error("ShaderHandler: _samplerCLAMP initialization failed.");
		}

		//Create a COMPARISON sampler state
		samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 1.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;
		samplerDesc.MinLOD = 0.f;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		samplerDesc.MipLODBias = 0.f;
		samplerDesc.MaxAnisotropy = 0;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

		result = device->CreateSamplerState(&samplerDesc, &_samplerCMP);
		if (FAILED(result))
		{
			throw std::runtime_error("ShaderHandler: _samplerCMP initialization failed.");
		}

		////////////////////////////////////////////////////////////// Create Shaders ///////////////////////////////////////////////////////////////
		D3D11_INPUT_ELEMENT_DESC inputDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		int numElements = sizeof(inputDesc) / sizeof(inputDesc[0]);

		_defaultVS = CreateVertexShader(device, L"Assets/Shaders/DefaultVS.hlsl", inputDesc, numElements);
		_defaultPS = CreatePixelShader(device, L"Assets/Shaders/DefaultPS.hlsl");

		//GEOMETRY pass init
		_geoPassVS = CreateVertexShader(device, L"Assets/Shaders/GeoVS.hlsl", inputDesc, numElements);
		_geoPassPS = CreatePixelShader(device, L"Assets/Shaders/GeoPS.hlsl");

		//LIGHT pass init
		D3D11_INPUT_ELEMENT_DESC lightInputDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		numElements = sizeof(lightInputDesc) / sizeof(lightInputDesc[0]);

		_lightPassVS = CreateVertexShader(device, L"Assets/Shaders/LightVS.hlsl", lightInputDesc, numElements);
		_lightPassPS = CreatePixelShader(device, L"Assets/Shaders/LightPS.hlsl");
		_lightApplySpotlightVolumePS = CreatePixelShader(device, L"Assets/Shaders/LightApplySpotlightVolumePS.hlsl");
		_lightApplyScreenQuadPS = CreatePixelShader(device, L"Assets/Shaders/LightApplyScreenQuadPS.hlsl");
		_lightApplyPointlightVolumePS = CreatePixelShader(device, L"Assets/Shaders/LightApplyPointlightVolumePS.hlsl");

		//Shadow map shaders init
		D3D11_INPUT_ELEMENT_DESC shadowInputDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		numElements = sizeof(shadowInputDesc) / sizeof(shadowInputDesc[0]);
		_shadowMapVS = CreateVertexShader(device, L"Assets/Shaders/ShadowVS.hlsl", shadowInputDesc, numElements);
		_lightApplyLightVolumeVS = CreateVertexShader(device, L"Assets/Shaders/LightApplyLightVolumeVS.hlsl", shadowInputDesc, numElements);

		//Animation pass init
		D3D11_INPUT_ELEMENT_DESC animInputDesc[] =
		{
			{ "BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		numElements = sizeof(animInputDesc) / sizeof(animInputDesc[0]);
		_animPassVS = CreateVertexShader(device, L"Assets/Shaders/AnimVS.hlsl", animInputDesc, numElements);

		//Grid pass init
		D3D11_INPUT_ELEMENT_DESC gridInputDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		numElements = sizeof(gridInputDesc) / sizeof(gridInputDesc[0]);

		_gridPassVS = CreateVertexShader(device, L"Assets/Shaders/GridVS.hlsl", gridInputDesc, numElements);
		_gridPassPS = CreatePixelShader(device, L"Assets/Shaders/GridPS.hlsl");

		//HUD pass init
		numElements = sizeof(lightInputDesc) / sizeof(lightInputDesc[0]);

		_hudPassVS = CreateVertexShader(device, L"Assets/Shaders/HudVS.hlsl", lightInputDesc, numElements);
		_hudPassPS = CreatePixelShader(device, L"Assets/Shaders/HudPS.hlsl");
	}

	ShaderHandler::~ShaderHandler()
	{
		delete _defaultVS;
		delete _geoPassVS;
		delete _lightPassVS;
		delete _animPassVS;
		delete _shadowMapVS;
		delete _gridPassVS;
		delete _hudPassVS;
		delete _lightApplyLightVolumeVS;

		SAFE_RELEASE(_defaultPS);
		SAFE_RELEASE(_geoPassPS);
		SAFE_RELEASE(_lightPassPS);
		SAFE_RELEASE(_gridPassPS);
		SAFE_RELEASE(_lightApplySpotlightVolumePS);
		SAFE_RELEASE(_lightApplyScreenQuadPS);
		SAFE_RELEASE(_hudPassPS);
		SAFE_RELEASE(_lightApplyPointlightVolumePS);

		SAFE_RELEASE(_samplerWRAP);
		SAFE_RELEASE(_samplerPOINT);
		SAFE_RELEASE(_samplerCLAMP);
		SAFE_RELEASE(_samplerCMP);
	}

	ShaderHandler::VertexShaderData* ShaderHandler::CreateVertexShader(ID3D11Device* device, LPCWSTR fileName, D3D11_INPUT_ELEMENT_DESC* inputDesc, int inputDescSize)
	{
		HRESULT result;
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* shaderBuffer = nullptr;
		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11InputLayout* inputLayout = nullptr;

		result = D3DCompileFromFile(fileName, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
		//result = D3DCompileFromFile(fileName, NULL, NULL, "main", "vs_5_0", D3DCOMPILE_DEBUG, 0, &shaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				throw std::runtime_error("ShaderHandler::CreateVertexShader: " + std::string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			}
			else
			{
				std::string s = ATL::CW2A(fileName);
				throw std::runtime_error("ShaderHandler::CreateVertexShader: Shader file not found. Filename: " + s);
			}
		}

		result = device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &vertexShader);
		if (FAILED(result))
		{
			std::string s = ATL::CW2A(fileName);
			throw std::runtime_error("ShaderHandler::CreateVertexShader: Shader file not found. Filename: " + s);
		}

		result = device->CreateInputLayout(inputDesc, inputDescSize, shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), &inputLayout);
		if (FAILED(result))
		{
			throw std::runtime_error("ShaderHandler::CreateVertexShader: Error creating input layout.");
		}
		return new VertexShaderData(vertexShader, inputLayout);
	}

	ID3D11HullShader* ShaderHandler::CreateHullShader(ID3D11Device* device, LPCWSTR fileName)
	{
		HRESULT result;
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* shaderBuffer = nullptr;
		ID3D11HullShader* hullShader = nullptr;

		result = D3DCompileFromFile(fileName, NULL, NULL, "main", "hs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				throw std::runtime_error("ShaderHandler::CreateHullShader: " + std::string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			}
			else
			{
				std::string s = ATL::CW2A(fileName);
				throw std::runtime_error("ShaderHandler::CreateHullShader: Shader file not found. Filename: " + s);
			}
		}

		result = device->CreateHullShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &hullShader);
		if (FAILED(result))
		{
			std::string s = ATL::CW2A(fileName);
			throw std::runtime_error("ShaderHandler::CreateHullShader: Shader file not found. Filename: " + s);
		}

		SAFE_RELEASE(errorMessage);
		SAFE_RELEASE(shaderBuffer);

		return hullShader;
	}
	ID3D11GeometryShader* ShaderHandler::CreateGeometryShader(ID3D11Device* device, LPCWSTR fileName)
	{
		HRESULT result;
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* shaderBuffer = nullptr;
		ID3D11GeometryShader* geometryShader = nullptr;

		result = D3DCompileFromFile(fileName, NULL, NULL, "main", "gs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				throw std::runtime_error("ShaderHandler::CreateGeometryShader: " + std::string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			}
			else
			{
				std::string s = ATL::CW2A(fileName);
				throw std::runtime_error("ShaderHandler::CreateGeometryShader: Shader file not found. Filename: " + s);
			}
		}

		result = device->CreateGeometryShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &geometryShader);
		if (FAILED(result))
		{
			std::string s = ATL::CW2A(fileName);
			throw std::runtime_error("ShaderHandler::CreateGeometryShader: Shader file not found.. Filename: " + s);
		}

		SAFE_RELEASE(errorMessage);
		SAFE_RELEASE(shaderBuffer);

		return geometryShader;
	}
	ID3D11DomainShader* ShaderHandler::CreateDomainShader(ID3D11Device* device, LPCWSTR fileName)
	{
		HRESULT result;
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* shaderBuffer = nullptr;
		ID3D11DomainShader* domainShader = nullptr;

		result = D3DCompileFromFile(fileName, NULL, NULL, "main", "ds_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				throw std::runtime_error("ShaderHandler::CreateDomainShader: " + std::string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			}
			else
			{
				std::string s = ATL::CW2A(fileName);
				throw std::runtime_error("ShaderHandler::CreateDomainShader: Shader file not found. Filename: " + s);
			}
		}

		result = device->CreateDomainShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &domainShader);
		if (FAILED(result))
		{
			std::string s = ATL::CW2A(fileName);
			throw std::runtime_error("ShaderHandler::CreateDomainShader: Shader file not found. Filename: " + s);
		}

		SAFE_RELEASE(errorMessage);
		SAFE_RELEASE(shaderBuffer);

		return domainShader;
	}
	ID3D11PixelShader* ShaderHandler::CreatePixelShader(ID3D11Device* device, LPCWSTR fileName)
	{
		HRESULT result;
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* shaderBuffer = nullptr;
		ID3D11PixelShader* pixelShader = nullptr;

		result = D3DCompileFromFile(fileName, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
		//result = D3DCompileFromFile(fileName, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG, 0, &shaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				throw std::runtime_error("ShaderHandler::CreatePixelShader: " + std::string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			}
			else
			{
				std::string s = ATL::CW2A(fileName);
				throw std::runtime_error("ShaderHandler::CreatePixelShader: Shader file not found. Filename: " + s);
			}
		}

		result = device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &pixelShader);
		if (FAILED(result))
		{
			std::string s = ATL::CW2A(fileName);
			throw std::runtime_error("ShaderHandler::CreatePixelShader: Shader file not found. Filename: " + s);
		}

		SAFE_RELEASE(errorMessage);
		SAFE_RELEASE(shaderBuffer);

		return pixelShader;
	}
	ID3D11ComputeShader* ShaderHandler::CreateComputeShader(ID3D11Device* device, LPCWSTR fileName)
	{
		HRESULT result;
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* shaderBuffer = nullptr;
		ID3D11ComputeShader* computeShader = nullptr;

		result = D3DCompileFromFile(fileName, NULL, NULL, "main", "cs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				throw std::runtime_error("ShaderHandler::CreateComputeShader: " + std::string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			}
			else
			{
				std::string s = ATL::CW2A(fileName);
				throw std::runtime_error("ShaderHandler::CreateComputeShader: Shader file not found. Filename: " + s);
			}
		}

		result = device->CreateComputeShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &computeShader);
		if (FAILED(result))
		{
			std::string s = ATL::CW2A(fileName);
			throw std::runtime_error("ShaderHandler::CreateComputeShader: Shader file not found. Filename: " + s);
		}

		SAFE_RELEASE(errorMessage);
		SAFE_RELEASE(shaderBuffer);

		return computeShader;
	}

	void ShaderHandler::SetDefaultShaders(ID3D11DeviceContext* deviceContext)
	{
		deviceContext->IASetInputLayout(_defaultVS->_inputLayout);

		deviceContext->VSSetShader(_defaultVS->_vertexShader, nullptr, 0);
		deviceContext->HSSetShader(nullptr, nullptr, 0);
		deviceContext->GSSetShader(nullptr, nullptr, 0);
		deviceContext->DSSetShader(nullptr, nullptr, 0);
		deviceContext->PSSetShader(_defaultPS, nullptr, 0);
		deviceContext->CSSetShader(nullptr, nullptr, 0);

		//Set sampler
		ID3D11SamplerState* samplers[2] = { _samplerWRAP, _samplerCLAMP };
		deviceContext->PSSetSamplers(0, 2, samplers);
	}

	void ShaderHandler::SetGeometryStageShaders(ID3D11DeviceContext * deviceContext)
	{
		// Set vertex layout
		deviceContext->IASetInputLayout(_geoPassVS->_inputLayout);

		// Set shaders
		deviceContext->VSSetShader(_geoPassVS->_vertexShader, nullptr, 0);
		deviceContext->HSSetShader(nullptr, nullptr, 0);
		deviceContext->GSSetShader(nullptr, nullptr, 0);
		deviceContext->DSSetShader(nullptr, nullptr, 0);
		deviceContext->PSSetShader(_geoPassPS, nullptr, 0);
		deviceContext->CSSetShader(nullptr, nullptr, 0);

		//Set sampler
		ID3D11SamplerState* samplers[2] = { _samplerWRAP, _samplerCLAMP };
		deviceContext->PSSetSamplers(0, 2, samplers);
	}

	void ShaderHandler::SetFinalPassShaders(ID3D11DeviceContext * deviceContext)
	{
		//Set vertex layout
		deviceContext->IASetInputLayout(_lightPassVS->_inputLayout);

		//Set shaders
		deviceContext->VSSetShader(_lightPassVS->_vertexShader, nullptr, 0);
		deviceContext->HSSetShader(nullptr, nullptr, 0);
		deviceContext->GSSetShader(nullptr, nullptr, 0);
		deviceContext->DSSetShader(nullptr, nullptr, 0);
		deviceContext->PSSetShader(_lightPassPS, nullptr, 0);
		deviceContext->CSSetShader(nullptr, nullptr, 0);

		//Set sampler
		deviceContext->PSSetSamplers(0, 1, &_samplerWRAP);
	}

	void ShaderHandler::SetAnimationPassShaders(ID3D11DeviceContext* deviceContext)
	{
		// Set vertex layout
		deviceContext->IASetInputLayout(_animPassVS->_inputLayout);

		// Set shaders
		deviceContext->VSSetShader(_animPassVS->_vertexShader, nullptr, 0);
		deviceContext->HSSetShader(nullptr, nullptr, 0);
		deviceContext->GSSetShader(nullptr, nullptr, 0);
		deviceContext->DSSetShader(nullptr, nullptr, 0);
		deviceContext->PSSetShader(_geoPassPS, nullptr, 0);

		//Set sampler
		deviceContext->PSSetSamplers(0, 1, &_samplerWRAP);
	}

	void ShaderHandler::SetGridPassShaders(ID3D11DeviceContext * deviceContext)
	{
		// Set vertex layout
		deviceContext->IASetInputLayout(_gridPassVS->_inputLayout);

		// Set shaders
		deviceContext->VSSetShader(_gridPassVS->_vertexShader, nullptr, 0);
		deviceContext->HSSetShader(nullptr, nullptr, 0);
		deviceContext->GSSetShader(nullptr, nullptr, 0);
		deviceContext->DSSetShader(nullptr, nullptr, 0);
		deviceContext->HSSetShader(nullptr, nullptr, 0);
		deviceContext->GSSetShader(nullptr, nullptr, 0);
		deviceContext->DSSetShader(nullptr, nullptr, 0);
		deviceContext->PSSetShader(_gridPassPS, nullptr, 0);
		deviceContext->CSSetShader(nullptr, nullptr, 0);

		//Set sampler
		deviceContext->PSSetSamplers(0, 1, &_samplerWRAP);
	}

	void ShaderHandler::SetHUDPassShaders(ID3D11DeviceContext * deviceContext)
	{
		//Set vertex layout
		deviceContext->IASetInputLayout(_hudPassVS->_inputLayout);

		//Set shaders
		deviceContext->VSSetShader(_hudPassVS->_vertexShader, nullptr, 0);
		deviceContext->HSSetShader(nullptr, nullptr, 0);
		deviceContext->GSSetShader(nullptr, nullptr, 0);
		deviceContext->DSSetShader(nullptr, nullptr, 0);
		deviceContext->PSSetShader(_hudPassPS, nullptr, 0);
		deviceContext->CSSetShader(nullptr, nullptr, 0);

		//Set sampler
		ID3D11SamplerState* samplers[2] = { _samplerWRAP, _samplerCLAMP };
		deviceContext->PSSetSamplers(0, 2, samplers);
	}

	void ShaderHandler::SetShadowGenerationShaders(ID3D11DeviceContext* deviceContext)
	{
		//Set vertex layout
		deviceContext->IASetInputLayout(_shadowMapVS->_inputLayout);

		//Set shaders
		deviceContext->VSSetShader(_shadowMapVS->_vertexShader, nullptr, 0);
		deviceContext->HSSetShader(nullptr, nullptr, 0);
		deviceContext->GSSetShader(nullptr, nullptr, 0);
		deviceContext->DSSetShader(nullptr, nullptr, 0);
		deviceContext->PSSetShader(nullptr, nullptr, 0);
		deviceContext->CSSetShader(nullptr, nullptr, 0);

		//Set sampler
		ID3D11SamplerState* samplers[2] = { _samplerWRAP, _samplerCLAMP };
		deviceContext->PSSetSamplers(0, 2, samplers);
	}

	void ShaderHandler::SetSpotlightApplicationShaders(ID3D11DeviceContext* deviceContext)
	{
		//Set vertex layout
		deviceContext->IASetInputLayout(_lightApplyLightVolumeVS->_inputLayout);

		//Set shaders
		deviceContext->VSSetShader(_lightApplyLightVolumeVS->_vertexShader, nullptr, 0);
		deviceContext->HSSetShader(nullptr, nullptr, 0);
		deviceContext->GSSetShader(nullptr, nullptr, 0);
		deviceContext->DSSetShader(nullptr, nullptr, 0);
		deviceContext->PSSetShader(_lightApplySpotlightVolumePS, nullptr, 0);
		deviceContext->CSSetShader(nullptr, nullptr, 0);

		//Set sampler
		ID3D11SamplerState* samplers[2] = { _samplerWRAP, _samplerCLAMP };
		deviceContext->PSSetSamplers(0, 2, samplers);
	}

	void ShaderHandler::SetPointlightApplicationShaders(ID3D11DeviceContext* deviceContext)
	{
		//Set vertex layout
		deviceContext->IASetInputLayout(_lightApplyLightVolumeVS->_inputLayout);

		//Set shaders
		deviceContext->VSSetShader(_lightApplyLightVolumeVS->_vertexShader, nullptr, 0);
		deviceContext->HSSetShader(nullptr, nullptr, 0);
		deviceContext->GSSetShader(nullptr, nullptr, 0);
		deviceContext->DSSetShader(nullptr, nullptr, 0);
		deviceContext->PSSetShader(_lightApplyPointlightVolumePS, nullptr, 0);
		deviceContext->CSSetShader(nullptr, nullptr, 0);

		//Set sampler
		ID3D11SamplerState* samplers[2] = { _samplerWRAP, _samplerCLAMP };
		deviceContext->PSSetSamplers(0, 2, samplers);
	}
}
