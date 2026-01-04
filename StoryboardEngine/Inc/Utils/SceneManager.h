#pragma once
#include <string>
#include <unordered_map>

struct ID3D11DeviceContext;

namespace StoryboardEngine
{
	class Scene;
	class EditorLayer;

	// ToDo: This doesn't really need to be static, just the loadScene function and sceneToLoad variable
	// Therefore probably doesn't need friendship with ApplicationCore either

	class SceneManager
	{
	public:
		static void LoadScene(std::string sceneName);

	protected:
		static void LoadSceneEditor(std::string sceneName);
		static void Initialize();
		static void Shutdown();

		static void UpdateScene();
		static void RenderScene(ID3D11DeviceContext* deviceContext);
		static void RenderSceneGUI();
		static void EndFrame();

		static void RegisterScene(const std::string& sceneName, const std::string& filePath);
		static void LoadInitialScene(std::string initialSceneName);

		friend class ApplicationCore;
		friend class EditorLayer;
	private:
		SceneManager() = delete;
		
		static std::shared_ptr<Scene> s_currentScene;
		static std::string s_sceneToLoad;
		static std::unordered_map<std::string, std::string> s_scenes;

#ifdef _EDITOR
		static std::unique_ptr<EditorLayer> s_editorLayer;
#endif
	};
}
