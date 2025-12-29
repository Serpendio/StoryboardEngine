#include "pch.h"
#include "Core/Scene.h"
#include <imgui.h>
#include <fstream>
#include "Utils/ApplicationUtils.h"
#include "Core/DrawableComponent.h"
#include "Core/CameraComponent.h"

StoryboardEngine::Scene::Scene(const std::filesystem::path& filepath)
{
	//root = new SceneObject(this, "Scene");

	std::ifstream i(filepath);

	if (!i.is_open())
	{
		Logger::LogInfo("File not found, creating new scene");
	}
	else
	{
		nlohmann::ordered_json j;

		try
		{
			ApplicationUtils::deserializingScene = true;

			i >> j;

			j.at("Root").get_to<SceneReference<SceneObject>>(root);

			nlohmann::ordered_json j_map = j.at("SerializableObjects").get<nlohmann::ordered_json>();

			for (auto& serObjPair : j_map)
			{
				std::string UUID_key = std::string(NAMEOF(SerializableObject::m_uuid)); // <-- ToDo: There is probably a way to get this as a constexpr?
				//std::string UUID_key = "m_uuid";
				auto& serObjData = serObjPair["data"];

				if (serObjData.is_null())
				{
					Logger::LogWarning("Warning: Corrupted object found during deserialization: No \"data\" key found"); // ToDo: Pass the full data of serObjPair for debug
				}

				std::string UUID_as_str = serObjData[UUID_key].get<std::string>(); // ToDo: Might need more checks for if a key doesn't exist

				UUID uuid = UUID::from_string(UUID_as_str);

				if (uuid.is_nil())
				{
					Logger::LogWarning("Warning: Corrupted object found during deserialization: invalid UUID"); // ToDo: Pass the full data of serObjPair for debug
					continue;
				}

				std::string serializedType = serObjPair["serialized_type"].get<std::string>();

				if (serializedType.empty())
				{
					Logger::LogWarning("Warning: Corrupted object found during deserialization: Empty serialized type for object with id ", uuid.ToString());
					continue;
				}

				std::shared_ptr<SerializableObject> serObj;

				if (serializedType == NAMEOF_SHORT_TYPE(SceneObject))
				{
					serObj = std::make_shared<SceneObject>();
				}
				else
				{
					// Try to construct component
					serObj = ComponentRegistry::ConstructComponent(ComponentRegistry::GetTypeHash(serializedType));

					if (!serObj)
					{
						Logger::LogWarning("Warning: Corrupted object found during deserialization: Could not construct object of type ", serializedType, " with id ", uuid.ToString());
						
						continue;
					}
				}

				serObjPair["data"].get_to<SerializableObject>(*serObj);

				SerializableObject::idToObject[uuid] = serObj;
			}

			// Link up all scene references
			auto& vecref = SceneReference<SerializableObject>::objectsToLink;
			for (auto& [refUUID, serRefP] : vecref)
			{
				// ToDo: It's possible that the referenced object doesn't exist (corrupted file etc), this should be handled, check
				// ToDo: It might be possible that the reference doesn't exist either, once again should be handled (in referece destructor)

				auto it = SerializableObject::idToObject.find(refUUID);

				if (it != SerializableObject::idToObject.end())
				{
					Logger::LogInfo("Linking object with ID ", refUUID.ToString());
					serRefP->Resolve(it->second);
					/*auto casted = dynamic_cast<SceneReference<SerializableObject>*>(serRefP);
					if (!casted)
					{
						Logger::LogError("Failed to cast SceneReferenceBase to SceneReference<SerializableObject> during linking for object with id ", refUUID.ToString());
						continue;
					}
					casted->ptr = it->second;*/
					//* (serRefP) = SceneReference<SerializableObject>(it->second);
				}
				else
				{
					Logger::LogWarning("No serializable object with id ", refUUID.ToString(), " found for SceneReference during linking");
				}
			}
			vecref.clear();

			// ToDo: Loop through all object and call the initialize. 
			// If the parent failed to deserialize move the object to root. 
			// If transform failed to deserialize create a new one

			SetupRootObject();

			for (auto& [uuid, serObj] : SerializableObject::idToObject)
			{
				if (auto sceneObj = std::dynamic_pointer_cast<SceneObject>(serObj))
				{
					if (!sceneObj->GetTransform())
					{
						if (auto tfr = sceneObj->GetComponent<SceneTransform>())
						{
							sceneObj->transform = tfr;
						}
						else
						{
							Logger::LogWarning("Scene object with id ", sceneObj->GetUUID().ToString(), " has no transform, creating new transform component");
							std::shared_ptr<SceneTransform> tf = ComponentRegistry::ConstructComponent<SceneTransform>();
							tf->Initialize(sceneObj);
							sceneObj->LinkComponent(tf, ComponentRegistry::GetTypeHash<SceneTransform>());
							sceneObj->transform = SceneReference<SceneTransform>(tf);
						}
					}

					if (!sceneObj->GetParent() && sceneObj != root)
					{
						GetRoot()->children.Add(sceneObj->GetUUID(), sceneObj);
						// Parent failed to deserialize, move to root
						sceneObj->Initialize(GetRoot(), this);

						continue;
					}

					sceneObj->Initialize(sceneObj->GetParent(), this);
				}
				else if (auto sceneComp = std::dynamic_pointer_cast<SceneComponent>(serObj))
				{
					if (!sceneComp->GetSceneObject())
					{
						Logger::LogWarning("Component with id ", sceneComp->GetUUID().ToString(), " has no attached object, deleting component");
						SerializableObject::idToObject.erase(sceneComp->GetUUID());

						continue;
					}

					sceneComp->Initialize(sceneComp->GetSceneObject());
				}
			}

			std::vector<SceneReference<SceneObject>> sceneLayout;
			std::vector<int> tillUnindent;

			sceneLayout.push_back(GetRoot());
			int indent = 0;

			while (!sceneLayout.empty())
			{
				auto lastRef = sceneLayout.back();
				sceneLayout.pop_back();

				std::string indents;
				for (int i = 0; i < indent; i++)
				{
					indents += "\t";
				}

				Logger::LogInfo(indents, lastRef->GetSerializedType(), ":", lastRef->GetUUID().ToString());
				indent++;
				indents += "\t";

				for (int i = 0; i < lastRef->components.Size(); i++)
				{
					if (!lastRef->components.At(i))
					{
						// ToDo: Eww:
						lastRef->components.RemoveFast(lastRef->components.GetKey(i));
					}
				}

				for (auto& comp : lastRef->components)
				{
					Logger::LogInfo(indents, comp->GetSerializedType(), ":", comp->GetUUID().ToString());
				}

				for (int i = 0; i < lastRef->children.Size(); i++)
				{
					if (!lastRef->children.At(i))
					{
						// ToDo: Eww:
						lastRef->children.Remove(lastRef->children.GetKey(i));
					}
				}

				tillUnindent.push_back(static_cast<int>(lastRef->children.Size()));
				for (auto& so : lastRef->children)
				{
					sceneLayout.push_back(so);
				}

				tillUnindent.back()--;
				if (tillUnindent.back() == 0)
				{
					indent--;
					tillUnindent.pop_back();
				}
			}

			GetRoot()->OnFinishedLoading();

			ApplicationUtils::deserializingScene = false;
		}
		catch (nlohmann::json::exception e)
		{
			Logger::LogError("Error loading scene: ", e.what());
		}
		i.close();
	}

	SetupRootObject();

#ifdef _EDITOR
	if (ApplicationUtils::IsPlaying())
	{
		root->OnAwake();
		root->OnStart();
	}
	else
	{
		root->OnAwakeEditor();
	}
#else
	root->OnAwake();
	root->OnStart();
#endif

}

StoryboardEngine::Scene::Scene()
{
	SetupRootObject();
}

StoryboardEngine::Scene::~Scene()
{
	Logger::LogInfo("Destroying scene");
	root->DestroyChildrenRecursively();
	SerializableObject::idToObject.erase(root->GetUUID());

	SerializableObject::idToObject.clear();
}

void StoryboardEngine::Scene::Update()
{
	for (auto& component : newComponents)
	{
		// ToDo: Should delay start call if component is not enabled
		component->OnStart();
	}
	newComponents.clear();

	root->OnUpdate();
}

void StoryboardEngine::Scene::Render(ID3D11DeviceContext* deviceContext)
{
	if (!sceneCamera)
	{
		Logger::LogWarning("No camera in scene");
		return;
	}

	sceneCamera->Render();

	// Sort based on draw priority. For most objects this'll stay 0, exceptions might perhaps be gui, skybox...
	// Could also sort based on distance to camera for transparency... but we don't support transparency
	std::sort(drawables.begin(), drawables.end(), [](const SceneReference<DrawableComponent>& a, const SceneReference<DrawableComponent>& b) -> bool { return a->GetRenderPriority() < b->GetRenderPriority(); });

	for (auto& drawable : drawables)
	{
		if (drawable && drawable->IsActive())
		{
			drawable->OnDraw(deviceContext);
		}
	}
}

void StoryboardEngine::Scene::RenderGUI()
{
	if (!sceneCamera)
	{
		// ToDo: Overlay a black window here
	}
}

void StoryboardEngine::Scene::RegisterDrawable(SceneReference<StoryboardEngine::DrawableComponent> drawable)
{
	drawables.Add(drawable->GetUUID(), drawable);
}

// ToDo: Consider different data structure (hashed vector) for better performance
void StoryboardEngine::Scene::DeregisterDrawable(SceneReference<StoryboardEngine::DrawableComponent> drawable)
{
	drawables.RemoveFast(drawable->GetUUID());
}

void StoryboardEngine::Scene::SetCamera(StoryboardEngine::SceneReference<StoryboardEngine::CameraComponent> cameraObject)
{
	if (!cameraObject)
	{
		Logger::LogWarning("Attempted to set a null camera object");
		return;
	}

	sceneCamera = cameraObject;
}

void StoryboardEngine::Scene::SetupRootObject()
{
	if (!root)
	{
		std::shared_ptr<SceneObject> rootShared = std::make_shared<SceneObject>(this, "Scene Root");

		// Register the child object in the global ID map and add it to this object's children
		SerializableObject::idToObject[rootShared->GetUUID()] = rootShared;

		root = SceneReference<SceneObject>(rootShared);
	}

	// if the child failed to deserialize a transform component, create one now
	if (!root->GetTransform())
	{
		if (auto tfr = root->GetComponent<SceneTransform>())
		{
			root->transform = tfr;
			return;
		}

		std::shared_ptr<SceneTransform> tf = ComponentRegistry::ConstructComponent<SceneTransform>();
		tf->Initialize(root);
		root->LinkComponent(tf, ComponentRegistry::GetTypeHash<SceneTransform>());
		root->transform = SceneReference<SceneTransform>(tf);
	}
}

void StoryboardEngine::Scene::RegisterNewComponent(StoryboardEngine::SceneReference<StoryboardEngine::SceneComponent> component)
{
	newComponents.push_back(component);
}

StoryboardEngine::SceneReference<StoryboardEngine::SceneObject> StoryboardEngine::Scene::GetRoot() const
{
	return root;
}
