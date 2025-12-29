#include "pch.h"
#include "Core/ComponentRegistry.h"

std::unordered_map<std::size_t, std::pair<std::string, std::shared_ptr<StoryboardEngine::SceneComponent> (*)()>> StoryboardEngine::ComponentRegistry::componentRegistry;


std::shared_ptr<StoryboardEngine::SceneComponent> StoryboardEngine::ComponentRegistry::ConstructComponent(std::size_t componentHash)
{
	auto it = componentRegistry.find(componentHash);
	if (it == componentRegistry.end())
	{
		// ToDo: Allow the user to relink components that have changed name (and fix other corruptions)
		Logger::LogError("Component type not registered with hash: ", componentHash);
		return nullptr;
	}

	// Hash clashes are very unlikely, but still possible
			
	auto& nameFuncPair = it->second;
	auto& factoryFunc = nameFuncPair.second;

	std::shared_ptr<SceneComponent> comp = factoryFunc();
	comp->componentHash = componentHash;
	return comp;
}

std::size_t StoryboardEngine::ComponentRegistry::GetTypeHash(const std::string& typeName)
{
	return std::hash<std::string>()(typeName);
}

std::string StoryboardEngine::ComponentRegistry::GetComponentName(std::size_t componentHash)
{
	auto it = componentRegistry.find(componentHash);
	if (it == componentRegistry.end())
	{
		Logger::LogWarning("Component type not registered with hash: ", componentHash);
		return std::string{};
	}
	return it->second.first;
}

void StoryboardEngine::ComponentRegistry::Shutdown()
{
	componentRegistry.clear();
}