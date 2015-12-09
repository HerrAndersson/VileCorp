#include "RenderModule.h"

using namespace DirectX;

namespace Renderer
{
	RenderModule::RenderModule(HWND hwnd, int screenWidth, int screenHeight)
	{
		_d3d = new DirectXHandler(hwnd, screenWidth, screenHeight);
		_shaderHandler = new ShaderHandler(_d3d->GetDevice());
		_shaderHandler->SetDefaultShaders(_d3d->GetDeviceContext());

		float left = -1.0f;
		float right = 1.0f;
		float top = 1.0f;
		float bottom = -1.0f;

		ScreenQuadVertex quad[] = 
		{
			//First triangle
			{
				left, top, 0.0f,
				0.0f, 0.0f
			},
			{
				right, bottom, 0.0f,
				1.0f, 1.0f
			},
			{
				left, bottom, 0.0f,
				0.0f, 1.0f
			},
			//Second triangle
			{
				left, top, 0.0f,
				0.0f, 0.0f
			},
			{
				right, top, 0.0f,
				1.0f, 0.0f
			},
			{
				right, bottom, 0.0f,
				1.0f, 1.0f
			}
		};

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(ScreenQuadVertex) * 6;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = quad;
		HRESULT result = _d3d->GetDevice()->CreateBuffer(&bufferDesc, &data, &_screenQuad);

		InitializeConstantBuffers();
	}

	RenderModule::~RenderModule()
	{
		delete _d3d;
		delete _shaderHandler;
		SAFE_RELEASE(_screenQuad);
		SAFE_RELEASE(_matrixBufferPerObject);
		SAFE_RELEASE(_matrixBufferPerFrame);
	}

	void RenderModule::InitializeConstantBuffers()
	{
		D3D11_BUFFER_DESC matrixBufferDesc;
		HRESULT result;
		ID3D11Device* device = _d3d->GetDevice();

		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;

		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerObject);
		result = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBufferPerObject);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::InitializeConstantBuffers: Failed to create MatrixBufferPerObject");
		}

		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerFrame);
		result = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBufferPerFrame);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::InitializeConstantBuffers: Failed to create MatrixBufferPerFrame");
		}
	}

	void RenderModule::ResizeResources(HWND hwnd, int windowWidth, int windowHeight)
	{
		_d3d->ResizeResources(hwnd, windowWidth, windowHeight);
	}

	void RenderModule::SetResourcesPerFrame(DirectX::XMMATRIX* view, DirectX::XMMATRIX* projection)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		MatrixBufferPerFrame* dataPtr;
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		//View,Projection
		XMMATRIX viewMatrixC, projectionMatrixC;

		viewMatrixC = XMMatrixTranspose(*view);
		projectionMatrixC = XMMatrixTranspose(*projection);

		result = deviceContext->Map(_matrixBufferPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::SetResourcesPerFrame: Failed to Map _matrixBufferPerFrame");
		}

		dataPtr = (MatrixBufferPerFrame*)mappedResource.pData;

		dataPtr->_viewMatrix = viewMatrixC;
		dataPtr->_projectionMatrix = projectionMatrixC;

		deviceContext->Unmap(_matrixBufferPerFrame, 0);

		deviceContext->VSSetConstantBuffers(0, 1, &_matrixBufferPerFrame);
	}

	void RenderModule::SetResourcesPerObject(XMMATRIX* world, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* specular)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		UINT32 offset = 0;

		deviceContext->PSSetShaderResources(0, 1, &diffuse);
		deviceContext->PSSetShaderResources(1, 1, &specular);

		XMMATRIX worldMatrixC;
		worldMatrixC = *world;

		result = deviceContext->Map(_matrixBufferPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::SetResourcesPerObject: Failed to Map _matrixBufferPerObject");
		}

		MatrixBufferPerObject* dataPtr = (MatrixBufferPerObject*)mappedResource.pData;

		dataPtr->_world = worldMatrixC;

		deviceContext->Unmap(_matrixBufferPerObject, 0);

		deviceContext->VSSetConstantBuffers(1, 1, &_matrixBufferPerObject);
	}

	void RenderModule::SetResourcesPerMesh(ID3D11Buffer* vertexBuffer, int vertexSize)
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		UINT32 offset = 0;
		UINT32 vs = vertexSize;

		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vs, &offset);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void RenderModule::SetShaderStage(ShaderStage stage)
	{
		switch(stage)
		{
		case GEO_PASS:
		{
			_d3d->SetGeometryPassRTVs();
			_shaderHandler->SetGeometryPassShaders(_d3d->GetDeviceContext());
			break;
		}
		case LIGHT_PASS:
		{
			_d3d->SetLightPassRTVs();
			_shaderHandler->SetLightPassShaders(_d3d->GetDeviceContext());
			break;
		}
		};
	}

	void RenderModule::BeginScene(float red, float green, float blue, float alpha)
	{
		_d3d->BeginScene(red, green, blue, alpha);
	}

	void RenderModule::Render(DirectX::XMMATRIX* world, RenderObject* renderObject)
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		ID3D11ShaderResourceView* diffuseData = nullptr;
		ID3D11ShaderResourceView* specularData = nullptr;

		Texture* diffuse = renderObject->_diffuseTexture;
		Texture* specular = renderObject->_specularTexture;

		if (diffuse)
		{
			diffuseData = diffuse->_data;
		}

		if (specular)
		{
			specularData = specular->_data;
		}

		SetResourcesPerObject(world, diffuseData, specularData);

		int vertexSize = sizeof(Vertex);

		if (renderObject->_skeleton != -1)
		{
			vertexSize = sizeof(WeightedVertex);
		}
		
		for (auto mesh : renderObject->_meshes)
		{
			SetResourcesPerMesh(mesh._vertexBuffer, vertexSize);
			deviceContext->Draw(mesh._vertexBufferSize, 0);
		}
	}

	void RenderModule::RenderLightQuad()
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		UINT32 vertexSize = sizeof(ScreenQuadVertex);
		UINT32 offset = 0;

		deviceContext->IASetVertexBuffers(0, 1, &_screenQuad, &vertexSize, &offset);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext->Draw(6, 0);
	}

	void RenderModule::EndScene()
	{
		_d3d->EndScene();
	}

	ID3D11Device* RenderModule::GetDevice() const
	{
		return _d3d->GetDevice();
	}
	ID3D11DeviceContext* RenderModule::GetDeviceContext() const
	{
		return _d3d->GetDeviceContext();
	}
}