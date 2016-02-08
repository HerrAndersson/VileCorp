#include "Game.h"
#include <stdexcept>
#include <DirectXMath.h>
#include <sstream>

Game::Game(HINSTANCE hInstance, int nCmdShow):
	_settingsReader("Assets/settings.xml")
{
	System::Settings* settings = _settingsReader.GetSettings();


	_gameHandle = this;
	_window = new System::Window("Amazing game", hInstance, settings, WndProc);

	_timer = System::Timer();

	_renderModule = new Renderer::RenderModule(_window->GetHWND(), settings);
	
	_assetManager = new AssetManager(_renderModule->GetDevice());
	_controls = new System::Controls(_window->GetHWND());
	_fontWrapper = new FontWrapper(_renderModule->GetDevice(), L"Assets/Fonts/Calibri.ttf", L"Calibri");

	//Init camera
	_camera = new System::Camera(settings);
	_camera->SetPosition(XMFLOAT3(3, 20, 0));
	_camera->SetRotation(XMFLOAT3(60, 0, 0));

	_timer = System::Timer();

	//GameObjectInfo* _data = new GameObjectInfo();
	GameObjectDataLoader gameObjectDataLoader;
	gameObjectDataLoader.WriteSampleGameObjects();
	gameObjectDataLoader.LoadGameObjectInfo(&_data);

	_objectHandler = new ObjectHandler(_renderModule->GetDevice(), _assetManager, &_data);
	_pickingDevice = new PickingDevice(_camera, settings);
	_SM = new StateMachine(_controls, _objectHandler, _camera, _pickingDevice, "Assets/gui.json", _assetManager, _fontWrapper, settings, &_settingsReader);

	_SM->Update(_timer.GetFrameTime());

	_enemiesHasSpawned = false;

	//_controls->SaveKeyBindings(System::MAP_EDIT_KEYMAP, "MOVE_CAMERA_UP", "M");

	Renderer::Spotlight* spot;
	for (int i = 0; i < 2; i++)
	{
		SpotlightData lightdata;
		lightdata._angle = XM_PIDIV4;
		lightdata._color = XMFLOAT3(0.6f, 0.6f, 0.6f);
		lightdata._range = 9.5f;
		lightdata._intensity = 1.0f;
		int d = _renderModule->SHADOWMAP_DIMENSIONS;
		spot = new Renderer::Spotlight(_renderModule->GetDevice(), lightdata, d, d, 0.1f, 1000.0f);
		spot->SetPositionAndRotation(XMFLOAT3(3 + i*2, 1, 2 + i * 2), XMFLOAT3(0,-65*i,0));
		_spotlights.push_back(spot);
	}
	Renderer::Pointlight* point;
	for (int i = 0; i < 10; i++)
	{
		point = new Renderer::Pointlight(_renderModule->GetDevice(), XMFLOAT3(5 + 3 * sin(25 + i * 2 + rand() % ((i+1)) / 8) * sin(i) + 1, 0.05f, 5 + 3 * sin(25 + i * 2 + rand() % ((i + 1)) / 8) * cos(i)), 1.0f, 1.0f, XMFLOAT3(sin(i*i * 6), sin(i * 50 * i), sin(120 * i * i)));
		_pointlights.push_back(point);
	}
	_lightCulling = nullptr;
}

Game::~Game()
{
	SAFE_DELETE(_window);
	SAFE_DELETE(_renderModule);
	SAFE_DELETE(_camera);
	SAFE_DELETE(_objectHandler);
	SAFE_DELETE(_SM);
	SAFE_DELETE(_controls);
	SAFE_DELETE(_assetManager);
	SAFE_DELETE(_pickingDevice);
	SAFE_DELETE(_fontWrapper);
	SAFE_DELETE(_lightCulling);
	for(auto s : _spotlights)
	{
		SAFE_DELETE(s);
	}

	for (auto p : _pointlights)
	{
		SAFE_DELETE(p);
	}
}

void Game::ResizeResources(System::Settings* settings)
{
	_window->ResizeWindow(settings);
	_renderModule->ResizeResources(_window->GetHWND(), settings);
	_camera->Resize(settings);
	_SM->Resize(settings);
}

bool Game::Update(float deltaTime)
{
	bool run = true;

	//Apply settings if they has changed
	if (_settingsReader.GetSettingsChanged())
	{
		ResizeResources(_settingsReader.GetSettings());
		_settingsReader.SetSettingsChanged(false);
	}

	_controls->Update();
	run = _SM->Update(deltaTime);

	/*
	_enemies = _objectHandler->GetAllByType(ENEMY);
	_loot = _objectHandler->GetAllByType(LOOT);

	if (_enemies.size() > 0)
	{
		_enemiesHasSpawned = true;
	}

	if (_enemies.size() == 0 && _enemiesHasSpawned == true)
	{
		if (_loot.size() >= 1)
		{
			//TODO: Add something to notify the player that they've beat the level
		}
		else
		{
			//TODO: Add something to notify the player that they've SUCK and they can replay the level
		}
	}
	*/

	//Save for debugging //Jonas
	int i = 0;
	for (auto s : _spotlights)
	{
		XMFLOAT3 color = s->GetColor();
		color.x = sin(_timer.GetGameTime() / 1000 + 100 * i);
		color.y = sin(_timer.GetGameTime() / 1000 + 100 * i + XMConvertToRadians(120));
		color.z = sin(_timer.GetGameTime() / 1000 + 100 * i + XMConvertToRadians(240));
		s->SetColor(color);
		XMFLOAT3 rot = s->GetRotation();
		rot.y -= 0.2f;
		s->SetRotation(rot);
		i++;
	}
	i = 0;
	for (auto p : _pointlights)
	{
		i++;
		XMFLOAT3 pos = p->GetPosition();
		p->SetPosition(XMFLOAT3(pos.x + (sin((_timer.GetGameTime() / 1000) * sin(120 * i))) / 20, pos.y, pos.z + (sin((_timer.GetGameTime() / 1000) * sin(17 * i * i))) / 20));
	}

	return run;
}

//TODO: TEMP! Should be removed later. Used for initializing of LightCulling. /Jonas
bool init = false;

void Game::Render()
{
	_renderModule->BeginScene(0.0f, 0.5f, 0.5f, 1.0f);
	_renderModule->SetDataPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());

	/*///////////////////////////////////////////////////////  Geometry pass  ////////////////////////////////////////////////////////////
	Render the objects to the diffuse and normal resource views. Camera depth is also generated here.									*/

	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::GEO_PASS);

	std::vector<std::vector<GameObject*>>* gameObjects = _objectHandler->GetGameObjects();

	/*------------------------------------------------  Render non-skinned objects  ---------------------------------------------------*/
	for (auto i : *gameObjects)
	{
		if (i.size() > 0)
		{
			RenderObject* renderObject = i.at(0)->GetRenderObject();
			if (renderObject->_isSkinned)
			{
				continue;
			}
			else
			{
				_renderModule->SetDataPerObjectType(renderObject);
				int vertexBufferSize = renderObject->_mesh._vertexBufferSize;

				for (GameObject* g : i)
				{
					if (g->IsVisible())
					{
					_renderModule->Render(g->GetMatrix(), vertexBufferSize, g->GetColorOffset());
					}
					//g->SetColorOffset(XMFLOAT3(0, 0, 0));
				}
			}
		}
	}

	/*--------------------------------------------------  Render skinned objects  -----------------------------------------------------*/
	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::ANIM_STAGE);
	if (gameObjects->size() > 0)
	{
		if (gameObjects->at(GUARD).size() > 0)
		{
			RenderObject* renderObject = gameObjects->at(GUARD).at(0)->GetRenderObject();

			_renderModule->SetDataPerObjectType(renderObject);
			int vertexBufferSize = renderObject->_mesh._vertexBufferSize;

			for (GameObject* a : gameObjects->at(GUARD))
			{
				// temporary uncommenting
				//_renderModule->RenderAnimation(a->GetMatrix(), vertexBufferSize, a->GetAnimation()->GetTransforms(), a->GetColorOffset());
			}
		}
	}
	
	//TEMPORARY!!
	std::vector<std::vector<std::vector<GameObject*>>> inLight;
	if (_SM->GetState() == PLAYSTATE)
	{
		if (!init)
		{
			init = true;
			_lightCulling = new LightCulling(_objectHandler->GetTileMap());
		}

		for (unsigned int i = 0; i < _spotlights.size(); i++)
		{
			inLight.push_back(*_lightCulling->GetObjectsInSpotlight(_spotlights[i]));
		}

		//"Fog of War"
		for (unsigned int i = 0; i < _spotlights.size(); i++)
		{
			if (inLight.at(i).size() >= ENEMY)
			{
				if (inLight.at(i).at(ENEMY).size() > 0)
				{
					RenderObject* renderObject = inLight.at(i).at(ENEMY).at(0)->GetRenderObject();

					_renderModule->SetDataPerObjectType(renderObject);
					int vertexBufferSize = renderObject->_mesh._vertexBufferSize;

					for (GameObject* a : inLight.at(i).at(ENEMY))
					{
						//_renderModule->RenderAnimation(a->GetMatrix(), vertexBufferSize, a->GetAnimation()->GetTransforms(), a->GetColorOffset());
						_renderModule->Render(a->GetMatrix(), vertexBufferSize, a->GetColorOffset());
					}
				}
			}
		}
	}

	if (_SM->GetState() == LEVELEDITSTATE)
	{
		Grid* gr = _objectHandler->GetBuildingGrid();

		_renderModule->SetShaderStage(Renderer::RenderModule::GRID_STAGE);
		_renderModule->SetDataPerLineList(gr->GetLineBuffer(), gr->GetVertexSize());

		std::vector<DirectX::XMMATRIX>* gridMatrices = gr->GetGridMatrices();
		for (auto &matrix : *gridMatrices)
		{
			_renderModule->RenderLineList(&matrix, gr->GetNrOfPoints(), gr->GetColorOffset());
		}
	}
	
	////////////////////////////////////////////////////////////  Light pass  //////////////////////////////////////////////////////////////
	if (_SM->GetState() == PLAYSTATE)
	{
	//----------------------------------------------------------  Spotlights  -------------------------------------------------------------
	//Generate the shadow map for each spotlight, then apply the lighting/shadowing to the render target with additive blending.           

		_renderModule->SetLightDataPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());
		for (unsigned int i = 0; i < _spotlights.size(); i++)
		{
			_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::SHADOW_GENERATION);
			_renderModule->SetShadowMapDataPerSpotlight(_spotlights[i]->GetViewMatrix(), _spotlights[i]->GetProjectionMatrix());

			
			for (auto j : inLight.at(i))
			{
				if (j.size() > 0)
				{
					RenderObject* renderObject = j.at(0)->GetRenderObject();
					_renderModule->SetShadowMapDataPerObjectType(renderObject);
					int vertexBufferSize = renderObject->_mesh._vertexBufferSize;

					for (GameObject* g : j)
					{
					if (g->IsVisible())
					{
						_renderModule->RenderShadowMap(g->GetMatrix(), vertexBufferSize);
					}
				}
			}
		}

			_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::LIGHT_APPLICATION_SPOTLIGHT);
			_renderModule->SetLightDataPerSpotlight(_spotlights[i]);

			_renderModule->RenderLightVolume(_spotlights[i]->GetVolumeBuffer(), _spotlights[i]->GetWorldMatrix(), _spotlights[i]->GetVertexCount(), _spotlights[i]->GetVertexSize());
		}
		
	/*---------------------------------------------------------  Pointlights  ------------------------------------------------------------*/
		_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::LIGHT_APPLICATION_POINTLIGHT);
		for (auto p : _pointlights)
		{
			_renderModule->SetLightDataPerPointlight(p);
			_renderModule->RenderLightVolume(p->GetVolumeBuffer(), p->GetWorldMatrix(), p->GetVertexCount(), p->GetVertexSize());
		}
	}

	/*-----------------------------------------------------------  FXAA  -----------------------------------------------------------------*/

	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::AA_STAGE);
	_renderModule->RenderScreenQuad();

	/////////////////////////////////////////////////////////  HUD and other 2D   ////////////////////////////////////////////////////
	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::HUD_STAGE);
	_renderModule->Render(_SM->GetCurrentStatePointer()->GetUITree()->GetRootNode(), _fontWrapper);

	_renderModule->EndScene();
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