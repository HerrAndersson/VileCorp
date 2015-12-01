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
		

		//TEMP!
		Vertex vertices[] =
		{
			{
				0.0f, 0.5f, 0.0f,
				1.0f, 0.0f,
				1.0f, 1.0f, 0.0f
			},
			{
				0.45f, -0.5, 0.0f,
				1.0f, 1.0f,
				1.0f, 1.0f, 0.0f
			},
			{
				-0.45f, -0.5f, 0.0f,
				1.0f, 0.0f,
				1.0f, 1.0f, 0.0f
			}
		};
		D3D11_BUFFER_DESC bufferDescTriangle;
		ZeroMemory(&bufferDescTriangle, sizeof(bufferDescTriangle));
		bufferDescTriangle.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDescTriangle.Usage = D3D11_USAGE_DEFAULT;
		bufferDescTriangle.ByteWidth = sizeof(Vertex) * 3;

		D3D11_SUBRESOURCE_DATA dataTriangle;
		dataTriangle.pSysMem = vertices;
		HRESULT resultTriangle = _d3d->GetDevice()->CreateBuffer(&bufferDescTriangle, &dataTriangle, &_vertexBuffer);

		if (FAILED(resultTriangle))
			throw std::runtime_error("Failed to create vertexBufferTriangle");
	}

	RenderModule::~RenderModule()
	{
		delete _d3d;
		delete _shaderHandler;
	}

	void RenderModule::ResizeResources(HWND hwnd, int windowWidth, int windowHeight)
	{
		_d3d->ResizeResources(hwnd, windowWidth, windowHeight);
	}

	bool RenderModule::SetResourcesPerFrame(DirectX::XMMATRIX* view, DirectX::XMMATRIX* projection)
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
			//TODO: Throw an exception //Mattias
			return false;
		}

		dataPtr = (MatrixBufferPerFrame*)mappedResource.pData;

		dataPtr->viewMatrix = viewMatrixC;
		dataPtr->projectionMatrix = projectionMatrixC;

		deviceContext->Unmap(_matrixBufferPerFrame, 0);

		deviceContext->VSSetConstantBuffers(0, 1, &_matrixBufferPerFrame);
		return true;
	}
	bool RenderModule::SetResourcesPerObject(XMMATRIX* world, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* specular)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		UINT32 offset = 0;

		deviceContext->PSSetShaderResources(0, 1, &diffuse);
		deviceContext->PSSetShaderResources(1, 1, &specular);

		XMMATRIX worldMatrixC;
		worldMatrixC = XMMatrixTranspose(*world);

		result = deviceContext->Map(_matrixBufferPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result)) { return false; }

		MatrixBufferPerObject* dataPtr = (MatrixBufferPerObject*)mappedResource.pData;

		dataPtr->world = worldMatrixC;

		deviceContext->Unmap(_matrixBufferPerObject, 0);

		deviceContext->VSSetConstantBuffers(0, 1, &_matrixBufferPerObject);
		return true;
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

	void RenderModule::Render()
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		UINT32 vertexSize = sizeof(Vertex);
		UINT32 offset = 0;

		deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &vertexSize, &offset);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext->Draw(3, 0);
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
}