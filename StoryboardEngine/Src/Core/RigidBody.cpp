#include "pch.h"
#include "Core/RigidBody.h"
#include <imgui.h>
#include "Core/SceneTransform.h"

// ToDo: Activation, shouldn't always activate

StoryboardEngine::RigidBody::~RigidBody()
{
	DestroyPhysicsBody();
}

void StoryboardEngine::RigidBody::OnAwake()
{
	CreatePhysicsBody();
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

void StoryboardEngine::RigidBody::OnDrawInspector()
{
	StoryboardEngine::SceneComponent::OnDrawInspector();

	Vector3 velVec = GetVelocity();
	ImGui::DragFloat3("Velocity", &velVec.x, 0.1f);
	if (ImGui::IsItemEdited()) // ToDo: Add IsItemEdited to all the other areas
	{
		SetVelocity(velVec);
	}
	Vector3 angVelVec = GetAngularVelocity();
	ImGui::DragFloat3("Angular Velocity", &angVelVec.x, 0.1f);
	if (ImGui::IsItemEdited())
	{
		SetAngularVelocity(angVelVec);
	}

	if (ApplicationUtils::IsPlaying())
	{
		float gravity = StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface().GetGravityFactor(bodyID);
		ImGui::DragFloat("Gravity Factor", &gravity, 0.01f);
		if (ImGui::IsItemEdited())
		{
			StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface().SetGravityFactor(bodyID, gravity);
			StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface().ActivateBody(bodyID);
		}
	}
	else
	{
		ImGui::DragFloat("Gravity Factor", &gravityFactor, 0.01f);
	}

	// Motion Type
	const char* motionTypes[] = { "Static", "Kinematic", "Dynamic" };
	int currentMotionType = static_cast<int>(motionType);
	if (ImGui::Combo("Motion Type", &currentMotionType, motionTypes, IM_ARRAYSIZE(motionTypes)))
	{
		SetMotionType(static_cast<JPH::EMotionType>(currentMotionType));
	}
}

void StoryboardEngine::RigidBody::OnDrawDebugInspector()
{
	StoryboardEngine::SceneComponent::OnDrawDebugInspector();

	ImGui::TextDisabled("Body ID: %u", bodyID.GetIndexAndSequenceNumber());

	if (ImGui::Button("Awaken Body"))
	{
		StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface().ActivateBody(bodyID);
	}
}

JPH::BodyID StoryboardEngine::RigidBody::GetBodyID() const
{
	return bodyID;
}

void StoryboardEngine::RigidBody::SetVelocity(const Vector3& velocity) const
{
	if (!IsActive())
	{
		return;
	}

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
	if (!IsActive())
	{
		return;
	}

	JPH::BodyInterface& bodyInterface = StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface();
	bodyInterface.SetAngularVelocity(bodyID, JPH::Vec3(angularVelocity.x, angularVelocity.y, angularVelocity.z));
}

Vector3 StoryboardEngine::RigidBody::GetAngularVelocity() const
{
	JPH::BodyInterface& bodyInterface = StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface();
	JPH::Vec3 angVel = bodyInterface.GetAngularVelocity(bodyID);
	return Vector3(angVel.GetX(), angVel.GetY(), angVel.GetZ());
}

void StoryboardEngine::RigidBody::SetMotionType(JPH::EMotionType type)
{
	motionType = type;
	DestroyPhysicsBody();
	CreatePhysicsBody();
}

void StoryboardEngine::RigidBody::CreatePhysicsBody()
{
	Vector3 position = GetTransform()->GetGlobalPosition();
	Vector3 rotation = GetTransform()->GetGlobalRotation();
	Vector3 scale = GetTransform()->GetGlobalScale() / 2;
	Quaternion rotationQuat = Quaternion::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);

	JPH::ObjectLayer layer = motionType == JPH::EMotionType::Static ? Physics3D::PhysicsLayers::STATIC : Physics3D::PhysicsLayers::DYNAMIC;

	JPH::BodyCreationSettings bodySettings(
		new JPH::BoxShape(JPH::RVec3(scale.x, scale.y, scale.z)),
		JPH::RVec3(position.x, position.y, position.z),
		JPH::Quat(rotationQuat.x, rotationQuat.y, rotationQuat.z, rotationQuat.w),
		motionType,
		layer
	);

	bodySettings.mGravityFactor = gravityFactor;

	JPH::Body* body = StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface().CreateBody(bodySettings);
	body->SetUserData(reinterpret_cast<uint64_t>(GetSceneObject().get()));
	StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::Activate);
	bodyID = body->GetID();
}

void StoryboardEngine::RigidBody::DestroyPhysicsBody()
{
	JPH::BodyInterface& bodyInterface = StoryboardEngine::Physics3D::GetPhysicsSystem().GetBodyInterface();
	bodyInterface.RemoveBody(bodyID);
}
