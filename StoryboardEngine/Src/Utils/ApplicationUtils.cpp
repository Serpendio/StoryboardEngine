#include "pch.h"
#include "Utils/ApplicationUtils.h"

bool StoryboardEngine::ApplicationUtils::deserializingScene = false;
bool StoryboardEngine::ApplicationUtils::isPlaying = false;
int StoryboardEngine::ApplicationUtils::screenWidth = 1920;
int StoryboardEngine::ApplicationUtils::screenHeight = 1080;

bool StoryboardEngine::ApplicationUtils::IsDeserializingScene()
{
	return deserializingScene;
}

bool StoryboardEngine::ApplicationUtils::IsPlaying()
{
	return isPlaying;
}

void StoryboardEngine::ApplicationUtils::GetScreenSize(int& width, int& height)
{
	width = screenWidth;
	height = screenHeight;
}

void StoryboardEngine::ApplicationUtils::Quit()
{
	isPlaying = false;
}
