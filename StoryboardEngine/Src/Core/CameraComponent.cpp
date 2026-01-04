#include "pch.h"
#include "Core/CameraComponent.h"
#include "Core/SceneTransform.h"
#include "Core/SceneObject.h"
#include "Core/Scene.h"
#include "Internal/ColourShader.h"
#include <Utils\MathUtils.h>

Matrix StoryboardEngine::CameraComponent::s_projectionMatrix;

void StoryboardEngine::CameraComponent::OnUpdate()
{
	GetSceneObject()->GetScene()->SetCamera(GetSelfReference());
}

void StoryboardEngine::CameraComponent::Render()
{
	// Cache the position of the camera.
	Vector3 positionVector = GetTransform()->GetGlobalPosition();

	// Translate the rotated camera position to the location of the viewer.
	// Get the rotation in radians
	Vector3 rotationVector = MathUtils::DegToRad(GetTransform()->GetGlobalRotation());

	// Create the rotation matrix from the (Y axis), pitch (X axis), and roll (Z axis) values (as radians)
	Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(rotationVector.y, rotationVector.x, rotationVector.z);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin
	Vector3 forwardVec = Vector3::TransformNormal(Vector3::Forward, rotationMatrix);
	Vector3 upVec = Vector3::TransformNormal(Vector3::Up, rotationMatrix);

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = DirectX::XMMatrixLookToLH(positionVector, forwardVec, upVec);

	ColourShader::SetViewMatrix(m_viewMatrix);



	/*// Cache the position of the camera.
	Vector3 positionVector = GetTransform()->GetGlobalPosition();

	// Translate the rotated camera position to the location of the viewer.
	Vector3 lookAtVector = positionVector + GetTransform()->GetForward();

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, GetTransform()->GetUp());

	ColourShader::SetViewMatrix(m_viewMatrix);*/
}

const Matrix& StoryboardEngine::CameraComponent::GetViewMatrix() const
{
	return m_viewMatrix;
}

const Matrix& StoryboardEngine::CameraComponent::GetProjectionMatrix()
{
	return s_projectionMatrix;
}
