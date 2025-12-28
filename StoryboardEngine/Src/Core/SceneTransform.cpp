#include "pch.h"
#include "Core/SceneTransform.h"
#include <imgui.h>
#include "Core/SceneObject.h"
#include "Utils/MathUtils.h"

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
	// ToDo: Implement global rotation
	return rotation;
}

Vector3 StoryboardEngine::SceneTransform::GetLocalPosition() const
{
	return position;
}

Vector3 StoryboardEngine::SceneTransform::GetGlobalPosition() const
{
	return GetSceneObject()->GetParent() ? GetSceneObject()->GetParent()->GetTransform()->GetGlobalPosition() + position : position;
}

Vector3 StoryboardEngine::SceneTransform::GetLocalScale() const
{
	return scale;
}

Vector3 StoryboardEngine::SceneTransform::GetGlobalScale() const
{
	// ToDo: Implement global scale
	return scale;
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
	rightVec = forwardVec.Cross(upVec);
}

void StoryboardEngine::SceneTransform::SetPosition(const Vector3& position)
{
	changedThisFrame = true;
	this->position = position;
}

void StoryboardEngine::SceneTransform::SetScale(const Vector3& scale)
{
	changedThisFrame = true;
	this->scale = scale;
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
