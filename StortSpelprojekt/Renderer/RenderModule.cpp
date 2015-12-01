#include "RenderModule.h"

namespace Renderer
{
	RenderModule::RenderModule(HWND hwnd, int screenWidth, int screenHeight)
	{
		_d3d = new DirectX(hwnd, screenWidth, screenHeight);
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

	void RenderModule::SetResources()
	{
		/*
		This is where we update our per frame buffer and set similar resources
		*/
	}

	void RenderModule::SetShaderStage(int stage)
	{
		switch(stage)
		{
		case 0:
		{
			_d3d->SetGeometryPassRTVs();
			_shaderHandler->SetGeometryPassShaders(_d3d->GetDeviceContext());
			break;
		}
		case 1:
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

	ID3D11Device* RenderModule::GetDevice() const
	{
		return _d3d->GetDevice();
	}
}