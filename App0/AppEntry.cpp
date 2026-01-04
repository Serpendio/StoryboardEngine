#include "pch.h"
#include "Core/UserRegistry.h"
#include "Components/MainMenu.h"
#include "Components/ReferenceLinkTest.h"
#include "Components/Projectile.h"
#include "Components/Gun.h"
#include "Components/Scoreboard.h"
#include "Components/Target.h"
#include "Components/TargetSpawner.h"
#include "Components/FirstPersonController.h"

void StoryboardEngine::RegisterApplicationComponents()
{
	StoryboardEngine::ComponentRegistry::RegisterComponent<MainMenu>();
	StoryboardEngine::ComponentRegistry::RegisterComponent<ReferenceLinkTest>();
	StoryboardEngine::ComponentRegistry::RegisterComponent<Projectile>();
	StoryboardEngine::ComponentRegistry::RegisterComponent<Gun>();
	StoryboardEngine::ComponentRegistry::RegisterComponent<Scoreboard>();
	StoryboardEngine::ComponentRegistry::RegisterComponent<Target>();
	StoryboardEngine::ComponentRegistry::RegisterComponent<TargetSpawner>();
	StoryboardEngine::ComponentRegistry::RegisterComponent<FirstPersonController>();
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
	modelNameToPathMap["Target"] = "Resources/balloon-target/source/BaloonTarget.fbx";
}

void StoryboardEngine::RegisterApplicationTextures(std::unordered_map<std::string, std::string>& textureNameToPathMap)
{
	// Example:
	// textureNameToPathMap["SampleTexture"] = "Resources/Textures/SampleTexture.png";
	textureNameToPathMap["Target"] = "Resources/balloon-target/textures/BalloonTargertexturemap.png";
}
