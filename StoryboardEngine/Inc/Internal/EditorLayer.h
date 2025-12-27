#pragma once
#include "Utils/SceneReference.h"

struct ID3D11DeviceContext;

namespace StoryboardEngine
{
	class SceneObject;
	class Scene;
	class SceneComponent;

	class EditorLayer
	{
	private:
		SceneReference<SceneObject> selectedInHeirarchy;

		void CreateHeirarchy(SceneReference<SceneObject> so);

		std::weak_ptr<Scene> currentScene;
		// The name of the scene currently being seen (whether in edit or play mode)
		std::string currentSceneName;
		// The name of the scene being edited (only when not in play mode, saved so we can restore it after play mode ends)
		std::string editingSceneName;
		// The file path of the scene being edited (so we can save to it)
		std::filesystem::path sceneFilePath;
		// The component the user wants to add to the selected object
		std::size_t componentToAdd;
		std::string componentToAddName;
		bool debugMode = false;
		bool renderScenePOV = false;

		std::shared_ptr<SceneObject> sceneCamera;
		std::vector<std::shared_ptr<SceneComponent>> sceneComponents; // We don't need to use the global manager for editor-only components
		
		void SaveScene();
	public:
		void Update();
		void Render(ID3D11DeviceContext* deviceContext);
		void RenderGUI();

		EditorLayer();
		~EditorLayer();
		void SetScene(std::filesystem::path filepath, std::string name, std::weak_ptr<Scene> scene);
	};
}
