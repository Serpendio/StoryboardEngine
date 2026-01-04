#include "pch.h"
#include "Utils/SceneManager.h"
#include "Core/Scene.h"
#include "Internal/EditorLayer.h"
#include "Utils/ApplicationUtils.h"
#include "Internal/TimeManager.h"
#include "Internal/Physics3D.h"

std::shared_ptr<StoryboardEngine::Scene> StoryboardEngine::SceneManager::s_currentScene = nullptr;
std::string StoryboardEngine::SceneManager::s_sceneToLoad;
#ifdef _EDITOR
std::unique_ptr<StoryboardEngine::EditorLayer> StoryboardEngine::SceneManager::s_editorLayer = nullptr;
#endif
std::unordered_map<std::string, std::string> StoryboardEngine::SceneManager::s_scenes;

void StoryboardEngine::SceneManager::LoadScene(std::string sceneName)
{
	if (!ApplicationUtils::IsPlaying())
	{
		// Stops the user from loading scenes outside of play mode (without using the scene switcher in the editor)
		return;
	}

	auto it = s_scenes.find(sceneName.data());
	if (it == s_scenes.end())
	{
		Logger::LogWarning("Scene not found: ", sceneName);
		return;
	}

	s_sceneToLoad = sceneName;
}

void StoryboardEngine::SceneManager::Shutdown()
{
	if (s_currentScene)
	{
		s_currentScene = nullptr;
	}

#ifdef _EDITOR
	if (s_editorLayer)
	{
		s_editorLayer = nullptr;
	}
#endif
}

void StoryboardEngine::SceneManager::LoadSceneEditor(std::string sceneName)
{
	auto it = s_scenes.find(sceneName.data());
	if (it == s_scenes.end())
	{
		Logger::LogWarning("Scene not found: ", sceneName);
		return;
	}

	s_sceneToLoad = sceneName;
}

void StoryboardEngine::SceneManager::Initialize()
{
	s_currentScene = nullptr;
	s_sceneToLoad.clear();

#ifdef _EDITOR
	s_editorLayer = std::make_unique<EditorLayer>();
#endif
}

void StoryboardEngine::SceneManager::UpdateScene()
{
	if (!s_sceneToLoad.empty())
	{
		if (s_currentScene)
		{
			s_currentScene = nullptr;
		}

		// ToDo: Render temporary "LOADING" or smthing

		s_currentScene = std::make_shared<Scene>(s_scenes[s_sceneToLoad]);
#ifdef _EDITOR
		s_editorLayer->SetScene(s_scenes[s_sceneToLoad], s_sceneToLoad, s_currentScene);
#endif
		s_sceneToLoad.clear();

		// Reset the delta time to avoid large jumps
		TimeManager::Frame();
		Physics3D::GetInstance()->ResetTimeAccumulator();
	}

	if (s_currentScene)
	{
#ifdef _EDITOR
		s_editorLayer->Update();
#else
		s_currentScene->Update();
#endif
	}
}

void StoryboardEngine::SceneManager::RenderScene(ID3D11DeviceContext* deviceContext)
{
	if (s_currentScene)
	{
#ifdef _EDITOR
		s_editorLayer->Render(deviceContext);
#else
		s_currentScene->Render(deviceContext);
#endif
	}
}

void StoryboardEngine::SceneManager::RenderSceneGUI()
{
	if (s_currentScene)
	{
#ifdef _EDITOR
		s_editorLayer->RenderGUI();
#else
		s_currentScene->RenderGUI();
#endif
	}
}

void StoryboardEngine::SceneManager::EndFrame()
{
	if (s_currentScene)
	{
		s_currentScene->EndFrame();
	}
}

void StoryboardEngine::SceneManager::RegisterScene(const std::string& sceneName, const std::string& filePath)
{
	s_scenes[sceneName] = filePath;
}

void StoryboardEngine::SceneManager::LoadInitialScene(std::string initialScene)
{
	if (s_scenes.empty())
	{
		Logger::LogWarning("No scenes registered to load.");
		return;
	}

	if (initialScene.empty())
	{
		Logger::LogWarning("No initial scene specified.");

		// Load the first registered scene
		auto it = s_scenes.begin();
		s_sceneToLoad = it->first;

		return;
	}

	if (!s_scenes.contains(initialScene))
	{
		Logger::LogWarning("Initial scene not found: ", initialScene);

		// Load the first registered scene
		auto it = s_scenes.begin();
		s_sceneToLoad = it->first;

		return;
	}

	s_sceneToLoad = initialScene;
}
