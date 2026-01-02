#include "pch.h"
#include "Internal/InputManager.h"
#include "Utils/Input.h"
#include <SDL3/SDL.h>
#include "imgui.h"

void StoryboardEngine::InputManager::Initialize()
{

	if (Input::m_keys != nullptr)
	{
		// A version of the input class has already been initialized.
		Logger::LogError("Input class already instantiated.");
		return;
	}

	Input::m_keys = new ButtonState[SDL_SCANCODE_COUNT];
	Input::m_mouseButtons = new ButtonState[5];

	// Initialize all the keys to being released and not pressed.
	for (int i = 0; i < SDL_SCANCODE_COUNT; ++i)
	{
		Input::m_keys[i] = ButtonState::UP;
	}

	for (int i = 0; i < 5; ++i)
	{
		Input::m_mouseButtons[i] = ButtonState::UP;
	}

	return;
}

void StoryboardEngine::InputManager::Shutdown()
{
	delete[] Input::m_keys;
	Input::m_keys = nullptr;

	delete[] Input::m_mouseButtons;
	Input::m_mouseButtons = nullptr;

	return;
}

void StoryboardEngine::InputManager::UpdateStates()
{
	ImGuiIO& io = ImGui::GetIO();

	auto keyboardState = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < SDL_SCANCODE_COUNT; ++i)
	{
		bool isDown = 
			io.WantCaptureKeyboard ? false : // If ImGui wants to capture the keyboard, we treat all keys as up
			keyboardState[i] != 0; // Otherwise, get the actual key state

		switch (Input::m_keys[i])
		{
		case ButtonState::UP:
			if (isDown)
			{
				Input::m_keys[i] = ButtonState::PRESSED;
			}
			break;
		case ButtonState::DOWN:
			if (!isDown)
			{
				Input::m_keys[i] = ButtonState::RELEASED;
			}
			break;
		case ButtonState::PRESSED:
			if (isDown)
			{
				Input::m_keys[i] = ButtonState::DOWN;
			}
			else
			{
				Input::m_keys[i] = ButtonState::RELEASED;
			}
			break;
		case ButtonState::RELEASED:
			if (isDown)
			{
				Input::m_keys[i] = ButtonState::PRESSED;
			}
			else
			{
				Input::m_keys[i] = ButtonState::UP;
			}
			break;
		}
	}

	SDL_MouseButtonFlags flags = SDL_GetMouseState(nullptr, nullptr);

	for (int i = 0; i < 5; ++i)
	{
		bool isDown = 
			io.WantCaptureMouse ? false : // If ImGui wants to capture the mouse, we treat all buttons as up
			(flags & SDL_BUTTON_MASK(i + 1)) != 0; // Otherwise, get the actual button state

		switch (Input::m_mouseButtons[i])
		{
		case ButtonState::UP:
			if (isDown)
			{
				Input::m_mouseButtons[i] = ButtonState::PRESSED;
			}
			break;
		case ButtonState::DOWN:
			if (!isDown)
			{
				Input::m_mouseButtons[i] = ButtonState::RELEASED;
			}
			break;
		case ButtonState::PRESSED:
			if (isDown)
			{
				Input::m_mouseButtons[i] = ButtonState::DOWN;
			}
			else
			{
				Input::m_mouseButtons[i] = ButtonState::RELEASED;
			}
			break;
		case ButtonState::RELEASED:
			if (isDown)
			{
				Input::m_mouseButtons[i] = ButtonState::PRESSED;
			}
			else
			{
				Input::m_mouseButtons[i] = ButtonState::UP;
			}
			break;
		}
	}

	DirectX::XMFLOAT2 previousMousePosition = Input::m_mousePosition;
	SDL_GetMouseState(&Input::m_mousePosition.x, &Input::m_mousePosition.y);
	Input::m_mouseDelta.x = Input::m_mousePosition.x - previousMousePosition.x;
	Input::m_mouseDelta.y = Input::m_mousePosition.y - previousMousePosition.y;
}