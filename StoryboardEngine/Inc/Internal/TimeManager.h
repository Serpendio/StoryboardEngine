#pragma once
#include <chrono>

namespace StoryboardEngine
{
	class TimeManager final
	{
	protected:
		static void Initialize();
		static void Frame();

		friend class ApplicationCore;
	private:
		TimeManager() = delete;

		static std::chrono::high_resolution_clock::time_point m_startTime;
		static std::chrono::high_resolution_clock::time_point m_lastFrameTime;
	};
}

