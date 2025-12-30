#include "pch.h"
#include "Core/RigidBody.h"
#include <imgui.h>
#include "Core/SceneTransform.h"

// ToDo: Activation, shouldn't always activate

StoryboardEngine::RigidBody::~RigidBody()
{
	JPH::BodyInterface& bodyInterface = StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface();
	bodyInterface.RemoveBody(bodyID);
}

void StoryboardEngine::RigidBody::OnAwake()
{
	Vector3 position = GetTransform()->GetGlobalPosition();
	Vector3 rotation = GetTransform()->GetGlobalRotation();
	Vector3 scale = GetTransform()->GetGlobalScale();
	Quaternion rotationQuat = Quaternion::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);

	JPH::BodyCreationSettings bodySettings(
		new JPH::BoxShape(JPH::RVec3(scale.x, scale.y, scale.z)),
		JPH::RVec3(position.x, position.y, position.z),
		JPH::Quat(rotationQuat.x, rotationQuat.y, rotationQuat.z, rotationQuat.w),
		JPH::EMotionType::Dynamic,
		StoryboardEngine::Physics3D::PhysicsLayers::DYNAMIC
	);

	bodySettings.mGravityFactor = 1.0f;

	JPH::Body* body = StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface().CreateBody(bodySettings);
	body->SetUserData(reinterpret_cast<uint64_t>(GetSceneObject().get()));
	StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::Activate);
	bodyID = body->GetID();
}

void StoryboardEngine::RigidBody::UpdateState(bool newState)
{
	bool changed = (newState != IsActive());

	StoryboardEngine::SceneComponent::UpdateState(newState);

	if (!changed)
	{
		return;
	}

	JPH::BodyInterface& bodyInterface = StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface();
	if (IsActive()) // This might need to be Add & Remove instead of Activate & Deactivate
	{
		bodyInterface.AddBody(bodyID, JPH::EActivation::Activate);
	}
	else
	{
		bodyInterface.RemoveBody(bodyID);
	}
}

void StoryboardEngine::RigidBody::OnDrawDebugInspector()
{
	StoryboardEngine::SceneComponent::OnDrawDebugInspector();

	ImGui::Text("Body ID: %u", bodyID.GetIndexAndSequenceNumber());
}

JPH::BodyID StoryboardEngine::RigidBody::GetBodyID() const
{
	return bodyID;
}

void StoryboardEngine::RigidBody::SetVelocity(const Vector3& velocity) const
{
	JPH::BodyInterface& bodyInterface = StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface();
	bodyInterface.SetLinearVelocity(bodyID, JPH::Vec3(velocity.x, velocity.y, velocity.z));
}

Vector3 StoryboardEngine::RigidBody::GetVelocity() const
{
	JPH::BodyInterface& bodyInterface = StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface();
	JPH::Vec3 vel = bodyInterface.GetLinearVelocity(bodyID);
	return Vector3(vel.GetX(), vel.GetY(), vel.GetZ());
}

void StoryboardEngine::RigidBody::SetAngularVelocity(const Vector3& angularVelocity) const
{
	JPH::BodyInterface& bodyInterface = StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface();
	bodyInterface.SetAngularVelocity(bodyID, JPH::Vec3(angularVelocity.x, angularVelocity.y, angularVelocity.z));
}

Vector3 StoryboardEngine::RigidBody::GetAngularVelocity() const
{
	JPH::BodyInterface& bodyInterface = StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface();
	JPH::Vec3 angVel = bodyInterface.GetAngularVelocity(bodyID);
	return Vector3(angVel.GetX(), angVel.GetY(), angVel.GetZ());
}
