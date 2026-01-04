#include "pch.h"
#include "FirstPersonController.h"
#include "Utils/Input.h"
#include "Utils/SceneReferenceGUIDrawer.h"
#include "Utils/Time.h"
#include "Components/Gun.h"
#include "Utils/MathUtils.h"

void FirstPersonController::OnAwake()
{
	rigidBody = GetSceneObject()->GetParent()->GetComponent<RigidBody>();
}

void FirstPersonController::OnUpdate()
{
	if (Input::GetMouseButtonState(MouseButton::RIGHT) == ButtonState::DOWN)
	{
		Vector3 this_rotation = GetTransform()->GetLocalRotation();
		Vector3 parent_rotation = GetSceneObject()->GetParent()->GetTransform()->GetLocalRotation();
		Vector3 inputRotationDelta = Vector3(0.0f, 0.0f, 0.0f);

		inputRotationDelta.x = Input::GetMouseDelta().y;
		inputRotationDelta.y = Input::GetMouseDelta().x;

		if (!MathUtils::Approximately(inputRotationDelta.x, 0.0f))
		{
			this_rotation.x += inputRotationDelta.x * lookSensitivity * Time::GetDeltaTime();
			parent_rotation.x = std::clamp(parent_rotation.x, -89.9f, 89.9f);

			GetTransform()->SetRotation(this_rotation);
		}

		if (!MathUtils::Approximately(inputRotationDelta.y, 0.0f))
		{
			parent_rotation.y += inputRotationDelta.y * lookSensitivity * Time::GetDeltaTime();

			GetSceneObject()->GetParent()->GetTransform()->SetRotation(parent_rotation);
		}
	}

	if (gun && Input::GetMouseButtonState(MouseButton::LEFT) == ButtonState::PRESSED)
	{
		gun->Fire();
	}

	if ((Input::GetKeyState(SDL_SCANCODE_SPACE) == ButtonState::PRESSED))
	{
		// Jump
		rigidBody->ApplyImpulse(Vector3(0.0f, jumpSpeed, 0.0f));
	}

	Vector3 movementInput = Vector3(0.0f, 0.0f, 0.0f);

	movementInput.x = static_cast<float>(Input::GetKeyState(SDL_SCANCODE_D) == ButtonState::DOWN) - static_cast<float>(Input::GetKeyState(SDL_SCANCODE_A) == ButtonState::DOWN);
	movementInput.z = static_cast<float>(Input::GetKeyState(SDL_SCANCODE_W) == ButtonState::DOWN) - static_cast<float>(Input::GetKeyState(SDL_SCANCODE_S) == ButtonState::DOWN);

	movementInput = 
		GetSceneObject()->GetParent()->GetTransform()->GetRight() * movementInput.x + 
		GetSceneObject()->GetParent()->GetTransform()->GetForward() * movementInput.z;

	movementInput.Normalize();
	movementInput.x *= moveSpeed;
	movementInput.z *= moveSpeed;

	rigidBody->ApplyForce(movementInput);

	/*if (!rigidBody || !rigidBody->IsActive())
		return;

	auto velocity = rigidBody->GetVelocity();

	if ((Input::GetKeyState(SDL_SCANCODE_SPACE) == ButtonState::PRESSED) && (fabsf(velocity.y) < 0.01f))
	{
		// Jump
		velocity.y = jumpSpeed;
	}

	velocity.x = static_cast<float>(Input::GetKeyState(SDL_SCANCODE_D) == ButtonState::DOWN) - static_cast<float>(Input::GetKeyState(SDL_SCANCODE_A) == ButtonState::DOWN);
	velocity.z = static_cast<float>(Input::GetKeyState(SDL_SCANCODE_W) == ButtonState::DOWN) - static_cast<float>(Input::GetKeyState(SDL_SCANCODE_S) == ButtonState::DOWN);

	velocity = 
		GetSceneObject()->GetParent()->GetTransform()->GetRight() * velocity.x + 
		GetSceneObject()->GetParent()->GetTransform()->GetForward() * velocity.z;
	velocity.Normalize();

	velocity.x *= moveSpeed;
	velocity.z *= moveSpeed;

	rigidBody->SetVelocity(velocity);*/
}

void FirstPersonController::OnDrawInspector()
{
	SceneReferenceGUIDrawer::DrawSceneReferenceGUI("Gun", gun);
	ImGui::DragFloat("Look Sensitivity", &lookSensitivity, 0.01f);
	ImGui::DragFloat("Move Speed", &moveSpeed, 0.1f);
	ImGui::DragFloat("Jump Speed", &jumpSpeed, 0.1f);
}

void FirstPersonController::OnDrawDebugInspector()
{
	ImGui::BeginDisabled();
	SceneReferenceGUIDrawer::DrawSceneReferenceGUI("Rigidbody", rigidBody);
	ImGui::EndDisabled();
}
