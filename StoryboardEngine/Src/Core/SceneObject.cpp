#include "pch.h"
#include "Core/SceneObject.h"
#include <imgui.h>
#include "Core/Scene.h"
#include "Core/ComponentRegistry.h"
#include "Utils/ApplicationUtils.h"

StoryboardEngine::SceneObject::SceneObject(Scene* scene, std::string name, SceneReference<SceneObject> parent)
	: SerializableObject(), name(name)
{
	this->Initialize(parent, scene);
}

StoryboardEngine::SceneObject::~SceneObject()
{
	children.Clear();

	components.Clear();
}

std::string StoryboardEngine::SceneObject::GetSerializedType() const
{
	return std::string(NAMEOF_SHORT_TYPE(SceneObject));
}

// ToDo: This should be cached for performance
Matrix StoryboardEngine::SceneObject::GetTransformMatrix()
{
	return parent ? parent->GetTransformMatrix() * transform->GetMatrix() : transform->GetMatrix();
}

void StoryboardEngine::SceneObject::DrawInspector(bool debug)
{
	auto activeState = isActive;
	ImGui::Checkbox("Is Active", &activeState);
	SetActive(activeState);

	// Sort components so that transform is always first, and the rest are hash ordered
	std::vector<StoryboardEngine::SceneReference<SceneComponent>> sortedComponents;
	for (const auto& component : components)
	{
		sortedComponents.push_back(component);
	}
	std::sort(sortedComponents.begin(), sortedComponents.end(), [](const StoryboardEngine::SceneReference<SceneComponent>& a, const StoryboardEngine::SceneReference<SceneComponent>& b)
	{
		if (a->GetComponentHash() == ComponentRegistry::GetTypeHash<SceneTransform>()) return true;
		if (b->GetComponentHash() == ComponentRegistry::GetTypeHash<SceneTransform>()) return false;
		return a->GetComponentHash() < b->GetComponentHash();
	});

	for (int i = 0; i < sortedComponents.size(); i++)
	{
		ImGui::PushID(static_cast<int>(sortedComponents[i]->GetComponentHash()));
		if (ImGui::CollapsingHeader(sortedComponents[i]->GetSerializedType().c_str()))
		{
			// Don't allow removing the transform component
			if (sortedComponents[i]->GetComponentHash() != ComponentRegistry::GetTypeHash<SceneTransform>())
			{
				// Remove button on the right side
				//ImGui::SameLine(ImGui::GetWindowWidth() - 40);
				if (ImGui::Button("Remove Component"))
				{
					RemoveComponent(sortedComponents[i]->GetComponentHash());
					ImGui::PopID();
					continue;
				}
			}

			sortedComponents[i]->OnDrawInspector();
			if (debug)
				sortedComponents[i]->OnDrawDebugInspector();
		}
		ImGui::PopID();
	}
}

void StoryboardEngine::SceneObject::Initialize(SceneReference<SceneObject> parent, Scene* scene)
{
	// Parent has been set already in deserialization
	this->parent = parent;

	if (parent)
		parentActive = parent->IsActive();

	transform = GetComponent<StoryboardEngine::SceneTransform>();
	this->attachedScene = scene;
}

void StoryboardEngine::SceneObject::OnBeginSave()
{
	for (auto& comp : components)
	{
		comp->OnBeginSave();
	}

	for (auto& child : children)
	{
		child->OnBeginSave();
	}
}

void StoryboardEngine::SceneObject::OnFinishedLoading()
{
	for (auto& comp : components)
	{
		comp->Initialize(SceneReference<SerializableObject>(shared_from_this()));
		comp->OnFinishedLoading();
	}

	for (auto& child : children)
	{
		child->Initialize(SceneReference<SerializableObject>(shared_from_this()), attachedScene);
		child->OnFinishedLoading();
	}
}

StoryboardEngine::SceneReference<StoryboardEngine::SceneObject> StoryboardEngine::SceneObject::AddChild()
{
	// Create the child object
	std::shared_ptr<SceneObject> child = std::make_shared<SceneObject>(attachedScene, "Scene Object", SceneReference<SerializableObject>(shared_from_this()));
	LinkChild(child);

	// Create and register the child's transform component
	std::shared_ptr<SceneTransform> tf = ComponentRegistry::ConstructComponent<SceneTransform>();
	tf->Initialize(child);
	SerializableObject::idToObject[tf->GetUUID()] = tf;

	// Add the transform component to the child's component list
	child->components.Add(ComponentRegistry::GetTypeHash<SceneTransform>(), StoryboardEngine::SceneReference<SceneTransform>(tf));
	
	child->Initialize(SceneReference<SerializableObject>(shared_from_this()), attachedScene);

	return StoryboardEngine::SceneReference<SceneObject>(child);
}

void StoryboardEngine::SceneObject::LinkChild(std::shared_ptr<SceneObject> child)
{
	// Register the child object in the global ID map and add it to this object's children
	SerializableObject::idToObject[child->GetUUID()] = child;
	children.Add(child->GetUUID(), StoryboardEngine::SceneReference<SceneObject>(child)); // explicit, but would work implicitly too
}

void StoryboardEngine::SceneObject::LinkComponent(std::shared_ptr<SceneComponent> component, size_t componentHash)
{
	SerializableObject::idToObject[component->GetUUID()] = component;
	auto& comp = components.Add(componentHash, SceneReference<SceneComponent>(component));
	comp->OnConstructed();
	GetScene()->RegisterNewComponent(comp);

#ifdef _EDITOR
	if (ApplicationUtils::IsPlaying()) // ToDo: This should be called by the scene when entering play mode / the attached object immediately after construction // ToDoToDo maybe it is anyway? check the deserialisation
	{
		component->OnAwake();
	}
	else
	{
		component->OnAwakeEditor();
	}
#else
	component->OnAwake();
#endif
}

void StoryboardEngine::SceneObject::DestroyChildrenRecursively()
{
	for (auto& child : children)
	{
		child->DestroyChildrenRecursively();
		SerializableObject::idToObject.erase(child->GetUUID());
	}
	children.Clear();

	for (auto& comp : components)
	{
		comp->OnDestroy();
		SerializableObject::idToObject.erase(comp->GetUUID());
	}
	components.Clear();
}

StoryboardEngine::SceneReference<StoryboardEngine::SceneObject> StoryboardEngine::SceneObject::GetChild(size_t index) const
{
	return children[index];
}

size_t StoryboardEngine::SceneObject::GetChildCount() const
{
	return children.Size();
}

void StoryboardEngine::SceneObject::DestroyChild(StoryboardEngine::UUID childUUID)
{
	if (auto it = children.find(childUUID); it != children.end())
	{
		(*it)->DestroyChildrenRecursively();
		SerializableObject::idToObject.erase(childUUID);
		children.Remove(childUUID);
	}
}

StoryboardEngine::SceneReference<StoryboardEngine::SceneTransform> StoryboardEngine::SceneObject::GetTransform() const
{
	return transform;
}

StoryboardEngine::SceneReference<StoryboardEngine::SceneObject> StoryboardEngine::SceneObject::GetParent() const
{
	return parent;
}

StoryboardEngine::Scene* StoryboardEngine::SceneObject::GetScene() const
{
	return attachedScene;
}

void StoryboardEngine::SceneObject::SetActive(bool activeState)
{
	if (isActive == activeState) return;

	isActive = activeState;

	for (auto& component : components)
	{
		component->UpdateState(activeState);
	}

	for (auto& child : children)
	{
		child->UpdateState(activeState);
	}
}

void StoryboardEngine::SceneObject::UpdateState(bool activeState)
{
	parentActive = activeState;

	if (!isActive) return;	 // if false, don't need to do anything, children & components are already inactive

	for (auto& component : components)
	{
		component->UpdateState(activeState);
	}

	for (auto& child : children)
	{
		child->UpdateState(parentActive);
	}
}

bool StoryboardEngine::SceneObject::IsActive() const
{
	return isActive && parentActive;
}

int StoryboardEngine::SceneObject::ComponentsCount()
{
	return static_cast<int>(components.Size());
}

StoryboardEngine::SceneReference<StoryboardEngine::SceneComponent> StoryboardEngine::SceneObject::GetComponentByHash(size_t componentHash, bool includeChildren) const
{
	if (auto it = components.find(componentHash); it != components.end())
	{
		return SceneReference<SceneComponent>(*it);
	}

	if (includeChildren)
	{
		for (auto& child : children)
		{
			auto comp = child->GetComponentByHash(componentHash, true);
			if (comp)
			{
				return comp;
			}
		}
	}

	return SceneReference<SceneComponent>();
}



// ToDo: Do I call all of these?
void StoryboardEngine::SceneObject::OnDestroy()
{
	for (auto& component : components)
	{
		component->OnDestroy();
	}

	for (auto& child : children)
	{
		child->OnDestroy();
	}
}

void StoryboardEngine::SceneObject::OnAwake()
{
	for (auto& component : components)
	{
		component->OnAwake();
	}

	for (auto& child : children)
	{
		child->OnAwake();
	}
}
void StoryboardEngine::SceneObject::OnAwakeEditor()
{
	for (auto& component : components)
	{
		component->OnAwakeEditor();
	}

	for (auto& child : children)
	{
		child->OnAwakeEditor();
	}
}
void StoryboardEngine::SceneObject::OnStart()
{
	// ToDo: Start should probably be delayed if the item is not enabled/active

	for (auto& component : components)
	{
		component->OnStart();
	}

	for (auto& child : children)
	{
		child->OnStart();
	}
}

void StoryboardEngine::SceneObject::OnUpdate()
{
	transform->ClearChangedThisFrame();

	for (auto& component : components)
	{
		if (!component->IsEnabled()) continue;

		component->OnUpdate();
	}

	for (auto& child : children)
	{
		if (!child->isActive) continue;

		child->OnUpdate();
	}
}

void StoryboardEngine::SceneObject::OnFixedUpdate()
{
	for (auto& component : components)
	{
		if (!component->IsEnabled()) continue;

		component->OnFixedUpdate();
	}

	for (auto& child : children)
	{
		if (!child->isActive) continue;

		child->OnFixedUpdate();
	}
}
void StoryboardEngine::SceneObject::OnCollisionEnter(SceneReference<SceneObject> other)
{
	for (auto& component : components)
	{
		component->OnCollisionEnter(other);
	}

	for (auto& child : children)
	{
		child->OnCollisionEnter(other);
	}
}
void StoryboardEngine::SceneObject::OnCollisionStay(SceneReference<SceneObject> other)
{
	for (auto& component : components)
	{
		component->OnCollisionStay(other);
	}

	for (auto& child : children)
	{
		child->OnCollisionStay(other);
	}
}
void StoryboardEngine::SceneObject::OnCollisionExit(SceneReference<SceneObject> other)
{
	for (auto& component : components)
	{
		component->OnCollisionExit(other);
	}

	for (auto& child : children)
	{
		child->OnCollisionExit(other);
	}
}
void StoryboardEngine::SceneObject::OnTriggerEnter(SceneReference<SceneObject> other)
{
	for (auto& component : components)
	{
		component->OnTriggerEnter(other);
	}

	for (auto& child : children)
	{
		child->OnTriggerEnter(other);
	}
}
void StoryboardEngine::SceneObject::OnTriggerStay(SceneReference<SceneObject> other)
{
	for (auto& component : components)
	{
		component->OnTriggerStay(other);
	}

	for (auto& child : children)
	{
		child->OnTriggerStay(other);
	}
}
void StoryboardEngine::SceneObject::OnTriggerExit(SceneReference<SceneObject> other)
{
	for (auto& component : components)
	{
		component->OnTriggerExit(other);
	}

	for (auto& child : children)
	{
		child->OnTriggerExit(other);
	}
}

void StoryboardEngine::SceneObject::OnDrawGizmos()
{
	for (auto& component : components)
	{
		component->OnDestroy();
	}

	for (auto& child : children)
	{
		child->OnDestroy();
	}
}

void StoryboardEngine::SceneObject::OnDrawGizmosSelected()
{
	for (auto& component : components)
	{
		component->OnDestroy();
	}

	for (auto& child : children)
	{
		child->OnDestroy();
	}
}



StoryboardEngine::SceneReference<StoryboardEngine::SceneComponent> StoryboardEngine::SceneObject::AddComponent(size_t componentHash)
{
	if (auto it = components.find(componentHash); it != components.end())
	{
		Logger::LogWarning("Component of type ", ComponentRegistry::GetComponentName(componentHash), " already exists on object");
		return *it;
	}

	std::shared_ptr<SceneComponent> comp = ComponentRegistry::ConstructComponent(componentHash);
	comp->Initialize(SceneReference<SerializableObject>(shared_from_this()));
	LinkComponent(comp, componentHash);

	SceneReference<SceneComponent>&& compToReturn = nullptr; // ToDo: Yeah... I don't know about this
	components.Get(componentHash, compToReturn);

	return compToReturn;
}

void StoryboardEngine::SceneObject::RemoveComponent(size_t componentHash)
{
	if (auto it = components.find(componentHash); it != components.end())
	{
		SerializableObject::idToObject.erase((*it)->GetUUID());
		components.Remove(componentHash);
	}
}
