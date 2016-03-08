#include "RenderModule.h"

using namespace DirectX;

namespace Renderer
{
	RenderModule::RenderModule(HWND hwnd, System::Settings* settings)
	{
		_ambientLight = DirectX::XMFLOAT3(0.14f, 0.15f, 0.2f);
		_d3d = new DirectXHandler(hwnd, settings);
		_settings = settings;
		_shaderHandler = new ShaderHandler(_d3d->GetDevice());
		_antialiasingEnabled = 0;

		InitializeScreenQuadBuffer();
		InitializeConstantBuffers();

		_shadowMap = new ShadowMap(_d3d->GetDevice(), settings->_shadowMapSize);
	}

	RenderModule::~RenderModule()
	{
		delete _d3d;
		delete _shaderHandler;
		delete _shadowMap;

		_screenQuad->Release();
		_matrixBufferPerObject->Release();
		_matrixBufferPerSkinnedObject->Release();
		_matrixBufferPerFrame->Release();
		_matrixBufferHUD->Release();
		_matrixBufferLightPassPerFrame->Release();
		_matrixBufferLightPassPerSpotlight->Release();
		_matrixBufferLightPassPerPointlight->Release();
		_matrixBufferParticles->Release();
	}

	void RenderModule::InitializeScreenQuadBuffer()
	{
		float left = -1.0f;
		float right = 1.0f;
		float top = 1.0f;
		float bottom = -1.0f;

		ScreenQuadVertex quad[] =
		{ { left, top, 0.0f,0.0f, 0.0f },{ right, bottom, 0.0f,1.0f, 1.0f },{ left, bottom, 0.0f,0.0f, 1.0f },
		{ left, top, 0.0f,0.0f, 0.0f },{ right, top, 0.0f,1.0f, 0.0f },{ right, bottom, 0.0f,1.0f, 1.0f } };

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(ScreenQuadVertex) * 6;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = quad;
		HRESULT result = _d3d->GetDevice()->CreateBuffer(&bufferDesc, &data, &_screenQuad);
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

		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerSkinnedObject);
		result = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBufferPerSkinnedObject);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::InitializeConstantBuffers: Failed to create MatrixBufferPerSkinnedObject");
		}

		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerFrame);
		result = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBufferPerFrame);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::InitializeConstantBuffers: Failed to create MatrixBufferPerFrame");
		}

		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferHud);
		result = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBufferHUD);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::InitializeConstantBuffers: Failed to create matrixBufferHUD");
		}
		
		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferLightPassPerFrame);
		result = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBufferLightPassPerFrame);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::InitializeConstantBuffers: Failed to create _matrixBufferLightPassPerFrame");
		}

		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferLightPassPerSpotlight);
		result = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBufferLightPassPerSpotlight);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::InitializeConstantBuffers: Failed to create _matrixBufferLightPassPerSpotlight");
		}

		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferLightPassPerPointlight);
		result = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBufferLightPassPerPointlight);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::InitializeConstantBuffers: Failed to create _matrixBufferLightPassPerPointlight");
		}

		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerParticleEmitter);
		result = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBufferParticles);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::InitializeConstantBuffers: Failed to create _matrixBufferParticles");
		}
	}

	void RenderModule::ResizeResources(System::Settings* settings)
	{
		_d3d->ResizeResources(settings);
		_settings = settings;

		_shadowMap->Resize(_d3d->GetDevice(), settings->_shadowMapSize);
	}

	void RenderModule::SetDataPerFrame(DirectX::XMMATRIX* view, DirectX::XMMATRIX* projection)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		MatrixBufferPerFrame* dataPtr;
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		XMMATRIX viewMatrixC, projectionMatrixC;

		viewMatrixC = XMMatrixTranspose(*view);
		projectionMatrixC = XMMatrixTranspose(*projection);

		result = deviceContext->Map(_matrixBufferPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::SetDataPerFrame: Failed to Map _matrixBufferPerFrame");
		}

		dataPtr = static_cast<MatrixBufferPerFrame*>(mappedResource.pData);

		dataPtr->_viewMatrix = viewMatrixC;
		dataPtr->_projectionMatrix = projectionMatrixC;
		dataPtr->_ambientLight = _ambientLight;

		deviceContext->Unmap(_matrixBufferPerFrame, 0);

		deviceContext->VSSetConstantBuffers(0, 1, &_matrixBufferPerFrame);
	}

	void RenderModule::SetDataPerSkinnedObject(XMMATRIX* world, DirectX::XMMATRIX* extra, int bonecount, const DirectX::XMFLOAT3& colorOffset)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		SetDataPerObject(world, colorOffset);

		XMMATRIX worldMatrixC = XMMatrixTranspose(*world);

		result = deviceContext->Map(_matrixBufferPerSkinnedObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::SetResourcesPerObject: Failed to Map _matrixBufferPerSkinnedObject");
		}

		MatrixBufferPerSkinnedObject* dataPtr = static_cast<MatrixBufferPerSkinnedObject*>(mappedResource.pData);

		memcpy(&dataPtr->_bones, extra, sizeof(DirectX::XMMATRIX) * bonecount);
		
		deviceContext->Unmap(_matrixBufferPerSkinnedObject, 0);

		deviceContext->VSSetConstantBuffers(5, 1, &_matrixBufferPerSkinnedObject);
	}

	void RenderModule::SetDataPerObjectType(RenderObject* renderObject)
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

		UINT32 vertexSize = sizeof(Vertex);

		if (renderObject->_mesh->_isSkinned)
		{
			vertexSize = sizeof(WeightedVertex);
		}

		SetDataPerMesh(renderObject->_mesh->_vertexBuffer, vertexSize);

		deviceContext->PSSetShaderResources(0, 1, &diffuseData);
		deviceContext->PSSetShaderResources(1, 1, &specularData);
	}

	void RenderModule::SetDataPerObject(XMMATRIX* world, const DirectX::XMFLOAT3& colorOffset)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		XMMATRIX worldMatrixC = XMMatrixTranspose(*world);

		result = deviceContext->Map(_matrixBufferPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::SetDataPerObject: Failed to Map _matrixBufferPerObject");
		}

		MatrixBufferPerObject* dataPtr = static_cast<MatrixBufferPerObject*>(mappedResource.pData);
		dataPtr->_world = worldMatrixC;
		dataPtr->_colorOffset = colorOffset;
		deviceContext->Unmap(_matrixBufferPerObject, 0);

		deviceContext->VSSetConstantBuffers(1, 1, &_matrixBufferPerObject);
	}

	void RenderModule::SetDataPerParticleEmitter(const XMFLOAT3& position, XMMATRIX* camView, XMMATRIX* camProjection,
												 const XMFLOAT3& camPos, float scale, ID3D11ShaderResourceView** textures, int textureCount, int isIcon)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		result = deviceContext->Map(_matrixBufferParticles, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::SetDataPerObject: Failed to Map _matrixBufferPerObject");
		}

		XMMATRIX world = XMMatrixTranslation(position.x, position.y, position.z);

		MatrixBufferPerParticleEmitter* dataPtr = static_cast<MatrixBufferPerParticleEmitter*>(mappedResource.pData);
		dataPtr->_world = XMMatrixTranspose(world);
		dataPtr->_camView = XMMatrixTranspose(*camView);
		dataPtr->_camProjection = XMMatrixTranspose(*camProjection);
		dataPtr->_camPosition = camPos;
		dataPtr->_scale = scale;
		dataPtr->_isIcon = isIcon;
		deviceContext->Unmap(_matrixBufferParticles, 0);

		deviceContext->GSSetConstantBuffers(6, 1, &_matrixBufferParticles);

		if (textures)
		{
			deviceContext->PSSetShaderResources(0, textureCount, textures);
		}
	}

	void RenderModule::SetDataPerMesh(ID3D11Buffer* vertexBuffer, int vertexSize)
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		UINT32 offset = 0;
		UINT32 vs = vertexSize;

		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vs, &offset);
	}

	void RenderModule::SetDataPerLineList(ID3D11Buffer* lineList, int vertexSize)
	{
		//This might seem like a useless function, but SetDataPerMesh should be private to prevent people from using it the wrong way.
		//This is a special case function that is public, and should only be used for the grid, or other lines.

		SetDataPerMesh(lineList, vertexSize);
	}

	void RenderModule::SetShadowMapDataPerSpotlight(DirectX::XMMATRIX* lightView, DirectX::XMMATRIX* lightProjection)
	{
		_shadowMap->SetDataPerFrame(_d3d->GetDeviceContext(), lightView, lightProjection);
	}

	void RenderModule::SetShadowMapDataPerObjectType(RenderObject* renderObject)
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		UINT32 offset = 0;
		UINT32 vertexSize = sizeof(Vertex);

		if (renderObject->_mesh->_isSkinned)
		{
			vertexSize = sizeof(WeightedVertex);
		}

		SetDataPerMesh(renderObject->_mesh->_vertexBuffer, vertexSize);
	}

	void RenderModule::RenderShadowMap(DirectX::XMMATRIX* world, int vertexBufferSize, DirectX::XMMATRIX* animTransformData, int bonecount)
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		_shadowMap->SetDataPerObject(_d3d->GetDeviceContext(), world);

		if (animTransformData)
		{
			HRESULT result;
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			result = deviceContext->Map(_matrixBufferPerSkinnedObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (FAILED(result))
			{
				throw std::runtime_error("RenderModule::SetResourcesPerObject: Failed to Map _matrixBufferPerSkinnedObject");
			}

			MatrixBufferPerSkinnedObject* dataPtr = static_cast<MatrixBufferPerSkinnedObject*>(mappedResource.pData);
			memcpy(&dataPtr->_bones, animTransformData, sizeof(DirectX::XMFLOAT4X4) * bonecount);
			deviceContext->Unmap(_matrixBufferPerSkinnedObject, 0);
			deviceContext->VSSetConstantBuffers(5, 1, &_matrixBufferPerSkinnedObject);
		}

		deviceContext->Draw(vertexBufferSize, 0);
	}

	void RenderModule::SetLightDataPerSpotlight(Spotlight* spotlight)
	{
		if (!spotlight)
		{
			throw std::runtime_error("RenderModule::SetLightDataPerSpotlight: Spotlight is nullptr");
		}

		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		MatrixBufferLightPassPerSpotlight* dataPtr;
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		XMMATRIX view = XMMatrixTranspose(*(spotlight->GetViewMatrix()));
		XMMATRIX proj = XMMatrixTranspose(*(spotlight->GetProjectionMatrix()));

		result = deviceContext->Map(_matrixBufferLightPassPerSpotlight, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::SetLightDataPerSpotlight: Failed to Map _matrixBufferLightPassPerSpotlight");
		}

		dataPtr = static_cast<MatrixBufferLightPassPerSpotlight*>(mappedResource.pData);

		dataPtr->_viewMatrix = view;
		dataPtr->_projectionMatrix = proj;
		dataPtr->_position = spotlight->GetPosition();
		dataPtr->_angle = spotlight->GetAngle();
		dataPtr->_direction = spotlight->GetDirection();
		dataPtr->_intensity = spotlight->GetIntensity();
		dataPtr->_color = spotlight->GetColor();
		dataPtr->_range = spotlight->GetRange();
		dataPtr->_shadowMapDimensions = _shadowMap->GetSize();

		deviceContext->Unmap(_matrixBufferLightPassPerSpotlight, 0);

		deviceContext->PSSetConstantBuffers(3, 1, &_matrixBufferLightPassPerSpotlight);
	}

	void RenderModule::SetLightDataPerPointlight(Pointlight* pointlight)
	{
		if (!pointlight)
		{
			throw std::runtime_error("RenderModule::SetLightDataPerSpotlight: Pointlight is nullptr");
		}

		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		MatrixBufferLightPassPerPointlight* dataPtr;
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		result = deviceContext->Map(_matrixBufferLightPassPerSpotlight, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::SetLightDataPerPointlight: Failed to Map _matrixBufferLightPassPerSpotlight");
		}

		dataPtr = static_cast<MatrixBufferLightPassPerPointlight*>(mappedResource.pData);

		dataPtr->_position = pointlight->GetPosition();
		dataPtr->_intensity = pointlight->GetIntensity();
		dataPtr->_color = pointlight->GetColor();
		dataPtr->_range = pointlight->GetRange();

		deviceContext->Unmap(_matrixBufferLightPassPerSpotlight, 0);

		deviceContext->PSSetConstantBuffers(3, 1, &_matrixBufferLightPassPerSpotlight);
	}

	void RenderModule::SetLightDataPerFrame(XMMATRIX* camView, XMMATRIX* camProjection)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		MatrixBufferLightPassPerFrame* dataPtr;
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		XMVECTOR v;
		XMMATRIX invView = XMMatrixTranspose(XMMatrixInverse(&v, *camView));
		XMMATRIX invProj = XMMatrixTranspose(XMMatrixInverse(&v, *camProjection));

		result = deviceContext->Map(_matrixBufferLightPassPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::SetLightDataPerFrame: Failed to Map _matrixBufferLightPassPerFrame");
		}

		dataPtr = static_cast<MatrixBufferLightPassPerFrame*>(mappedResource.pData);
		dataPtr->_invertedView = invView;
		dataPtr->_invertedProjection = invProj;
		dataPtr->_screenHeight = _settings->_screenHeight;
		dataPtr->_screenWidth = _settings->_screenWidth;
		deviceContext->Unmap(_matrixBufferLightPassPerFrame, 0);

		deviceContext->PSSetConstantBuffers(2, 1, &_matrixBufferLightPassPerFrame);
	}

	void RenderModule::SetShaderStage(ShaderStage stage)
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		switch(stage)
		{
		case ANIM_STAGE:
		{
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			_shaderHandler->SetAnimationPassShaders(_d3d->GetDeviceContext());

			break;
		}
		case ANIM_SHADOW_GENERATION:
		{
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			_d3d->SetBlendState(Renderer::DirectXHandler::BlendState::DISABLE);
			_d3d->SetCullingState(Renderer::DirectXHandler::CullingState::FRONT);

			_shaderHandler->SetAnimaShadowGenerationShaders(deviceContext);

			break;
		}
		case GEO_PASS:
		{
			_d3d->SetBlendState(Renderer::DirectXHandler::BlendState::DISABLE);
			_d3d->SetCullingState(Renderer::DirectXHandler::CullingState::BACK);
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			_d3d->SetGeometryStage();
			_shaderHandler->SetGeometryStageShaders(deviceContext);

			break;
		}
		case AA_STAGE:
		{
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			_d3d->SetCullingState(Renderer::DirectXHandler::CullingState::BACK);
			_d3d->SetBlendState(Renderer::DirectXHandler::BlendState::DISABLE);
			_d3d->SetAntiAliasingState();

			_shaderHandler->SetFXAAPassShaders(deviceContext, _antialiasingEnabled);
			break;
		}
		case SHADOW_GENERATION:
		{
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			_d3d->SetBlendState(Renderer::DirectXHandler::BlendState::DISABLE);
			_d3d->SetCullingState(Renderer::DirectXHandler::CullingState::FRONT);

			_shaderHandler->SetShadowGenerationShaders(deviceContext);
			_shadowMap->SetShadowGenerationStage(deviceContext);

			break;
		}
		case LIGHT_APPLICATION_SPOTLIGHT:
		{
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			_d3d->SetCullingState(Renderer::DirectXHandler::CullingState::FRONT);
			_d3d->SetBlendState(Renderer::DirectXHandler::BlendState::ENABLE);

			int nrOfSRVs = _d3d->SetLightStage();

			_shaderHandler->SetSpotlightApplicationShaders(deviceContext);
			ID3D11ShaderResourceView* shadowMapSRV = _shadowMap->GetShadowSRV();
			_d3d->GetDeviceContext()->PSSetShaderResources(nrOfSRVs, 1, &shadowMapSRV);

			break;
		}
		case LIGHT_APPLICATION_POINTLIGHT:
		{
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			_d3d->SetBlendState(Renderer::DirectXHandler::BlendState::ENABLE);
			_d3d->SetCullingState(Renderer::DirectXHandler::CullingState::FRONT);

			int nrOfSRVs = _d3d->SetLightStage();

			_shaderHandler->SetPointlightApplicationShaders(deviceContext);

			break;
		}
		case RENDER_LINESTRIP:
		{
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
			_d3d->SetLinestripStage();
			_shaderHandler->SetLinestripShaders(_d3d->GetDeviceContext());
			break;
		}
		case HUD_STAGE:
		{
			_d3d->SetBlendState(Renderer::DirectXHandler::BlendState::ENABLE);
			_d3d->SetCullingState(Renderer::DirectXHandler::CullingState::BACK);
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			_d3d->SetHUDStage();
			_shaderHandler->SetHUDPassShaders(_d3d->GetDeviceContext());

			break;
		}
		case BILLBOARDING_STAGE:
		{
			//Since this is part of the geometry pass, there is no need to set render targets etc.
			_d3d->SetBlendState(Renderer::DirectXHandler::BlendState::ENABLE);
			_d3d->SetCullingState(Renderer::DirectXHandler::CullingState::NONE);
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

			_shaderHandler->SetBillboardingStageShaders(_d3d->GetDeviceContext());

			break;
		}
		default:
		{
			throw std::runtime_error("RenderModule::SetShaderStage: Invalid shader stage");
			break;
		}
		};
	}

	DirectX::XMFLOAT3 RenderModule::GetAmbientLight() const
	{
		return _ambientLight;
	}

	void RenderModule::SetAmbientLight(const DirectX::XMFLOAT3 &ambientLight)
	{
		_ambientLight = ambientLight;
	}

	void RenderModule::SetAntialiasingEnabled(bool enabled)
	{
		_antialiasingEnabled = enabled;
	}

	void RenderModule::BeginScene(float red, float green, float blue, float alpha, bool clearBackBuffer)
	{
		_d3d->SetBlendState(Renderer::DirectXHandler::BlendState::DISABLE);
		_d3d->SetCullingState(Renderer::DirectXHandler::CullingState::BACK);
		_d3d->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_d3d->BeginScene(red, green, blue, alpha, clearBackBuffer);
	}

	void RenderModule::Render(DirectX::XMMATRIX* world, int vertexBufferSize, const DirectX::XMFLOAT3& colorOffset)
	{
		SetDataPerObject(world, colorOffset);
		_d3d->GetDeviceContext()->Draw(vertexBufferSize, 0);
	}

	void RenderModule::RenderAnimation(DirectX::XMMATRIX* world, int vertexBufferSize, DirectX::XMMATRIX* extra, int bonecount, const DirectX::XMFLOAT3& colorOffset)
	{
		SetDataPerSkinnedObject(world, extra, bonecount, colorOffset);
		_d3d->GetDeviceContext()->Draw(vertexBufferSize, 0);
	}

	void RenderModule::Render(GUI::Node* root, FontWrapper* fontWrapper, int brightness)
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();
		UINT32 vertexSize = sizeof(ScreenQuadVertex);

		SetDataPerMesh(_screenQuad, vertexSize);
		Render(root, root->GetModelMatrix(), fontWrapper, brightness);
	}

	void RenderModule::Render(GUI::Node* current, XMMATRIX* transform, FontWrapper* fontWrapper, int brightness)
	{
		if (!current->GetHidden())
		{
			Texture* tex = current->GetTexture();
			if (tex)
			{
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				HRESULT result;
				ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

				result = deviceContext->Map(_matrixBufferHUD, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
				if (FAILED(result))
				{
					throw std::runtime_error("RenderModule::SetResourcesPerObject: Failed to Map _matrixBufferHUD");
				}

				XMFLOAT2 s = current->GetScale();
				DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(s.x, s.y, 1);

				XMMATRIX t = DirectX::XMMatrixMultiply(scale, *transform);

				MatrixBufferHud* dataPtr = static_cast<MatrixBufferHud*>(mappedResource.pData);
				dataPtr->_model = XMMatrixTranspose(t);
				XMFLOAT4 offset;
				offset.x = current->GetColorOffset().x + 0.04f * brightness - 0.20f;
				offset.y = current->GetColorOffset().y + 0.04f * brightness - 0.20f;
				offset.z = current->GetColorOffset().z + 0.04f * brightness - 0.20f;
				offset.w = current->GetColorOffset().w;
				dataPtr->_colorOffset = offset;

				deviceContext->Unmap(_matrixBufferHUD, 0);

				deviceContext->VSSetConstantBuffers(1, 1, &_matrixBufferHUD);
				deviceContext->PSSetShaderResources(0, 1, &tex->_data);

				deviceContext->Draw(6, 0);
			}
			//Render text
			int len = current->GetText().length();
			if (len > 0)
			{
				XMFLOAT4X4 temp;
				float x, y;
				XMFLOAT2 pos;

				XMStoreFloat4x4(&temp, *transform);
				pos.x = temp._41;
				pos.y = temp._42;
				XMFLOAT2 scale = current->GetScale();
				x = pos.x - scale.x;
				y = pos.y*-1.0f - scale.y;

				//x and y is in -1,1 coordinate system, convert to pixel coordinate system
				x = (x + 1.0f) * 0.5f * _settings->_screenWidth;
				y = (y + 1.0f) * 0.5f * _settings->_screenHeight;
				fontWrapper->GetFontWrapper()->DrawTextLayout(_d3d->GetDeviceContext(), current->GetFont()->_textLayout, x, y, current->GetColor(), FW1_RESTORESTATE);
			}
			for (GUI::Node* i : *current->GetChildren())
			{
				XMMATRIX a = *(i->GetModelMatrix());
				XMMATRIX t = XMMatrixMultiply(*transform, a);
				Render(i, &t, fontWrapper, brightness);
			}
		}
	}

	void RenderModule::RenderLineStrip(XMMATRIX* world, int nrOfPoints, const XMFLOAT3& colorOffset)
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		SetDataPerObject(world, colorOffset);
		deviceContext->Draw(nrOfPoints, 0);
	}

	void RenderModule::RenderScreenQuad()
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();
		UINT32 vertexSize = sizeof(ScreenQuadVertex);
		UINT32 offset = 0;

		deviceContext->IASetVertexBuffers(0, 1, &_screenQuad, &vertexSize, &offset);
		deviceContext->Draw(6, 0);
	}

	void RenderModule::RenderVertexBuffer(ID3D11Buffer* vertexBuffer, DirectX::XMMATRIX* world, int vertexCount, int vertexSize)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		XMMATRIX worldMatrixC = XMMatrixTranspose(*world);

		result = deviceContext->Map(_matrixBufferPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			throw std::runtime_error("RenderModule::SetDataPerObject: Failed to Map _matrixBufferPerObject");
		}

		MatrixBufferPerObject* dataPtr = static_cast<MatrixBufferPerObject*>(mappedResource.pData);
		dataPtr->_world = worldMatrixC;
		deviceContext->Unmap(_matrixBufferPerObject, 0);

		deviceContext->VSSetConstantBuffers(1, 1, &_matrixBufferPerObject);

		UINT32 vtxs = vertexSize;
		UINT32 offset = 0;

		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vtxs, &offset);

		deviceContext->Draw(vertexCount, 0);
	}

	void RenderModule::RenderParticles(ID3D11Buffer* particlePointsBuffer, int vertexCount, int vertexSize)
	{
		ID3D11DeviceContext* deviceContext = _d3d->GetDeviceContext();

		UINT32 offset = 0;
		UINT32 vs = vertexSize;
		deviceContext->IASetVertexBuffers(0, 1, &particlePointsBuffer, &vs, &offset);

		deviceContext->Draw(vertexCount, 0);
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