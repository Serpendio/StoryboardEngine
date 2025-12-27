#pragma once

namespace StoryboardEngine
{
	class Time
	{
	public:
		static float GetDeltaTime();
		static float GetTotalTime();

	private:
		static float m_deltaTime;
		static float m_totalTime;

		friend class TimeManager;
	};
}

