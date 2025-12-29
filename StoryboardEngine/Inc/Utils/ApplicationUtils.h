#pragma once

namespace StoryboardEngine
{
	class ApplicationUtils
	{
		static bool deserializingScene;
		static bool isPlaying;
		static int screenWidth, screenHeight;

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
		static void GetScreenSize(int& width, int& height);

		static void Quit();
	};
}