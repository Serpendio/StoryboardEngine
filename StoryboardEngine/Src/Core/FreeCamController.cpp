#include "pch.h"
#include "Core/FreeCamController.h"
#include "Utils/Input.h"
#include "Core/SceneTransform.h"
#include "Utils/Time.h"
#include <imgui.h>

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
			rotation.x += inputRotationDelta.x * m_rotateSpeed * Time::GetDeltaTime();
			rotation.y += inputRotationDelta.y * m_rotateSpeed * Time::GetDeltaTime();

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

	position += moveDelta * m_moveSpeed * Time::GetDeltaTime();

	GetTransform()->SetPosition(Vector3(position.x, position.y, position.z));
}

void StoryboardEngine::FreeCamController::OnDrawInspector()
{
	ImGui::DragFloat("Move Speed", &m_moveSpeed, .1f);
	ImGui::DragFloat("Rotate Speed", &m_rotateSpeed, .5f);
}
