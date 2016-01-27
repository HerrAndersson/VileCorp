#include "Game.h"
#include <stdexcept>
#include <DirectXMath.h>
#include <sstream>

Game::Game(HINSTANCE hInstance, int nCmdShow)
{
	CheckSettings();
	_gameHandle = this;
	_window = new System::Window("Amazing game", hInstance, _windowSettings, WndProc);

	_timer = System::Timer();

	_renderModule = new Renderer::RenderModule(_window->GetHWND(), _windowSettings._width, _windowSettings._height, _gameSettings._fullScreen);
	
	_assetManager = new AssetManager(_renderModule->GetDevice());
	_controls = new System::Controls(_window->GetHWND());
	_fontWrapper = new FontWrapper(_renderModule->GetDevice(), L"Assets/Fonts/Calibri.ttf", L"Calibri");

	//Init camera
	_camera = new System::Camera(0.1f, 1000.0f, DirectX::XM_PIDIV2, _windowSettings._width, _windowSettings._height);
	_camera->SetPosition(XMFLOAT3(3, 20, 0));
	_camera->SetRotation(XMFLOAT3(60, 0, 0));

	_timer = System::Timer();

	GameObjectInfo* data = new GameObjectInfo();
	GameObjectDataLoader gameObjectDataLoader;
	gameObjectDataLoader.WriteSampleGameObjects();
	gameObjectDataLoader.LoadGameObjectInfo(data);

	_objectHandler = new ObjectHandler(_renderModule->GetDevice(), _assetManager, data);
	_pickingDevice = new PickingDevice(_camera, _window);
	_SM = new StateMachine(_controls, _objectHandler, _camera, _pickingDevice, "Assets/gui.json", _assetManager, _fontWrapper, _windowSettings._width, _windowSettings._height);

	_SM->Update(_timer.GetFrameTime());
	
	if (_SM->GetState() == LEVELEDITSTATE)
	{
		//TODO: Use dimensions from the tilemap /Jonas
		_grid = new Grid(_renderModule->GetDevice(), 1, 1, 1, DirectX::XMFLOAT3(0.4f, 1.0f, 0.3f));
	}
	else
	{
		_grid = nullptr;
	}

	_enemiesHasSpawned = false;

	//_controls->SaveKeyBindings(System::MAP_EDIT_KEYMAP, "MOVE_CAMERA_UP", "M");

	Renderer::Spotlight* spot;
	for (int i = 0; i < 5; i++)
	{
		int d = _renderModule->SHADOWMAP_DIMENSIONS;
		spot = new Renderer::Spotlight(_renderModule->GetDevice(), 0.1f, 1000.0f, XM_PIDIV4, d, d, 1.0f, 9.5f, XMFLOAT3(1.0f, 1.0f, 1.0f), 72);
		spot->SetPositionAndRotation(XMFLOAT3(rand() % 35, 1, rand() % 35), XMFLOAT3(0,-35*i,0));
		_spotlights.push_back(spot);
	}
}

void Game::CheckSettings()
{
	//System::saveJSON(&_gameSettings, "Assets/GameSettings.json", "Game Settings");
	System::loadJSON(&_gameSettings, "Assets/GameSettings.json");

	if (_gameSettings._default == false)
	{
		_windowSettings._width = _gameSettings._resX;
		_windowSettings._height = _gameSettings._resY;
		_windowSettings._flags = 0;
		if (_gameSettings._fullScreen == true)
		{
			_windowSettings._flags |= System::WindowSettings::FULLSCREEN;
		}
		if (_gameSettings._bordeless == true)
		{
			_windowSettings._flags |= System::WindowSettings::BORDERLESS;
		}
		if (_gameSettings._showMouseCursor == true)
		{
			_windowSettings._flags |= System::WindowSettings::SHOW_CURSOR;
		}
	}
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
	SAFE_DELETE(_grid);
	delete _fontWrapper;

	for(auto s : _spotlights)
	{
		delete s;
	}
}

void Game::ResizeResources(System::WindowSettings settings)
{
	_window->ResizeWindow(settings);
	_renderModule->ResizeResources(_window->GetHWND(), settings._width, settings._height);
	_camera->Resize(settings._width, settings._height);
	_SM->Resize(settings._width, settings._height);
}

void Game::Update(float deltaTime)
{
	_controls->Update();

	if (_controls->IsFunctionKeyDown("EVERYWHERE:FULLSCREEN"))
	{
		System::WindowSettings windowSettings = _window->GetWindowSettings();
		_window->ResizeWindow(windowSettings);
	}

	_SM->Update(deltaTime);

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

	for (unsigned int i = 0; i < _spotlights.size(); i++)
	{
		XMFLOAT3 rot = _spotlights[i]->GetRotation();
		rot.y -= 15;
		_spotlights[i]->SetRotation(rot);

		XMFLOAT3 color = _spotlights[i]->GetColor();
		color.x = sin(_timer.GetGameTime() / 1000 + 100 * i);
		color.y = sin(_timer.GetGameTime() / 1000 + 100 * i + XMConvertToRadians(120));
		color.z = sin(_timer.GetGameTime() / 1000 + 100 * i + XMConvertToRadians(240));
		_spotlights[i]->SetColor(color);
	}
}

void Game::Render()
{
	_renderModule->BeginScene(0.0f, 0.5f, 0.5f, 1.0f);
	_renderModule->SetDataPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());
	/*--------------------------------------------------------- Geometry pass ------------------------------------------------------------
	Render the objects to the diffuse and normal resource views. Camera depth is also generated here.									*/

	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::GEO_PASS);

	std::vector<std::vector<GameObject*>>* gameObjects = _objectHandler->GetGameObjects();

	//TODO: store this in object handler instead of building each frame? /Jonas
	std::vector<std::vector<GameObject*>> animatedObjects;
	for (auto i : *gameObjects)
	{
		if (i.size() > 0)
		{
			RenderObject* renderObject = i.at(0)->GetRenderObject();
			if (!renderObject->_isSkinned)
			{
				_renderModule->SetDataPerObjectType(renderObject);
				int vertexBufferSize = renderObject->_mesh._vertexBufferSize;

				for (GameObject* g : i)
				{
					_renderModule->Render(g->GetMatrix(), vertexBufferSize, g->GetColorOffset());
				}
			}
			else
			{
				animatedObjects.push_back(i);
			}
		}
	}

	////TODO: Check if this works. /Jonas
	//_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::ANIM_STAGE);
	//for (unsigned int i = 0; i < animatedObjects.size(); i++)
	//{
	//	if (animatedObjects.at(i).size() > 0)
	//	{
	//		RenderObject* renderObject = animatedObjects.at(i).at(0)->GetRenderObject();

	//		_renderModule->SetDataPerObjectType(renderObject);
	//		int vertexBufferSize = renderObject->_mesh._vertexBufferSize;

	//		for (GameObject* a : animatedObjects.at(i))
	//		{
	//			_renderModule->Render(a->GetMatrix(), vertexBufferSize, a->GetColorOffset());
	//		}
	//	}
	//}

	if (_SM->GetState() == LEVELEDITSTATE)
	{
		_renderModule->SetShaderStage(Renderer::RenderModule::GRID_STAGE);
		_renderModule->SetDataPerLineList(_grid->GetLineBuffer(), _grid->GetVertexSize());

		std::vector<DirectX::XMMATRIX>* gridMatrices = _grid->GetGridMatrices();
		for (auto &matrix : *gridMatrices)
		{
			_renderModule->RenderLineList(&matrix, _grid->GetNrOfPoints(), _grid->GetColorOffset());
		}
	}

	/*------------------------------------------------------------ Light pass --------------------------------------------------------------
	Generate the shadow map for each spotlight, then apply the lighting/shadowing to the backbuffer render target with additive blending. */

	//_renderModule->RenderLightVolume(_spotlights[0]->GetVolumeBuffer(), _spotlights[0]->GetWorldMatrix(), _spotlights[0]->GetVertexCount(), _spotlights[0]->GetVertexSize());

	_renderModule->SetLightDataPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());

	//TODO: Get vector of objects in light to use instead of gameObjects. Or use a vector (one for each light) of vectors (one for each object type) of vectors (he objects itself. 
	//May be ugly, but this way you have more flexibility of which objects should be rendered. For example, just render the enemies found in these vectors, since they are in light.
	for (auto spot : _spotlights)
	{
		_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::SHADOW_GENERATION);
		_renderModule->SetShadowMapDataPerSpotlight(spot->GetViewMatrix(), spot->GetProjectionMatrix());

		for (auto i : *gameObjects)
		{
			if (i.size() > 0)
			{
				RenderObject* renderObject = i.at(0)->GetRenderObject();
				_renderModule->SetShadowMapDataPerObjectType(renderObject);
				int vertexBufferSize = renderObject->_mesh._vertexBufferSize;

				for (GameObject* g : i)
				{
					_renderModule->RenderShadowMap(g->GetMatrix(), vertexBufferSize);
				}
			}
		}

		_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::LIGHT_APPLICATION);
		_renderModule->SetLightDataPerSpotlight(spot);

		_renderModule->RenderLightVolume(spot->GetVolumeBuffer(), spot->GetWorldMatrix(), spot->GetVertexCount(), spot->GetVertexSize());
	}

	/*-------------------------------------------------------- HUD and other 2D -----------------------------------------------------------*/
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
					Update(_timer.GetFrameTime());
				}
				Render();
				string s = to_string(_timer.GetFrameTime()) + " " + to_string(_timer.GetFPS());

				SetWindowText(_window->GetHWND(), s.c_str());

				_timer.Reset();
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