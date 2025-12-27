#pragma once

namespace StoryboardEngine
{
	class InputManager final
	{
	protected:
		static void Initialize();
		static void Shutdown();
		static void UpdateStates();

		friend class ApplicationCore;
	private:
		InputManager() = delete;
	};
}