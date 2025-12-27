#include "pch.h"
#include "Internal/TimeManager.h"
#include "Core/Time.h"

std::chrono::high_resolution_clock::time_point StoryboardEngine::TimeManager::m_startTime;
std::chrono::high_resolution_clock::time_point StoryboardEngine::TimeManager::m_lastFrameTime;

void StoryboardEngine::TimeManager::Initialize()
{
	m_startTime = std::chrono::high_resolution_clock::now();
	m_lastFrameTime = m_startTime;
	Time::m_deltaTime = 0.0f;
	Time::m_totalTime = 0.0f;
}

void StoryboardEngine::TimeManager::Frame()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> deltaDuration = currentTime - m_lastFrameTime;
	Time::m_deltaTime = deltaDuration.count();
	Time::m_totalTime = std::chrono::duration<float>(currentTime - m_startTime).count();
	m_lastFrameTime = currentTime;
}