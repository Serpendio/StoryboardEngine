#include "pch.h"
#include "Core/UserRegistry.h"
#include "Components/MainMenu.h"
#include "Components/ReferenceLinkTest.h"

void StoryboardEngine::RegisterApplicationComponents()
{
	StoryboardEngine::ComponentRegistry::RegisterComponent<MainMenu>();
	StoryboardEngine::ComponentRegistry::RegisterComponent<ReferenceLinkTest>();
}

void StoryboardEngine::RegisterApplicationScenes(std::unordered_map<std::string, std::string>& sceneNameToPathMap, std::string& initialScene)
{
	sceneNameToPathMap["MenuScene"] = "Resources/Scenes/MenuScene.scene";
	sceneNameToPathMap["TestScene"] = "Resources/Scenes/TestScene.scene";
	sceneNameToPathMap["GameScene"] = "Resources/Scenes/GameScene.scene";
	initialScene = "MenuScene";
}

[[nodiscard]] void StoryboardEngine::RegisterApplicationModels(std::unordered_map<std::string, std::string>& modelNameToPathMap)
{
	// Example:
	//modelNameToPathMap["SampleModel"] = "Resources/Models/SampleModel.obj";
}

void StoryboardEngine::RegisterApplicationTextures(std::unordered_map<std::string, std::string>& textureNameToPathMap)
{
	// Example:
	// textureNameToPathMap["SampleTexture"] = "Resources/Textures/SampleTexture.png";
}
