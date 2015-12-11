#include "ShaderHandler.h"
#include <d3dcompiler.h>
#include <stdexcept>
#include <string>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

namespace Renderer
{
	ShaderHandler::ShaderHandler(ID3D11Device* device)
	{

		//Create samplers
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

		HRESULT hResult = device->CreateSamplerState(&sampDesc, &_samplerWRAP);
		if (FAILED(hResult))
		{
			throw std::runtime_error("ShaderHandler::ShaderHandler: Error creating WRAP sampler");
		}

		D3D11_INPUT_ELEMENT_DESC inputDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		int numElements = sizeof(inputDesc) / sizeof(inputDesc[0]);

		_defaultVS = CreateVertexShader(device, L"../Renderer/Shaders/DefaultVS.hlsl", inputDesc, numElements);
		_defaultPS = CreatePixelShader(device, L"../Renderer/Shaders/DefaultPS.hlsl");

		//GEOMETRY pass init
		_geoPassVS = CreateVertexShader(device, L"../Renderer/Shaders/GeoVS.hlsl", inputDesc, numElements);
		_geoPassPS = CreatePixelShader(device, L"../Renderer/Shaders/GeoPS.hlsl");

		//LIGHT pass init
		D3D11_INPUT_ELEMENT_DESC lightInputDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		numElements = sizeof(lightInputDesc) / sizeof(lightInputDesc[0]);

		_lightPassVS = CreateVertexShader(device, L"../Renderer/Shaders/LightVS.hlsl", lightInputDesc, numElements);
		_lightPassPS = CreatePixelShader(device, L"../Renderer/Shaders/LightPS.hlsl");

		//Grid pass init
		D3D11_INPUT_ELEMENT_DESC gridInputDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		numElements = sizeof(gridInputDesc) / sizeof(gridInputDesc[0]);

		_gridPassVS = CreateVertexShader(device, L"../Renderer/Shaders/GridVS.hlsl", gridInputDesc, numElements);
		_gridPassPS = CreatePixelShader(device, L"../Renderer/Shaders/GridPS.hlsl");
		
		//HUD pass init
		numElements = sizeof(lightInputDesc) / sizeof(lightInputDesc[0]);

		_hudPassVS = CreateVertexShader(device, L"../Renderer/Shaders/HudVS.hlsl", lightInputDesc, numElements);
		_hudPassPS = CreatePixelShader(device, L"../Renderer/Shaders/HudPS.hlsl");
	}

	ShaderHandler::~ShaderHandler()
	{
		delete _defaultVS;
		SAFE_RELEASE(_defaultPS);
		delete _geoPassVS;
		SAFE_RELEASE(_geoPassPS);
		delete _lightPassVS;
		SAFE_RELEASE(_lightPassPS);
		delete _gridPassVS;
		SAFE_RELEASE(_gridPassPS);
		delete _hudPassVS;
		SAFE_RELEASE(_hudPassPS);
	}

	ShaderHandler::VertexShaderData* ShaderHandler::CreateVertexShader(ID3D11Device* device, LPCWSTR fileName, D3D11_INPUT_ELEMENT_DESC* inputDesc, int inputDescSize)
	{
		HRESULT result;
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* shaderBuffer = nullptr;
		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11InputLayout* inputLayout = nullptr;

		result = D3DCompileFromFile(fileName, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				throw std::runtime_error("ShaderHandler::CreateVertexShader: " + std::string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			}
			else
			{
				throw std::runtime_error("ShaderHandler::CreateVertexShader: Shader file not found");
			}
		}

		result = device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &vertexShader);
		if (FAILED(result))
		{
			throw std::runtime_error("ShaderHandler::CreateVertexShader: Shader file not found.");
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
				throw std::runtime_error("ShaderHandler::CreateHullShader: Shader file not found");
			}
		}
		
		result = device->CreateHullShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &hullShader);
		if (FAILED(result))
		{
			throw std::runtime_error("ShaderHandler::CreateHullShader: Shader file not found.");
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
				throw std::runtime_error("ShaderHandler::CreateGeometryShader: Shader file not found");
			}
		}
		
		result = device->CreateGeometryShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &geometryShader);
		if (FAILED(result))
		{
			throw std::runtime_error("ShaderHandler::CreateGeometryShader: Shader file not found.");
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
				throw std::runtime_error("ShaderHandler::CreateDomainShader: Shader file not found");
			}
		}

		result = device->CreateDomainShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &domainShader);
		if (FAILED(result))
		{
			throw std::runtime_error("ShaderHandler::CreateDomainShader: Shader file not found.");
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
		if (FAILED(result))
		{
			if (errorMessage)
			{
				throw std::runtime_error("ShaderHandler::CreatePixelShader: " + std::string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			}
			else
			{
				throw std::runtime_error("ShaderHandler::CreatePixelShader: Shader file not found");
			}
		}

		result = device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &pixelShader);
		if (FAILED(result))
		{
			throw std::runtime_error("ShaderHandler::CreatePixelShader: Shader file not found.");
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
				throw std::runtime_error("ShaderHandler::CreateComputeShader: Shader file not found");
			}
		}

		result = device->CreateComputeShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &computeShader);
		if (FAILED(result))
		{
			throw std::runtime_error("ShaderHandler::CreateComputeShader: Shader file not found.");
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
		deviceContext->PSSetSamplers(0, 1, &_samplerWRAP);
	}

	void ShaderHandler::SetGeometryPassShaders(ID3D11DeviceContext * deviceContext)
	{
		// Set vertex layout
		deviceContext->IASetInputLayout(_geoPassVS->_inputLayout);

		// Set shaders
		deviceContext->VSSetShader(_geoPassVS->_vertexShader, nullptr, 0);
		deviceContext->HSSetShader(nullptr, nullptr, 0);
		deviceContext->GSSetShader(nullptr , nullptr, 0);
		deviceContext->DSSetShader(nullptr, nullptr, 0);
		deviceContext->PSSetShader(_geoPassPS, nullptr, 0);
		deviceContext->CSSetShader(nullptr, nullptr, 0);

		//Set sampler
		deviceContext->PSSetSamplers(0, 1, &_samplerWRAP);
	}

	void ShaderHandler::SetLightPassShaders(ID3D11DeviceContext * deviceContext)
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
		deviceContext->PSSetSamplers(0, 1, &_samplerWRAP);
	}
}