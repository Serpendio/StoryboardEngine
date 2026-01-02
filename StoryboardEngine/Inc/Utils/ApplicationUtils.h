#pragma once

#include "pch.h"

namespace StoryboardEngine
{
	class ApplicationUtils
	{
		static bool deserializingScene;
		static bool isPlaying;
		static Vector2 screenSize;

		friend class Scene;
		friend class EditorLayer;
		friend class ApplicationCore;
	public:
		static constexpr bool InEditor()
		{
#ifdef _EDITOR
			return true;
#else
			return false;
#endif
		}
		static bool IsDeserializingScene();
		static bool IsPlaying();
		static Vector2 GetScreenSize();

		static void Quit();
	};
}