#include "pch.h"
#include "Internal/ApplicationCore.h"
#include <SDL3/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_dx11.h"
#include "Internal/D3DRenderer.h"
#include "Internal/InputManager.h"
#include "Internal/TimeManager.h"
#include "Utils/SceneManager.h"
#include "Internal/ColourShader.h"
#include "Core/ResourceManager.h"
#include "Core/Input.h"
#include "Core/ComponentRegistry.h"
#include "Core/UserRegistry.h"
#include "Internal/PrimativeModel.h"

// Include default components
#include "Core/SceneTransform.h"
#include "Core/FreeCamController.h"
#include "Core/CameraComponent.h"
#include "Core/SerializableObject.h"
#include "Core/ModelRenderer.h"

StoryboardEngine::ApplicationCore* StoryboardEngine::ApplicationCore::s_instance = nullptr;

StoryboardEngine::ApplicationCore::ApplicationCore()
{
	m_Window = nullptr;
	m_HWND = nullptr;
	m_Direct3D = nullptr;
	m_isFullscreen = false;
}

StoryboardEngine::ApplicationCore::~ApplicationCore()
{
}

bool StoryboardEngine::ApplicationCore::Initialize()
{
	if (s_instance)
	{
		Logger::LogError("Error: ApplicationCore instance already exists!");
		return false;
	}

	s_instance = this;

	int windowWidth = 800;
	int windowHeight = 600;

	// 2. Create an SDL3 Window
	m_Window = SDL_CreateWindow(
		"SDL3 D3D11 Window",
		windowWidth,
		windowHeight,
		SDL_WINDOW_RESIZABLE // Allow resizing if you want to handle it
	);

	if (m_Window == nullptr)
	{
		Logger::LogError("Window could not be created! SDL_Error: ", SDL_GetError());
		SDL_Quit();
		return false;
	}

	SDL_PropertiesID props = SDL_GetWindowProperties(m_Window);
	// In SDL3, use SDL_GetPointerProperty for platform-specific handles
	m_HWND = (HWND)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);

	if (m_HWND == nullptr)
	{
		Logger::LogError("Failed to get HWND from SDL window! SDL_Error: ", SDL_GetError());
		SDL_DestroyWindow(m_Window);
		SDL_Quit();
		return false;
	}

	bool result;

	// Create and initialize the input object.  This object will be used to handle reading the keyboard input from the user.
	InputManager::Initialize();

	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DRenderer();

	result = m_Direct3D->Initialize(windowWidth, windowHeight, VSYNC_ENABLED, m_HWND, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		Logger::LogError("Could not initialize Direct3D");
		SDL_DestroyWindow(m_Window);
		SDL_Quit();
		return false;
	}
	m_isFullscreen = FULL_SCREEN;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	// Setup Platform/Renderer backends
	ImGui_ImplSDL3_InitForD3D(m_Window);
	ImGui_ImplDX11_Init(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());

	ComponentRegistry::RegisterComponent<SceneTransform>();
	ComponentRegistry::RegisterComponent<FreeCamController>();
	ComponentRegistry::RegisterComponent<CameraComponent>();
	ComponentRegistry::RegisterComponent<ModelRenderer>();

	SceneManager::Initialize();

	ResourceManager::UpdateDevice(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());

	m_ColourShader = new ColourShader();
	m_ColourShader->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
	Matrix projectionMatrix;
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_ColourShader->SetProjectionMatrix(projectionMatrix);

	ResourceManager::LoadModel("Cube", PrimativeModel::MakeCube(m_Direct3D->GetDevice()), 0);
	ResourceManager::LoadModel("Capsule", PrimativeModel::MakeCapsule(m_Direct3D->GetDevice()), 1);

	{
		std::unordered_map<std::string, std::string> modelNameToPathMap;
		RegisterApplicationModels(modelNameToPathMap);
		for (const auto& pair : modelNameToPathMap)
		{
			const std::string& modelName = pair.first;
			const std::string& modelPath = pair.second;
			size_t modelID = ResourceManager::LoadModel(modelPath, modelName);
			if (modelID == -1)
			{
				Logger::LogError("Failed to load model: ", modelName, " from path: ", modelPath);
			}
		}
		ResourceManager::ReleaseImporter();
	}
	{
		std::unordered_map<std::string, std::string> textureNameToPathMap;
		RegisterApplicationTextures(textureNameToPathMap);
		for (const auto& pair : textureNameToPathMap)
		{
			const std::string& textureName = pair.first;
			const std::string& texturePath = pair.second;
			size_t textureID = ResourceManager::LoadTexture(texturePath, textureName);
			if (textureID == -1)
			{
				Logger::LogError("Failed to load texture: ", textureName, " from path: ", texturePath);
			}
		}
	}

	RegisterApplicationComponents();

	{
		std::unordered_map<std::string, std::string> sceneNameToPathMap;
		RegisterApplicationScenes(sceneNameToPathMap);
		for (const auto& pair : sceneNameToPathMap)
		{
			const std::string& sceneName = pair.first;
			const std::string& scenePath = pair.second;
			SceneManager::RegisterScene(sceneName, scenePath);
		}
	}

	return true;
}

void StoryboardEngine::ApplicationCore::Shutdown()
{
	ComponentRegistry::Shutdown();

	if (m_ColourShader)
	{
		m_ColourShader->Shutdown();
		delete m_ColourShader;
		m_ColourShader = nullptr;
	}

	// Release the resource manager.
	ResourceManager::Shutdown();

	// Release the scene manager.
	SceneManager::Shutdown();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}

	// Shutdown the input
	InputManager::Shutdown();

	// Shutdown the window.
	SDL_DestroyWindow(m_Window);
	SDL_Quit();

	if (s_instance == this)
	{
		s_instance = nullptr;
	}

	return;
}

void StoryboardEngine::ApplicationCore::Run()
{
	bool running = true;
	bool result;

	// Loop until there is a quit message from the window or the user.
	SDL_Event e;

	TimeManager::Initialize();
	
	SceneManager::LoadInitialScene();

#ifdef _EDITOR
	while (running)
#else
	while (running && ApplicationUtils::IsPlaying())
#endif
	{
		TimeManager::Frame();

		while (SDL_PollEvent(&e) != 0)
		{
			ImGui_ImplSDL3_ProcessEvent(&e);

			if (e.type == SDL_EVENT_QUIT)
			{
				running = false;
			}
			else if (e.type == SDL_EVENT_WINDOW_RESIZED || e.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
			{
				// Handle window resize: Recreate swap chain and render target view
				int windowWidth, windowHeight;
				SDL_GetWindowSizeInPixels(m_Window, &windowWidth, &windowHeight); // For drawable size in pixels
				ApplicationUtils::screenWidth = windowWidth;
				ApplicationUtils::screenHeight = windowHeight;

				// ToDo: Resize without recreating D3DRenderer
				ImGui_ImplDX11_Shutdown();
				ImGui_ImplSDL3_Shutdown();

				m_Direct3D->Shutdown();
				result = m_Direct3D->Initialize(windowWidth, windowHeight, VSYNC_ENABLED, m_HWND, m_isFullscreen, SCREEN_DEPTH, SCREEN_NEAR);
				if (!result)
				{
					return;
				}

				ImGui_ImplSDL3_InitForD3D(m_Window);
				ImGui_ImplDX11_Init(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());

				ResourceManager::UpdateDevice(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
				m_ColourShader->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
				Matrix projectionMatrix;
				m_Direct3D->GetProjectionMatrix(projectionMatrix);
				m_ColourShader->SetProjectionMatrix(projectionMatrix);
			}
		}

		InputManager::UpdateStates();


		// Otherwise do the frame processing.
		Frame();
	}

	return;
}

void StoryboardEngine::ApplicationCore::Frame()
{
	// Start the Dear ImGui frame
	// Platform new frame first, then renderer new frame, then ImGui::NewFrame
	ImGui_ImplSDL3_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();

	// Update scene logic
	SceneManager::UpdateScene();

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(.4f, .6f, 1.0f, 1.0f);

	// Render engine content (world, objects, etc.)
	SceneManager::RenderScene(m_Direct3D->GetDeviceContext());
	// Build UI
	//ImGui::ShowDemoWindow();
	SceneManager::RenderSceneGUI();

	// Finalize ImGui and get draw data
	ImGui::Render();
	// Render ImGui draw data while the D3D render target is valid
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();
}
