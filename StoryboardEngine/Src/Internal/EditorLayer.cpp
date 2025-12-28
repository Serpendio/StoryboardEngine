#include "pch.h"
#include "Internal/EditorLayer.h"
#include <fstream>
#include <imgui.h>
#include <imgui_stdlib.h>
#include "Core/SceneObject.h"
#include "Core/Scene.h"
#include "Utils/SceneManager.h"
#include "Core/SceneComponent.h"
#include "Utils/ApplicationUtils.h"
#include "Core/CameraComponent.h"
#include "Core/SceneTransform.h"
#include "Core/FreeCamController.h"

void StoryboardEngine::EditorLayer::CreateHeirarchy(SceneReference<SceneObject> so)
{
	std::string uuidStr = so->GetUUID().ToString();
	//ImGui::PushID(uuidStr.c_str());
	if (so->GetChildCount() == 0)
	{
		if (ImGui::TreeNodeEx(uuidStr.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf | (so == selectedInHeirarchy ? ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected : 0), so->name.c_str()))
		{
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				selectedInHeirarchy = (so == selectedInHeirarchy) ? SceneReference<SceneObject>() : so;

			ImGui::TreePop(); // For the treeNodeEx
		}
		//ImGui::PopID();
		return;
	}

	bool nodeOpen = ImGui::TreeNodeEx(uuidStr.c_str(), (so == selectedInHeirarchy ? ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow, so->name.c_str());

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		selectedInHeirarchy = so == selectedInHeirarchy ? SceneReference<SceneObject>() : so;

	if (nodeOpen)
	{
		for (int i = 0; i < so->GetChildCount(); i++)
		{
			CreateHeirarchy(so->GetChild(i));
		}
		ImGui::TreePop(); // For the treeNodeEx
	}
	//ImGui::PopID();
}

void StoryboardEngine::EditorLayer::SaveScene()
{
	// ToDo: Should store if dirty for speed

	std::shared_ptr<Scene> scene = currentScene.lock();
	if (!scene)
		return;

	// ToDo: Unlike other files, saving should be done both directly to the project files and relative to the executable, but loading should still be done relative to the executable

	// create folder if it doesn't exist
	std::filesystem::create_directories(sceneFilePath.parent_path());

	nlohmann::ordered_json j;

	scene->GetRoot()->OnBeginSave();

	// If the save system changes, we can use the version to handle backwards compatibility
	j["Version"] = 1;

	// ToDo: Allow partial saving if something breaks, rather than just failing entirely
	j["Root"] = scene->GetRoot();

	nlohmann::ordered_json j_map;

	for (auto const& [uuid, serObj] : SerializableObject::idToObject)
	{
		auto j_pair = nlohmann::ordered_json::object();
		j_pair["serialized_type"] = serObj->GetSerializedType();
		j_pair["data"] = *serObj;

		j_map.push_back(j_pair);
	}

	j["SerializableObjects"] = j_map;

	std::ofstream o(sceneFilePath);
	o << std::setw(4) << j << std::endl;
	o.close();

	Logger::LogInfo("Scene saved successfully: ", sceneFilePath.wstring());
}

void StoryboardEngine::EditorLayer::Update()
{

	//sceneCamera->OnUpdate();

	if (!renderScenePOV)
		sceneCamera->GetComponent<FreeCamController>()->OnUpdate();

	if (ApplicationUtils::IsPlaying())
	{
		std::shared_ptr<Scene> scene = currentScene.lock();
		if (!scene)
			return;

		scene->Update();
	}
}

void StoryboardEngine::EditorLayer::Render(ID3D11DeviceContext* deviceContext)
{
	std::shared_ptr<Scene> scene = currentScene.lock();
	if (!scene)
		return;

	if (!renderScenePOV)
		scene->SetCamera(sceneCamera->GetComponent<CameraComponent>());
	else if (!ApplicationUtils::IsPlaying())
	{
		auto cameras = scene->root->GetComponents<CameraComponent>();
		if (cameras.empty())
		{
			scene->sceneCamera = SceneReference<CameraComponent>();
		}
		else
		{
			scene->sceneCamera = cameras.back();
		}

	}

	scene->Render(deviceContext);
}

void StoryboardEngine::EditorLayer::RenderGUI()
{
	//ImGui::ShowDemoWindow();
	std::shared_ptr<Scene> scene = currentScene.lock();
	if (!scene)
		return;

	ImGui::Begin(currentSceneName.c_str());

	//CreateHeirarchy(skybox->GetSceneObject());
	CreateHeirarchy(sceneCamera);

	ImGui::Checkbox("Debug View", &debugMode);

	if (!ApplicationUtils::IsPlaying())
	{
		if (ImGui::Button("Save Scene"))
		{
			SaveScene();
		}
		if (ImGui::Button("Enter play mode"))
		{
#ifdef _EDITOR
			SaveScene();

			ApplicationUtils::isPlaying = true;
			SceneManager::LoadScene(editingSceneName);
#endif
		}
	}
	else
	{
		if (ImGui::Button("Exit play mode"))
		{
#ifdef _EDITOR
			ApplicationUtils::isPlaying = false;
			SceneManager::LoadScene(editingSceneName);
#endif
		}
	}

	ImGui::Checkbox("Scene View", &renderScenePOV);

	ImGui::End();

	ImGui::Begin("Heirarchy");
	SceneReference<SceneObject> selected = scene->GetRoot();
	for (int i = 0; i < selected->GetChildCount(); i++)
	{
		CreateHeirarchy(selected->GetChild(i));
	}

	if (ImGui::Button("Add Empty Object"))
	{
		auto newObj = scene->GetRoot()->AddChild();
		newObj->name = "New Object";
		selectedInHeirarchy = newObj;
	}

	if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
	{
		selectedInHeirarchy = nullptr;
	}
	ImGui::End();//ends that window

	ImGui::Begin("Inspector");
	if (selectedInHeirarchy)
	{
		ImGui::InputText("Name", &selectedInHeirarchy->name);

		std::string uuidText = "UUID: " + selectedInHeirarchy->GetUUID().ToString();
		ImGui::Text(uuidText.c_str());
		
		if (ImGui::Button("Add Empty Child"))
		{
			auto newObj = selectedInHeirarchy->AddChild();
			newObj->name = "New Object";
			selectedInHeirarchy = newObj;
		}

		if (ImGui::Button("Delete Object"))
		{
			auto parent = selectedInHeirarchy->GetParent();

			parent->DestroyChild(selectedInHeirarchy->GetUUID());
			selectedInHeirarchy = nullptr;
			ImGui::End();
			return;
		}

		selectedInHeirarchy->DrawInspector(debugMode);

		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("Add_Component");
			componentToAddName = "";
			componentToAdd = NULL;
		}

		if (ImGui::BeginPopup("Add_Component"))
		{
			if (ImGui::BeginCombo("Components", componentToAddName.c_str()))
			{
				for (auto& possibleComponent : ComponentRegistry::componentRegistry)
				{
					const std::string& componentName = possibleComponent.second.first;

					// Skip if the component already exists on the object
					if (selectedInHeirarchy->GetComponentByHash(possibleComponent.first))
						continue;

					const bool is_selected = (componentToAdd == possibleComponent.first);
					if (ImGui::Selectable(componentName.c_str(), is_selected))
					{
						componentToAddName = componentName;
						componentToAdd = possibleComponent.first;
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			if (componentToAdd == NULL)
				ImGui::BeginDisabled();

			if (ImGui::Button("Add"))
			{
				selectedInHeirarchy->AddComponent(componentToAdd);
				ImGui::CloseCurrentPopup();
			}

			if (componentToAdd == NULL)
				ImGui::EndDisabled();

			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

StoryboardEngine::EditorLayer::EditorLayer()
	: componentToAdd(NULL), componentToAddName("")
{
	sceneCamera = std::make_shared<SceneObject>(nullptr, "Editor Camera");

	auto transform = ComponentRegistry::ConstructComponent<SceneTransform>();
	transform->Initialize(sceneCamera);
	sceneCamera->components.Add(transform->GetComponentHash(), SceneReference<SceneTransform>(transform));
	sceneComponents.push_back(std::static_pointer_cast<SceneComponent>(transform));
	sceneCamera->transform = SceneReference<SceneTransform>(transform);

	auto cameraComp = ComponentRegistry::ConstructComponent<CameraComponent>();
	cameraComp->Initialize(sceneCamera);
	sceneCamera->components.Add(cameraComp->GetComponentHash(), SceneReference<CameraComponent>(cameraComp));
	sceneComponents.push_back(std::static_pointer_cast<SceneComponent>(cameraComp));

	auto freeCamComp = ComponentRegistry::ConstructComponent<FreeCamController>();
	freeCamComp->Initialize(sceneCamera);
	sceneCamera->components.Add(freeCamComp->GetComponentHash(), SceneReference<FreeCamController>(freeCamComp));
	sceneComponents.push_back(std::static_pointer_cast<SceneComponent>(freeCamComp));

	auto pos = sceneCamera->transform->GetLocalPosition();
	pos.z = -10;
	sceneCamera->transform->SetPosition(pos);
	sceneCamera->transform->FaceDirection(Vector3(0, 0, 1), Vector3(0, 1, 0));
}

StoryboardEngine::EditorLayer::~EditorLayer()
{
}

void StoryboardEngine::EditorLayer::SetScene(std::filesystem::path filepath, std::string name, std::weak_ptr<Scene> scene)
{
	if (!ApplicationUtils::IsPlaying())
	{
		// We want to be able to edit/save the scene only when not in play mode
		sceneFilePath = filepath;
		editingSceneName = name;
	}
	currentSceneName = name;
	currentScene = scene;
}
