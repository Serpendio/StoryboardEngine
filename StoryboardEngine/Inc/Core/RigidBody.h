#pragma once
#include "Core/SceneComponent.h"
#include "Internal/Physics3D.h" // ToDo: Don't really need the whole header

namespace StoryboardEngine
{
	class RigidBody : public StoryboardEngine::SceneComponent
	{
		JPH::BodyID bodyID;

	public:
		~RigidBody() override;
		void OnAwake() override;
		void UpdateState(bool newState) override;
		void OnDrawDebugInspector() override;

		JPH::BodyID GetBodyID() const;

		void SetVelocity(const Vector3& velocity) const;
		Vector3 GetVelocity() const;
		void SetAngularVelocity(const Vector3& angularVelocity) const;
		Vector3 GetAngularVelocity() const;
	};
}
