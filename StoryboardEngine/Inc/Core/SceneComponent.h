#pragma once
#include "pch.h"
#include "Core/SerializableObject.h"
#include "Utils/SceneReference.h"

namespace StoryboardEngine
{
	class SceneObject;
	class SceneTransform;

	class SceneComponent : public SerializableObject
	{
	private:
		bool isActive = true;
		bool isEnabled = true;
		std::size_t componentHash = 0; // Purely for getting the name of the component & for sorting the inspector
		SceneReference<SceneObject> parentObject;


		friend class ComponentRegistry; // To set the component hash
	public:
		SceneComponent() = default;

		virtual ~SceneComponent() = 0;
		void Initialize(SceneReference<SceneObject> parentObject); // Should only be callable once
		// Called only once, immediately after first construction. Will not be called again on scene load, etc.
		virtual void OnConstructed();
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
		// Called when inspecting this component if the linked sceneObject is selected in editor
		virtual void OnDrawInspector();
		// Called on top of OnDrawInspector() if debug view is enabled
		virtual void OnDrawDebugInspector();
		// Should only be called by the attached object or self, ToDo: Make this use a friend class?
		// Sets the active state of this component
		// Is active if both parent is active, and if this component is enabled, otherwise inactive
		virtual void UpdateState(bool parentState);
		// Sets if this component is enabled or not
		virtual void SetEnabled(bool enabled);
		size_t GetComponentHash() const;
		// If this component is enabled regardless of whether the scene object is also active
		bool IsEnabled() const;
		// If the parent is active and this is enabled
		bool IsActive() const;
		SceneReference<SceneComponent> GetSelfReference();
		SceneReference<SceneObject> GetSceneObject() const;
		SceneReference<SceneTransform> GetTransform() const;
		std::string GetSerializedType() const override;
		void DestroyObject(SceneReference<SerializableObject> objToDestroy) const;

		ADD_SERIALIZATION(SerializableObject, StoryboardEngine::SceneComponent,
			JSON_VARIABLE(isEnabled), JSON_VARIABLE(parentObject))
	};
}
