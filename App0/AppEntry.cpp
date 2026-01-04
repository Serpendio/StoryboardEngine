#include "pch.h"
#include "Core/UserRegistry.h"

void StoryboardEngine::RegisterApplicationComponents()
{
	// Example:
	// StoryboardEngine::ComponentRegistry::RegisterComponent<YourCustomComponent>();
}

void StoryboardEngine::RegisterApplicationScenes(std::unordered_map<std::string, std::string>& sceneNameToPathMap, std::string& initialScene)
{
	// Example:
	// sceneNameToPathMap["SampleScene"] = "Assets/Scenes/SampleScene.scene";
	// initialScene = "SampleScene";
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
