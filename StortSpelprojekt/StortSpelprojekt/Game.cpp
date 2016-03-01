#include "Game.h"
#include <stdexcept>
#include <DirectXMath.h>
#include <sstream>

Game::Game(HINSTANCE hInstance, int nCmdShow):
	_settingsReader("Assets/settings.xml", "Assets/profile.xml"),
	_soundModule(_settingsReader.GetSettings())
{
	srand(time(NULL));
	System::Settings* settings = _settingsReader.GetSettings();

	_gameHandle = this;
	_window = new System::Window("Amazing game", hInstance, settings, WndProc);
	_timer = System::Timer();
	_renderModule = new Renderer::RenderModule(_window->GetHWND(), settings);
	
	_assetManager = new AssetManager(_renderModule->GetDevice());
	_combinedMeshGenerator = new CombinedMeshGenerator(_renderModule->GetDevice(), _renderModule->GetDeviceContext());
	_controls = new System::Controls(_window->GetHWND());
	_fontWrapper = new FontWrapper(_renderModule->GetDevice(), L"Assets/Fonts/Calibri.ttf", L"Calibri");

	_camera = new System::Camera(settings);
	_camera->SetPosition(XMFLOAT3(3, 20, 0));
	_camera->SetRotation(XMFLOAT3(60, 0, 0));

	ParticleTextures particleTextures;
	ParticleModifierOffsets modifiers;
	LoadParticleSystemData(particleTextures, modifiers);
	_particleHandler = new Renderer::ParticleHandler(_renderModule->GetDevice(), _renderModule->GetDeviceContext(), particleTextures, modifiers);

	_objectHandler = new ObjectHandler(_renderModule->GetDevice(), _assetManager, &_data, _settingsReader.GetSettings(), _particleHandler->GetParticleEventQueue(), &_soundModule);
	_pickingDevice = new PickingDevice(_camera, settings);

	GameObject::_particleEventQueue = _particleHandler->GetParticleEventQueue();
	_SM = new StateMachine(_controls, _objectHandler, _camera, _pickingDevice, "Assets/gui.json", _assetManager, _fontWrapper, settings, &_settingsReader, &_soundModule, &_ambientLight, _combinedMeshGenerator);

	_SM->Update(_timer.GetFrameTime());

	_enemiesHasSpawned = false;

	_ambientLight = _renderModule->GetAmbientLight();	
	ResizeResources(settings);//This fixes a bug which offsets mousepicking, do not touch! //Markus
}

Game::~Game()
{
	SAFE_DELETE(_objectHandler);
	SAFE_DELETE(_window);
	SAFE_DELETE(_renderModule);
	SAFE_DELETE(_camera);
	SAFE_DELETE(_SM);
	SAFE_DELETE(_controls);
	SAFE_DELETE(_assetManager);
	SAFE_DELETE(_pickingDevice);
	SAFE_DELETE(_fontWrapper);
	SAFE_DELETE(_particleHandler);
	SAFE_DELETE(_combinedMeshGenerator);
}

void Game::ResizeResources(System::Settings* settings)
{
	_renderModule->ResizeResources(settings);
	_window->ResizeWindow(settings);
	_SM->Resize(settings);
	_camera->Resize(settings);
}

void Game::LoadParticleSystemData(ParticleTextures& particleTextures, ParticleModifierOffsets& modifiers)
{
	ParticleSystemData data;
	try
	{
		ifstream in("Assets/ParticleSystemData.json");
		cereal::JSONInputArchive inArchive(in);
		inArchive(data);
		in.close();
	}
	catch (...)
	{
		throw std::runtime_error("Game::LoadParticleSystemData: Failed to load file");
	}

	for (unsigned int i = 0; i < data._subtypeTexturePaths.at(ParticleSubType::BLOOD_SUBTYPE).size(); i++)
	{
		particleTextures._bloodTextures[i] = _assetManager->GetTexture(data._subtypeTexturePaths.at(ParticleSubType::BLOOD_SUBTYPE).at(i))->_data;
	}

	for (unsigned int i = 0; i < data._subtypeTexturePaths.at(ParticleSubType::WATER_SUBTYPE).size(); i++)
	{
		particleTextures._waterTextures[i] = _assetManager->GetTexture(data._subtypeTexturePaths.at(ParticleSubType::WATER_SUBTYPE).at(i))->_data;
	}

	for (unsigned int i = 0; i < data._subtypeTexturePaths.at(ParticleSubType::SPARK_SUBTYPE).size(); i++)
	{
		particleTextures._sparkTextures[i] = _assetManager->GetTexture(data._subtypeTexturePaths.at(ParticleSubType::SPARK_SUBTYPE).at(i))->_data;
	}

	for (unsigned int i = 0; i < data._subtypeTexturePaths.at(ParticleSubType::SMOKE_SUBTYPE).size(); i++)
	{
		particleTextures._smokeTextures[i] = _assetManager->GetTexture(data._subtypeTexturePaths.at(ParticleSubType::SMOKE_SUBTYPE).at(i))->_data;
	}

	for (unsigned int i = 0; i < data._subtypeTexturePaths.at(ParticleSubType::FIRE_SUBTYPE).size(); i++)
	{
		particleTextures._fireTextures[i] = _assetManager->GetTexture(data._subtypeTexturePaths.at(ParticleSubType::FIRE_SUBTYPE).at(i))->_data;
	}

	for (unsigned int i = 0; i < data._subtypeTexturePaths.at(ParticleSubType::MUZZLE_FLASH_SUBTYPE).size(); i++)
	{
		particleTextures._muzzleFlashTextures[i] = _assetManager->GetTexture(data._subtypeTexturePaths.at(ParticleSubType::MUZZLE_FLASH_SUBTYPE).at(i))->_data;
	}

	for (unsigned int i = 0; i < data._iconTexturePaths.size(); i++)
	{
		particleTextures._iconTextures[i] = _assetManager->GetTexture(data._iconTexturePaths.at(i))->_data;
	}

	modifiers._splashPositionOffset = data._splashPositionOffset;
	modifiers._smokePositionOffset = data._smokePositionOffset;
	modifiers._firePositionOffset = data._firePositionOffset;

	modifiers._splashDirectionOffset = data._splashDirectionOffset;
	modifiers._smokeDirectionOffset = data._smokeDirectionOffset;
	modifiers._fireDirectionOffset = data._fireDirectionOffset;

	modifiers._splashSpeedRange = data._splashSpeedRange;
	modifiers._smokeSpeedRange = data._smokeSpeedRange;
	modifiers._fireSpeedRange = data._fireSpeedRange;

	modifiers._smokeRepeatTime = data._smokeRepeatTime;
	modifiers._fireRepeatTime = data._fireRepeatTime;
	modifiers._lightningRepeatTime = data._lightningRepeatTime;
}

bool Game::Update(double deltaTime)
{
	_soundModule.Update(_camera->GetPosition().x, _camera->GetPosition().y, _camera->GetPosition().z);

	if (_SM->GetState() == PLACEMENTSTATE)
	{
		_objectHandler->UpdateLights();
	}

	bool run = true;

	//Apply settings if they has changed
	if (_settingsReader.GetSettingsChanged())
	{
		System::Settings* settings = _settingsReader.GetSettings();
		ResizeResources(settings);
		_soundModule.SetVolume(settings->_volume / 100.0f, CHMASTER);
		_settingsReader.SetSettingsChanged(false);
	}

	_controls->Update();
	run = _SM->Update(deltaTime);

	if (_controls->IsFunctionKeyDown("DEBUG:REQUEST_PARTICLE"))
	{
		if (!tempAlreadyCombined)
		{
			//_combinedMeshGenerator->CombineMeshes(_objectHandler->GetTileMap(), System::FLOOR);
			_combinedMeshGenerator->CombineMeshes(_objectHandler->GetTileMap(), System::WALL);
			tempAlreadyCombined = true;
		}

		tempTestCombinedStuff = !tempTestCombinedStuff;
	}

	_particleHandler->Update(deltaTime);

	/*
		{
			_enemies = _objectHandler->GetAllByType(ENEMY);
			_loot = _objectHandler->GetAllByType(LOOT);

			if (_enemies.size() > 0)
			{
				_enemiesHasSpawned = true;
			}
		}
		*/
	return run;
}

void Game::Render()
{
	_renderModule->SetAmbientLight(_ambientLight);
	_renderModule->BeginScene(0.0f, 0.5f, 0.5f, 1.0f);
	_renderModule->SetDataPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());

	/*///////////////////////////////////////////////////////  Geometry pass  ////////////////////////////////////////////////////////////
	Render the objects to the diffuse and normal resource views. Camera depth is also generated here.									*/
	std::vector<std::vector<GameObject*>>* gameObjects = _objectHandler->GetGameObjects();
	
	/*-------------------------------------------------  Render non-skinned objects  ---------------------------------------------------*/
	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::GEO_PASS);
	RenderGameObjects(Renderer::RenderModule::ShaderStage::GEO_PASS, gameObjects);

	/*---------------------------------------------------  Render skinned objects  -----------------------------------------------------*/
	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::ANIM_STAGE);
	RenderGameObjects(Renderer::RenderModule::ShaderStage::ANIM_STAGE, gameObjects);

	/*-------------------------------------------------  Render billboarded objects  ---------------------------------------------------*/
	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::BILLBOARDING_STAGE);
	RenderParticles();

	/*-------------------------------------------------------  Render grid  ------------------------------------------------------------*/
	if (_SM->GetState() == LEVELEDITSTATE)
	{
		_renderModule->SetShaderStage(Renderer::RenderModule::RENDER_LINESTRIP);

		Grid* gr = _objectHandler->GetBuildingGrid();
		_renderModule->SetDataPerLineList(gr->GetLineBuffer(), gr->GetVertexSize());

		std::vector<DirectX::XMMATRIX>* gridMatrices = gr->GetGridMatrices();
		for (auto &matrix : *gridMatrices)
		{
			_renderModule->RenderLineStrip(&matrix, gr->GetNrOfPoints(), gr->GetColorOffset());
		}
	}
	
	////////////////////////////////////////////////////////////  Light pass  //////////////////////////////////////////////////////////////
	if (_SM->GetState() == PLAYSTATE || _SM->GetState() == PLACEMENTSTATE)
	{
		/*------------------------------------------------------  Spotlights  --------------------------------------------------------------
		Generate the shadow map for each spotlight, then apply the lighting/shadowing to the render target with additive blending.        */ 

		_renderModule->SetLightDataPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());

		map<GameObject*, Renderer::Spotlight*>* spotlights = _objectHandler->GetSpotlights();
		for (pair<GameObject*, Renderer::Spotlight*> spot : *spotlights)
		{
			if (spot.second != nullptr && spot.second->IsActive() && spot.first->IsActive())
			{
				GenerateShadowMap(spot.second, spot.first->GetID());

				_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::LIGHT_APPLICATION_SPOTLIGHT);
				_renderModule->SetLightDataPerSpotlight(spot.second);

				_renderModule->RenderVertexBuffer(spot.second->GetVolumeBuffer(), spot.second->GetWorldMatrix(), spot.second->GetVertexCount(), spot.second->GetVertexSize());
			}
		}
		/*------------------------------------------------------  Pointlights  -----------------------------------------------------------*/
		_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::LIGHT_APPLICATION_POINTLIGHT);

		map<GameObject*, Renderer::Pointlight*>* pointlights = _objectHandler->GetPointlights();
		for (pair<GameObject*, Renderer::Pointlight*> pointlight : *pointlights)
		{
			if (pointlight.second != nullptr && pointlight.second->IsActive() && pointlight.first->IsActive())
			{
				_renderModule->SetLightDataPerPointlight(pointlight.second);
				_renderModule->RenderVertexBuffer(pointlight.second->GetVolumeBuffer(), pointlight.second->GetWorldMatrix(), pointlight.second->GetVertexCount(), pointlight.second->GetVertexSize());
			}
		}
	}

	/*-----------------------------------------------------------  FXAA  -----------------------------------------------------------------*/
	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::AA_STAGE);
	_renderModule->RenderScreenQuad();

	///////////////////////////////////////////////////////  HUD and other 2D   ////////////////////////////////////////////////////////////
	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::HUD_STAGE);
	_renderModule->Render(_SM->GetCurrentStatePointer()->GetUITree()->GetRootNode(), _fontWrapper);

	_renderModule->EndScene();
}

void Game::RenderGameObjects(int forShaderStage, std::vector<std::vector<GameObject*>>* gameObjects)
{
	if (tempTestCombinedStuff)
	{
		if (forShaderStage == Renderer::RenderModule::ShaderStage::GEO_PASS)
		{
			std::vector<std::vector<CombinedMesh>>* combinedMeshes = _combinedMeshGenerator->GetCombinedMeshes();
			for (auto& objVector : *combinedMeshes)
			{
				for (auto& obj : objVector)
				{
					_renderModule->SetDataPerObjectType(obj._combinedObject);
					_renderModule->Render(&obj._world, obj._vertexCount);
				}
			}
		}
	}

	for (auto& i : *gameObjects)
	{
		if (i.size() > 0)
		{
			if (tempTestCombinedStuff)
			{
				//The floors in the gameObjects vector should not be rendered, as these are combined in a single mesh to reduce draw calls
				if ((_SM->GetState() == PLACEMENTSTATE || _SM->GetState() == PLAYSTATE) && (i.at(0)->GetType() == System::FLOOR || i.at(0)->GetType() == System::WALL))
				{
					continue;
				}
			}

			//if ((_SM->GetState() == PLACEMENTSTATE || _SM->GetState() == PLAYSTATE) && (i.at(0)->GetType() == System::WALL))
			//{
			//	continue;
			//}


			GameObject* lastGameObject = nullptr;
			RenderObject* lastRenderObject = nullptr;
			int vertexBufferSize = 0;
			for (int j = 0; j < i.size(); j++)
			{
				GameObject* gameObject = i.at(j);
				RenderObject* renderObject = gameObject->GetRenderObject();

				if ((forShaderStage == Renderer::RenderModule::ShaderStage::GEO_PASS && renderObject->_mesh->_isSkinned)
					|| (forShaderStage == Renderer::RenderModule::ShaderStage::ANIM_STAGE && !renderObject->_mesh->_isSkinned))
				{
					continue;
				}
				else
				{
					if (lastGameObject == nullptr || *lastRenderObject != *renderObject || lastGameObject->GetSubType() != gameObject->GetSubType())
					{
						_renderModule->SetDataPerObjectType(renderObject);
						vertexBufferSize = renderObject->_mesh->_vertexBufferSize;
					}
					if (gameObject->IsVisible())
					{
						if (forShaderStage == Renderer::RenderModule::ShaderStage::GEO_PASS)
						{
							_renderModule->Render(gameObject->GetMatrix(), vertexBufferSize, gameObject->GetColorOffset());
						}
						else
						{
							_renderModule->RenderAnimation(gameObject->GetMatrix(), vertexBufferSize, gameObject->GetAnimation()->GetFloats(), gameObject->GetColorOffset());
						}
					}
					lastGameObject = gameObject;
					lastRenderObject = renderObject;
				}
			}
		}
	}
}

void Game::GenerateShadowMap(Renderer::Spotlight* spotlight, unsigned short ownerID)
{
	//Non skinned objects
	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::SHADOW_GENERATION);
	_renderModule->SetShadowMapDataPerSpotlight(spotlight->GetViewMatrix(), spotlight->GetProjectionMatrix());

	vector<vector<GameObject*>>* inLight = _objectHandler->GetObjectsInLight(spotlight);
	GameObject* lastGameObject = nullptr;
	RenderObject* lastRenderObject = nullptr;
	int vertexBufferSize = 0;

	for (auto j : *inLight)
	{
		if (j.size() > 0)
		{
			vertexBufferSize = 0;

			for (int i = 0; i < j.size(); i++)
			{
				GameObject* obj = j.at(i);
				RenderObject* renderObject = obj->GetRenderObject();

				if (!renderObject->_mesh->_isSkinned)
				{
					//If the current object and the previous object are different, set the new data
					unsigned int thisSubType = obj->GetSubType();
					if (lastGameObject == nullptr || *lastRenderObject != *renderObject || lastGameObject->GetSubType() != obj->GetSubType())
					{
						_renderModule->SetShadowMapDataPerObjectType(renderObject);
						vertexBufferSize = renderObject->_mesh->_vertexBufferSize;
						lastRenderObject = renderObject;
						lastGameObject = obj;
					}

					//Render the visible objects, but skip the owner itself
					if (obj->IsVisible() && obj->GetID() != ownerID)
					{
						_renderModule->RenderShadowMap(obj->GetMatrix(), vertexBufferSize);
					}
				}
			}
		}
	}

	//Animated/skinned objects
	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::ANIM_SHADOW_GENERATION);
	_renderModule->SetShadowMapDataPerSpotlight(spotlight->GetViewMatrix(), spotlight->GetProjectionMatrix());

	for (auto j : *inLight)
	{
		if (j.size() > 0)
		{
			vertexBufferSize = 0;

			for (int i = 0; i < j.size(); i++)
			{
				GameObject* obj = j.at(i);
				RenderObject* renderObject = obj->GetRenderObject();
				Animation* anim = obj->GetAnimation();

				if (renderObject->_mesh->_isSkinned && anim)
				{
					//If the current object and the previous object are different, set the new data
					unsigned int thisSubType = obj->GetSubType();
					if (lastGameObject == nullptr || *lastRenderObject != *renderObject || lastGameObject->GetSubType() != obj->GetSubType())
					{
						_renderModule->SetShadowMapDataPerObjectType(renderObject);
						vertexBufferSize = renderObject->_mesh->_vertexBufferSize;
						lastRenderObject = renderObject;
						lastGameObject = obj;
					}

					//Render the visible objects, but skip the owner itself
					if (obj->IsVisible() && obj->GetID() != ownerID)
					{
						_renderModule->RenderShadowMap(obj->GetMatrix(), vertexBufferSize, anim->GetFloats());
					}
				}
			}
		}
	}
}

void Game::RenderParticles()
{
	int count = _particleHandler->GetEmitterCount();

	//Render all particle emitter except for electricity, as those use linestrip instead of pointlist
	for (int i = 0; i < count; i++)
	{
		Renderer::ParticleEmitter* emitter = _particleHandler->GetEmitter(i);

		if (emitter)
		{
			ID3D11Buffer* vertexBuffer = emitter->GetParticleBuffer();

			if (vertexBuffer)
			{
				ParticleType type = emitter->GetType();
				if (type != ParticleType::ELECTRICITY)
				{
					int textureCount = PARTICLE_TEXTURE_COUNT;
					if (type == ParticleType::ICON)
					{
						ParticleSubType subType = emitter->GetSubType();
						XMFLOAT3 campos = _camera->GetPosition();
						XMFLOAT3 emitterPosition = emitter->GetPosition();
						bool isMarker = (subType == ParticleSubType::QUESTIONMARK_SUBTYPE);
						if (isMarker)
						{	
							campos = emitterPosition;
							campos.y += 1.0f;
						}
						textureCount = 1;
						ID3D11ShaderResourceView* textures[1];
						textures[0] = _particleHandler->GetIconTexture(emitter->GetSubType());

						_renderModule->SetDataPerParticleEmitter(emitterPosition, _camera->GetViewMatrix(), _camera->GetProjectionMatrix(), campos, emitter->GetParticleScale(), textures, textureCount, (int)!isMarker);
						_renderModule->RenderParticles(vertexBuffer, emitter->GetParticleCount(), emitter->GetVertexSize());
					}
					else
					{
						ID3D11ShaderResourceView** textures = _particleHandler->GetTextures(textureCount, emitter->GetSubType());
						_renderModule->SetDataPerParticleEmitter(emitter->GetPosition(), _camera->GetViewMatrix(), _camera->GetProjectionMatrix(), _camera->GetPosition(), emitter->GetParticleScale(), textures, textureCount, 0);

						_renderModule->RenderParticles(vertexBuffer, emitter->GetParticleCount(), emitter->GetVertexSize());
					}
				}
			}
		}
	}

	//Render all electricity "particles"
	_renderModule->SetShaderStage(Renderer::RenderModule::RENDER_LINESTRIP);

	for (int i = 0; i < count; i++)
	{
		Renderer::ParticleEmitter* emitter = _particleHandler->GetEmitter(i);

		if (emitter)
		{
			ID3D11Buffer* vertexBuffer = emitter->GetParticleBuffer();

			if (vertexBuffer)
			{
				XMFLOAT4 color(0.9f, 0.6f, 0.25f, 1.0f);
				if (emitter->GetType() == ParticleType::ELECTRICITY)
				{
					_renderModule->SetShaderStage(Renderer::RenderModule::RENDER_LINESTRIP);
					XMFLOAT3 pos = emitter->GetPosition();

					XMMATRIX m = XMMatrixTranslation(pos.x, pos.y, pos.z);

					_renderModule->SetDataPerLineList(emitter->GetParticleBuffer(), emitter->GetVertexSize());
					_renderModule->RenderLineStrip(&m, emitter->GetParticleCount(), XMFLOAT3(0.3f, 0.6f, 0.95f));
				}
			}
		}
	}
}

int Game::Run()
{
	bool run = true;

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (run)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				run = false;
			}
		}
		else
		{
			_timer.Update();
			if (_timer.GetFrameTime() >= MS_PER_FRAME)
			{
				if (_hasFocus)
				{
					run = Update(_timer.GetFrameTime());

					if (run)
					{
						Render();

						string s = to_string(_timer.GetFrameTime()) + " " + to_string(_timer.GetFPS());
						SetWindowText(_window->GetHWND(), s.c_str());

						_timer.Reset();
					}
				}
			}
		}
	}

	return 0;
}

LRESULT CALLBACK Game::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}


LRESULT CALLBACK Game::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_QUIT:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CHAR:
	{
		_gameHandle->_controls->HandleTextInput(wparam, lparam);
		break;
	}
	case WM_INPUT:
	{
		_gameHandle->_controls->HandleRawInput(lparam);
		break;
	}
	case WM_SETFOCUS:
	{
		_gameHandle->_hasFocus = true;
		break;
	}
	case WM_KILLFOCUS:
	{
		_gameHandle->_controls->ResetInputBuffers();
		_gameHandle->_hasFocus = false;
		break;
	}
	default:
	{
		return _gameHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}