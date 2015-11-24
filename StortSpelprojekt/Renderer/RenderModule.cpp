#include "RenderModule.h"

namespace Renderer
{
	RenderModule::RenderModule(HWND hwnd, int screenWidth, int screenHeight)
	{
		_d3d = new DirectX(hwnd, screenWidth, screenHeight);
		_shaderHandler = new ShaderHandler(_d3d->GetDevice());
		_shaderHandler->SetDefaultShaders(_d3d->GetDeviceContext());





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

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(Vertex) * 3;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = vertices;
		HRESULT result = _d3d->GetDevice()->CreateBuffer(&bufferDesc, &data, &vertexBuffer);

		if (FAILED(result))
			throw std::runtime_error("Failed to create vertexBuffer");

	}

	void RenderModule::Render()
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		UINT32 vertexSize = sizeof(Vertex);
		UINT32 offset = 0;

		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext->Draw(vertexSize * 3, 0);
	}

	RenderModule::~RenderModule()
	{
		delete _d3d;
		delete _shaderHandler;
	}

	void RenderModule::BeginScene(float red, float green, float blue, float alpha)
	{
		_d3d->BeginScene(red, green, blue, alpha);
	}

	void RenderModule::EndScene()
	{
		_d3d->EndScene();
	}

	void RenderModule::ResizeResources(HWND hwnd, int windowWidth, int windowHeight)
	{
		_d3d->ResizeResources(hwnd, windowWidth, windowHeight);
	}
}