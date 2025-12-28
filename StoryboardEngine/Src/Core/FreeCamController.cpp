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

		inputRotationDelta.x = Input::GetMouseDelta().y;
		inputRotationDelta.y = Input::GetMouseDelta().x;

		if (inputRotationDelta.LengthSquared() != 0)
		{
			rotation.x += inputRotationDelta.x * m_rotateSpeed;
			rotation.y += inputRotationDelta.y * m_rotateSpeed;

			GetTransform()->SetRotation(Vector3(rotation.x, rotation.y, rotation.z));
		}
	}

	Vector3 inputMoveDelta = Vector3(0.0f, 0.0f, 0.0f);

	inputMoveDelta.x = static_cast<float>(Input::GetKeyState(SDL_SCANCODE_D) == ButtonState::DOWN) - static_cast<float>(Input::GetKeyState(SDL_SCANCODE_A) == ButtonState::DOWN);
	inputMoveDelta.y = static_cast<float>(Input::GetKeyState(SDL_SCANCODE_E) == ButtonState::DOWN) - static_cast<float>(Input::GetKeyState(SDL_SCANCODE_Q) == ButtonState::DOWN);
	inputMoveDelta.z = static_cast<float>(Input::GetKeyState(SDL_SCANCODE_W) == ButtonState::DOWN) - static_cast<float>(Input::GetKeyState(SDL_SCANCODE_S) == ButtonState::DOWN);

	Vector3 moveDelta = 
		inputMoveDelta.x * GetTransform()->GetRight() +
		inputMoveDelta.y * GetTransform()->GetUp() +
		inputMoveDelta.z * GetTransform()->GetForward();

	position += moveDelta * m_moveSpeed;

	GetTransform()->SetPosition(Vector3(position.x, position.y, position.z));
}
