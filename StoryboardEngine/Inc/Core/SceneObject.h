#pragma once
#include "pch.h"
#include "Core/SerializableObject.h"
#include "Core/SceneComponent.h"
#include "Core/SceneTransform.h"
#include "Utils/SceneReference.h"
#include "Core/ComponentRegistry.h"
#include "Utils/HashedVector.h"

namespace StoryboardEngine
{
	class Scene;

	class SceneObject final : public SerializableObject
	{
		Scene* attachedScene;
		HashedVector<UUID, SceneReference<SceneObject>> children;
		HashedVector<size_t, SceneReference<SceneComponent>> components{};
	public:
		std::string name = "Scene Object";
	public:
		SceneObject() : SerializableObject(), attachedScene(nullptr), transform(nullptr), parent(nullptr) {}
		SceneObject(Scene* scene, std::string name, SceneReference<SceneObject> parent = SceneReference<SceneObject>());
		~SceneObject();
		std::string GetSerializedType() const override;
		Matrix GetTransformMatrix();
		void OnBeginSave() override;
		void OnFinishedLoading() override;
		SceneReference<SceneObject> AddChild();
		SceneReference<SceneObject> GetChild(size_t index) const;
		size_t GetChildCount() const;
		void DestroyChild(UUID childID);
		SceneReference<SceneTransform> GetTransform() const;
		SceneReference<SceneObject> GetParent() const;
		Scene* GetScene() const;
		void SetActive(bool activeState);
		void UpdateState(bool activeState);
		bool IsActive() const;
		int ComponentsCount();
		SceneReference<SceneComponent> GetComponentByHash(size_t componentHash, bool includeChildren = false) const;
		
		// Called at the end of the frame if the object has been marked for destruction
		void OnDestroy() override;
		// Called immediately after construction when in play
		virtual void OnAwake();
		// Called immediately after construction when in editor
		virtual void OnAwakeEditor();
		// Called at the start of the first frame after construction during play
		virtual void OnStart();
		// Called every frame
		virtual void OnUpdate();
		// Called in fixed timesteps to align with physics update
		virtual void OnFixedUpdate();
		// Called at the end of the physics update if an object just began colliding with this object's collider
		virtual void OnCollisionEnter(SceneReference<SceneObject> other);
		// Called at the end of the physics update if an object continued colliding with this object's collider
		virtual void OnCollisionStay(SceneReference<SceneObject> other);
		// Called at the end of the physics update if an object just finished colliding with this object's collider
		virtual void OnCollisionExit(SceneReference<SceneObject> other);
		// Called at the end of the physics update if an object just entered this object's trigger
		virtual void OnTriggerEnter(SceneReference<SceneObject> other);
		// Called at the end of the physics update if an object stayed inside this object's trigger
		virtual void OnTriggerStay(SceneReference<SceneObject> other);
		// Called at the end of the physics update if an object just exited this object's trigger
		virtual void OnTriggerExit(SceneReference<SceneObject> other);
		// Called in editor post render
		virtual void OnDrawGizmos();
		// Called in editor post render if the object is selected
		virtual void OnDrawGizmosSelected();

		// ToDo: Construct child as a copy of another child (Prefab instantiation)
		/* template<typename T>
		SceneReference<T> AddChildAsCopy(SceneReference<T> otherChild)
		{
		// if T is a SceneObject construct exact copy and return
		// if T is a Component, construct its parent as a copy and return the component reference
		}*/
		
		// Adds a component of type T to the object. Returns a reference to the new component, or the existing one if it already exists
		template<typename T>
		SceneReference<T> AddComponent()
		{
			static_assert(std::is_base_of<SceneComponent, T>::value, "Must be called with base type SceneComponent");
			static_assert(!std::is_same<SceneTransform, T>::value, "Cannot add transform component to object");
			static_assert(!std::is_abstract<T>::value, "Trying to add an abstract component");

			size_t compHash = ComponentRegistry::GetTypeHash<T>();
			if (auto it = components.find(compHash); it != components.end())
			{
				Logger::LogWarning("Component of type ", ComponentRegistry::GetTypeName<T>(), " already exists on object");
				return SceneReference<T>(*it);
			}

			std::shared_ptr<T> comp = ComponentRegistry::ConstructComponent<T>(SceneReference<SerializableObject>(shared_from_this()));
			LinkComponent(comp, compHash);
			return components.Get(compHash);
		}

		SceneReference<SceneComponent> AddComponent(size_t componentHash);

		// If a component of type T exists on the object, it is removed
		template<typename T>
		void RemoveComponent()
		{
			static_assert(std::is_base_of<SceneComponent, T>::value, "Must be called with base type SceneComponent");
			static_assert(!std::is_same<StoryboardEngine::SceneTransform, T>::value, "Cannot remove transform component from object");
			static_assert(!std::is_abstract<T>::value, "Trying to remove an abstract component");

			auto compHash = ComponentRegistry::GetTypeHash<T>();
			// ToDo: Implement OnDestroy and delayed destruction
			if (auto it = components.find(compHash); it != components.end())
			{
				// Remove from both the components list and the global ID map
				SerializableObject::idToObject.erase((*it)->GetUUID());
				components.Remove(compHash);
			}
		}

		void RemoveComponent(size_t componentHash);

		// Gets a component of type T from the object. If includeChildren is true, will also search children recursively. Returns an empty reference if not found
		template<class T>
		SceneReference<T> GetComponent(bool includeChildren = false) const
		{
			static_assert(std::is_base_of<SceneComponent, T>::value, "Must be called with base type SceneComponent");
			static_assert(!std::is_abstract<T>::value, "Trying to get an abstract component");
			
			if (auto it = components.find(ComponentRegistry::GetTypeHash<T>()); it != components.end())
			{
				return SceneReference<SceneComponent>(*it).As<T>();
			}

			if (includeChildren)
			{
				for (auto& child : children)
				{
					auto c = child->GetComponent<T>();
					if (c)
					{
						return c;
					}
				}
			}

			return SceneReference<T>();
		}

		// Gets all components of type T from the object and its children recursively. Returns an empty vector if none found
		template<class T>
		inline std::vector<SceneReference<T>> GetComponents() const
		{
			static_assert(std::is_base_of<SceneComponent, T>::value, "Must be called with base type SceneComponent");
			static_assert(!std::is_abstract<T>::value, "Trying to get an abstract component");

			std::vector<SceneReference<T>> result;

			if (auto it = components.find(ComponentRegistry::GetTypeHash<T>()); it != components.end())
			{
				result.push_back(it->As<T>());
			}

			for (auto& childIt : children)
			{
				auto c = childIt->GetComponents<T>();
				if (!c.empty())
				{
					result.insert(result.end(), c.begin(), c.end());
				}
			}

			return result;
		}

		ADD_SERIALIZATION(SerializableObject, SceneObject,
			JSON_VARIABLE(name), JSON_VARIABLE(isActive), JSON_VARIABLE(parent), JSON_VARIABLE(children), JSON_VARIABLE(components)
		)

		// ToDo: Support other json types

	private:
		void LinkChild(std::shared_ptr<SceneObject> child);
		void LinkComponent(std::shared_ptr<SceneComponent> component, size_t componentHash);

		void DestroyChildrenRecursively();
	private:
		bool isActive = true, parentActive = true;
		SceneReference<SceneTransform> transform; // ToDo: Thoughts: Does heirarchy get stored in transform instead in unity due to performance reasons?
		SceneReference<SceneObject> parent;

		void DrawInspector(bool debug = false);
		// Initializes the scene object with its parent and attached scene, also links the transform reference
		void Initialize(SceneReference<SceneObject> parent, Scene* scene);

		friend class Scene;
		friend class EditorLayer;
	};
}

