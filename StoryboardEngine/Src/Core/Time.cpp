#include "pch.h"
#include "Core/Time.h"

float StoryboardEngine::Time::m_deltaTime;
float StoryboardEngine::Time::m_totalTime;

float StoryboardEngine::Time::GetDeltaTime()
{
	return m_deltaTime;
}

float StoryboardEngine::Time::GetTotalTime()
{
	return m_totalTime;
}
