#include "pch.h"
#include "Core/SerializableObject.h"
#include "Utils/ApplicationUtils.h"

std::unordered_map<StoryboardEngine::UUID, std::shared_ptr<StoryboardEngine::SerializableObject>> StoryboardEngine::SerializableObject::idToObject;

/*StoryboardEngine::SceneReference<StoryboardEngine::SerializableObject> StoryboardEngine::SerializableObject::GetSelfReference()
{
	// If we try to get the shared_ptr from this before it has been added to idToObject, it will fail
	//return SceneReference<SerializableObject>(idToObject[m_uuid]);

	// Supposedly this should be safe as long as SerializableObject is always created as part of a shared_ptr
	// Therefore will work when deserializing, when it hasn't yet been added to idToObject
	return SceneReference<SerializableObject>(shared_from_this());
}*/

StoryboardEngine::SerializableObject::SerializableObject()
{
	/*if (ApplicationUtils::IsDeserializingScene())
	{
		// If this object was created from deserialization, there'll already be a UUID assigned
		return;
	}*/

	// Generate a unique UUID
	do
	{
		m_uuid = UUID::Generate();
	} while (idToObject.find(m_uuid) != idToObject.end());

	// Register this object
	// idToObject[m_uuid] = this; // Now handled when made by the Scene/SceneObject
}

StoryboardEngine::SerializableObject& StoryboardEngine::SerializableObject::operator=(const SerializableObject& other)
{
	// Note: UUIDs are not copied
	return *this;
}

StoryboardEngine::SerializableObject::~SerializableObject()
{
	/*if (idToObject.find(m_uuid) == idToObject.end())
	{
		return;
	}

	idToObject.erase(m_uuid);*/ // Undefined behaviour if this is called due to the map being cleared, now handled by Scene/SceneObject // ToDo: (Not actually handled yet)
}

const StoryboardEngine::UUID& StoryboardEngine::SerializableObject::GetUUID() const
{ 
	return m_uuid; 
}

bool StoryboardEngine::SerializableObject::DoesObjectExist(const UUID& id)
{
	return idToObject.contains(id);
}

void StoryboardEngine::SerializableObject::OnBeginSave()
{

}

void StoryboardEngine::SerializableObject::OnFinishedLoading()
{
	/*if (idToObject.find(m_uuid) != idToObject.end())
	{
		Logger::LogError("Error: Duplicate UUID detected during SerializableObject deserialization");
	}

	//idToObject[m_uuid] = this;*/ // Now handled when made by the Scene/SceneObject
}

// ToDo: Do I call this, and at frame end?
void StoryboardEngine::SerializableObject::OnDestroy()
{

}