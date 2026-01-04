#include "pch.h"
#include "Core/ContactListener.h"

#include "Internal/Physics3D.h"
#include "Core/SceneObject.h"
#include "Utils/SerializableUtils.h"

// ToDo: These will crash if the objects have been destroyed, check that that will never happen (Pretty sure rigid body should handle this)
// ToDo: Triggers, not just colliders

void StoryboardEngine::ContactListener::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
	const auto obj1 = reinterpret_cast<SceneObject*>(inBody1.GetUserData());
	const auto obj2 = reinterpret_cast<SceneObject*>(inBody2.GetUserData());

	Logger::LogInfo("Contact Added between ", obj1->name, " and ", obj2->name);

	std::scoped_lock lock(queueMutex);
	queue.push_back({ 
		Type::Begin, 
		SerializableUtils::TryResolveSceneReference<SceneObject>(obj1->GetUUID()), 
		SerializableUtils::TryResolveSceneReference<SceneObject>(obj2->GetUUID()) 
		});
}

void StoryboardEngine::ContactListener::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
	const auto obj1 = reinterpret_cast<SceneObject*>(inBody1.GetUserData());
	const auto obj2 = reinterpret_cast<SceneObject*>(inBody2.GetUserData());

	std::scoped_lock lock(queueMutex);
	queue.push_back({ 
		Type::Persist, 
		SerializableUtils::TryResolveSceneReference<SceneObject>(obj1->GetUUID()), 
		SerializableUtils::TryResolveSceneReference<SceneObject>(obj2->GetUUID()) 
		});
}

void StoryboardEngine::ContactListener::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
{
	const auto body1 = Physics3D::GetPhysicsSystem().GetBodyLockInterface().TryGetBody(inSubShapePair.GetBody1ID());
	const auto body2 = Physics3D::GetPhysicsSystem().GetBodyLockInterface().TryGetBody(inSubShapePair.GetBody2ID());

	if (!body1 || !body2) return;

	const auto obj1 = reinterpret_cast<SceneObject*>(body1->GetUserData());
	const auto obj2 = reinterpret_cast<SceneObject*>(body2->GetUserData());

	if (SerializableObject::DoesObjectExist(obj1->GetUUID()) == false ||
		SerializableObject::DoesObjectExist(obj2->GetUUID()) == false)
	{
		// One of the objects has been destroyed, skip the event
		return; 

		// ToDo: This possibly is an exploitable vulnerability, a pointer to a spot in memory that is no longer valid
	}

	std::scoped_lock lock(queueMutex);
	queue.push_back({ 
		Type::End, 
		SerializableUtils::TryResolveSceneReference<SceneObject>(obj1->GetUUID()), 
		SerializableUtils::TryResolveSceneReference<SceneObject>(obj2->GetUUID()) 
		});
}

void StoryboardEngine::ContactListener::FlushContactEvents()
{
	std::vector<Event> local;
	{
		std::scoped_lock lock(queueMutex);
		local.swap(queue);
	}

	for (const auto& [type, a, b] : local)
	{
		switch (type)
		{
		case Type::Begin:
			a->OnCollisionEnter(b);
			b->OnCollisionEnter(a);
			break;
		case Type::Persist:
			a->OnCollisionStay(b);
			b->OnCollisionStay(a);
			break;
		case Type::End:
			if (a)
			{
				a->OnCollisionExit(b);
			}
			if (b)
			{
				b->OnCollisionExit(a);
			}
			break;
		}
	}
}
