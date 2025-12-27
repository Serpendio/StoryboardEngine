#pragma once
#include <memory>
#include "Utils/Logger.h"
#include "Utils/Serialization.h"
#include "Internal/UUID.h"
//#include "Utils/SceneReference.h"

namespace StoryboardEngine
{
	class SerializableObject : public std::enable_shared_from_this<SerializableObject>
	{
	private:
		UUID m_uuid;

		// ToDo: consider if a serializable object is copied or moved, should it get a new UUID?

		friend class SceneObject;
		friend class SceneComponent; // for shared_from_this
		friend class Scene;
		friend class EditorLayer;

		SerializableObject(const SerializableObject&) = delete;
		SerializableObject(SerializableObject&&) = delete;

		static std::unordered_map<UUID, std::shared_ptr<SerializableObject>> idToObject;
	protected:
		SerializableObject();
		SerializableObject& operator=(const SerializableObject&);

		//SceneReference<SerializableObject> GetSelfReference();
	public:
		~SerializableObject();
		virtual void OnBeginSave();
		virtual void OnFinishedLoading();
		// Called at the end of the frame if the object/component has been marked for destruction
		virtual void OnDestroy();
		virtual std::string GetSerializedType() const = 0;

		const UUID& GetUUID() const;

		auto operator<=>(const SerializableObject& other) const = default;

		ADD_SERIALIZATION_NO_PARENT(SerializableObject, JSON_VARIABLE(m_uuid))
	private:

		using std::enable_shared_from_this<SerializableObject>::shared_from_this;
	};
}
