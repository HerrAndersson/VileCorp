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
		{ { left, top, 0.0f,0.0f, 0.0f }, { right, bottom, 0.0f,1.0f, 1.0f }, { left, bottom, 0.0f,0.0f, 1.0f },
		  { left, top, 0.0f,0.0f, 0.0f }, { right, top, 0.0f,1.0f, 0.0f }, { right, bottom, 0.0f,1.0f, 1.0f } };

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(ScreenQuadVertex) * 6;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = quad;
		HRESULT result = _d3d->GetDevice()->CreateBuffer(&bufferDesc, &data, &_screenQuad);

		InitializeConstantBuffers();

		_shadowMap = new ShadowMap(_d3d->GetDevice(), 256);
	}

	RenderModule::~RenderModule()
	{
		delete _d3d;
		delete _shaderHandler;
		delete _shadowMap;
		SAFE_RELEASE(_screenQuad);
		SAFE_RELEASE(_matrixBufferPerObject);
		SAFE_RELEASE(_matrixBufferPerFrame);
		SAFE_RELEASE(_matrixBufferLightPassPerFrame);
		SAFE_RELEASE(_matrixBufferLightPassPerLight);
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

		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferLightPassPerFrame);
		result = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBufferLightPassPerFrame);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::InitializeConstantBuffers: Failed to create MatrixBufferLightPassPerFrame");
		}

		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferLightPassPerLight);
		result = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBufferLightPassPerLight);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::InitializeConstantBuffers: Failed to create MatrixBufferLightPassPerLight");
		}
	}

	void RenderModule::ResizeResources(HWND hwnd, int windowWidth, int windowHeight)
	{
		_d3d->ResizeResources(hwnd, windowWidth, windowHeight);
	}

	void RenderModule::SetDataPerFrame(DirectX::XMMATRIX* view, DirectX::XMMATRIX* projection)
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
			throw std::runtime_error("RenderModule::SetDataPerFrame: Failed to Map _matrixBufferPerFrame");
		}

		dataPtr = (MatrixBufferPerFrame*)mappedResource.pData;

		dataPtr->viewMatrix = viewMatrixC;
		dataPtr->projectionMatrix = projectionMatrixC;

		deviceContext->Unmap(_matrixBufferPerFrame, 0);

		deviceContext->VSSetConstantBuffers(0, 1, &_matrixBufferPerFrame);
	}

	void RenderModule::SetDataPerObject(XMMATRIX* world, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* specular)
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
			throw std::runtime_error("RenderModule::SetDataPerObject: Failed to Map _matrixBufferPerObject");
		}

		MatrixBufferPerObject* dataPtr = (MatrixBufferPerObject*)mappedResource.pData;

		dataPtr->world = worldMatrixC;

		deviceContext->Unmap(_matrixBufferPerObject, 0);

		deviceContext->VSSetConstantBuffers(1, 1, &_matrixBufferPerObject);
	}

	void RenderModule::SetDataPerMesh(ID3D11Buffer* vertexBuffer, int vertexSize)
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		UINT32 offset = 0;
		UINT32 vs = vertexSize;

		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vs, &offset);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void RenderModule::SetShadowMapDataPerSpotLight(DirectX::XMMATRIX* lightView, DirectX::XMMATRIX* lightProjection)
	{
		_shadowMap->SetDataPerFrame(_d3d->GetDeviceContext(), lightView, lightProjection);
	}

	void RenderModule::SetShadowMapDataPerObject(DirectX::XMMATRIX* world)
	{
		_shadowMap->SetDataPerObject(_d3d->GetDeviceContext(), world);
	}

	void RenderModule::RenderShadowMap(DirectX::XMMATRIX* world, RenderObject* renderObject)
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();
		SetShadowMapDataPerObject(world);
		//_d3d->SetCullingState(DirectXHandler::CullingState::FRONT);

		int vertexSize = sizeof(Vertex);

		if (renderObject->_skeleton != -1)
		{
			vertexSize = sizeof(WeightedVertex);
		}

		for (auto mesh : renderObject->_meshes)
		{
			SetDataPerMesh(mesh._vertexBuffer, vertexSize);
			deviceContext->Draw(mesh._vertexBufferSize, 0);
		}
	}

	void RenderModule::SetLightDataPerLight(Spotlight* spotlight)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		MatrixBufferLightPassPerLight* dataPtr;
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		//View,Projection
		XMMATRIX view, proj;

		view = XMMatrixTranspose(*(spotlight->GetViewMatrix()));
		proj = XMMatrixTranspose(*(spotlight->GetProjectionMatrix()));

		result = deviceContext->Map(_matrixBufferLightPassPerLight, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::SetLightPassData: Failed to Map _matrixBufferLightPass");
		}

		dataPtr = (MatrixBufferLightPassPerLight*)mappedResource.pData;

		dataPtr->viewMatrix = view;
		dataPtr->projectionMatrix = proj;
		dataPtr->position = spotlight->GetPosition();
		dataPtr->angle = XMConvertToDegrees(spotlight->GetAngle());
		dataPtr->direction = spotlight->GetDirection();
		dataPtr->intensity = spotlight->GetIntensity();
		dataPtr->color = spotlight->GetColor();
		dataPtr->range = spotlight->GetRange();

		deviceContext->Unmap(_matrixBufferLightPassPerLight, 0);

		deviceContext->PSSetConstantBuffers(3, 1, &_matrixBufferLightPassPerLight);
	}

	void RenderModule::SetLightDataPerFrame(XMMATRIX* camView, XMMATRIX* camProjection)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		MatrixBufferLightPassPerFrame* dataPtr;
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		//View,Projection
		XMMATRIX invView, invProj;

		XMVECTOR v;
		invView = XMMatrixTranspose(XMMatrixInverse(&v, *camView));
		invProj = XMMatrixTranspose(XMMatrixInverse(&v, *camProjection));

		result = deviceContext->Map(_matrixBufferLightPassPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::SetLightPassData: Failed to Map _matrixBufferLightPass");
		}

		dataPtr = (MatrixBufferLightPassPerFrame*)mappedResource.pData;

		dataPtr->invertedView = invView;
		dataPtr->invertedProjection = invProj;

		deviceContext->Unmap(_matrixBufferLightPassPerFrame, 0);

		deviceContext->PSSetConstantBuffers(2, 1, &_matrixBufferLightPassPerFrame);
	}

	void RenderModule::SetShaderStage(ShaderStage stage)
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		switch(stage)
		{
		case GEO_PASS:
		{
			_d3d->SetBlendState(Renderer::DirectXHandler::BlendState::DISABLE);
			_d3d->SetCullingState(Renderer::DirectXHandler::CullingState::BACK);

			_d3d->SetGeometryStage();
			_shaderHandler->SetGeometryStageShaders(deviceContext);

			break;
		}
		case SHADOW_GENERATION:
		{
			_d3d->SetBlendState(Renderer::DirectXHandler::BlendState::DISABLE);
			_d3d->SetCullingState(Renderer::DirectXHandler::CullingState::FRONT);

			_shadowMap->SetShadowGenerationStage(deviceContext);
			_shaderHandler->SetShadowGenerationShaders(deviceContext);

			break;
		}
		case LIGHT_APPLICATION:
		{
			_d3d->SetBlendState(Renderer::DirectXHandler::BlendState::ENABLE);
			_d3d->SetCullingState(Renderer::DirectXHandler::CullingState::BACK);

			int nrOfSRVs = _d3d->SetLightStage();
			_shaderHandler->SetLightApplicationShaders(deviceContext);
			ID3D11ShaderResourceView* shadowMapSRV = _shadowMap->GetShadowSRV();
			_d3d->GetDeviceContext()->PSSetShaderResources(nrOfSRVs, 1, &shadowMapSRV);

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

		SetDataPerObject(world, diffuseData, specularData);

		int vertexSize = sizeof(Vertex);

		if (renderObject->_skeleton != -1)
		{
			vertexSize = sizeof(WeightedVertex);
		}
		
		for (auto mesh : renderObject->_meshes)
		{
			SetDataPerMesh(mesh._vertexBuffer, vertexSize);
			deviceContext->Draw(mesh._vertexBufferSize, 0);
		}
	}

	void RenderModule::RenderScreenQuad()
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		UINT32 vertexSize = sizeof(ScreenQuadVertex);
		UINT32 offset = 0;

		deviceContext->IASetVertexBuffers(0, 1, &_screenQuad, &vertexSize, &offset);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext->Draw(6, 0);
	}

	void RenderModule::DEBUG_RenderLightVolume(ID3D11Buffer* volume, DirectX::XMMATRIX* world)
	{
		_d3d->SetCullingState(Renderer::DirectXHandler::CullingState::NONE);

		XMMATRIX i = XMMatrixTranspose(*world);
		SetDataPerObject(&i, nullptr, nullptr);

		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		UINT32 vertexSize = sizeof(XMFLOAT3);
		UINT32 offset = 0;

		deviceContext->IASetVertexBuffers(0, 1, &volume, &vertexSize, &offset);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext->Draw(72*3*2, 0);
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