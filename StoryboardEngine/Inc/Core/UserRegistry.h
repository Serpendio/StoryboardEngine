#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "Core/ComponentRegistry.h"
#include "Utils/Serialization.h"

namespace StoryboardEngine
{
	// All user-defined components should be registered in this function
	extern void RegisterApplicationComponents();
	// All user-defined scenes should be registered in this function
	extern void RegisterApplicationScenes(std::unordered_map<std::string, std::string>& sceneNameToPathMap);
	// All user-defined models should be registered in this function
	extern void RegisterApplicationModels(std::unordered_map<std::string, std::string>& modelNameToPathMap);
	// All user-defined textures should be registered in this function
	extern void RegisterApplicationTextures(std::unordered_map<std::string, std::string>& textureNameToPathMap);
}