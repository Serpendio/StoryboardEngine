#pragma once
#include <vector>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Body/Body.h>
#include <mutex>
#include "Utils/SceneReference.h"

namespace StoryboardEngine
{
	class SceneObject;

	// This will break if bodies have sub shapes
	class ContactListener : public JPH::ContactListener
	{
		enum class Type { Begin, Persist, End };
		struct Event
		{
			Type type;
			SceneReference<SceneObject> a;
			SceneReference<SceneObject> b;
		};

		std::mutex queueMutex;
		std::vector<Event> queue;

	public:
		void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;
		void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;
		void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override;

		void FlushContactEvents();
	};
}
