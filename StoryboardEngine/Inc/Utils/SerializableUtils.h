#pragma once
#include "Core/SerializableObject.h"
#include "Utils/SceneReference.h"

namespace StoryboardEngine
{
	class SerializableUtils
	{
	public:
		template<typename T>
		static SceneReference<T> TryResolveSceneReference(const UUID& id)
		{
			auto it = SerializableObject::idToObject.find(id);
			if (it != SerializableObject::idToObject.end())
			{
				// Found the object, try to cast to T
				std::shared_ptr<SerializableObject> serObj = it->second;
				std::shared_ptr<T> castedObj = std::dynamic_pointer_cast<T>(serObj);
				if (castedObj)
				{
					return SceneReference<T>(castedObj);
				}
				else
				{
					Logger::LogWarning("Failed to cast SerializableObject with UUID ", id.ToString(), " to type ", NAMEOF_SHORT_TYPE(T));
					return SceneReference<T>(nullptr);
				}
			}
			else
			{
				Logger::LogWarning("No SerializableObject found with UUID ", id.ToString());
				return SceneReference<T>(nullptr);
			}
		}
	};
}
