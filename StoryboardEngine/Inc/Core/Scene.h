#pragma once
#include "pch.h"
#include "Core/SceneComponent.h"
#include "Core/SceneObject.h"

struct ID3D11DeviceContext;

namespace StoryboardEngine
{
	class DrawableComponent;
	class CameraComponent;

	class Scene : public std::enable_shared_from_this<Scene>
	{
	public:
		Scene(const std::filesystem::path& filepath);

		Scene();
		~Scene();

		virtual void Update();
		virtual void Render(ID3D11DeviceContext* deviceContext);
		virtual void RenderGUI();
	protected:
#ifdef _EDITOR
		friend class EditorLayer;
#endif
	public:
		StoryboardEngine::SceneReference<StoryboardEngine::SceneObject> GetRoot() const;
		// Costly, use sparingly
		template<class T>
		inline SceneReference<T> GetComponent() const
		{
			static_assert(std::is_base_of<StoryboardEngine::SceneComponent, T>::value, "Must be called with base type SceneComponent");
			return root->GetComponent<T>(true);
		}
		// Costly, use sparingly
		template<class T>
		inline std::vector<SceneReference<T>> GetComponents() const 
		{
			static_assert(std::is_base_of<StoryboardEngine::SceneComponent, T>::value, "Must be called with base type SceneComponent");
			return root->GetComponents<T>();
		}

		// ToDo: These could be events
		void RegisterDrawable(StoryboardEngine::SceneReference<StoryboardEngine::DrawableComponent> drawable);
		void DeregisterDrawable(StoryboardEngine::SceneReference<StoryboardEngine::DrawableComponent> drawable);
		void RegisterNewComponent(StoryboardEngine::SceneReference<StoryboardEngine::SceneComponent> component);

		void SetCamera(StoryboardEngine::SceneReference<StoryboardEngine::CameraComponent> cameraObject);
	private:
		void SetupRootObject();
	private:

		SceneReference<StoryboardEngine::SceneObject> root;
		StoryboardEngine::SceneReference<StoryboardEngine::CameraComponent> sceneCamera;
		StoryboardEngine::HashedVector<StoryboardEngine::UUID, SceneReference<StoryboardEngine::DrawableComponent>> drawables;
		//StoryboardEngine::ModelRenderer* skybox;

		std::vector<StoryboardEngine::SceneReference<StoryboardEngine::SceneComponent>> newComponents;
		//std::vector<StoryboardEngine::SceneReference<StoryboardEngine::SceneComponent>> updatingComponents;
		std::vector<StoryboardEngine::SceneReference<StoryboardEngine::SerializableObject>> componentsToDelete; // ToDo: Case where destroy is called on the sceneobject and its component, causing OnDestroy to be called twice for the component
	};
}
