#include "pch.h"
#include "Core/SceneTransform.h"
#include <imgui.h>
#include "Core/SceneObject.h"
#include "Utils/MathUtils.h"
#include "Core/RigidBody.h"
#include "Internal/Physics3D.h"

bool StoryboardEngine::SceneTransform::suppressPhysicsSync = false;

void StoryboardEngine::SceneTransform::OnDrawInspector()
{
	SceneComponent::OnDrawInspector();

	auto vec = position;
	ImGui::DragFloat3("Position", &vec.x, 0.01f);
	SetPosition(vec);
	vec = rotation;
	ImGui::DragFloat3("Rotation", &vec.x, 0.1f);
	SetRotation(vec);
	vec = scale;
	ImGui::DragFloat3("Scale", &vec.x, 0.01f);
	SetScale(vec);
}

void StoryboardEngine::SceneTransform::OnDrawDebugInspector()
{
	SceneComponent::OnDrawDebugInspector();

	ImGui::BeginDisabled();
	ImGui::InputFloat3("Up", &upVec.x);
	ImGui::InputFloat3("Forward", &forwardVec.x);
	ImGui::InputFloat3("Right", &rightVec.x);
	ImGui::EndDisabled();
}

Matrix StoryboardEngine::SceneTransform::GetMatrix() const
{
	// ToDo: Optimize by caching
	// ToDo: Store as quaternion

	DirectX::XMMATRIX matrix = 1
		* Matrix::CreateScale(scale.x, scale.y, scale.z)
		* Matrix::CreateRotationX(DirectX::XMConvertToRadians(rotation.x))
		* Matrix::CreateRotationY(DirectX::XMConvertToRadians(rotation.y))
		* Matrix::CreateRotationZ(DirectX::XMConvertToRadians(rotation.z))
		* Matrix::CreateTranslation(position.x, position.y, position.z)
		;

	return matrix;
}

Vector3 StoryboardEngine::SceneTransform::GetUp() const
{
	return upVec;
}

Vector3 StoryboardEngine::SceneTransform::GetForward() const
{
	return forwardVec;
}

Vector3 StoryboardEngine::SceneTransform::GetRight() const
{
	return rightVec;
}

Vector3 StoryboardEngine::SceneTransform::GetLocalRotation() const
{
	return rotation;
}

Vector3 StoryboardEngine::SceneTransform::GetGlobalRotation() const
{
	return LocalToGlobalRotation(rotation);
}

Vector3 StoryboardEngine::SceneTransform::GetLocalPosition() const
{
	return position;
}

Vector3 StoryboardEngine::SceneTransform::GetGlobalPosition() const
{
	return LocalToGlobalPosition(position);
}

Vector3 StoryboardEngine::SceneTransform::GetLocalScale() const
{
	return scale;
}

Vector3 StoryboardEngine::SceneTransform::GetGlobalScale() const
{
	return LocalToGlobalScale(scale);
}

void StoryboardEngine::SceneTransform::SetRotation(const Vector3& rotation)
{
	// ToDo: Work out if this avoids gimbal lock and if not, fix it
	// ToDo: This function is so wrong...

	if (rotation == this->rotation)
		return;

	changedThisFrame = true;
	this->rotation = Vector3(std::fmod(rotation.x, 360.f), std::fmod(rotation.y, 360.f), std::fmod(rotation.z, 360.f));

	// Default direction vectors
	upVec = Vector3(0.0f, 1.0f, 0.0f);
	forwardVec = Vector3(0.0f, 0.0f, 1.0f);

	// Get the rotation in radians
	Vector3 rotationVector = MathUtils::DegToRad(this->rotation);

	// Create the rotation matrix from the (Y axis), pitch (X axis), and roll (Z axis) values (as radians)
	Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(rotationVector.y, rotationVector.x, rotationVector.z);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin
	forwardVec = Vector3::TransformNormal(forwardVec, rotationMatrix);
	upVec = Vector3::TransformNormal(upVec, rotationMatrix);
	rightVec = upVec.Cross(forwardVec);

	if (suppressPhysicsSync)
		return;

	SyncPhysicsBodyToJolt();
}

void StoryboardEngine::SceneTransform::SetPosition(const Vector3& position)
{
	changedThisFrame = true;
	this->position = position;

	if (suppressPhysicsSync)
		return;

	SyncPhysicsBodyToJolt();
}

void StoryboardEngine::SceneTransform::SetScale(const Vector3& scale)
{
	changedThisFrame = true;
	this->scale = scale;

	SyncPhysicsBodyToJolt();
}

void StoryboardEngine::SceneTransform::LookAt(const Vector3& toPosition, const Vector3& toUpVec)
{
	FaceDirection(toPosition - position, toUpVec);
}

void StoryboardEngine::SceneTransform::FaceDirection(const Vector3& direction, const Vector3& toUpVec)
{
	changedThisFrame = true;
	Matrix rotationMatrix = Matrix::CreateWorld(position, -direction, toUpVec);

	forwardVec = Vector3::TransformNormal(Vector3(0, 0, 1), rotationMatrix);
	upVec = Vector3::TransformNormal(Vector3(0, 1, 0), rotationMatrix);
	rightVec = Vector3::TransformNormal(Vector3(1, 0, 0), rotationMatrix);

	Vector3 scale, translation;
	Quaternion rotationQuat;
	rotationMatrix.Decompose(scale, rotationQuat, translation);

	SetRotation(MathUtils::RadToDeg(rotationQuat.ToEuler()));
}

bool StoryboardEngine::SceneTransform::GetChangedThisFrame() const
{
	return changedThisFrame;
}

void StoryboardEngine::SceneTransform::SetChangedThisFrame()
{
	changedThisFrame = true;
}

void StoryboardEngine::SceneTransform::ClearChangedThisFrame()
{
	changedThisFrame = false;
}

void StoryboardEngine::SceneTransform::OnFinishedLoading()
{
	SceneComponent::OnFinishedLoading();
	SetRotation(rotation);
}

Vector3 StoryboardEngine::SceneTransform::LocalToGlobalPosition(const Vector3& localPosition) const
{
	// Gets the global position, accounting for parent scale and rotation
	Vector3 globalPosition = localPosition;
	SceneReference<SceneObject> parent = GetSceneObject()->GetParent();
	while (parent)
	{
		SceneReference<SceneTransform> parentTransform = parent->GetTransform();
		// Scale the position by the parent's scale
		globalPosition.x *= parentTransform->scale.x;
		globalPosition.y *= parentTransform->scale.y;
		globalPosition.z *= parentTransform->scale.z;
		// Rotate the position by the parent's rotation
		Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(
			MathUtils::DegToRad(parentTransform->rotation.y),
			MathUtils::DegToRad(parentTransform->rotation.x),
			MathUtils::DegToRad(parentTransform->rotation.z)
		);
		globalPosition = Vector3::TransformNormal(globalPosition, rotationMatrix);
		// Add the parent's position
		globalPosition += parentTransform->position;
		parent = parent->GetParent();
	}
	return globalPosition;
}

Vector3 StoryboardEngine::SceneTransform::LocalToGlobalRotation(const Vector3& localRotation) const
{
	// ToDo: Rotation should be stored internally as quaternion
	Quaternion globalRotation = Quaternion::CreateFromYawPitchRoll(
		MathUtils::DegToRad(localRotation.y),
		MathUtils::DegToRad(localRotation.x),
		MathUtils::DegToRad(localRotation.z)
	);
	SceneReference<SceneObject> parent = GetSceneObject()->GetParent();
	while (parent)
	{
		SceneReference<SceneTransform> parentTransform = parent->GetTransform();
		Quaternion parentRotation = Quaternion::CreateFromYawPitchRoll(
			MathUtils::DegToRad(parentTransform->rotation.y),
			MathUtils::DegToRad(parentTransform->rotation.x),
			MathUtils::DegToRad(parentTransform->rotation.z)
		);
		globalRotation = parentRotation * globalRotation;
		parent = parent->GetParent();
	}
	return globalRotation.ToEuler();
}

Vector3 StoryboardEngine::SceneTransform::LocalToGlobalScale(const Vector3& localScale) const
{
	// Gets the global scale, accounting for parent scale
	Vector3 globalScale = localScale;
	SceneReference<SceneObject> parent = GetSceneObject()->GetParent();
	while (parent)
	{
		SceneReference<SceneTransform> parentTransform = parent->GetTransform();
		globalScale.x *= parentTransform->scale.x;
		globalScale.y *= parentTransform->scale.y;
		globalScale.z *= parentTransform->scale.z;
		parent = parent->GetParent();
	}
	return globalScale;
}

void StoryboardEngine::SceneTransform::SyncPhysicsBodyToJolt()
{
#ifdef _EDITOR
	if (!ApplicationUtils::IsPlaying())
	{
		return;
	}
#endif

	SceneReference<RigidBody> rigidBody = GetSceneObject()->GetComponent<RigidBody>();

	if (rigidBody)
	{
		JPH::BodyID bodyID = rigidBody->GetBodyID();
		auto& physicsSystem = Physics3D::GetPhysicsSystem();

		Vector3 globalPosition = GetGlobalPosition();
		Vector3 globalRotation = GetGlobalRotation();
		Quaternion rotationQuat = Quaternion::CreateFromYawPitchRoll(
			MathUtils::DegToRad(globalRotation.y),
			MathUtils::DegToRad(globalRotation.x),
			MathUtils::DegToRad(globalRotation.z)
		);
		physicsSystem.GetBodyInterface().SetPositionAndRotation(
			bodyID,
			JPH::RVec3(globalPosition.x, globalPosition.y, globalPosition.z),
			JPH::Quat(rotationQuat.x, rotationQuat.y, rotationQuat.z, rotationQuat.w),
			JPH::EActivation::Activate
		);

		Vector3 globalScale = GetGlobalScale() / 2;
		auto shape = physicsSystem.GetBodyInterface().GetShape(bodyID);
		shape->ScaleShape(JPH::Vec3(globalScale.x, globalScale.y, globalScale.z));
	}

	size_t childCount = GetSceneObject()->GetChildCount();
	for (size_t i = 0; i < childCount; i++)
	{
		SceneReference<SceneObject> childObject = GetSceneObject()->GetChild(i);
		SceneReference<SceneTransform> childTransform = childObject->GetTransform();
		childTransform->SyncPhysicsBodyToJolt();
	}
}

void StoryboardEngine::SceneTransform::SyncPhysicsBodyFromJolt()
{
#ifdef _EDITOR
	if (!ApplicationUtils::IsPlaying())
	{
		return;
	}
#endif

	SceneReference<RigidBody> rigidBody = GetSceneObject()->GetComponent<RigidBody>();
	if (rigidBody)
	{
		JPH::BodyID bodyID = rigidBody->GetBodyID();
		auto& physicsSystem = Physics3D::GetPhysicsSystem();

		JPH::RVec3 joltPosition = physicsSystem.GetBodyInterface().GetPosition(bodyID);
		JPH::Quat joltRotation = physicsSystem.GetBodyInterface().GetRotation(bodyID);
		Vector3 globalPosition = Vector3(static_cast<float>(joltPosition.GetX()), static_cast<float>(joltPosition.GetY()), static_cast<float>(joltPosition.GetZ()));
		Quaternion rotationQuat = Quaternion(joltRotation.GetX(), joltRotation.GetY(), joltRotation.GetZ(), joltRotation.GetW());
		Vector3 globalRotation = rotationQuat.ToEuler();
	
		suppressPhysicsSync = true;

		SetPosition(GlobalToLocalPosition(globalPosition));
		SetRotation(GlobalToLocalRotation(MathUtils::RadToDeg(globalRotation)));
		suppressPhysicsSync = false;
	}

	size_t childCount = GetSceneObject()->GetChildCount();
	for (size_t i = 0; i < childCount; i++)
	{
		SceneReference<SceneObject> childObject = GetSceneObject()->GetChild(i);
		SceneReference<SceneTransform> childTransform = childObject->GetTransform();
		childTransform->SyncPhysicsBodyFromJolt();
	}
}

Vector3 StoryboardEngine::SceneTransform::GlobalToLocalPosition(const Vector3& globalPosition) const
{
	if (!GetSceneObject()->GetParent())
	{
		return globalPosition;
	}

	SceneReference<SceneTransform> parentTransform = GetSceneObject()->GetParent()->GetTransform();

	// Translate into parent's origin
	Vector3 delta = globalPosition - parentTransform->GetGlobalPosition();

	// Rotate by inverse of parent's global rotation
	Vector3 parentGlobalRot = parentTransform->GetGlobalRotation(); // degrees
	Matrix parentRot = Matrix::CreateFromYawPitchRoll(
		MathUtils::DegToRad(parentGlobalRot.y),
		MathUtils::DegToRad(parentGlobalRot.x),
		MathUtils::DegToRad(parentGlobalRot.z)
	);

	Matrix invParentRot = parentRot.Transpose();
	Vector3 rotated = Vector3::TransformNormal(delta, invParentRot);

	// Divide by parent's global scale
	Vector3 parentGlobalScale = parentTransform->GetGlobalScale();

	Vector3 safeScale(
		fabs(parentGlobalScale.x) > FLT_EPSILON ? parentGlobalScale.x : FLT_EPSILON,
		fabs(parentGlobalScale.y) > FLT_EPSILON ? parentGlobalScale.y : FLT_EPSILON,
		fabs(parentGlobalScale.z) > FLT_EPSILON ? parentGlobalScale.z : FLT_EPSILON
	);

	return Vector3(rotated.x / safeScale.x, rotated.y / safeScale.y, rotated.z / safeScale.z);
}

Vector3 StoryboardEngine::SceneTransform::GlobalToLocalRotation(const Vector3& globalRotation) const
{
	if (!GetSceneObject()->GetParent())
	{
		return globalRotation;
	}

	Vector3 localRotation = globalRotation - GetSceneObject()->GetParent()->GetTransform()->GetGlobalRotation();
	return localRotation;
}

Vector3 StoryboardEngine::SceneTransform::GlobalToLocalScale(const Vector3& globalScale) const
{
	if (!GetSceneObject()->GetParent())
	{
		return globalScale;
	}

	SceneReference<SceneTransform> parentTransform = GetSceneObject()->GetParent()->GetTransform();
	Vector3 parentGlobalScale = parentTransform->GetGlobalScale();
	Vector3 safeScale(
		fabs(parentGlobalScale.x) > FLT_EPSILON ? parentGlobalScale.x : FLT_EPSILON,
		fabs(parentGlobalScale.y) > FLT_EPSILON ? parentGlobalScale.y : FLT_EPSILON,
		fabs(parentGlobalScale.z) > FLT_EPSILON ? parentGlobalScale.z : FLT_EPSILON
	);

	return Vector3(globalScale.x / safeScale.x, globalScale.y / safeScale.y, globalScale.z / safeScale.z);
}
