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
		D3D11_INPUT_ELEMENT_DESC inputDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		int numElements = sizeof(inputDesc) / sizeof(inputDesc[0]);

		_defaultVS = static_cast<ID3D11VertexShader*>(CreateShader(device, L"../Renderer/Shaders/DefaultVS.hlsl", VERTEX_SHADER, inputDesc, numElements));
		_defaultPS = static_cast<ID3D11PixelShader*>(CreateShader(device, L"../Renderer/Shaders/DefaultPS.hlsl", PIXEL_SHADER));

		/*
		GEOMETRY PASS INITS
		*/
		/*TODO Sebastian 25/11
		_geoPassVS = static_cast<ID3D11VertexShader*>(CreateShader(device, L"../Renderer/Shaders/[insert name here].hlsl, VERTEX_SHADER, inputDesc, numElements));
		_geoPassGS = 
		_geoPassPS =
		*/
	}

	ShaderHandler::~ShaderHandler()
	{
		int i = 0;
		_defaultVS->Release();
		_defaultPS->Release();
	}

	ID3D11DeviceChild* ShaderHandler::CreateShader(ID3D11Device* device, LPCWSTR fileName, ShaderType type, D3D11_INPUT_ELEMENT_DESC* inputDesc, int inputDescSize)
	{
		HRESULT result;
		ID3D10Blob* errorMessage = nullptr;
		ID3D10Blob* shaderBuffer = nullptr;
		LPCSTR shaderVersion = "";
		ID3D11DeviceChild* shader = nullptr;

		switch (type)
		{
			case Renderer::ShaderHandler::VERTEX_SHADER:
				shaderVersion = "vs_5_0";
				break;
			case Renderer::ShaderHandler::HULL_SHADER:
				shaderVersion = "hs_5_0";
				break;
			case Renderer::ShaderHandler::GEOMETRY_SHADER:
				shaderVersion = "gs_5_0";
				break;
			case Renderer::ShaderHandler::DOMAIN_SHADER:
				shaderVersion = "ds_5_0";
				break;
			case Renderer::ShaderHandler::PIXEL_SHADER:
				shaderVersion = "ps_5_0";
				break;
			case Renderer::ShaderHandler::COMPUTE_SHADER:
				shaderVersion = "cs_5_0";
				break;
			default:
				break;
		}

		result = D3DCompileFromFile(fileName, NULL, NULL, "main", shaderVersion, D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				throw std::runtime_error("ShaderHandler::CreateShader: " + std::string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			}
			else
			{
				throw std::runtime_error("ShaderHandler::CreateShader: Shader file not found");
			}
		}

		switch (type)
		{
			case Renderer::ShaderHandler::VERTEX_SHADER:
			{
				ID3D11VertexShader* vertexShader = nullptr;
				result = device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &vertexShader);
				shader = vertexShader;

				if (FAILED(result))
				{
					throw std::runtime_error("ShaderHandler::CreateShader-VERTEX_SHADER: Shader file not found.");
				}

				result = device->CreateInputLayout(inputDesc, inputDescSize, shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), &_layoutPosUvNorm);
				if (FAILED(result))
				{
					throw std::runtime_error("ShaderHandler::CreateShader-VERTEX_SHADER: Error creating input layout.");
				}

				break;
			}
			case Renderer::ShaderHandler::HULL_SHADER:
			{
				ID3D11HullShader* hullShader = nullptr;
				result = device->CreateHullShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &hullShader);
				shader = hullShader;

				if (FAILED(result))
				{
					throw std::runtime_error("ShaderHandler::CreateShader-HULL_SHADER: Shader file not found.");
				}

				break;
			}
			case Renderer::ShaderHandler::GEOMETRY_SHADER:
			{
				ID3D11GeometryShader* geometryShader = nullptr;
				result = device->CreateGeometryShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &geometryShader);
				shader = geometryShader;

				if (FAILED(result))
				{
					throw std::runtime_error("ShaderHandler::CreateShader-GEOMETRY_SHADER: Shader file not found.");
				}

				break;
			}
			case Renderer::ShaderHandler::DOMAIN_SHADER:
			{
				ID3D11DomainShader* domainShader = nullptr;
				result = device->CreateDomainShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &domainShader);
				shader = domainShader;

				if (FAILED(result))
				{
					throw std::runtime_error("ShaderHandler::CreateShader-DOMAIN_SHADER: Shader file not found.");
				}
				break;
			}
			case Renderer::ShaderHandler::PIXEL_SHADER:
			{
				ID3D11PixelShader* pixelShader = nullptr;
				result = device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &pixelShader);
				shader = pixelShader;

				if (FAILED(result))
				{
					throw std::runtime_error("ShaderHandler::CreateShader-PIXEL_SHADER: Shader file not found.");
				}
				break;
			}
			case Renderer::ShaderHandler::COMPUTE_SHADER:
			{
				ID3D11ComputeShader* computeShader = nullptr;
				result = device->CreateComputeShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &computeShader);
				shader = computeShader;

				if (FAILED(result))
				{
					throw std::runtime_error("ShaderHandler::CreateShader-COMPUTE_SHADER: Shader file not found.");
				}
				break;
			}
			default:
			{
				break;
			}
		}


		if (shaderBuffer)
		{
			shaderBuffer->Release();
		}

		return shader;
	}

	void ShaderHandler::SetDefaultShaders(ID3D11DeviceContext* deviceContext)
	{
		deviceContext->IASetInputLayout(_layoutPosUvNorm);

		deviceContext->VSSetShader(_defaultVS, NULL, 0);
		deviceContext->PSSetShader(_defaultPS, NULL, 0);
	}

	void ShaderHandler::SetGeometryPassShaders(ID3D11DeviceContext * deviceContext)
	{
		// Set vertex layout
		deviceContext->IASetInputLayout(_geoPassLayout);

		// Set shaders
		deviceContext->VSSetShader(_geoPassVS, nullptr, 0);
		deviceContext->GSSetShader(_geoPassGS, nullptr, 0);
		deviceContext->PSSetShader(_geoPassPS, nullptr, 0);
	}
}