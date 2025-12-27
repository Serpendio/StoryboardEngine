#include "pch.h"
#include "Core/Input.h"
#include <SDL3/SDL.h>

StoryboardEngine::ButtonState* StoryboardEngine::Input::m_keys = nullptr;
StoryboardEngine::ButtonState* StoryboardEngine::Input::m_mouseButtons = nullptr;
DirectX::XMFLOAT2 StoryboardEngine::Input::m_mousePosition = DirectX::XMFLOAT2(0.0f, 0.0f);
DirectX::XMFLOAT2 StoryboardEngine::Input::m_mouseDelta = DirectX::XMFLOAT2(0.0f, 0.0f);

StoryboardEngine::ButtonState StoryboardEngine::Input::GetKeyState(SDL_Scancode key)
{
	if (key < 0 || key >= SDL_SCANCODE_COUNT)
	{
		return ButtonState::UP;
	}

	return m_keys[key];
}

StoryboardEngine::ButtonState StoryboardEngine::Input::GetMouseButtonState(MouseButton button)
{
	int buttonIndex = static_cast<int>(button);

	if (buttonIndex < 0 || buttonIndex >= 5)
	{
		return ButtonState::UP;
	}

	return m_mouseButtons[buttonIndex];
}

DirectX::XMFLOAT2 StoryboardEngine::Input::GetMousePosition()
{
	return m_mousePosition;
}

DirectX::XMFLOAT2 StoryboardEngine::Input::GetMouseDelta()
{
	return m_mouseDelta;
}
