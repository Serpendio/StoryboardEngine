#include "pch.h"
#include "Core/SceneComponent.h"
#include <imgui.h>
#include "Core/SceneObject.h"
#include "Core/SceneTransform.h"
#include "Core/Scene.h"
#include "Core/ComponentRegistry.h"
#include "Utils/ApplicationUtils.h"


void StoryboardEngine::SceneComponent::Initialize(SceneReference<SceneObject> attachedToObject)
{
	// ToDo: This has been manually ported to relevant locations, perhaps turn back into a function somewhere
	// ToDo2: This should be a function again, check it works

	this->parentObject = attachedToObject;

	if (!attachedToObject)
	{
		//throw std::invalid_argument("Parent cannot be null");
		Logger::LogWarning("SceneComponent of type ", GetSerializedType(), " with UUID ", GetUUID().ToString(), " initialized with null parent SceneObject");
		return;
	}

	UpdateState(attachedToObject->IsActive());
}

StoryboardEngine::SceneComponent::~SceneComponent() {} // https://stackoverflow.com/a/14631710 - Keeps SceneComponent from being instantiatable but also solve the issue of the destructor needing to be defined

// ToDo: Do I call all of these?
void StoryboardEngine::SceneComponent::OnAwake() {}
void StoryboardEngine::SceneComponent::OnAwakeEditor() {}
void StoryboardEngine::SceneComponent::OnStart() {}
void StoryboardEngine::SceneComponent::OnUpdate() {}
void StoryboardEngine::SceneComponent::OnFixedUpdate() {}
void StoryboardEngine::SceneComponent::OnCollisionEnter(SceneReference<SceneObject> other) {}
void StoryboardEngine::SceneComponent::OnCollisionStay(SceneReference<SceneObject> other) {}
void StoryboardEngine::SceneComponent::OnCollisionExit(SceneReference<SceneObject> other) {}
void StoryboardEngine::SceneComponent::OnTriggerEnter(SceneReference<SceneObject> other) {}
void StoryboardEngine::SceneComponent::OnTriggerStay(SceneReference<SceneObject> other) {}
void StoryboardEngine::SceneComponent::OnTriggerExit(SceneReference<SceneObject> other) {}
void StoryboardEngine::SceneComponent::OnDrawGizmos() {}
void StoryboardEngine::SceneComponent::OnDrawGizmosSelected() {}
void StoryboardEngine::SceneComponent::OnDrawInspector() {}

void StoryboardEngine::SceneComponent::OnDrawDebugInspector()
{
	ImGui::TextDisabled("UUID: %llu", GetUUID().ToString());
}

void StoryboardEngine::SceneComponent::UpdateState(bool newState)
{
	isActive = newState && isEnabled;
}

void StoryboardEngine::SceneComponent::SetEnabled(bool enabled)
{
	isEnabled = enabled;

	bool parentState = GetSceneObject()->IsActive();

	if (!parentState) return; // saves from running potentially costly code

	UpdateState(enabled);
}

size_t StoryboardEngine::SceneComponent::GetComponentHash() const
{
	return componentHash;
}

bool StoryboardEngine::SceneComponent::IsEnabled() const
{
	return isEnabled;
}

bool StoryboardEngine::SceneComponent::IsActive() const
{
	return isActive;
}

StoryboardEngine::SceneReference<StoryboardEngine::SceneComponent> StoryboardEngine::SceneComponent::GetSelfReference()
{
	return SceneReference<SerializableObject>(shared_from_this()).As<SceneComponent>();
}

StoryboardEngine::SceneReference<StoryboardEngine::SceneObject> StoryboardEngine::SceneComponent::GetSceneObject() const
{
	return parentObject;
}

StoryboardEngine::SceneReference<StoryboardEngine::SceneTransform> StoryboardEngine::SceneComponent::GetTransform() const
{
	return GetSceneObject()->GetTransform();
}

std::string StoryboardEngine::SceneComponent::GetSerializedType() const
{
	return ComponentRegistry::GetComponentName(componentHash);
}