#include "pch.h"
#include "Core/FreeCamController.h"
#include "Core/Input.h"
#include "Core/SceneTransform.h"

void StoryboardEngine::FreeCamController::OnUpdate()
{
	Vector3 position = GetTransform()->GetLocalPosition();
	Vector3 rotation = GetTransform()->GetLocalRotation();

	if (Input::GetMouseButtonState(MouseButton::RIGHT) == ButtonState::DOWN)
	{
		Vector3 inputRotationDelta = Vector3(0.0f, 0.0f, 0.0f);

		inputRotationDelta.x = -Input::GetMouseDelta().y;
		inputRotationDelta.y = Input::GetMouseDelta().x;

		rotation.x += inputRotationDelta.x * m_rotateSpeed;
		rotation.y += inputRotationDelta.y * m_rotateSpeed;
	}

	Vector3 inputMoveDelta = Vector3(0.0f, 0.0f, 0.0f);

	inputMoveDelta.x = static_cast<float>(Input::GetKeyState(SDL_SCANCODE_D) == ButtonState::DOWN) - (Input::GetKeyState(SDL_SCANCODE_A) == ButtonState::DOWN);
	inputMoveDelta.y = static_cast<float>(Input::GetKeyState(SDL_SCANCODE_E) == ButtonState::DOWN) - (Input::GetKeyState(SDL_SCANCODE_Q) == ButtonState::DOWN);
	inputMoveDelta.z = static_cast<float>(Input::GetKeyState(SDL_SCANCODE_W) == ButtonState::DOWN) - (Input::GetKeyState(SDL_SCANCODE_S) == ButtonState::DOWN);

	// ToDo: I don't feel like these should have to be negated. Something is probably wrong somewhere else
	inputMoveDelta.x = -inputMoveDelta.x;
	inputMoveDelta.z = -inputMoveDelta.z;

	Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(rotation.y),
		DirectX::XMConvertToRadians(rotation.x),
		DirectX::XMConvertToRadians(rotation.z)
	);

	Vector3::TransformNormal(inputMoveDelta, rotationMatrix, inputMoveDelta);

	position += inputMoveDelta * m_moveSpeed;

	GetTransform()->SetPosition(Vector3(position.x, position.y, position.z));
	GetTransform()->SetRotation(Vector3(rotation.x, rotation.y, rotation.z));
}
