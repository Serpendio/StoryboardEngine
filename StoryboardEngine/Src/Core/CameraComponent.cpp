#include "pch.h"
#include "Core/CameraComponent.h"
#include "Core/SceneTransform.h"
#include "Core/SceneObject.h"
#include "Core/Scene.h"
#include "Internal/ColourShader.h"

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
	Vector3 lookAtVector = positionVector + GetTransform()->GetForward();

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, GetTransform()->GetUp());

	ColourShader::SetViewMatrix(m_viewMatrix);
}

const Matrix& StoryboardEngine::CameraComponent::GetViewMatrix() const
{
	return m_viewMatrix;
}

const Matrix& StoryboardEngine::CameraComponent::GetProjectionMatrix()
{
	return s_projectionMatrix;
}
