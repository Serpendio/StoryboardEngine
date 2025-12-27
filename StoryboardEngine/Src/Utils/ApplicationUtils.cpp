#include "pch.h"
#include "Utils/ApplicationUtils.h"

bool StoryboardEngine::ApplicationUtils::deserializingScene = false;
#ifdef _EDITOR
bool StoryboardEngine::ApplicationUtils::isPlaying = false;
#endif

bool StoryboardEngine::ApplicationUtils::IsDeserializingScene()
{
	return deserializingScene;
}

bool StoryboardEngine::ApplicationUtils::IsPlaying()
{
#ifdef _EDITOR
	return isPlaying;
#else
	return true;
#endif
}
