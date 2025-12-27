#pragma once

#include <directxmath.h>
#include <SDL3/SDL_scancode.h>

namespace StoryboardEngine
{
	/// Represents the state of a button (i.e. controller button, keyboard key)
	enum class ButtonState
	{
		// The button is currently in the down position
		DOWN,
		// The button is currently in the up position
		UP,
		// The button was just pressed
		PRESSED,
		// The button was just released
		RELEASED
	};

	enum class MouseButton
	{
		LEFT,
		MIDDLE,
		RIGHT,
		X1,
		X2
	};

	class Input
	{
	public:
		// Gets the current state of the specified keyboard key.
		static ButtonState GetKeyState(SDL_Scancode key);
		// Gets the current state of the specified mouse button.
		static ButtonState GetMouseButtonState(MouseButton button);
		// Gets the current mouse position relative to the window.
		static DirectX::XMFLOAT2 GetMousePosition();
		// Gets the change in mouse position since the last frame.
		static DirectX::XMFLOAT2 GetMouseDelta();

	private:
		static ButtonState* m_keys;
		static ButtonState* m_mouseButtons;
		static DirectX::XMFLOAT2 m_mousePosition;
		static DirectX::XMFLOAT2 m_mouseDelta;

		friend class InputManager;
	};
}
