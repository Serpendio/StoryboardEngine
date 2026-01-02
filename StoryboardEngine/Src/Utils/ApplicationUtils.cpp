#include "pch.h"
#include "Utils/ApplicationUtils.h"

bool StoryboardEngine::ApplicationUtils::deserializingScene = false;
bool StoryboardEngine::ApplicationUtils::isPlaying = false;
Vector2 StoryboardEngine::ApplicationUtils::screenSize = Vector2(1280, 720);

bool StoryboardEngine::ApplicationUtils::IsDeserializingScene()
{
	return deserializingScene;
}

bool StoryboardEngine::ApplicationUtils::IsPlaying()
{
	return isPlaying;
}

Vector2 StoryboardEngine::ApplicationUtils::GetScreenSize()
{
	return screenSize;
}

void StoryboardEngine::ApplicationUtils::Quit()
{
	isPlaying = false;
}
