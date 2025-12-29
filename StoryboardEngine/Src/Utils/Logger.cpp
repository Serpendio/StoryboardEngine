#include "pch.h"
#include "Utils/Logger.h"

void StoryboardEngine::Logger::CheckDebugBreak()
{
	// Needed as a separate function, as it seems definitions are not defined inside templated functions
#if defined(_DEBUG) && defined(_BREAK_ON_LOG_ERROR)
	DebugBreak();
#endif
}