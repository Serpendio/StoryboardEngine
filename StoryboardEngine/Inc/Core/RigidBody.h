#pragma once
#include "Core/SceneComponent.h"
#include "Internal/Physics3D.h" // ToDo: Don't really need the whole header

namespace StoryboardEngine
{
	class RigidBody : public StoryboardEngine::SceneComponent
	{
		JPH::BodyID bodyID;
		bool isStatic = false;

		// ToDo: It'd be nice not to have to store the gravity all the time just for serialization, we don't need the variable at runtime
		// Maybe we can have a custom serialization that accepts functions
		float gravityFactor = 1.0f;
	public:
		~RigidBody() override;
		void OnAwake() override;
		void UpdateState(bool newState) override;
		void OnDrawInspector() override;
		void OnDrawDebugInspector() override;

		JPH::BodyID GetBodyID() const;

		void SetVelocity(const Vector3& velocity) const;
		Vector3 GetVelocity() const;
		void SetAngularVelocity(const Vector3& angularVelocity) const;
		Vector3 GetAngularVelocity() const;

		void SetIsStatic(bool staticState);

		ADD_SERIALIZATION(SceneComponent, RigidBody,
			JSON_VARIABLE(gravityFactor), JSON_VARIABLE(isStatic))
	private:
		void CreatePhysicsBody();
		void DestroyPhysicsBody();
	};
}
