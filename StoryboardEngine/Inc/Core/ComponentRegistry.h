#pragma once
#include <typeinfo>
#include <unordered_map>
#include "nameof.hpp"
#include "Utils/Serialization.h"
#include "Utils/Logger.h"
#include "Core/SceneComponent.h"
#include "Utils/StringUtils.h"

namespace StoryboardEngine
{
	class SceneObject;

	class ComponentRegistry final
	{
	public:
		template<typename TComponent>
		[[nodiscard]] static void RegisterComponent()
		{
			// Ensure TComponent is derived from SceneComponent and not abstract
			static_assert(std::is_base_of<SceneComponent, TComponent>::value, "TComponent must be derived from SceneComponent");
			static_assert(!std::is_abstract<TComponent>::value, "TComponent cannot be abstract");

			std::size_t componentHash = GetTypeHash<TComponent>();
			// Register a factory function for creating instances of TComponent
			componentRegistry[componentHash] = std::make_pair(std::string(NAMEOF_SHORT_TYPE(TComponent)), []() -> std::shared_ptr<SceneComponent>
			{
				std::shared_ptr<TComponent> comp = std::make_shared<TComponent>();
				return comp;
			});
		}

		template<typename TComponent>
		static std::shared_ptr<TComponent> ConstructComponent()
		{
			// Ensure TComponent is derived from SceneComponent
			static_assert(std::is_base_of<SceneComponent, TComponent>::value, "TComponent must be derived from SceneComponent");

			std::size_t componentHash = GetTypeHash<TComponent>();
			auto it = componentRegistry.find(componentHash);
			if (it == componentRegistry.end())
			{
				Logger::LogError("Component type not registered: ", NAMEOF_SHORT_TYPE(TComponent));
				return nullptr;
			}
			
			auto& nameFuncPair = it->second;
			auto& factoryFunc = nameFuncPair.second;

			// Hash clashes are very unlikely, but still possible
			std::shared_ptr<TComponent> comp = std::static_pointer_cast<TComponent>(factoryFunc());
			comp->componentHash = componentHash;
			return comp;
		}

		// ToDo: Hash should perhaps be the full name, not just the short name to avoid clashes between namespaces

		template<typename T>
		static std::size_t GetTypeHash()
		{
			return GetTypeHash(GetTypeName<T>());
		}

		// ToDo: This could pass on the string_view directly to avoid a copy

		template<typename T>
		static std::string GetTypeName()
		{
			return StringUtils::ToString(NAMEOF_SHORT_TYPE(T));
		}

		static std::size_t GetTypeHash(const std::string& typeName);

		static std::string GetComponentName(std::size_t componentHash);

		static std::shared_ptr<SceneComponent> ConstructComponent(std::size_t componentHash);
	private:
		ComponentRegistry() = delete;

		static void Shutdown();

		friend class EditorLayer;
		friend class ApplicationCore;

		// Registry mapping component type hashes to a pair of <names and factory functions>
		static std::unordered_map<std::size_t, std::pair<std::string, std::shared_ptr<SceneComponent> (*)()>> componentRegistry;
	};
}
